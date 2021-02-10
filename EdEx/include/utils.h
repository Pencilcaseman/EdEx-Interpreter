#pragma once

// Include some things
#include <iostream>
#include <vector>
#include <chrono>

namespace edex
{
	inline double seconds()
	{
		return (double) std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1000000000;
	}
}

#include "olcPixelGameEngine.h"
