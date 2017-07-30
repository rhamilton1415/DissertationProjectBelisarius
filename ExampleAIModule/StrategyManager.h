#pragma once
#include "Agent.h"
#include "TacticalManager.h"
class StrategyManager :
	public Agent
{
public:
	StrategyManager();
	~StrategyManager();
	void onFrame();
	void addUnit(BWAPI::Unit u);
	void onUnitDestroyed(BWAPI::Unit u);
	void onUnitDiscovered(BWAPI::Unit u);
	std::string getPreamble() override { return preamble; }
private:
	std::string preamble = "Strategy Manager: ";
	std::vector<BWAPI::Unit> combatUnits;
	TacticalManager homeGuard = TacticalManager("Home Guard");
	TacticalManager scouting = TacticalManager("Scouting");
	TacticalManager offensiveOperations = TacticalManager("Offencive Operations");
};

