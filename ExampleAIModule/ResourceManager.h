#pragma once
#include "Agent.h"
class ResourceManager :
	public Agent
{
public:
	ResourceManager();
	~ResourceManager();
	void onFrame();
	void unitCreatedUpdate();
	void unitDestroyedUpdate();
	void addUnit(BWAPI::Unit u);
	int workerCount() { return resourceCollectors.size(); }
	BWAPI::Unit& requestUnit();
	std::string getPreamble() override { return preamble; }
private:
	std::vector<BWAPI::Unit> resourceCollectors;
	std::string preamble = "Resource Manager: ";
};

