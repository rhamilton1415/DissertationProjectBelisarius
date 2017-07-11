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
	int getBuildingCount() { return buildings.size(); }
	const std::vector<BWAPI::UnitType>& getBuildOrders() const { return buildOrders; }
	std::string getPreamble() override { return preamble; }
private:
	std::vector<BWAPI::Unit> buildings;
	std::string preamble = "Building Manager: ";
	ResourceManager* rRef;
	std::vector<BWAPI::UnitType> buildOrders;
};

