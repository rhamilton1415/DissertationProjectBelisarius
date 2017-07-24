#pragma once
#include "Agent.h"
#include "ResourceManager.h"

//does things with buildings
class BuildingManager :
	public Agent
{
public:
	BuildingManager(ResourceManager &r);
	~BuildingManager();
	void onFrame();
	void unitCreatedUpdate();
	void unitDestroyedUpdate();
	void addUnit(BWAPI::Unit u);
	void addBuildOrder(BWAPI::UnitType uT);
	int getOrders(BWAPI::UnitType orderType);
	int getOrders();
	int getBuildingCount() { return buildings.size(); }
	int getBuildingCount(BWAPI::UnitType specificType);
	const std::vector<BWAPI::UnitType>& getBuildOrders() const { return buildOrders; }
	std::string getPreamble() override { return preamble; }
	int getMineralDebt(); //The amount of minerals the build order queue will cost to execute
private:
	std::vector<BWAPI::Unit> buildings;
	std::string preamble = "Building Manager: ";
	ResourceManager* rRef;
	std::vector<BWAPI::UnitType> buildOrders;
};

