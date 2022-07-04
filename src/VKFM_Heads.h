#pragma once

/*
 * VKFM_Heads.h
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/

#define VKMAKE_VER "VortKeen Make version 1.0"

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <cstring>
#include <math.h> 

// Enum class with all the flags

enum class VKFLAG  {
	NONE 			= 0x0,
	GRAPHICS_ONLY 	= 0x1,
	SOUND_ONLY 		= 0x2,
	LEVELS_ONLY 	= 0x4,
	TEXT_ONLY 		= 0x8,

	HD_TEXTURES		= 0x100,
	HD_SPRITES		= 0x200,
	SKIP_SND_HEAD	= 0x400,
};


#include "VKFM_Console.h"
#include "VKFM_Bitmap.h"
#include "VKFM_Waveform.h"

extern int VKMake_Flags;
extern std::vector<std::string> VK_ErrorList;
extern std::string VK_FileDirectory;
extern std::string VK_KeenEpisode;
extern int VK_EpisodeNumber;

void VK_ExpandSwapped(std::vector<uint16_t> &dst, std::vector<uint8_t> &src, uint16_t key);

void VK_CompileLevels();
void VK_CompileGraphics();
void VK_CompileSounds();
void VK_CompileText();

