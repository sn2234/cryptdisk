
#pragma once

#include "Singleton.h"
#include "DNDriverControl.h"

class DriverHelper
{
public:
	DriverHelper(std::shared_ptr<DNDriverControl> cntrl)
		: m_control(cntrl)
	{}

	DNDriverControl* getDriverControl() { return m_control.get(); }

private:
	std::shared_ptr<DNDriverControl> m_control;
};

class InitDriverHelper
{
public:
	static DriverHelper* Alloc();
};

typedef Singleton<DriverHelper, InitDriverHelper> AppDriver;
