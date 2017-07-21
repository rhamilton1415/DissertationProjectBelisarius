#include "ConstructionPair.h"



ConstructionPair::ConstructionPair()
{
}
ConstructionPair::ConstructionPair(BWAPI::Unit _building, BWAPI::Unit _worker)
{
	building = _building;
	worker = _worker;
}
ConstructionPair::ConstructionPair(BWAPI::UnitType building)
{
	incipientBuilding = building;
}
ConstructionPair::~ConstructionPair()
{
}
void ConstructionPair::setIncipientBuilding(BWAPI::UnitType building)
{
	incipientBuilding = building;
}