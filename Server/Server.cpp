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
std::map<BWAPI::UnitType, int> playerState; //Existing/Functional Units
std::map<BWAPI::UnitType, int> queued; //Units queued, training or constructing
BWAPI::UnitType lastQueued = BWAPI::UnitTypes::Terran_SCV;
//Build Order related functions
int getRemainingSupply();
BWAPI::UnitType buildOrderManagerB();
void printGameSession();

//JSON related functions
void handle_get(http_request request);
void handle_post(http_request request);
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
			lastQueued = buildOrderManagerB();
			//I don't think it's good practise to try and keep track of the player state from here, but it'll be overwritten by the next stateUpdate anyway
			queued[lastQueued]++;
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
		printGameSession();
	}
	catch (...)
	{
		request.reply(status_codes::InternalError);
	}
}
void handle_post(http_request request)
{
	try
	{
		if (request.headers().content_type() == L"application/json") //If we have JSON data to deal with
		{
			if (request.headers().has(L"Init"));
			{

			}
			if (request.headers().has(L"State")) //The client is trying to POST a state update
			{
				updatePlayerState(request.extract_json().get());
				request.reply(status_codes::OK);
			}
		}
		else
		{
			request.reply(status_codes::InternalError);
		}
		printGameSession();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		request.reply(status_codes::InternalError);
	}
}
void updatePlayerState(json::value completePlayerState)
{
	json::value playerStateJSONobj = completePlayerState[L"PlayerState"];
	json::value queuedJSONobj = completePlayerState[L"Queued"];
	for (auto iter = playerStateJSONobj.as_object().cbegin(); iter != playerStateJSONobj.as_object().cend(); iter++)
	{
		playerState[std::stoi(iter->first)] = iter->second.as_integer();
	}
	try
	{
		for (auto iter = queuedJSONobj.as_object().cbegin(); iter != queuedJSONobj.as_object().cend(); iter++)
		{
			queued[std::stoi(iter->first)] = iter->second.as_integer();
		}
	}
	catch (...)
	{
		//If there is nothing queued, the previous call will fail but it is not an erronous complete state so just continue
		//If there is no Player State, an "ex nihilo" Build Order Manager is out of scope
	}
}
int getRemainingSupply()
{
	int supply = 0;
	for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
	{
		//Record supply for every unit that provides supply
		supply += (it->first.supplyProvided() * playerState.at(it->first));
		//Record supply usage for every unit requiring supply
		supply -= (it->first.supplyRequired() * playerState.at(it->first));
	}
	for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
	{
		//Record supply for every unit that provides supply
		supply += (it->first.supplyProvided() * queued.at(it->first));
		//Record supply usage for every unit requiring supply
		supply -= (it->first.supplyRequired() * queued.at(it->first));
	}
	return supply;
}
BWAPI::UnitType buildOrderManagerB()
{
	//Priorities should be normalized
	//Calculate the priority for building a new worker
	BWAPI::UnitType workerRecommendation = BWAPI::UnitTypes::Terran_SCV;
	double workerUnitPriority;

	//Calculate the priority for expanding the combat capabilities
	BWAPI::UnitType combatUnitRecommendation;
	double combatUnitPriority;

	//Calculate the priority for expanding the infrastructure
	BWAPI::UnitType buildingRecommendation;
	double buildingPriority;

	int workerCount = playerState[BWAPI::UnitTypes::Terran_SCV] + queued[BWAPI::UnitTypes::Terran_SCV];

	if (getRemainingSupply > 0)
	{
		workerUnitPriority = (1.0 - (workerCount / 10.0));
		if (workerUnitPriority <= 0)
		{
			workerUnitPriority = 0.1;
		}
	}
	else
	{
		workerUnitPriority = 0;
	}
	//We need barracks and supply to make marines
	if (playerState[BWAPI::UnitTypes::Terran_Barracks] > 0 && getRemainingSupply()>0)
	{
		combatUnitRecommendation = BWAPI::UnitTypes::Terran_Marine;
		combatUnitPriority = 0.4;
	}
	else
	{
		combatUnitPriority = 0;
	}

	//When checking to see what buildings we have, check the construction manager for queued buildings as well
	//If we don't have a refinery, that'll be the only building in consideration.
	if ((playerState[BWAPI::UnitTypes::Terran_Refinery] + queued[BWAPI::UnitTypes::Terran_Refinery]) < 1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Refinery;
		buildingPriority = 0.5;
	}
	else if (playerState[BWAPI::UnitTypes::Terran_Barracks] + queued[BWAPI::UnitTypes::Terran_Barracks] <1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Barracks;
		buildingPriority = 0.5;
	}
	else if (getRemainingSupply() + (queued[BWAPI::UnitTypes::Terran_Supply_Depot] * 8)) //One supply depo gives 8 supply
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Supply_Depot;
		buildingPriority = 1 - ((getRemainingSupply() + (queued[BWAPI::UnitTypes::Terran_Supply_Depot] * 8)) / 5);
	}
	else
	{
		buildingPriority = 0;
	}
	if (buildingPriority > combatUnitPriority && buildingPriority > workerUnitPriority)
	{
		return buildingRecommendation;
	}
	else if (combatUnitPriority > workerUnitPriority)
	{
		return combatUnitRecommendation;
	}
	else
	{
		return workerRecommendation;
	}
}
int main()
{
	initscr();
	http_listener listener(L"http://localhost:80/");
	listener.support(methods::GET, handle_get);
	listener.support(methods::POST, handle_post);
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
void printGameSession()
{
	try
	{
		clear();
		attron(A_BOLD);
		printw("Player State:\n");
		attroff(A_BOLD);
		for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
		{
			printw(" ");
			printw(((it->first.getName()) + " " + std::to_string(playerState.at(it->first))).c_str());
			printw("\n");
		}
		attron(A_BOLD);
		printw("Queued: \n");
		attroff(A_BOLD);
		for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
		{
			printw(" ");
			printw(((it->first.getName()) + " " + std::to_string(queued.at(it->first))).c_str());
			printw("\n");
		}
		attron(A_BOLD);
		printw("Last Order: \n");
		attroff(A_BOLD);
		printw(" ");
		printw(lastQueued.getName().c_str());
		refresh();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << endl;
	}
}
