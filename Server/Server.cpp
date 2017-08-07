// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameSession.h"
#include <curses.h>
#include <BWAPI.h>
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <iostream>
using namespace utility;                    // Common utilities like string conversions
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;
using namespace web::http::experimental::listener;
using namespace std;


#define TRACE(msg)            wcout << msg
#define TRACE_ACTION(a, k, v) wcout << a << L" (" << k << L", " << v << L")\n"

map<utility::string_t, utility::string_t> dictionary;
std::vector<GameSession> sessions;
GameSession& getSession(int sessionID);

std::map<BWAPI::UnitType, int> playerState; //Existing/Functional Units
std::map<BWAPI::UnitType, int> queued; //Units queued, training or constructing
BWAPI::UnitType lastQueued = BWAPI::UnitTypes::Terran_SCV;
//Build Order related functions
int getRemainingSupply();
BWAPI::UnitType buildOrderManagerB();
void printServerStatus();

//JSON related functions
void handle_get(http_request request);
void handle_post(http_request request);
void handle_delete(http_request request);
void updatePlayerState(json::value completePlayerState);


/* handlers implementation */
void handle_get(http_request request)
{
	try
	{
		json::value obj;
		for (auto const & p : dictionary)
		{
			obj[p.first] = json::value::string(p.second);
		}
		if (request.headers().has(L"Order"))
		{
			//find the respective game session by extracting the session id 
			http_headers h = request.headers();
			int sID = std::stoi(h[L"SessionId"]);
			lastQueued = getSession(sID).getLastQueued();
			getSession(sID).getQueued()[lastQueued]++;
			getSession(sID).buildOrderManagerB();
			json::value order = json::value::number(lastQueued);
			request.reply(status_codes::OK, order);
		}
		else if (request.headers().has(L"Init"))
		{
			request.reply(status_codes::OK);
		}
		else
		{
			// this is just for debugging
			utility::stringstream_t stream;
			obj.serialize(stream);
			std::wcout << stream.str();
			request.reply(status_codes::OK, obj);
		}
		printServerStatus();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		request.reply(status_codes::InternalError);
	}
	catch (int i)
	{
		if (i = -1)
		{
			std::cout << "No such session exists" << std::endl;
		}
	}
}
void handle_post(http_request request)
{
	try
	{
		if (request.headers().has(L"Init"))
		{
			GameSession gS = GameSession::createGameSession();
			int token = gS.getID(); //This will be the gameSession token for 
			sessions.push_back(gS);
			request.reply(status_codes::OK, json::value::number(token));
		}
		else if (request.headers().content_type() == L"application/json") //If we have JSON data to deal with
		{
			if (request.headers().has(L"State")) //The client is trying to POST a state update
			{
				//Update the respective session
				//find the respective game session by extracting the session id
				http_headers h = request.headers();
				int sID = std::stoi(h[L"SessionId"]);
				getSession(sID).updatePlayerState(request.extract_json().get());
				request.reply(status_codes::OK);
			}
			else
			{
				request.reply(status_codes::InternalError);
			}
		}
		else
		{
			request.reply(status_codes::InternalError);
		}
		printServerStatus();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		request.reply(status_codes::InternalError);
	}
}
void handle_delete(http_request request)
{
	//Session has ended
	try
	{
		http_headers h = request.headers();
		int sID = std::stoi(h[L"SessionId"]);
		GameSession& gS = getSession(sID);
		sessions.erase(std::find(sessions.begin(), sessions.end(), gS));
		request.reply(status_codes::OK);
		printServerStatus();
	}
	catch (...)
	{
		request.reply(status_codes::InternalError);
	}
}
int main()
{
	initscr();
	http_listener listener(L"http://localhost:80/");
	listener.support(methods::GET, handle_get);
	listener.support(methods::POST, handle_post);
	listener.support(methods::DEL, handle_delete);
	try
	{
		listener
			.open()
			.then([&listener]() { })
			.wait();

		while (true);
	}
	catch (exception const & e)
	{
		std::string s;
		cin >> s;
	}

	return 0;
}
void printServerStatus()
{
	try
	{
		clear();
		for (auto&u : sessions)
		{
			attron(A_STANDOUT);
			printw(("\nSession ID: " + std::to_string(u.getID())).c_str());
			printw("\n");
			attroff(A_STANDOUT);
			u.printGameSession();
		}
		refresh();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << endl;
	}
}
GameSession& getSession(int sessionID)
{
	for (auto& u : sessions)
	{
		if (u.getID() == sessionID)
		{
			return u;
		}
	}
	throw - 1;
}
