#include "BuildingManager.h"


BuildingManager::BuildingManager(ResourceManager &r)
{
	rRef = &r;
}


BuildingManager::~BuildingManager()
{
}
void BuildingManager::onFrame()
{
	//if there are no orders don't do anything
	if (buildOrders.size() == 0)
	{
		return;
	}
	// Iterate through all the units that we own
	for (auto &u : buildings)
	{
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
		{
			continue;
		}

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
		{
			continue;
		}

		//If the unit is busy, don't spam it with orders actually you can
		//if (!u->isIdle())
		//{
		//	continue;
		//}

		// Finally make the unit do some stuff!
		//iterate through all the build orders until you find one which this building can make
		for (int i = 0; i < buildOrders.size(); i++)
		{
			//get the building to train the build order
			if (!u->train(buildOrders.at(i)))
			{
				if (Broodwar->self()->minerals() < buildOrders.at(i).mineralPrice())
				{
					broadcast("Not enough minerals for the current order");
				}
				if (Broodwar->self()->gas() < buildOrders.at(i).gasPrice())
				{
					broadcast("Not enough gas for current order");
				}
				if (buildOrders.at(i).isBuilding())
				{
					broadcast("Bad Order: " + buildOrders.at(i).getName() + " cannot be built here");
				}
				if (u->getTrainingQueue().size() == 5)
				{
					broadcast("Unit cannot be queued - queue is full");
				}
			}
			else
			{
				broadcast("Training " + buildOrders.at(i).getName());
				buildOrders.erase((buildOrders.begin()+i)); //erase the build order as it is already being constructed
				continue;
			}

		}

	} // closure: unit iterator
}
void BuildingManager::addUnit(BWAPI::Unit u)
{
	try
	{
		std::string str = preamble + "Building aquired " + u->getType().getName() + " " + std::to_string(u->getID());
		//don't add duplicates, don't know why it happens but it can
		if (!std::binary_search(buildings.begin(), buildings.end(), u))
		{
			buildings.push_back(u);
		}
	}
	catch (...)
	{
		BWAPI::Broodwar->sendText("Something went wrong...");
	}
}
void BuildingManager::addBuildOrder(BWAPI::UnitType uT)
{
	//TODO add check to see if this is a valid build order
	//Although the BOM should do this too
	buildOrders.push_back(uT);
	broadcast("Build order recieved");
}
int BuildingManager::getBuildingCount(BWAPI::UnitType uT)
{
	int count = 0;
	for (auto& u : buildings)
	{
		if (u->getType() == uT)
		{
			count++;
		}
	}
	return count;
}
int BuildingManager::getOrders(BWAPI::UnitType orderType)
{
	int count = 0;
	//iterate through the build order queue
	for (int i = 0; i < buildOrders.size(); i++)
	{
		if (buildOrders.at(i) == orderType)
		{
			count++;
		}
	}
	//iterate through the buildings training queues
	for (auto& u : buildings)
	{
		for (int i = 0; i < u->getTrainingQueue().size(); i++)
		{
			if (u->getTrainingQueue().at(i) == orderType)
			{
				count++;
			}
		}
	}
}
int BuildingManager::getOrders()
{
	int count = buildOrders.size();
	//iterate through the buildings training queues
	for (auto& u : buildings)
	{
		count += u->getTrainingQueue().size();
	}
	return count;
}
int BuildingManager::getMineralDebt()
{
	int cost = 0;
	for (int i = 0; i < buildOrders.size(); i++)
	{
		cost += buildOrders.at(i).mineralPrice();
	}
	for (auto& u : buildings)
	{
		for (int i = 1; i < u->getTrainingQueue().size(); i++) //Start at one because index 0 will have already been paid for. 
		{
			cost += u->getTrainingQueue().at(i).mineralPrice();
		}
	}
	return cost;
}
int BuildingManager::getGasDebt() {
	int cost = 0;
	for (int i = 0; i < buildOrders.size(); i++)
	{
		cost += buildOrders.at(i).gasPrice();
	}
	for (auto& u : buildings)
	{
		for (int i = 1; i < u->getTrainingQueue().size(); i++) //Start at one because index 0 will have already been paid for. 
		{
			cost += u->getTrainingQueue().at(i).gasPrice();
		}
	}
	return cost;
}