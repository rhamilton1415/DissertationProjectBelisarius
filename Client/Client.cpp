// Client.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <BWAPI.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace utility;                    // Common utilities like string conversions
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

int main()
{
	std::map<BWAPI::UnitType, int> playerState;
	std::map<BWAPI::UnitType, int> queued;
	std::ifstream playerStateFile;
	playerStateFile.open("in.txt");
	if (playerStateFile.is_open()) 
	{
		std::string line;
		std::getline(playerStateFile, line);
		if (line == "Queued") //go to the next line and start building the queued map
		{
			std::getline(playerStateFile, line);
			while (line != "PlayerState") //go until we start reaaching playerstate info
			{
				std::string ID = "";
				std::string count = "";
				int i = 0;
				while (line[i] != '+') //get the ID before the +
				{
					ID += line[i];
					i++;
				}
				i++; //move past the seperator
				while (i < line.size()) //get the count after the +
				{
					count += line[i];
					i++;
				}
				queued[BWAPI::UnitType(std::stoi(ID))] = std::stoi(count);
				std::getline(playerStateFile, line);
			}
			std::getline(playerStateFile, line); //go to the next line and start building the active playerstate
			while (line != "eof") //go until the end of file
			{
				std::string ID = "";
				std::string count = "";
				int i = 0;
				while (line[i] != '+') //get the ID before the +
				{
					ID += line[i];
					i++;
				}
				i++;
				while (i < line.size()) //get the count after the +
				{
					count += line[i];
					i++;
				}
				playerState[BWAPI::UnitType(std::stoi(ID))] = std::stoi(count);
				std::getline(playerStateFile, line);
			}
		}
	}
	playerStateFile.close();
	std::cout << BWAPI::UnitTypes::Terran_Marine;
	for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
	{
		std::cout << (std::to_string(it->first) + " " + std::to_string(playerState.at(it->first))) << std::endl;
	}
	for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
	{
		std::cout << (std::to_string(it->first) + " " + std::to_string(queued.at(it->first))) << std::endl;
	}

	//web::http::client::http_client client(L"http://localhost:80/");
	//uri_builder builder(U("/search"));
	//builder.append_query(U("q"), U("cpprestsdk github"));
	//pplx::task<web::http::http_response> resp = client.request(web::http::methods::GET);
	////pplx::task<web::http::http_response> resp = client.request(web::http::methods::GET, builder.to_string());
	//std::cout << resp.get().status_code() << std::endl;
	//fprintf(stdout, "What up\n");
	int x = 0;
	std::cin >> x;
    return 0;
}

