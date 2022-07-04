#pragma once

#include <stdio.h>
#include <string>

#ifdef COLOR_CONSOLE
#ifdef WINDOWS

#include <windows.h>   // WinApi header

// Enum class with all the colors

enum class VK_CONSOLE_COLORS  {
	DEFAULT 	= 0,
	BLACK 		= 0,
	DARK_BLUE 	= 1,
	DARK_GREEN 	= 2,
	CYAN 		= 3,
	DARK_RED 	= 4,
	PURPLE 		= 5,
	BROWN 		= 6,
	GREY 		= 7,
	DARK_GREY 	= 8,
	BLUE 		= 9,
	GREEN 		= 10,
	LIGHT_BLUE 	= 11,
	RED 		= 12,
	PINK 		= 13,
	YELLOW 		= 14,
	WHITE 		= 15,
};

std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg);


#endif
#ifdef LINUX

// Enum class with all the colors

enum class VK_CONSOLE_COLORS {
	DEFAULT 	= 0,
	BLACK 		= 30,
	DARK_BLUE 	= 34,
	DARK_GREEN 	= 32,
	CYAN 		= 36,
	DARK_RED 	= 31,
	PURPLE 		= 35,
	BROWN 		= 33,
	GREY 		= 37,
	DARK_GREY 	= 90,
	BLUE 		= 94,
	GREEN 		= 92,
	LIGHT_BLUE 	= 96,
	RED 		= 91,
	PINK 		= 95,
	YELLOW 		= 93,
	WHITE 		= 97,
};

std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg);

#endif
#else
// No color

// Enum class with all the colors

enum class VK_CONSOLE_COLORS {
	DEFAULT 	= 0,
	BLACK 		= 0,
	DARK_BLUE 	= 1,
	DARK_GREEN 	= 2,
	CYAN 		= 3,
	DARK_RED 	= 4,
	PURPLE 		= 5,
	BROWN 		= 6,
	GREY 		= 7,
	DARK_GREY 	= 8,
	BLUE 		= 9,
	GREEN 		= 10,
	LIGHT_BLUE 	= 11,
	RED 		= 12,
	PINK 		= 13,
	YELLOW 		= 14,
	WHITE 		= 15,
};
std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg);
#endif
