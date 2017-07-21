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
	//Units constructing buildings may be killed or otherwise drafted for other tasks - the construction manager must replace them
	int unfinishedBuildings = 0;
	for (int i = 0; i < buildingsUnderConstruction.size(); i++)
	{
		if (buildingsUnderConstruction.at(i).getWorker() == NULL)
		{
			unfinishedBuildings++;
		}
	}

	//for every order and unfinished building, take a worker from the resourceManager
	//nick a builder for every buildOrder
	for (int i = 0; i < (buildOrders.size()+unfinishedBuildings) - builders.size(); i++)
	{
		if (buildOrders.size() == 0)
		{
			continue;
		}
		builders.push_back(rRef->requestUnit());
		Broodwar->sendText("Construction Manager: requesting unit");
	}
	// Iterate through all the units that we own
	for (int i = 0; i < builders.size(); i++)
	{
		BWAPI::Unit& u = builders.at(i);
		for (int i = 0; i < buildingsUnderConstruction.size(); i++)
		{
			//don't find a new job for a worker who already has a job
			if (buildingsUnderConstruction.at(i).getWorker() == u)
			{
				continue;
			}
		}

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
			
			// If the worker is carrying cargo, order it to return before doing anything else
			if (u->isCarryingGas() || u->isCarryingMinerals())
			{
				u->returnCargo();
			}
			// if our worker is idle and stopped(meaning it has not yet been assigned a task by this manager as any previous task will have been cancelled)
			//second condidition because idle is liberally interpreted 
			else if (u->isIdle())
			{
				if (!u->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag
											  // Harvest from the nearest mineral patch or gas refinery
					//build something
					TilePosition buildPosition = Broodwar->getBuildLocation(buildOrders.at(i), u->getTilePosition());
					if (!u->build(buildOrders.at(i), buildPosition))
					{
						//perform some nonesense 
					}
					else
					{
						//build successful - create an incipient construction pair
						broadcast(buildOrders.front().getName() + " order being executed by " + std::to_string(u->getID()));
					}
				} // closure: has no powerup
			} // closure: if idle
		}

	} // closure: unit iterator
}

void ConstructionManager::addBuildOrder(BWAPI::UnitType uT)
{
	broadcast("Build order recieved");
	buildOrders.push_back(uT);
}
void ConstructionManager::addBuildingUnderConstruction(BWAPI::Unit building)
{
	//add the building to its worker in the "under construction list"
	//TODO Sanitise workers
	//find the linked incipient building and properly initialise it
	buildingsUnderConstruction.push_back(ConstructionPair(building, building->getBuildUnit()));

	//Remove the order from the build order list so that it won't be duplicated
	buildOrders.erase(std::find(buildOrders.begin(), buildOrders.end(), building->getType()));

	//Remove the worker from the builders list as it can't work on any new tasks
	builders.erase(std::find(builders.begin(), builders.end(), building->getBuildUnit()));

	//When the building is completed, make sure to ping this agent so that the entry in the map can be handled
	broadcast("Construction started on " + building->getType().getName());
}
void ConstructionManager::constructionCompleted(BWAPI::Unit completedBuilding)
{
	
	try
	{
		for (int i = 0; i < buildingsUnderConstruction.size(); i++) //Find the building in the list so it can be removed
		{
			if (buildingsUnderConstruction.at(i).getBuilding() == completedBuilding)
			{
				//The worker may automatically pick up a new task; stop this
				buildingsUnderConstruction.at(i).getWorker()->stop();
				//add the builder back into the pool
				builders.push_back(buildingsUnderConstruction.at(i).getWorker());
				//remove the construction pair as it is now finished
				buildingsUnderConstruction.erase(buildingsUnderConstruction.begin() + i);
				broadcast("Building completed, worker added back into operating pool");
				return;
			}
		}
	}
	catch (...)
	{
		broadcast("Something went wrong...");
	}
}
int ConstructionManager::orderCount()
{
	return buildOrders.size() + buildingsUnderConstruction.size(); //The buildings being built and the buildings waiting to be built
}
int ConstructionManager::orderCount(BWAPI::UnitType specificUnitType)
{
	int count = 0;
	for (auto& uT : buildOrders)
	{
		if (uT == specificUnitType)
		{
			count++;
		}
	}
	for (auto& uT : buildingsUnderConstruction)
	{
		if (uT.getBuilding()->getType() == specificUnitType)
		{
			count++;
		}
	}
	return count;
}
void ConstructionManager::unitDestroyedUpdate()
{

}