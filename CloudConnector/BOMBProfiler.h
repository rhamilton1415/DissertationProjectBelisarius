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
	void BOMBEstablishStart();
	void BOMBEstablishFinish();
	void addConnectionAttempt() { connectionAttempts++; }
	void addConnectionFailure() { connectionFailures++; }
	void addNetowrkIn(int s) { networkIn.push_back(s); }
	void addNetworkOut(int s) { networkOut.push_back(s); }

private:
	ptime BOMBUpdateTimer;
	ptime BOMBFunctionTimer;
	ptime BOMBEstablishTimer;
	int connectionAttempts = 0;
	int connectionFailures = 0;
	std::vector<time_duration> BOMBUpdatePerformance;
	std::vector<time_duration> BOMBFunctionPerformance;
	std::vector<time_duration> BOMBEstablishPerformance;
	std::vector<int> networkIn;
	std::vector<int> networkOut;
	int* id;
};

