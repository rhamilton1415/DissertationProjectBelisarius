#include "ConstructionManager.h"



ConstructionManager::ConstructionManager(ResourceManager &r)
{
	rRef = &r;
}


ConstructionManager::~ConstructionManager()
{
}

void ConstructionManager::onFrame()
{
	if (buildOrders.size() == 0)
	{
		//If there are no buildings queued, return all the workers to the resource manager
		for (int i = 0; i < builders.size(); i++)
		{
			if (!builders.at(i)->isConstructing()) //If it has finished its job
			{
				rRef->addUnit(builders.at(i));
				builders.erase(builders.begin() + i);
				Broodwar->sendText("Construction Manager: returning Unit");
			}
		}
		return;
	}
	//for every order, take a worker from the resourceManager
	for (int i = 0; i < buildOrders.size()-builders.size(); i++)
	{
		builders.push_back(rRef->requestUnit());
		builders.back()->stop(); //cancel its current orders, making the worker idle
		Broodwar->sendText("Construction Manager: requesting unit");
	}
	// Iterate through all the units that we own
	for (auto &u : builders)
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
			continue;

		
		// Finally make the unit do some stuff!


		// If the unit is a worker unit
		if (u->getType().isWorker())
		{
			// if our worker is idle (meaning it has not yet been assigned a task by this manager as any previous task will have been cancelled)
			if (u->isIdle())
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (u->isCarryingGas() || u->isCarryingMinerals())
				{
					u->returnCargo();
				}
				else if (!u->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag
											  // Harvest from the nearest mineral patch or gas refinery
					//build something
					//TODO replace hardcoded refinery with the build order
					TilePosition buildPosition = Broodwar->getBuildLocation(buildOrders.front(), u->getTilePosition());
					if (!u->build(buildOrders.front(), buildPosition))
					{
						//perform some nonesense 
					}
					else
					{
						//build successful
						Broodwar->sendText("Building being built");
					}
				} // closure: has no powerup
			} // closure: if idle
			if (!u->getBuildUnit() == NULL)
			{
				//Broodwar->sendText(u->getBuildUnit()->getType().getName().c_str());
			}
		}

	} // closure: unit iterator
}

void ConstructionManager::addBuildOrder(BWAPI::UnitType uT)
{
	Broodwar->sendText("Construction Manager: Build order recieved");
	buildOrders.push_back(uT);
}
void ConstructionManager::addBuildingUnderConstruction(BWAPI::Unit building)
{
	//add the building and its worker to the "under construction list"
	buildingsUnderConstruction[building->getBuildUnit()] = building;
	//Remove the order from the build order list so that it won't be duplicated
	buildOrders.erase(std::find(buildOrders.begin(),buildOrders.end(),building->getType()));
	//Remove the worker from the builders list as it can't work on any new tasks
	builders.erase(std::find(builders.begin(),builders.end(),building->getBuildUnit()));
	//When the building is completed, make sure to ping this agent so that the entry in the map can be handled
	broadcast("HI");
}