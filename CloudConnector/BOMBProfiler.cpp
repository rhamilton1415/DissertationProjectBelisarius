#include "BOMBProfiler.h"



BOMBProfiler::BOMBProfiler(int* i)
{
	id = i;
}


BOMBProfiler::~BOMBProfiler()
{
	std::ofstream outfile(("profileInfo" + std::to_string(*id) + ".txt").c_str());
	outfile << "BOMB Function performance: " << std::endl;
	for (int i = 0; i < BOMBFunctionPerformance.size(); i++)
	{
		outfile << std::to_string(BOMBFunctionPerformance.at(i).total_milliseconds()) << std::endl;
	}
	outfile << std::endl;
	outfile << "BOMB Update performance" << std::endl;
	for (int i = 0; i < BOMBUpdatePerformance.size(); i++)
	{
		outfile << std::to_string(BOMBUpdatePerformance.at(i).total_milliseconds()) << std::endl;
	}
	outfile << std::endl;
	outfile << "Total Connection Attempts: " + std::to_string(connectionAttempts) << std::endl;
	outfile << "Total Connection Failures: " + std::to_string(connectionFailures) << std::endl;
	outfile.close();
}
void BOMBProfiler::BOMBFunctionStart()
{
	BOMBFunctionTimer = ptime(microsec_clock::universal_time());
}
void BOMBProfiler::BOMBFunctionFinish()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - BOMBFunctionTimer;
	BOMBFunctionPerformance.push_back(duration);
}
void BOMBProfiler::BOMBUpdateStart()
{
	BOMBUpdateTimer = ptime(microsec_clock::universal_time());
}
void BOMBProfiler::BOMBUpdateFinish()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - BOMBUpdateTimer;
	BOMBUpdatePerformance.push_back(duration);
}
