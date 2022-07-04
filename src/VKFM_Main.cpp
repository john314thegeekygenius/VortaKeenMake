/*
 * VKFM_Main.cpp
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/
 
 
#include "VKFM_Heads.h"

int VKMake_Flags = static_cast<int>(VKFLAG::NONE);

std::vector<std::string> VK_ErrorList;

std::string VK_FileDirectory = "";
std::string VK_KeenEpisode = "";
int VK_EpisodeNumber = 0;


int VK_HandleArguments(const std::vector<std::string> &argv){
	
	// Set up the color scheme
	std::cout << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) << std::flush;
	
	for(auto arg : argv){
		/*
		 * Things you could add here:
		 * 		Extended Tileset Flag
		 * 		
		 * 
		*/
		if(arg.compare("--help")==0){
			// Print out the info
			std::cout << "Usage: vkmake [options] <episode> gamedir\n" <<
			"Options:\n" <<
			"\t--help\t\tDisplay this information\n" <<
			"\t--version\tDisplay the version of VKMake\n" <<
			"\t<episode>\tWhich episode to compile [ck1,ck2,ck3]\n" <<
			"\t-Ogfx\t\tOnly build the graphics\n" <<
			"\t-Osfx\t\tOnly build the sound effects\n" <<
			"\t-Olvl\t\tOnly build the levels\n" <<
			"\t-Otxt\t\tOnly build the text files\n" <<
			"\t-Thd\t\tCompile tileset and misc with 256 color palette\n" <<
			"\t-Shd\t\tCompile sprites with 256 color palette\n" <<
			"\t-Ssh\t\tSkip the SOUNDS.CKV file header (for compatability)\n" <<
			std::endl;
			return 1;
		}
		if(arg.compare("--version")==0){
			// Print out the info
			std::cout << VKMAKE_VER << std::endl;
			return 1;
		}
		if(arg.compare("-Ogfx")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::GRAPHICS_ONLY);
		}
		if(arg.compare("-Osfx")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::SOUND_ONLY);
		}
		if(arg.compare("-Olvl")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::LEVELS_ONLY);
		}
		if(arg.compare("-Otxt")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::TEXT_ONLY);
		}
		if(arg.compare("-Ssh")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::SKIP_SND_HEAD);
		}
		if(arg.compare("-Thd")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::HD_TEXTURES);
		}
		if(arg.compare("-Shd")==0){
			VKMake_Flags |= static_cast<int>(VKFLAG::HD_SPRITES);
		}
		
	}
	if(argv.size()<2){
		std::cout << "vkmake: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::DARK_RED,VK_CONSOLE_COLORS::DEFAULT) << 
			"fatal error: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"no directory specified\nbuild terminated.\ntype: 'vmake --help' for help " << std::endl;
		return 1;
	}

	// Set the episode
	VK_KeenEpisode = argv[argv.size()-2];
	// Make sure it's all capitolized
	for(auto& x: VK_KeenEpisode)
        x = toupper(x);
	
	// Set the file directory
	VK_FileDirectory = argv.back(); // Set it as the last variable
		
	if(VK_FileDirectory[VK_FileDirectory.size()-1]=='/'||
		VK_FileDirectory[VK_FileDirectory.size()-1]=='\\'){
		// Remove the last char
		VK_FileDirectory.pop_back();
	}
	
	// Automatically set the flags
	if((VKMake_Flags&0xFF)==0){
		VKMake_Flags |= static_cast<int>(VKFLAG::GRAPHICS_ONLY);
		VKMake_Flags |= static_cast<int>(VKFLAG::SOUND_ONLY);
		VKMake_Flags |= static_cast<int>(VKFLAG::LEVELS_ONLY);
		VKMake_Flags |= static_cast<int>(VKFLAG::TEXT_ONLY);
	}
	
	if(VK_KeenEpisode.compare("CK1")==0){
		VK_EpisodeNumber = 1;
	}else
	if(VK_KeenEpisode.compare("CK2")==0){
		VK_EpisodeNumber = 2;
	}else
	if(VK_KeenEpisode.compare("CK3")==0){
		VK_EpisodeNumber = 3;
	}else{
		std::cout << "vkmake: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) << 
			"warning: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"no episode specified! defaulting to episode 1" << std::endl;
		
		VK_KeenEpisode = "CK1";
		VK_EpisodeNumber = 1;
	}
	std::cout << "Creating Files for Commander Keen Episode " << VK_EpisodeNumber << std::endl;
	
	return 0;
};



int main(int argc, char *args[]){
	// Get all the arguments into 1 array
	std::vector<std::string> argv;
	for(int i = 0; i < argc; i++){
		argv.push_back(args[i]);
	}
	if(VK_HandleArguments(argv)==0){
		// Run the sub processes
		VK_CompileGraphics();
		VK_CompileLevels();
		VK_CompileSounds();
		VK_CompileText();
	}
	
	// Print any errors that may have occured
	for(auto error : VK_ErrorList){
		std::cout << error << std::endl;
	}
	
	// Return to console
	return 0;
};

