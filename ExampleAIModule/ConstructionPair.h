#pragma once
#include <vector>
#include <BWAPI.h>
using namespace BWAPI;
using namespace Filter;
class ConstructionPair
{
public:
	ConstructionPair();
	ConstructionPair(BWAPI::Unit _building, BWAPI::Unit _worker);
	ConstructionPair(BWAPI::UnitType building);
	~ConstructionPair();
	void setBuilding(BWAPI::Unit _building) { building = _building; incipientBuilding = NULL; }
	void setIncipientBuilding(BWAPI::UnitType buildingType);
	void setWorker(BWAPI::Unit _worker) { worker = _worker; }
	BWAPI::Unit getBuilding() { return building; }
	BWAPI::Unit getWorker() { return worker; }
	BWAPI::UnitType getIncipientBuilding() { return incipientBuilding; }
private:
	BWAPI::Unit building;
	BWAPI::UnitType incipientBuilding = NULL;
	BWAPI::Unit worker;
};

