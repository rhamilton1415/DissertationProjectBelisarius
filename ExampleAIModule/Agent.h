#pragma once
#include <vector>
#include <BWAPI.h>
#include <exception>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
using namespace BWAPI;
using namespace Filter;
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency;
using namespace concurrency::streams;

class Agent
{
public:
	Agent();
	~Agent();
	virtual std::string getPreamble() { return preamble; }
	void connect();
protected:
	void broadcast(std::string message);
	std::string preamble = "this needs to be set ";
};

