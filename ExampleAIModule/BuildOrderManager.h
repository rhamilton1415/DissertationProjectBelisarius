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
private:
	ResourceManager* rRef;
	BuildingManager* bRef;
	ConstructionManager* cRef;
	std::string preamble = "Build Order Manager: ";
	BWAPI::UnitType nextOrder = NULL;
	BWAPI::UnitType getNextBuildRecommendation();
};

