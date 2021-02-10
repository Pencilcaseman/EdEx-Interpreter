#pragma once

namespace edex
{
	class TextWindow
	{
	public:
		// The actual text stored
		std::vector<std::string> lines;

		// Position data
		int originX;
		int originY;
		int width;
		int height;

		// Appearance data
		olc::Pixel background;
		olc::Pixel textColor;
		int textScale;

		TextWindow() : originX(0), originY(0), width(100), height(100), textScale(1)
		{}

		TextWindow(int x, int y, int w, int h) : originX(x), originY(y), width(w), height(h), textScale(1)
		{}

		inline void setBackground(const olc::Pixel &newColor)
		{
			background = newColor;
		}

		inline void setTextColor(const olc::Pixel &newColor)
		{
			textColor = newColor;
		}

		inline void setTextScale(const int &newScale)
		{
			textScale = newScale;
		}

		bool render(olc::PixelGameEngine *window)
		{
			// Draw the background
			int drawOriginX = std::max(originX, 0);
			int drawOriginY = std::max(originY, 0);
			int drawWidth = std::min(originX + width, window->ScreenWidth());
			int drawHeight = std::min(originY + height, window->ScreenHeight());
			window->FillRect(drawOriginX, drawOriginY, drawWidth, drawHeight, background);

			for (size_t i = 0; i < lines.size(); i++)
			{
				window->DrawString(10, i * 10, lines[i], textColor, textScale);
			}

			return true;
		}

		inline void makeFullScreen(olc::PixelGameEngine *window)
		{
			originX = 0;
			originY = 0;
			width = window->ScreenWidth();
			height = window->ScreenHeight();
		}

		bool registerKey(uint32_t key)
		{
			if (key >= 0 && key < 26)
				lines[0] += (char) (key + 65);

			return true;
		}
	};
}