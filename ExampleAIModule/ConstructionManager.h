#pragma once
#include "Agent.h"
#include "ResourceManager.h"
#include "ConstructionPair.h"
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
	int getWorkerCount() { return builders.size(); }
	void addBuildOrder(BWAPI::UnitType uT);
	void addBuildingUnderConstruction(BWAPI::Unit u);
	void constructionCompleted(BWAPI::Unit u);
	int orderCount(); //How many orders are currently being processed
	int orderCount(BWAPI::UnitType specificType); //how many orders of a specific type are being processed
	const std::vector<BWAPI::UnitType>& getBuildOrders() const { return buildOrders; }
	const std::vector<ConstructionPair>& getBuildingsUnderConstruction() const { return buildingsUnderConstruction; }
	std::string getPreamble() override { return preamble; }
	int getMineralDebt(); //The amount of minerals it will cost to clear the build order queue
private:
	std::vector<BWAPI::Unit> builders;
	std::vector<ConstructionPair> buildingsUnderConstruction;

	std::string preamble = "Construction Manager: ";
	std::vector<BWAPI::UnitType> buildOrders;
	ResourceManager* rRef;
	int maxWorkers = INT16_MAX; //leave open the potential for a strategic limitation
};

