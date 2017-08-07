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
	int getWorkerCount() { return resourceCollectors.size(); }
	BWAPI::Unit& requestUnit();
	std::string getPreamble() override { return preamble; }
	bool hasUnit(BWAPI::Unit u); //for debugging, there may be some issues with workers not being added to other managers correctly
private:
	std::vector<BWAPI::Unit> resourceCollectors;
	std::string preamble = "Resource Manager: ";
};

