#pragma once
#include <vector>
#include <BWAPI.h>
#include <exception>
using namespace BWAPI;
using namespace Filter;
class Agent
{
public:
	Agent();
	~Agent();
	virtual std::string getPreamble() { return preamble; }
protected:
	void broadcast(std::string message);
	std::string preamble = "this needs to be set ";
};

