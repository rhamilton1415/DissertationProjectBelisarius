#pragma once
#include <BWAPI.h>
//#include <BWTA.h>
#include "..\ResourceManager.h"
#include "..\BuildingManager.h"
#include "..\ConstructionManager.h"
#include "..\BuildOrderManager.h"
// Remember not to use "Broodwar" in any global class constructor!

class Belisarius : public BWAPI::AIModule
{
public:
  // Virtual functions for callbacks, leave these as they are.
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onSendText(std::string text);
  virtual void onReceiveText(BWAPI::Player player, std::string text);
  virtual void onPlayerLeft(BWAPI::Player player);
  virtual void onNukeDetect(BWAPI::Position target);
  virtual void onUnitDiscover(BWAPI::Unit unit);
  virtual void onUnitEvade(BWAPI::Unit unit);
  virtual void onUnitShow(BWAPI::Unit unit);
  virtual void onUnitHide(BWAPI::Unit unit);
  virtual void onUnitCreate(BWAPI::Unit unit);
  virtual void onUnitDestroy(BWAPI::Unit unit);
  virtual void onUnitMorph(BWAPI::Unit unit);
  virtual void onUnitRenegade(BWAPI::Unit unit);
  virtual void onSaveGame(std::string gameName);
  virtual void onUnitComplete(BWAPI::Unit unit);
  // Everything below this line is safe to modify.

  //set up the managers
  ResourceManager r;
  BuildingManager b = BuildingManager(r);
  ConstructionManager c = ConstructionManager(r);
  BuildOrderManager bOM = BuildOrderManager(r, b, c);
};
