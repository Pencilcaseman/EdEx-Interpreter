#pragma once

namespace edex
{
	typedef struct Cursor
	{
		// Activity
		bool active = false;

		// Position
		uint32_t line = 0;
		uint32_t linePos = 0;

		// Appearance
		olc::Pixel color = {0, 0, 0};
		uint32_t width = 1;
	};
}
