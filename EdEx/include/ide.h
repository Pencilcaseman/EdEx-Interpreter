#pragma once

#include "interpreter.h"

namespace edex
{
	class IDE;
	IDE *ptrIDE;

	class IDE : public olc::PixelGameEngine
	{
	public:
		std::vector<TextWindow> windows;
		double timeOpened;

		IDE()
		{
			timeOpened = 0;
			std::cout << "Initializing IDE window\n";
		}

		bool OnUserCreate()
		{
			std::cout << "Opening window\n";

			ptrIDE = this;

			windows.push_back(TextWindow(10, 10, ScreenWidth() - 10, ScreenHeight() - 10, this));
			windows[0].setBackground({35, 50, 50});
			windows[0].setTextColor({204, 204, 204});

			timeOpened = seconds();

			windows[0].setSyntaxHighlight(true);
			windows[0].setHighlightRules(generateRules());

			windows[0].setTextScale(2);
			windows[0].cursor.active = true;

			return true;
		}

		bool OnUserUpdate(float fElapsed)
		{
			// TODO: Do something about this
			Clear(windows[0].background);

			for (auto &window : windows)
			{
				if (!window.render(this))
				{
					std::cout << "Window at " << &window << " failed to render\n";
					// TODO implement a window.fix function to repair a damaged window
					// window.fix();
				}
			}

			return true;
		}

		bool OnUserDestroy()
		{
			std::cout << "Closing window\n";

			return true;
		}

		void OnKeyPress(uint32_t key)
		{
			if (!windows[0].registerKey(key, 1))
			{
				std::cout << "Unable to process key: " << key << "\n";
			}
		};

		void OnKeyRelease(uint32_t key)
		{
			windows[0].registerKey(key, 0);
		};

		void OnMouseDown(uint32_t button)
		{
			std::cout << "Mouse Pressed: " << button << "\n";
		};

		void OnMouseRelease(uint32_t button)
		{
			std::cout << "Mouse Released: " << button << "\n";
		};

		void OnMouseWheel(int32_t delta)
		{
			std::cout << "Scroll: " << delta << "\n";
		};
	};
}
