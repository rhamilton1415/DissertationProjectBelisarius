#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

void ResourceManager::onFrame()
{
	// Iterate through all the units that we own
	for (auto &u : resourceCollectors)
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
			// if our worker is idle
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
					if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
					{
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle

		}

	} // closure: unit iterator
}

void ResourceManager::addUnit(BWAPI::Unit u)
{
	try 
	{
		resourceCollectors.push_back(u);
	}
	catch (...)
	{
		BWAPI::Broodwar->sendText("Something went wrong");
	}
}

BWAPI::Unit& ResourceManager::requestUnit()
{
		//remove the reference from the resourceCollecters vector and then return it
		BWAPI::Unit& u = resourceCollectors.back();
		resourceCollectors.erase(std::find(resourceCollectors.begin(), resourceCollectors.end(), u));
		u->stop();
		return u;
}
bool ResourceManager::hasUnit(BWAPI::Unit u)
{
	return std::binary_search(resourceCollectors.begin(), resourceCollectors.end(), u);
}
