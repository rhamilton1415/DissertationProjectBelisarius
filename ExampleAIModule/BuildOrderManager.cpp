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
	}
	else //if (Broodwar->self()->minerals() >= nextOrder.mineralPrice())
	{
		if (nextOrder.isBuilding()) //The construction manager will deal with it also don't spam
		{
			if (Broodwar->self()->minerals() >= (nextOrder.mineralPrice() + bRef->getMineralDebt() + cRef->getMineralDebt()))
			{
				cRef->addBuildOrder(nextOrder);
				broadcast(nextOrder.getName() + " has been queued (Building)");
				nextOrder = NULL; //reset the flag
			}
		}
		//It's a unit and therefore should be sent to the building manager for training
		//Make sure we have both enough minerals and gas
		else if(Broodwar->self()->minerals() >= (nextOrder.mineralPrice() + bRef->getMineralDebt() + cRef->getMineralDebt()))
		{
			if (Broodwar->self()->gas() >= (nextOrder.gasPrice() + bRef->getGasDebt()))
			{
				bRef->addBuildOrder(nextOrder);
				broadcast(nextOrder.getName() + " has been queued (Unit)");
				nextOrder = NULL;
			}
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
	workerUnitPriority = (1.0 - (workerCount / 10.0));

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
	//When checking to see what buildings we have, check the construction manager for queued buildings as well
	BWAPI::UnitType buildingRecommendation;
	double buildingPriority;
	//If we don't have a refinery, that'll be the only building in consideration.
	if ((bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Refinery)+cRef->orderCount(BWAPI::UnitTypes::Terran_Refinery)) < 1)
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