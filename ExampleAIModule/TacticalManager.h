#pragma once
#include "Agent.h"
class TacticalManager :
	public Agent
{
public:
	TacticalManager();
	~TacticalManager();
	std::string getPreamble() override { return preamble; }
};

