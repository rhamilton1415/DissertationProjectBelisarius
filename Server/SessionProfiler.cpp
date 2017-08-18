#include "stdafx.h"
#include "SessionProfiler.h"


SessionProfiler::SessionProfiler()
{
}


SessionProfiler::~SessionProfiler()
{
}

void SessionProfiler::startUpdate()
{
	updateTimerStart = ptime(microsec_clock::universal_time());
}
void SessionProfiler::startFunction()
{
	functionTimerStart = ptime(microsec_clock::universal_time());
}
void SessionProfiler::finishUpdate()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - updateTimerStart;
	updateTimes.push_back(std::pair<long,ptime>(duration.total_milliseconds(), updateTimerStart));
}
void SessionProfiler::finishFunction()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - functionTimerStart;
	functionTimes.push_back(std::pair<long,ptime>(duration.total_milliseconds(),functionTimerStart));
}
long SessionProfiler::getAverageUpdateTime()
{
	if (updateTimes.size() != 0)
	{
		long total = 0;
		int count = updateTimes.size();
		for (int i = 0; i < count; i++)
		{
			total += updateTimes.at(i).first;
		}
		return total / count;
	}
	else
	{
		return 0;
	}
}
long SessionProfiler::getAverageFunctionTime()
{
	if (functionTimes.size() != 0)
	{
		long total = 0;
		int count = functionTimes.size();
		for (int i = 0; i < count; i++)
		{
			total += functionTimes.at(i).first;
		}
		return total / count;
	}
	else
	{
		return 0;
	}
}
long SessionProfiler::getUpdateOperationsPerMinute()
{
	ptime earliest = ptime(microsec_clock::universal_time());
	for (int i = 0; i < updateTimes.size(); i++)
	{
		if (updateTimes.at(i).second < earliest)
		{
			earliest = updateTimes.at(i).second;
		}
	}
	time_duration duration = ptime(microsec_clock::universal_time()) - earliest; //The time range of operations
	if (duration.total_milliseconds() == 0)
	{
		return 0;
	}
	long operationsPerMillisecond = updateTimes.size() / duration.total_milliseconds(); //The number of operations divided by the time
	return operationsPerMillisecond * 1000 * 60;
}
long SessionProfiler::getFunctionOperationsPerMinute()
{
	ptime earliest = ptime(microsec_clock::universal_time());
	for (int i = 0; i < functionTimes.size(); i++)
	{
		if (functionTimes.at(i).second < earliest)
		{
			earliest = functionTimes.at(i).second;
		}
	}
	time_duration duration = ptime(microsec_clock::universal_time()) - earliest; //The time range of operations
	if (duration.total_milliseconds() == 0)
	{
		return 0;
	}
	long operationsPerMillisecond = functionTimes.size() / duration.total_milliseconds(); //The number of operations divided by the time
	return operationsPerMillisecond * 1000 * 60;
}