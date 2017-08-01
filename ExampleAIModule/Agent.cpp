#include "Agent.h"      // Asynchronous streams

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
void Agent::connect()
{
	pplx::task<std::string> t([]()
	{
		try
		{
			
		}
		catch (...)
		{

		}
		return "tasky";
	});
	broadcast(t.get());
	return;
}
