#pragma once
#include <BWAPI.h>
#include <cpprest/http_client.h>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include <cpprest/filestream.h>
#include <iostream>
#include <curses.h>

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
	static GameSession createGameSession();
	GameSession(int sessionId);
	~GameSession();
	void updatePlayerState(json::value completeState);
	BWAPI::UnitType buildOrderManagerB();
	std::map<BWAPI::UnitType, int>& getPlayerState() { return playerState; }
	std::map<BWAPI::UnitType, int>& getQueued() { return queued; }
	BWAPI::UnitType getLastQueued() { return lastQueued; }
	int getID() { return id; }
	void GameSession::printGameSession();
	bool operator==(GameSession rhs) { return(id == rhs.getID()); }
private:
	static int sessionCount;
	int id;
	std::map<BWAPI::UnitType, int> playerState; //Existing/Functional Units
	std::map<BWAPI::UnitType, int> queued; //Units queued, training or constructing
	BWAPI::UnitType lastQueued;
	int getRemainingSupply();
};

