// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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
std::map<BWAPI::UnitType, int> playerState;
std::map<BWAPI::UnitType, int> queued;

/* handlers implementation */
void handle_get(http_request request)
{
	TRACE(L"\nhandle GET\n");

	json::value obj;
	for (auto const & p : dictionary)
	{
		obj[p.first] = json::value::string(p.second);
	}

	// this is just for debugging
	utility::stringstream_t stream;
	obj.serialize(stream);
	std::wcout << stream.str();

	request.reply(status_codes::OK, obj);
}
void handle_post(http_request request)
{
	try
	{
		TRACE(L"\nhandle POST\n");
		json::value completeState = request.extract_json().get();
		json::value playerStateJSONobj = completeState[L"PlayerState"];
		json::value queuedJSONobj = completeState[L"Queued"];
		std::wcout << playerStateJSONobj.serialize() << std::endl;
		std::wcout << queuedJSONobj.serialize() << std::endl;
		/*for (auto iter = playerStateJSONobj.as_object().cbegin(); iter != playerStateJSONobj.as_object().cend(); iter++)
		{
			
		}
		for (auto iter = queuedJSONobj.as_object().cbegin(); iter != queuedJSONobj.as_object().cend(); iter++)
		{

		}*/
		request.reply(status_codes::OK);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		request.reply(status_codes::InternalError);
	}
}
int main()
{
	http_listener listener(L"http://localhost:80/");
	listener.support(methods::GET, handle_get);
	listener.support(methods::POST, handle_post);
	try
	{
		listener
			.open()
			.then([&listener]() {TRACE(L"\nstarting to listen\n"); })
			.wait();

		while (true);
	}
	catch (exception const & e)
	{
		std::string s;
		wcout << e.what() << endl;
		cin >> s;
	}

	return 0;
}

