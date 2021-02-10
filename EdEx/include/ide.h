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
			windows.emplace_back(TextWindow(0, 0, ScreenWidth(), ScreenHeight()));
		}

		bool OnUserCreate()
		{
			std::cout << "Opening window\n";

			ptrIDE = this;

			timeOpened = seconds();
			windows[0].makeFullScreen(this);

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
			windows[0].registerKey(key);
		};

		void OnKeyRelease(uint32_t key)
		{
			std::cout << "Key Released: " << key << "\n";
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
