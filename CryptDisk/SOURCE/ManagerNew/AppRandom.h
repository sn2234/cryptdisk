
#pragma once

#include "Singleton.h"
#include "RandomGenerator.h"

class InitAppRandom
{
public:
	static RandomGenerator* Alloc();
};

typedef Singleton<RandomGenerator, InitAppRandom> AppRandom;
