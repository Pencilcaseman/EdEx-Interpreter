#pragma once

#include "cursor.h"
#include "highlightRule.h"

namespace edex
{
	class TextWindow
	{
	public:
		// The actual text stored
		std::vector<std::string> lines = {""};

		// Position data
		int32_t originX;
		int32_t originY;
		int32_t width;
		int32_t height;

		// Appearance data
		bool syntaxHighlight = false;
		Rules highlightRules;
		olc::Pixel background;
		olc::Pixel textColor;
		int32_t textScale;
		bool renderBackground = true;

		// Graphics window the text is in
		olc::PixelGameEngine *window = nullptr;

		// Key information
		bool shiftHeld = false;
		bool capsLock = false;

		// Cursor
		Cursor cursor;

		TextWindow() : originX(0), originY(0), width(100), height(100), textScale(1)
		{
			cursor.lastUpdate = seconds();
		}

		TextWindow(int32_t x, int32_t y, int32_t w, int32_t h, olc::PixelGameEngine *olcWindow) : originX(x), originY(y), width(w), height(h), textScale(1), window(olcWindow)
		{
			cursor.lastUpdate = seconds();
		}

		inline void setBackground(const olc::Pixel &newColor)
		{
			background = newColor;
		}

		inline void setTextColor(const olc::Pixel &newColor)
		{
			textColor = newColor;
		}

		inline void setTextScale(const int32_t &newScale)
		{
			textScale = newScale;
		}

		inline void setSyntaxHighlight(bool val)
		{
			syntaxHighlight = val;
		}

		inline void setHighlightRules(const Rules &rules)
		{
			highlightRules = rules;
		}

		inline void setRenderBackground(const bool &shouldRender)
		{
			renderBackground = shouldRender;
		}

		inline std::vector<std::pair<std::string, olc::Pixel>> highlightLine(const std::string line)
		{
			std::vector<std::pair<std::string, olc::Pixel>> res;
			std::vector<std::string> tokens = splitString(line, highlightRules.delimiters);

			Rule selectedRule;
			bool ruleFound = false;

			// Parse regex expressions for a match
			for (const auto &rule : highlightRules.rule)
			{
				if (!rule.checkTokens)
				{
					if (std::regex_search(line, std::regex(rule.regex)))
					{
						selectedRule = rule;
						ruleFound = true;
					}
				}
			}

			for (const auto &token : tokens)
			{
				bool set = false;
				if (ruleFound)
				{
					for (const auto &key : selectedRule.format)
					{
						if (token == key.first)
						{
							res.push_back(std::make_pair(token, key.second));
							set = true;
						}
					}
				}

				for (const auto &rule : highlightRules.rule)
				{
					if (rule.checkTokens)
					{
						for (const auto &key : rule.format)
						{
							if (token == key.first)
							{
								res.push_back(std::make_pair(token, key.second));
								set = true;
							}
						}
					}
				}

				if (!set)
					res.push_back(std::make_pair(token, textColor));
			}

			return res;
		}

		bool render(olc::PixelGameEngine *window)
		{
			int32_t drawOriginX = std::max(originX, 0);
			int32_t drawOriginY = std::max(originY, 0);
			int32_t drawWidth = std::min(originX + width, window->ScreenWidth());
			int32_t drawHeight = std::min(originY + height, window->ScreenHeight());

			if (renderBackground)
			{
				// Draw the background
				if (drawOriginX == 0 && drawOriginY == 0 && drawWidth == window->ScreenWidth() && drawHeight == window->ScreenHeight())
					window->Clear(background);
				else
					window->FillRect(drawOriginX, drawOriginY, drawWidth, drawHeight, background);
			}

			// Draw the lines of text on the screen
			for (size_t i = 0; i < lines.size(); i++)
			{
				if (!syntaxHighlight)
				{
					window->DrawString(drawOriginX, drawOriginY + i * 10 * textScale, lines[i], textColor, textScale);
				}
				else
				{
					auto highlighted = highlightLine(lines[i]);

					uint64_t len = 0;
					for (const auto &token : highlighted)
					{
						window->DrawString(drawOriginX + len * 8 * textScale, drawOriginY + i * 10 * textScale, token.first, token.second, textScale);
						len += token.first.length();
					}
				}
			}

			// Render the cursor
			if (cursor.active)
			{
				if (((int32_t) ((seconds() - cursor.lastUpdate) * 2)) % 2 == 0)
				{
					uint32_t x = originX + cursor.linePos * 8 * textScale;
					uint32_t y = originY + cursor.line * 10 * textScale;
					window->FillRect(x, y, cursor.width * textScale, 10 * textScale, cursor.color);
				}
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

		inline int32_t getCharacter(int32_t offset = 0) const
		{
			auto &line = lines[cursor.line];

			if ((int32_t) cursor.linePos + (int32_t) offset < 0 || (int32_t) cursor.linePos + (int32_t) offset >= (int32_t) line.length()) // update to check bounds when cursor is implemented
				return 0;

			return line[(uint64_t) cursor.linePos + (uint64_t) offset - 1];
		}

		inline bool typeCharacter(int32_t key)
		{
			if (cursor.active)
			{
				auto &line = lines[cursor.line];
				line.insert(cursor.linePos, std::string(1, (char) key));
				cursor.linePos++;
				cursor.lastUpdate = seconds();
			}

			return true;
		}

		inline bool deleteCharacter()
		{
			if (cursor.active)
			{
				auto &line = lines[cursor.line];

				if (cursor.linePos > 0)
				{
					cursor.linePos--;
					line.erase(line.begin() + cursor.linePos, line.begin() + cursor.linePos + 1);
				}
				else if (lines.size() > 1)
				{
					std::string endLine(lines[cursor.line].begin() + cursor.linePos, lines[cursor.line].end());
					lines[cursor.line].erase(lines[cursor.line].begin() + cursor.linePos, lines[cursor.line].end());
					lines[cursor.line - 1] += endLine;

					lines.erase(lines.begin() + cursor.line, lines.begin() + cursor.line + 1);
					cursor.line--;
					cursor.linePos = lines[cursor.line].length() - endLine.length();
				}
				if (line.length() == 0)
					line = "";

				cursor.lastUpdate = seconds();
			}

			return true;
		}

		bool registerKey(uint32_t key, int32_t press)
		{
			// Check for cursor operations first
			if (press)
			{
				if (key == olc::UP)
				{
					if (cursor.line > 0)
						cursor.line--;
					if (lines[cursor.line].length() <= cursor.linePos)
						cursor.linePos = lines[cursor.line].length();

					return true;
				}
				else if (key == olc::DOWN)
				{
					if (cursor.line < lines.size() - 1)
						cursor.line++;
					if (lines[cursor.line].length() <= cursor.linePos)
						cursor.linePos = lines[cursor.line].length();

					return true;
				}
				else if (key == olc::LEFT)
				{
					if (cursor.linePos > 0)
						cursor.linePos--;
					else if (cursor.line > 0)
					{
						cursor.line--;
						cursor.linePos = lines[cursor.line].length();
					}

					return true;
				}
				else if (key == olc::RIGHT)
				{
					if (lines[cursor.line].length() == 0)
						return true;

					if (cursor.linePos < lines[cursor.line].length())
						cursor.linePos++;
					else if (cursor.line < lines.size() - 1)
					{
						cursor.line++;
						cursor.linePos = lines[cursor.line].length();
					}

					return true;
				}
			}

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
				if (key > 0 && key < 27)													// Letters (upper and lower case)
					return typeCharacter(key + (shiftHeld || capsLock ? 64 : 96));
				else if (key > 26 && key < 37)												// Number row
				{
					if (!shiftHeld)
					{
						return typeCharacter(key + 47 - 26);
					}
					else
					{
						switch (key)														// Shift + number row
						{
							// "�" doesn't render
							case olc::K0: return typeCharacter(')'); break;
							case olc::K1: return typeCharacter('!'); break;
							case olc::K2: return typeCharacter('"'); break;
							case olc::K4: return typeCharacter('$'); break;
							case olc::K5: return typeCharacter('%'); break;
							case olc::K6: return typeCharacter('^'); break;
							case olc::K7: return typeCharacter('&'); break;
							case olc::K8: return typeCharacter('*'); break;
							case olc::K9: return typeCharacter('('); break;
						}
					}
				}
				else if (key == 54)
				{
					if (!shiftHeld)
					{
						for (int32_t tabIndex = 0; tabIndex < 4; tabIndex++)
							if (!typeCharacter(' '))
								return false;
						return true;
					}

					bool previousIsTab = true;
					for (int32_t tabIndex = 3; tabIndex >= 0; tabIndex--)
						if (getCharacter(-tabIndex) != ' ')
							return false;

					for (int32_t tabIndex = 0; tabIndex < 4; tabIndex++)
						if (!deleteCharacter())
							return false;

					return true;
				}
				else if (key >= olc::NP0 && key < olc::NP9)									// Numpad numbers
					return typeCharacter(key);
				else if (key == olc::ENTER)													// Enter
				{
					// Extract the remaining line text
					std::string endLine(lines[cursor.line].begin() + cursor.linePos, lines[cursor.line].end());
					lines[cursor.line].erase(lines[cursor.line].begin() + cursor.linePos, lines[cursor.line].end());
					lines.insert(lines.begin() + cursor.line + 1, endLine);
					cursor.line++;
					cursor.linePos = 0;
					return true;
				}
				else if (key == olc::SPACE)													// Space
					return typeCharacter(' ');
				else if (key == olc::BACK)													// Backspace
					return deleteCharacter();
				else if (key == olc::COMMA && !shiftHeld)									// Comma
					return typeCharacter(',');
				else if (key == olc::COMMA && shiftHeld)									// Less than
					return typeCharacter('<');
				else if (key == olc::OEM_2 && !shiftHeld)									// Forward slash
					return typeCharacter('/');
				else if (key == olc::OEM_2 && shiftHeld)									// Question mark
					return typeCharacter('?');
				else if (key == 84 && !shiftHeld)											// Period
					return typeCharacter('.');
				else if (key == 84 && shiftHeld)											// Greater than
					return typeCharacter('>');
				else if (key == 85 && !shiftHeld)											// Equals
					return typeCharacter('=');
				else if (key == 85 && shiftHeld)											// Add
					return typeCharacter('+');
				else if (key == 87 && !shiftHeld)											// Subtract
					return typeCharacter('-');
				else if (key == 87 && shiftHeld)											// Underscore
					return typeCharacter('_');
				else if (key == 88 && !shiftHeld)											// Colon
					return typeCharacter(';');
				else if (key == 88 && shiftHeld)											// Semicolon
					return typeCharacter(':');
				else if (key == 91 && !shiftHeld)											// Open square bracket
					return typeCharacter('[');
				else if (key == 91 && shiftHeld)											// Open curly bracket
					return typeCharacter('{');
				else if (key == 93 && !shiftHeld)											// Close square bracket
					return typeCharacter(']');
				else if (key == 93 && shiftHeld)											// Close curly bracket
					return typeCharacter('}');
				else if (key == 90 && !shiftHeld)											// Single quote
					return typeCharacter('\'');
				else if (key == 90 && shiftHeld)											// "At" symbol
					return typeCharacter('@');
			}

			return false;
		}
	};
}
