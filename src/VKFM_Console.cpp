
#include "VKFM_Console.h"

#ifdef COLOR_CONSOLE
#ifdef WINDOWS

std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg){
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	// Set the color
	SetConsoleTextAttribute(hConsole, ((bg<<4)+fg));
	// Return nothing
	return ""; 
};


#endif
#ifdef LINUX


std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg){
	std::string out = "\033[";// Add 1; for BOLD text
	out += std::to_string(static_cast<int>(bg)+10);
	out += ";";
	out += std::to_string(static_cast<int>(fg));
	out += "m";
	return out;
};

#endif
#else
// No color

std::string VK_SetConsoleColor(VK_CONSOLE_COLORS fg, VK_CONSOLE_COLORS bg){
	// Return nothing
	return "";
};
#endif
