#pragma once

namespace edex
{
	class TextWindow
	{
	public:
		// The actual text stored
		std::vector<std::string> lines = {""};

		// Position data
		int originX;
		int originY;
		int width;
		int height;

		// Appearance data
		olc::Pixel background;
		olc::Pixel textColor;
		int textScale;

		// Graphics window the text is in
		olc::PixelGameEngine *window = nullptr;

		// Key information
		bool shiftHeld = false;
		bool capsLock = false;

		TextWindow() : originX(0), originY(0), width(100), height(100), textScale(1)
		{}

		TextWindow(int x, int y, int w, int h, olc::PixelGameEngine *olcWindow) : originX(x), originY(y), width(w), height(h), textScale(1), window(olcWindow)
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

		inline void makeFullScreen()
		{
			originX = 0;
			originY = 0;
			width = window->ScreenWidth();
			height = window->ScreenHeight();
		}

		inline bool typeCharacter(int32_t key)
		{
			lines[lines.size() - 1] += (char) key;

			return true;
		}

		bool registerKey(uint32_t key, int press)
		{
			if (key == olc::SHIFT && press)
			{
				shiftHeld = true;
				return true;
			}
			else if (key == olc::SHIFT && !press)
			{
				shiftHeld = false;
				return true;
			}
			else if (key == olc::CAPS_LOCK && press)
			{
				capsLock = !capsLock;
				return true;
			}

			if (press)
			{
				if (key > 0 && key < 27) // Letters (upper and lower case)
					return typeCharacter(key + (shiftHeld || capsLock ? 64 : 96));
				else if (key > 26 && key < 37) // Number row
				{
					if (!shiftHeld)
					{
						return typeCharacter(key + 47 - 26);
					}
					else
					{
						// Shift + number row
						switch (key)
						{
							case olc::K0: return typeCharacter(')'); break;
							case olc::K1: return typeCharacter('!'); break;
							case olc::K2: return typeCharacter('"'); break;
							case olc::K3: return typeCharacter('£'); break;
							case olc::K4: return typeCharacter('$'); break;
							case olc::K5: return typeCharacter('%'); break;
							case olc::K6: return typeCharacter('^'); break;
							case olc::K7: return typeCharacter('&'); break;
							case olc::K8: return typeCharacter('*'); break;
							case olc::K9: return typeCharacter('('); break;
						}
					}
				}
				else if (key == olc::ENTER)
				{
					lines.emplace_back("");
					return true;
				}
				else if (key == olc::SPACE)
					return typeCharacter(' ');
			}

			return false;
		}
	};
}
