#pragma once
#include <vector>
#include <exception>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <BWAPI.h>
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency;
using namespace concurrency::streams;
namespace Connectors
{
	//This class acts as an interface to the "Build Order Manager B" - the cloud based decision system.
	class Connector
	{
	public:
		static void establishConnection();
		static void updateState(std::map<BWAPI::UnitType, int> queued, std::map<BWAPI::UnitType, int> playerState);
		static BWAPI::UnitType getBOMBOrder();
		static bool isConnectionAvailable() { return connectionAvailable; }
		static std::string getErrMessage() { return errMessage; }
	private:
		//static int Connect(std::map<BWAPI::UnitType, int> queued, std::map<BWAPI::UnitType, int> playerState);
		static std::string updateBOMBState();
		static std::map<BWAPI::UnitType, int> playerState;
		static std::map<BWAPI::UnitType, int> queued;
		static bool connectionAvailable;
		static std::string errMessage;
	};
}
