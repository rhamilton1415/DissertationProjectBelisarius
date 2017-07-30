#include "StrategyManager.h"



StrategyManager::StrategyManager()
{
}


StrategyManager::~StrategyManager()
{
}

void StrategyManager::onFrame()
{

}
void StrategyManager::addUnit(BWAPI::Unit u)
{
	combatUnits.push_back(u);
}
void StrategyManager::onUnitDestroyed(BWAPI::Unit u)
{

}
void StrategyManager::onUnitDiscovered(BWAPI::Unit u)
{

}
