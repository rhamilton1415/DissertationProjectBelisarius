#include "AgentProfile.h"



AgentProfile::AgentProfile(Agent& agent,int APHistorySize) : historySize(APHistorySize)
{
	a = &agent;
}


AgentProfile::~AgentProfile()
{
}

void AgentProfile::start()
{
	frameCount++;
	startTime = ptime(microsec_clock::universal_time());
}
void AgentProfile::finish()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - startTime;
	performanceHistory.push_back(duration.total_microseconds());
	//It should be impossible for performance history to be more than one value over the size cap
	if (performanceHistory.size() > historySize)
	{
		performanceHistory.erase(performanceHistory.begin());
	}
}
std::vector<long> AgentProfile::getPerformanceHistory()
{
	return performanceHistory;
}
long AgentProfile::getAveragePerformance()
{
	if (performanceHistory.size() == 0)
	{
		return 0.0;
	}
	long total = 0.0;
	int count = performanceHistory.size();
	for (int i = 0; i < count; i++)
	{
		total += performanceHistory.at(i);
	}
	return total / (long)count;
}


