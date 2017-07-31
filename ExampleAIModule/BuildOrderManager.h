#pragma once
#include "Agent.h"
#include "ResourceManager.h"
#include "ConstructionManager.h"
#include "BuildingManager.h"

class BuildOrderManager :
	public Agent
{
public:
	BuildOrderManager(ResourceManager &r, BuildingManager &b, ConstructionManager &c);
	~BuildOrderManager();
	void onFrame();
	std::string getPreamble() override { return preamble; }
	void printPlayerState();
private:
	ResourceManager* rRef;
	BuildingManager* bRef;
	ConstructionManager* cRef;
	std::string preamble = "Build Order Manager: ";
	BWAPI::UnitType nextOrder = BWAPI::UnitTypes::None;
	BWAPI::UnitType getNextBuildRecommendation();
	std::map<BWAPI::UnitType, int> getPlayerState();
};

