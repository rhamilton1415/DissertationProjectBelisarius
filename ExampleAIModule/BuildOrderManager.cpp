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
	int remainingSupply = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();
	if (nextOrder == BWAPI::UnitTypes::None) //TODO Murder the person who set NULL to mean something in this namespace
	{
		nextOrder = getNextBuildRecommendation();
	}
	else //if (Broodwar->self()->minerals() >= nextOrder.mineralPrice())
	{
		if (nextOrder.isBuilding()) //The construction manager will deal with it also don't spam
		{
			if (Broodwar->self()->minerals() >= (nextOrder.mineralPrice() + bRef->getMineralDebt() + cRef->getMineralDebt()))
			{
				cRef->addBuildOrder(nextOrder);
				broadcast(nextOrder.getName() + " has been queued (Building)");
				nextOrder = BWAPI::UnitTypes::None; //reset the flag
			}
		}
		//It's a unit and therefore should be sent to the building manager for training
		//Make sure we have both enough minerals and gas
		else if(Broodwar->self()->minerals() >= (nextOrder.mineralPrice() + bRef->getMineralDebt() + cRef->getMineralDebt()))
		{
			if (Broodwar->self()->gas() >= (nextOrder.gasPrice() + bRef->getGasDebt()))
			{
				//Make sure we have enough supply as well - the BOM algorithm considers incipient supply as actual supply
				if (nextOrder.supplyRequired() <= remainingSupply)
				{
					bRef->addBuildOrder(nextOrder);
					broadcast(nextOrder.getName() + " has been queued (Unit)");
					nextOrder = BWAPI::UnitTypes::None;
				}
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

	//Calculate the priority for expanding the combat capabilities
	BWAPI::UnitType combatUnitRecommendation;
	double combatUnitPriority;

	//Calculate the priority for expanding the infrastructure
	BWAPI::UnitType buildingRecommendation;
	double buildingPriority;

	int remainingSupply = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();
	int workerCount = rRef->getWorkerCount() + cRef->getWorkerCount();

	if (remainingSupply > 0)
	{
		workerUnitPriority = (1.0 - (workerCount / 10.0));
		if (workerUnitPriority <= 0)
		{
			workerUnitPriority = 0.1;
		}
	}
	else
	{
		workerUnitPriority = 0;
	}

	if (bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Barracks) > 0 && remainingSupply>0)
	{
		combatUnitRecommendation = BWAPI::UnitTypes::Terran_Marine;
		combatUnitPriority = 0.4;
	}
	else
	{
		combatUnitPriority = 0;
	}
	
	//When checking to see what buildings we have, check the construction manager for queued buildings as well
	//If we don't have a refinery, that'll be the only building in consideration.
	if ((bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Refinery)+cRef->orderCount(BWAPI::UnitTypes::Terran_Refinery)) < 1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Refinery;
		buildingPriority = 0.5;
	}
	else if(bRef->getBuildingCount(BWAPI::UnitTypes::Terran_Barracks)+cRef->orderCount(BWAPI::UnitTypes::Terran_Barracks) <1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Barracks;
		buildingPriority = 0.5;
	}
	else if(remainingSupply + (cRef->orderCount(BWAPI::UnitTypes::Terran_Supply_Depot)*8)) //One supply depo gives 8 supply
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Supply_Depot;
		buildingPriority = 1 - ((remainingSupply + (cRef->orderCount(BWAPI::UnitTypes::Terran_Supply_Depot) * 8)) / 5);
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
void BuildOrderManager::buildPlayerState()
{
	try
	{
		queued.clear();
		playerState.clear();
		for (auto & u : Broodwar->self()->getUnits())
		{
			if (u->isCompleted())
			{
				playerState[u->getType()] += 1;
			}
			else
			{
				queued[u->getType()] += 1;
			}
		}
		for (auto & u : cRef->getBuildOrders()) //All the buildings in the construction queue which haven't been started yet
		{
			queued[u] += 1;
		}
		for (auto& u : bRef->getBuildOrders()) //All the Units in the training queue which haven't been paid for yet
		{
			queued[u] += 1;
		}
	}
	catch (...)
	{
		Broodwar->drawTextScreen(10, 10, "Building the playerState is what broke it");
	}
	
}
void BuildOrderManager::printPlayerState()
{
	buildPlayerState();
	try 
	{
		int count = 0;
		Broodwar->drawTextScreen(10, 20 + (count * 10), std::string("Available:").c_str());
		count++;
		for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
		{
			Broodwar->drawTextScreen(15, 20 + (count * 10), (it->first.getName() + " " + std::to_string(playerState.at(it->first))).c_str());
			count++;
		}
		count++;
		Broodwar->drawTextScreen(10, 20 + (count * 10), std::string("Queued:").c_str());
		count++;
		for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
		{
			Broodwar->drawTextScreen(15, 20 + (count * 10), (it->first.getName() + " " + std::to_string(queued.at(it->first))).c_str());
			count++;
		}
		count++;
		Broodwar->drawTextScreen(10, 20 + (count * 10), std::string("Next Order:").c_str());
		count++;
		Broodwar->drawTextScreen(15, 20 + (count * 10), (nextOrder.getName()).c_str());
		count++;

		/*char buffer[MAX_PATH]; Prints the local directory
		GetModuleFileName(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		Broodwar->drawTextScreen(10, 20 + (count * 10), std::string(buffer).substr(0, pos).c_str());*/
	}
	catch (std::exception& e)
	{
		Broodwar->drawTextScreen(10,10, e.what());
	}
	connectToBOMB();
}
void BuildOrderManager::connectToBOMB()
{
	c.updateState(queued, playerState);
}