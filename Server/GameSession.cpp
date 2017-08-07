#include "stdafx.h"
#include "GameSession.h"

int GameSession::sessionCount;
GameSession GameSession::createGameSession()
{
	sessionCount++;
	return GameSession(sessionCount);
}
GameSession::GameSession(int sessionId) : id(sessionId)
{

}
GameSession::~GameSession()
{
}
void GameSession::updatePlayerState(json::value completePlayerState)
{
	playerState.clear();
	queued.clear();
	json::value playerStateJSONobj = completePlayerState[L"PlayerState"];
	json::value queuedJSONobj = completePlayerState[L"Queued"];
	for (auto iter = playerStateJSONobj.as_object().cbegin(); iter != playerStateJSONobj.as_object().cend(); iter++)
	{
		playerState[std::stoi(iter->first)] = iter->second.as_integer();
	}
	try
	{
		for (auto iter = queuedJSONobj.as_object().cbegin(); iter != queuedJSONobj.as_object().cend(); iter++)
		{
			queued[std::stoi(iter->first)] = iter->second.as_integer();
		}
	}
	catch (...)
	{
		//If there is nothing queued, the previous call will fail but it is not an erronous complete state so just continue
		//If there is no Player State, an "ex nihilo" Build Order Manager is out of scope
	}
	buildOrderManagerB();
}
BWAPI::UnitType GameSession::buildOrderManagerB()
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

	int workerCount = playerState[BWAPI::UnitTypes::Terran_SCV] + queued[BWAPI::UnitTypes::Terran_SCV];

	if (getRemainingSupply() > 0)
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
	//We need barracks and supply to make marines
	if (playerState[BWAPI::UnitTypes::Terran_Barracks] > 0 && getRemainingSupply()>0)
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
	if ((playerState[BWAPI::UnitTypes::Terran_Refinery] + queued[BWAPI::UnitTypes::Terran_Refinery]) < 1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Refinery;
		buildingPriority = 0.5;
	}
	else if (playerState[BWAPI::UnitTypes::Terran_Barracks] + queued[BWAPI::UnitTypes::Terran_Barracks] <1)
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Barracks;
		buildingPriority = 0.5;
	}
	else if (getRemainingSupply() + (queued[BWAPI::UnitTypes::Terran_Supply_Depot] * 8)) //One supply depo gives 8 supply
	{
		buildingRecommendation = BWAPI::UnitTypes::Terran_Supply_Depot;
		buildingPriority = 1 - ((getRemainingSupply() + (queued[BWAPI::UnitTypes::Terran_Supply_Depot] * 8)) / 5);
	}
	else
	{
		buildingPriority = 0;
	}
	if (buildingPriority > combatUnitPriority && buildingPriority > workerUnitPriority)
	{
		lastQueued = buildingRecommendation;
		return buildingRecommendation;
	}
	else if (combatUnitPriority > workerUnitPriority)
	{
		lastQueued = combatUnitRecommendation;
		return combatUnitRecommendation;
	}
	else
	{
		lastQueued = workerRecommendation;
		return workerRecommendation;
	}
}
int GameSession::getRemainingSupply()
{
	int supply = 0;
	for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
	{
		//Record supply for every unit that provides supply
		supply += (it->first.supplyProvided() * playerState.at(it->first));
		//Record supply usage for every unit requiring supply
		supply -= (it->first.supplyRequired() * playerState.at(it->first));
	}
	for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
	{
		//Record supply for every unit that provides supply
		supply += (it->first.supplyProvided() * queued.at(it->first));
		//Record supply usage for every unit requiring supply
		supply -= (it->first.supplyRequired() * queued.at(it->first));
	}
	return supply;
}
void GameSession::printGameSession()
{
	try
	{
		attron(A_BOLD);
		printw("Player State:\n");
		attroff(A_BOLD);
		for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
		{
			printw(" ");
			printw(((it->first.getName()) + " " + std::to_string(playerState.at(it->first))).c_str());
			printw("\n");
		}
		attron(A_BOLD);
		printw("Queued: \n");
		attroff(A_BOLD);
		for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
		{
			printw(" ");
			printw(((it->first.getName()) + " " + std::to_string(queued.at(it->first))).c_str());
			printw("\n");
		}
		attron(A_BOLD);
		printw("Last Order: \n");
		attroff(A_BOLD);
		printw(" ");
		printw(lastQueued.getName().c_str());
	}
	catch (const std::exception& e)
	{
	}
}
