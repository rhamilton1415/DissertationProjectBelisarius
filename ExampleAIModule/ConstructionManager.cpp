#include "ConstructionManager.h"
#include <Windows.h>


ConstructionManager::ConstructionManager(ResourceManager &r)
{
	rRef = &r;
}


ConstructionManager::~ConstructionManager()
{
}

void ConstructionManager::onFrame()
{
	//If there is nothing to do
	if (buildOrders.size() == 0 && buildingsUnderConstruction.size() == 0)
	{
		return;
	}
	//Units constructing buildings may be killed or otherwise drafted for other tasks - the construction manager must replace them
	int unfinishedBuildings = 0;
	for (int i = 0; i < buildingsUnderConstruction.size(); i++)
	{
		if (!buildingsUnderConstruction.at(i).getWorker()->exists())//if the worker has died
		{
			unfinishedBuildings++;
		}
	}

	//We should have a builder for every build order and building under construction - for every surplus order try and get more workers
	while (workerRequired())
	{
		addNewWorker();
	}
	// Iterate through all the units that we own
	for (int i = 0; i < builders.size(); i++)
	{
		BWAPI::Unit& u = builders.at(i);

		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
		{
			continue;
		}
		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
		{
			broadcast("is locked down, maelstrommed or stasised");
			continue;
		}

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
		{
			broadcast("is stuck");
			continue;
		}
		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
		{
			broadcast("isn't completed or is constructing");
			continue;
		}

		// If the worker is carrying cargo, order it to return before doing anything else
		if (u->isCarryingGas() || u->isCarryingMinerals())
		{
			u->returnCargo();
		}
		// Finally make the unit do some stuff!

		// If the unit is a worker unit
		if (u->getType().isWorker())
		{
			bool taskedFlag = false;
			for (int i = 0; i < buildingsUnderConstruction.size(); i++)
			{
				try
				{
					//don't find a new job for a worker who already has a job
					if (buildingsUnderConstruction.at(i).getWorker() == u)
					{
						//You can do it!
						if (!u->build(buildingsUnderConstruction.at(i).getBuilding()->getType(), buildingsUnderConstruction.at(i).getBuilding()->getTilePosition()))
						{
							//something went wrong
						}
						//Worker is already working
						taskedFlag = true;
						continue;
					}
					else if (!buildingsUnderConstruction.at(i).getWorker()->exists()) //If the Buildings' worker has died
					{

						if (!u->build(buildingsUnderConstruction.at(i).getBuilding()->getType(), buildingsUnderConstruction.at(i).getBuilding()->getTilePosition()))
						{
							//something went wrong
						}
						else
						{
							buildingsUnderConstruction.at(i).setWorker(u);
						}
						//Worker has been given a new task
						taskedFlag = true;
						continue;
					}
				}
				catch (...)
				{
					broadcast("Something went wrong dealing with BuildingsUnderConstruction");
				}
			}
			//If our worker already has a task, don't give him a task from the BuildOrder queue
			if (taskedFlag)
			{
				continue;
			}
			// if our worker is idle and stopped(meaning it has not yet been assigned a task by this manager as any previous task will have been cancelled)
			//second condidition because idle is liberally interpreted 
			if (u->isIdle() || u->isHoldingPosition() || u->getOrder()==BWAPI::Orders::PlayerGuard)
			{
				if (!u->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag
											  // Harvest from the nearest mineral patch or gas refinery
					//build from the buildOrder queue
					try
					{
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
					}
					catch (std::exception& e)
					{
						broadcast("Something went wrong with the BuildOrder queue: " + *e.what());
					}
				} 
			} 
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
	//builders.erase(std::find(builders.begin(), builders.end(), building->getBuildUnit()));

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
				buildingsUnderConstruction.at(i).getWorker()->stop();
				//If we no longer need the worker, hand it back to the resource manager
				if (builders.size() >(buildOrders.size() + buildingsUnderConstruction.size()))
				{
					rRef->addUnit(buildingsUnderConstruction.at(i).getWorker());
					builders.erase(std::find(builders.begin(), builders.end(), buildingsUnderConstruction.at(i).getWorker()));
					broadcast("Worker no longer required and has been returned to the resource manager");
				}
				else
				{
					broadcast("Building completed, worker added back into operating pool");
				}
				//remove the construction pair as it is now finished
				buildingsUnderConstruction.erase(buildingsUnderConstruction.begin() + i);
				
				return;
				//TODO release workers here if there are more builders than build orders
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
int ConstructionManager::getMineralDebt()
{
	int cost = 0;
	for (int i = 0; i < buildOrders.size(); i++)
	{
		cost += buildOrders.at(i).mineralPrice();
	}
	return cost;
}
void ConstructionManager::unitDestroyedUpdate()
{
	for (int i = 0; i < builders.size(); i++)
	{
		//remove any dead builders from our data structures
		if (!builders.at(i)->exists())
		{
			builders.erase(builders.begin() + i);
		}
	}
}
bool ConstructionManager::workerRequired()
{
	//if there are more tasks than we have workers
	if ((buildOrders.size() + buildingsUnderConstruction.size()) > builders.size())
	{
		//and we have fewer workers than the cap
		if (builders.size() < maxWorkers)
		{
			return true;
		}
	}
	return false;
}
//Retrieve a new worker from the resource Manager
void ConstructionManager::addNewWorker()
{
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = 0x79;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
	try
	{
		builders.push_back(rRef->requestUnit());
		broadcast("Unit recieved from Resource Manager");
	}
	catch (...)
	{
		broadcast(std::to_string(buildOrders.size()));
		broadcast(std::to_string(buildingsUnderConstruction.size()));
		broadcast(std::to_string(builders.size()));
	}
}