#pragma once
#include "Agent.h"
class AdvesarialIntelligenceManager :
	public Agent
{
public:
	AdvesarialIntelligenceManager();
	~AdvesarialIntelligenceManager();
	std::string getPreamble() override { return preamble; }
private:
	std::string preamble = "Advesarial Intelligence Manager: ";
};

