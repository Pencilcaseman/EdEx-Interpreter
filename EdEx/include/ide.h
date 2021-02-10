#pragma once

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

			windows.emplace_back(TextWindow(0, 0, ScreenWidth(), ScreenHeight(), this));
			windows[0].setBackground({255, 255, 255});
			windows[0].setTextColor({0, 0, 0});

			timeOpened = seconds();
			windows[0].makeFullScreen();

			return true;
		}

		bool OnUserUpdate(float fElapsed)
		{
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
				std::cout << "Error typing key. Could be invalid or not supported yet\n";
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
