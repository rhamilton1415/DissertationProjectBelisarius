#include "Belisarius.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

void Belisarius::onStart()
{
	//BWTA::readMap(); pls update

  // Print the map name.
  // BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
  Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
  Broodwar << "Project Belisarius AI" << std::endl;

  // Enable the UserInput flag, which allows us to control the bot and type messages.
  Broodwar->enableFlag(Flag::UserInput);

  // Uncomment the following line and the bot will know about everything through the fog of war (cheat).
  //Broodwar->enableFlag(Flag::CompleteMapInformation);

  // Set the command optimization level so that common commands can be grouped
  // and reduce the bot's APM (Actions Per Minute).
  Broodwar->setCommandOptimizationLevel(2);

  // Check if this is a replay
  if ( Broodwar->isReplay() )
  {

    // Announce the players in the replay
    Broodwar << "The following players are in this replay:" << std::endl;
    
    // Iterate all the players in the game using a std:: iterator
    Playerset players = Broodwar->getPlayers();
    for(auto p : players)
    {
      // Only print the player if they are not an observer
      if ( !p->isObserver() )
        Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
    }

  }
  else // if this is not a replay
  {
    // Retrieve you and your enemy's races. enemy() will just return the first enemy.
    // If you wish to deal with multiple enemies then you must use enemies().
	  if (Broodwar->enemy())
	  {// First make sure there is an enemy
		  Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	  }
	  //ResourceManager *r = new ResourceManager();

  }

}

void Belisarius::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

void Belisarius::onFrame()
{
  // Called once every game frame

  // Display the game frame rate as text in the upper left area of the screen
  Broodwar->drawTextScreen(200, 0,  "FPS: %d", Broodwar->getFPS() );
  Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS() );

  //some debugging info
  Broodwar->drawTextScreen(10, 0, "Project Belisarius");
  
  Broodwar->drawTextScreen(10, 20, ("Resource Manager: " + std::to_string(r.getWorkerCount())).c_str());
  Broodwar->drawTextScreen(10, 40, ("Building Manager: " + std::to_string(b.getBuildingCount())).c_str());
  Broodwar->drawTextScreen(10, 60, ("Construction Manager: "+ std::to_string(c.getWorkerCount()) + " orders:  " + std::to_string(c.orderCount())).c_str());
  
  // Return if the game is a replay or is paused
  if ( Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() )
    return;

  // Prevent spamming by only running our onFrame once every number of latency frames.
  // Latency frames are the number of frames before commands are processed.
  if ( Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0 )
    return;

  r.onFrame(); //try to only ping top level agents from here
  b.onFrame();
  bOM.onFrame();
  c.onFrame();
}

void Belisarius::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void Belisarius::onReceiveText(BWAPI::Player player, std::string text)
{
  // Parse the received text
  Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void Belisarius::onPlayerLeft(BWAPI::Player player)
{
  // Interact verbally with the other players in the game by
  // announcing that the other player has left.
  Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void Belisarius::onNukeDetect(BWAPI::Position target)
{

  // Check if the target is a valid position
  if ( target )
  {
    // if so, print the location of the nuclear strike target
    Broodwar << "Nuclear Launch Detected at " << target << std::endl;
  }
  else 
  {
    // Otherwise, ask other players where the nuke is!
    Broodwar->sendText("Where's the nuke?");
  }

  // You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void Belisarius::onUnitDiscover(BWAPI::Unit unit)
{
}

void Belisarius::onUnitEvade(BWAPI::Unit unit)
{
}

void Belisarius::onUnitShow(BWAPI::Unit unit)
{
}

void Belisarius::onUnitHide(BWAPI::Unit unit)
{
}

void Belisarius::onUnitCreate(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
  else //in future ping all the top level agents with this so that they can decide where to put them
  {
	  
	  //If it's a worker, assing it to the resource collector by default, as workers should by default be resource collecting
	  //If other services want workers they should request them from the resourceManager
	  if (unit->getType().isWorker())
	  {
		  r.addUnit(unit);
	  }
	  else if (unit->getType().isBuilding() && (unit->getPlayer() == Broodwar->self())) //If it's a building, give it to the building manager
	  {
		  b.addUnit(unit);
	  }
	  else if (unit->getPlayer()==Broodwar->self() && unit->isBeingConstructed())
	  {
		  
	  }
  }
}

void Belisarius::onUnitDestroy(BWAPI::Unit unit)
{
}

//This fires when a building begins construction
void Belisarius::onUnitMorph(BWAPI::Unit unit)
{
  if ( Broodwar->isReplay() )
  {
    // if we are in a replay, then we will print out the build order of the structures
    if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
    {
      int seconds = Broodwar->getFrameCount()/24;
      int minutes = seconds/60;
      seconds %= 60;
      Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
    }
  }
  if (unit->getPlayer() == Broodwar->self())
  {
	  c.addBuildingUnderConstruction(unit);
  }
}

void Belisarius::onUnitRenegade(BWAPI::Unit unit)
{
}

void Belisarius::onSaveGame(std::string gameName)
{
  Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void Belisarius::onUnitComplete(BWAPI::Unit unit)
{
	if (unit->getType().isBuilding() && (unit->getPlayer() == Broodwar->self())) //If it's a building, give it to the building manager
	{
		c.constructionCompleted(unit);
		b.addUnit(unit);
	}
}
