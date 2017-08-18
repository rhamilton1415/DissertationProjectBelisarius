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
	outfile << "BOMB Establish performance" << std::endl;
	for (int i = 0; i < BOMBEstablishPerformance.size(); i++)
	{
		outfile << std::to_string(BOMBEstablishPerformance.at(i).total_milliseconds()) << std::endl;
	}
	outfile << std::endl;
	outfile << "Total Connection Attempts: " + std::to_string(connectionAttempts) << std::endl;
	outfile << "Total Connection Failures: " + std::to_string(connectionFailures) << std::endl;
	outfile << std::endl;
	outfile << "Network in" << std::endl;
	for (int i = 0; i < networkIn.size(); i++)
	{
		outfile << std::to_string(networkIn.at(i)) << std::endl;
	}
	outfile << std::endl;
	outfile << "Network out" << std::endl;
	for (int i = 0; i < networkOut.size(); i++)
	{
		outfile << std::to_string(networkOut.at(i)) << std::endl;
	}
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
void BOMBProfiler::BOMBEstablishStart()
{
	BOMBEstablishTimer = ptime(microsec_clock::universal_time());
}
void BOMBProfiler::BOMBEstablishFinish()
{
	time_duration duration = ptime(microsec_clock::universal_time()) - BOMBEstablishTimer;
	BOMBEstablishPerformance.push_back(duration);
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
