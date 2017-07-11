#pragma once
#include "Agent.h"
class StrategyManager :
	public Agent
{
public:
	StrategyManager();
	~StrategyManager();
	std::string getPreamble() override { return preamble; }
};

