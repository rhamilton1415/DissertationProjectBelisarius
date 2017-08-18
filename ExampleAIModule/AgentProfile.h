#pragma once
#include "Agent.h";
#include "boost/date_time/posix_time/posix_time_types.hpp" //no i/o just types
using namespace boost::posix_time;
class AgentProfile
{
public:
	AgentProfile(Agent& agent, int APHistorySize);
	~AgentProfile();
	void start();
	void finish();
	Agent* getAgent(){ return a; }
	std::vector<long> getPerformanceHistory();
	long getAveragePerformance();
	int getFrameCount() { return frameCount; }
private:
	Agent* a;
	std::vector<long> performanceHistory;
	const int historySize;
	ptime startTime;
	int frameCount = 0;
};

