#pragma once
#include <iostream>
#include <fstream> 
#include <vector>
#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types
using namespace boost::posix_time;
class BOMBProfiler
{
public:
	BOMBProfiler(int* i);
	~BOMBProfiler();
	void BOMBUpdateStart();
	void BOMBFunctionStart();
	void BOMBUpdateFinish();
	void BOMBFunctionFinish();
	void addConnectionAttempt() { connectionAttempts++; }
	void addConnectionFailure() { connectionFailures++; }

private:
	ptime BOMBUpdateTimer;
	ptime BOMBFunctionTimer;
	int connectionAttempts = 0;
	int connectionFailures = 0;
	std::vector<time_duration> BOMBUpdatePerformance;
	std::vector<time_duration> BOMBFunctionPerformance;
	int* id;
};

