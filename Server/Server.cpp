// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
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
int main()
{
	http_listener listener(L"http://localhost:80/");
	listener.support(methods::GET, handle_get);
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

