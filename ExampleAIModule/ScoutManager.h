#pragma once
#include "Agent.h"
class ScoutManager :
	public Agent
{
public:
	ScoutManager();
	~ScoutManager();
	std::string getPreamble() override { return preamble; }
};

