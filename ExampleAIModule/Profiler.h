#pragma once
#include "Agent.h";
#include "AgentProfile.h";
#include <iostream>
#include <fstream> 
#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types
using namespace boost::posix_time;
class Profiler
{
public:
	Profiler();
	~Profiler();
	void addAgent(Agent& agent);
	void start();
	void finish();

	long getAverageBotPerformace();
	std::vector<long> getBotPerformanceHistory() { return botPerformanceHistory; }
	AgentProfile& getAgentProfile(Agent& identifier);
	std::vector<AgentProfile> getAgentProfiles() { return agentProfiles; }
private:
	std::vector<AgentProfile> agentProfiles;
	std::vector<long> botPerformanceHistory;
	const int historySize = 100;
	ptime startTime;
};

