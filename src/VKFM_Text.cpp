/*
 * VKFM_Text.cpp
 *
 *  Created on: 06.17.2022
 *      Author: John314
 *
*/

#include "VKFM_Heads.h"

std::string VK_RemoveExt(std::string in){
	std::string out;
	for(int i = 0; i < in.length(); i++){
		if(in[i]=='.'){
			return out;
		}
		if(in[i]=='/' || in[i] == '\\'){
			in.clear();
		}else{
			in.push_back(in[i]);
		}
	}
	return out;
};

int VK_SaveText(std::string txt_name){
	std::ifstream textfile;

	std::vector<uint8_t> VK_TextData;
	
	std::string name = VK_FileDirectory+"/"+txt_name+"."+VK_KeenEpisode;


	// Open the file
	textfile.open(name,std::ios::binary);
	
	if(!textfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmtxt: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	std::cout << "vkmtxt: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"opened " << name << std::endl;		
	
	while(1){
		uint8_t b = 0;
		textfile.read(reinterpret_cast<char *>(&b),sizeof(uint8_t));
		VK_TextData.push_back(b);
		if(textfile.eof()){
			break;
		}
	}
		
	textfile.close();
	std::cout << "vkmtxt: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;		
	
	// Save the level in a useable format
	std::ofstream exportfile;
	
	name = VK_FileDirectory+"/exports/"+txt_name+".h";
		
	// Open the file
	exportfile.open(name,std::ios::binary);
	
	if(!exportfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmtxt: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	std::cout << "vkmtxt: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"opened " << name << std::endl;		

	int count = 0;
	for(auto c : VK_TextData){
		if(c==13){
			count += 1;
		}
	}
	
	exportfile << "// Vortakeen Text exported by vkmake\n" << 
		"// \"|--THIS IS THE MAX LENGTH--|\"\n" <<
		"const unsigned short " << txt_name << "_length = " << count << ";\n" <<
		"const unsigned char *" << txt_name << "_text[] = {\n";

	
	exportfile << "\"";
	for(auto c : VK_TextData){
		if(c==10||c==26) continue;
		if(c=='\''||c=='\"'){
			exportfile << "\\";
		}
		if(c==13){
			exportfile << "\",\n\"";
		}else if(c==0){
			exportfile << "\",\n";
		}else{
			if(c>=' ' && c <= 'z'+2){
				exportfile << (char)(c);
			}else{
				exportfile << "\\x" << std::hex << std::setw(2) <<  std::setfill('0') << (int)(c);
			}
		}
		/*
		count += 1;
		if(count > 32){
			count = 0;
			exportfile << "\n";
		}*/
	}
	exportfile << "\n};\n";

	exportfile.close();
	std::cout << "vkmtxt: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;		
	return 0;
};


void VK_CompileText(){
	if(! (VKMake_Flags&static_cast<int>(VKFLAG::TEXT_ONLY)) ){
		// We didn't want to make the levels!
		return;
	}
	std::string level_dir = VK_FileDirectory;
	// Check if the directory exist
	std::filesystem::path level_path(level_dir);
	if(!std::filesystem::exists(level_path)){
		VK_ErrorList.push_back(std::string("vkmtxt: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open directory ") + level_dir);
		return;
	}
	
	// Create all the needed directories
	std::filesystem::create_directory(VK_FileDirectory+"/exports");


	VK_SaveText("ENDTEXT");
	VK_SaveText("HELPTEXT");
	VK_SaveText("STORYTXT");

//	VK_SaveText("FINALE");
	
};

