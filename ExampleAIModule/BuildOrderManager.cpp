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
	else if((Broodwar->self()->minerals() >= UnitTypes::Terran_Barracks.mineralPrice()*2) && (cRef->orderCount()==0) && (bRef->getBuildingCount(UnitTypes::Terran_Refinery)==0)) //if we have enough minerals and no current orders
	{
		cRef->addBuildOrder(BWAPI::UnitTypes::Terran_Barracks);
		cRef->addBuildOrder(BWAPI::UnitTypes::Terran_Barracks);
	}
}

BWAPI::UnitType BuildOrderManager::getNextBuildRecommendation()
{
	//Priorities should be normalized
	//Calculate the priority for building a new worker
	BWAPI::UnitType workerRecommendation = BWAPI::UnitTypes::Terran_SCV;
	double workerUnitPriority;
	int workerCount = rRef->getWorkerCount() + cRef->getWorkerCount();
	workerUnitPriority = 1 - (workerCount / 10);

	//Calculate the priority for expanding the combat capabilities
	BWAPI::UnitType combatUnitRecommendation;
	double combatUnitPriority;

	//Calculate the priority for expanding the infrastructure
	BWAPI::UnitType buildingRecommendation;
	double buildingPrioirty;
	//If we don't have a refinery, that'll be the only building in consideration
	if (!bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Refinery > 0))
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Refinery;
		buildingPrioirty = 0.5;
	}
	else
	{

	}
}