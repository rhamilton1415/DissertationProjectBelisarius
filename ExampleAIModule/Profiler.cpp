#include "Profiler.h"



Profiler::Profiler()
{
}


Profiler::~Profiler()
{
}
void Profiler::start()
{
	startTime = ptime(microsec_clock::universal_time());
}
void Profiler::finish()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - startTime;
	botPerformanceHistory.push_back(duration.total_microseconds());
	//It should be impossible for performance history to be more than one value over the size cap
	if (botPerformanceHistory.size() > historySize)
	{
		botPerformanceHistory.erase(botPerformanceHistory.begin());
	}
}
long Profiler::getAverageBotPerformace()
{
	if (botPerformanceHistory.size() == 0)
	{
		return 0;
	}
	long total = 0;
	int count = botPerformanceHistory.size();
	for (int i = 0; i < count; i++)
	{
		total += botPerformanceHistory.at(i);
	}
	return (total / (long)count);
}
void Profiler::addAgent(Agent& a)
{
	try
	{
		getAgentProfile(a);
	}
	catch (int i)
	{
		//messy but if getAgentProfile throws -1 it means a doesn't already have a profile for this agent
		if (i = -1)
		{
			agentProfiles.push_back(AgentProfile(a,historySize));
		}
	}
}
AgentProfile& Profiler::getAgentProfile(Agent& id)
{
	for (auto & u : agentProfiles)
	{
		//Return on address match
		if (u.getAgent() == &id)
		{
			return u;
		}
	}
	throw - 1;
}
