#pragma once
#include <fstream> 
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

#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types
using namespace boost::posix_time;
class SessionProfiler
{
public:
	SessionProfiler();
	~SessionProfiler();
	void startUpdate();
	void finishUpdate();
	void startFunction();
	void finishFunction();
	long getAverageUpdateTime();
	long getAverageFunctionTime();
	long getUpdateOperationsPerMinute();
	long getFunctionOperationsPerMinute();
private:
	std::vector<std::pair<long,ptime>> updateTimes; //A vector of pairs which link each update execution duration to the time at which the function is executed
	std::vector<std::pair<long,ptime>> functionTimes;
	ptime updateTimerStart;
	ptime functionTimerStart;
	
	
};

