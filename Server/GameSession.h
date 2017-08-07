#pragma once
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
class GameSession
{
public:
	GameSession();
	~GameSession();
	void updatePlayerState(json::value completeState);
	BWAPI::UnitType buildOrderManagerB();
	std::map<BWAPI::UnitType, int> getPlayerState();
	std::map<BWAPI::UnitType, int> getQueued();
	BWAPI::UnitType getLastQueued();
private:
	std::map<BWAPI::UnitType, int> playerState; //Existing/Functional Units
	std::map<BWAPI::UnitType, int> queued; //Units queued, training or constructing
	BWAPI::UnitType lastQueued;
};

