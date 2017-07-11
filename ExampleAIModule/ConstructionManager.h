#pragma once
#include "Agent.h"
#include "ResourceManager.h"
class ConstructionManager :
	public Agent
{
public:
	ConstructionManager(ResourceManager &r);
	~ConstructionManager();
	void onFrame();
	void unitCreatedUpdate();
	void unitDestroyedUpdate();
	void addUnit(BWAPI::Unit u);
	int workerCount() { return builders.size(); }
	void addBuildOrder(BWAPI::UnitType uT);
	void addBuildingUnderConstruction(BWAPI::Unit u);
	const std::vector<BWAPI::UnitType>& getBuildOrders() const { return buildOrders; }
	const std::map<BWAPI::Unit, BWAPI::Unit>& getBuildingsUnderConstruction() const { return buildingsUnderConstruction; }
	std::string getPreamble() override { return preamble; }
private:
	std::vector<BWAPI::Unit> builders;
	std::map<BWAPI::Unit, BWAPI::Unit> buildingsUnderConstruction; //Worker mapped to Building
	std::string preamble = "Construction Manager: ";
	std::vector<BWAPI::UnitType> buildOrders;
	ResourceManager* rRef;
	int maxWorkers = INT16_MAX; //leave open the potential for a strategic limitation
};

