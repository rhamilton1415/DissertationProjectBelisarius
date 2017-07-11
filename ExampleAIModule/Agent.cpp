#include "Agent.h"



Agent::Agent()
{
}


Agent::~Agent()
{
}

void Agent::broadcast(std::string message)
{
	Broodwar->sendText((getPreamble() + message).c_str());
}