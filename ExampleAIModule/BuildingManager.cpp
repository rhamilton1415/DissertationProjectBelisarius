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

		//If the unit is busy, don't spam it with orders
		if (!u->isIdle())
		{
			continue;
		}

		// Finally make the unit do some stuff!
		//iterate through all the build orders until you find one which this building can make
		for (int i = 0; i < buildOrders.size(); i++)
		{
			if (u->canBuild(buildOrders.at(i))) //If the current building is able to make the build order
			{
				//get the building to train the build order
				if (!u->train(buildOrders.at(i)))
				{
					// If that fails, draw the error at the location so that you can visibly see what went wrong!
					// However, drawing the error once will only appear for a single frame
					// so create an event that keeps it on the screen for some frames
					Position pos = u->getPosition();
					Error lastErr = Broodwar->getLastError();
					Broodwar->registerEvent([pos, lastErr](Game*) { Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
						nullptr,    // condition
						Broodwar->getLatencyFrames());  // frames to run

														// Retrieve the supply provider type in the case that we have run out of supplies
					UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
					static int lastChecked = 0;

					// If we are supply blocked and haven't tried constructing more recently
					if (lastErr == Errors::Insufficient_Supply &&
						lastChecked + 400 < Broodwar->getFrameCount() &&
						Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
					{
						lastChecked = Broodwar->getFrameCount();

						// Retrieve a unit that is capable of constructing the supply needed
						Unit supplyBuilder = u->getClosestUnit(GetType == supplyProviderType.whatBuilds().first &&
							(IsIdle || IsGatheringMinerals) &&
							IsOwned);
						// If a unit was found
						if (supplyBuilder)
						{
							if (supplyProviderType.isBuilding())
							{
								TilePosition targetBuildLocation = Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
								if (targetBuildLocation)
								{
									// Register an event that draws the target build location
									Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
									{
										Broodwar->drawBoxMap(Position(targetBuildLocation),
											Position(targetBuildLocation + supplyProviderType.tileSize()),
											Colors::Blue);
									},
										nullptr,  // condition
										supplyProviderType.buildTime() + 100);  // frames to run

																				// Order the builder to construct the supply structure
									supplyBuilder->build(supplyProviderType, targetBuildLocation);
								}
							}
							else
							{
								// Train the supply provider (Overlord) if the provider is not a structure
								supplyBuilder->train(supplyProviderType);
							}
						} // closure: supplyBuilder is valid
					} // closure: insufficient supply
				} // closure: failed to train idle unit
				else
				{
					buildOrders.erase((buildOrders.begin()+i)); //erase the build order as it is already being constructed
					continue;
				}
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
		BWAPI::Broodwar->sendText("hi");
	}
}
void BuildingManager::addBuildOrder(BWAPI::UnitType uT)
{
	//TODO add check to see if this is a valid build order
	//Although the BOM should do this too
	buildOrders.push_back(uT);
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