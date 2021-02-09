#pragma once

namespace edex
{
	class IDE : public olc::PixelGameEngine
	{
	public:
		std::vector<TextWindow> windows;

		IDE()
		{
			std::cout << "Initializing IDE window\n";
			windows.emplace_back(TextWindow(0, 0, ScreenWidth(), ScreenHeight()));
		}

		bool OnUserCreate() override
		{
			std::cout << "Opening window\n";
			return true;
		}

		bool OnUserUpdate(float fElapsed) override
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

		bool OnUserDestroy() override
		{
			std::cout << "Closing window\n";


			return true;
		}
	};
}
