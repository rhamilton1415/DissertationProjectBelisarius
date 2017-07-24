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
	if (nextOrder == NULL)
	{
		nextOrder = getNextBuildRecommendation(); //don't spam it I guess
		broadcast(nextOrder.getName() + " has been queued");
	}
	if (Broodwar->self()->minerals() >= nextOrder.mineralPrice())
	{
		if (nextOrder == BWAPI::UnitTypes::Buildings) //The construction manager will deal with it
		{
			cRef->addBuildOrder(nextOrder);
			nextOrder = NULL; //reset the flag
		}
		else //The building manager will deal with it
		{
			bRef->addBuildOrder(nextOrder);
			nextOrder = NULL;
		}
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
	if (bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Barracks) > 0)
	{
		combatUnitRecommendation = BWAPI::UnitTypes::Terran_Marine;
		combatUnitPriority = 0.4;
	}
	else
	{
		combatUnitPriority = 0;
	}

	//Calculate the priority for expanding the infrastructure
	BWAPI::UnitType buildingRecommendation;
	double buildingPriority;
	//If we don't have a refinery, that'll be the only building in consideration
	if (bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Refinery) < 1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Refinery;
		buildingPriority = 0.5;
	}
	else if(bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Barracks) <1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Barracks;
		buildingPriority = 0.5;
	}
	else
	{
		buildingPriority = 0;
	}
	if (buildingPriority > combatUnitPriority && buildingPriority > workerUnitPriority)
	{
		return buildingRecommendation;
	}
	else if (combatUnitPriority > workerUnitPriority)
	{
		return combatUnitRecommendation;
	}
	else
	{
		return workerRecommendation;
	}
}