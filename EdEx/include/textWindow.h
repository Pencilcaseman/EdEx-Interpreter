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

		/// <summary>
		/// Default constructor. Create an empty window
		/// </summary>
		TextWindow() : originX(0), originY(0), width(100), height(100), textScale(1)
		{}

		/// <summary>
		/// Main constructor. Create a window with a specified origin,
		/// width and height.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="w"></param>
		/// <param name="h"></param>
		TextWindow(int x, int y, int w, int h) : originX(x), originY(y), width(w), height(h)
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
	};
}