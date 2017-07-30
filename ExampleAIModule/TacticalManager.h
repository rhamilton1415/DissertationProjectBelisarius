#pragma once
#include "Agent.h"
class TacticalManager :
	public Agent
{
public:
	TacticalManager(std::string name);
	~TacticalManager(); 
	void onFrame();
	void addUnit(BWAPI::Unit u);
	void onUnitDestroyed(BWAPI::Unit u);
	void onUnitDiscovered(BWAPI::Unit u);
	void react();
	std::string getPreamble() override { return preamble; }
private:
	std::string preamble;
	std::vector<BWAPI::Unit> combatUnits;
	std::vector<BWAPI::Unit> advesaries;
};

