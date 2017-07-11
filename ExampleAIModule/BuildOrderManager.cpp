#include "BuildOrderManager.h"



BuildOrderManager::BuildOrderManager(ResourceManager &r, BuildingManager &b, ConstructionManager &c)
{
	rRef = &r;
	bRef = &b;
	cRef = &c;
}


BuildOrderManager::~BuildOrderManager()
{
}

void BuildOrderManager::onFrame()
{
	if (/*rRef->workerCount() < 6 && bRef->getBuildOrders().size() == 0*/false)
	{
		bRef->addBuildOrder(BWAPI::UnitTypes::Terran_SCV);
	}
	else if(Broodwar->self()->minerals() >= UnitTypes::Terran_Refinery.mineralPrice()&&cRef->getBuildOrders().size()==0) //if we have enough minerals
	{
		cRef->addBuildOrder(BWAPI::UnitTypes::Terran_Refinery);
	}
}