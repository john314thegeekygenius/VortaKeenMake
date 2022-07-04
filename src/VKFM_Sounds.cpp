/*
 * VKFM_Sounds.cpp
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/

#include "VKFM_Heads.h"

#include "math.h"

// https://moddingwiki.shikadi.net/wiki/Inverse_Frequency_Sound_format


struct VK_SoundHeader {
	char hash[4]; // signature 4 bytes at start of file
	uint16_t size; // size of file (little endian)
	uint16_t unknown; // unknown what is for, usally 0x003C
	uint16_t count; // number of sounds in file
	uint8_t pad[6]; // make the header 16 bytes
};

struct VK_SoundInfo {
	uint16_t offset; // offset of sound from start of file
	uint8_t priority; // whether or not sound will be interruped by another sound
	uint8_t rate; // update rate of timer (usually 8)
	char name[12]; // null padded name of sound
};

struct VK_SoundData {
	std::vector<uint16_t> data;
};

bool VK_ReadSoundHeader = true;

int VK_ExportSound(VK_SoundData &data,VK_SoundInfo &info, int id){
	std::ofstream soundfile;

	std::string name = VK_FileDirectory+"/sounds/exports/SOUND"+str_to_format(id)+".h";

	// Open the file
	soundfile.open(name,std::ios::binary);
	
	if(!soundfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmsfx: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	std::cout << "vkmsfx: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"opened " << name << std::endl;		

	soundfile << "// Commander Keen Sound exported by vkmake\n" <<
				"// Sound name : " << info.name << "\n" <<
				"const unsigned short VK_SOUND_"+str_to_format(id)+"_PRIO = " << (int)info.priority << ";\n" <<
				"const unsigned short VK_SOUND_"+str_to_format(id)+"_length = " << (int)data.data.size() << ";\n" <<
				"const unsigned short VK_SOUND_"+str_to_format(id)+"_data[] = {\n";

	soundfile << "\t";
	
	int count = 0;
	for(uint16_t d : data.data){
		soundfile << "0x" << std::hex << std::setw(4) << std::setfill('0') << ((int)d&0xFFFF) << ", ";
		if(count ++ > 32){
			count = 0;
			soundfile << "\n\t";
			std::cout << std::endl;
		}
		
	}
	soundfile << "\n};\n";

	
	soundfile.close();
	std::cout << "vkmsfx: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;		
	
	return 0;
};


int VK_LoadSounds(){
	std::ifstream soundfile;
	
	std::string name = VK_FileDirectory+"/SOUNDS."+VK_KeenEpisode;
	
	// Open the file
	soundfile.open(name,std::ios::binary);
	
	if(!soundfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmsfx: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	std::cout << "vkmsfx: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"opened " << name << std::endl;		
		
	VK_SoundHeader sound_header;		
	std::vector<VK_SoundInfo> sound_info;
	
	// Read the header
	soundfile.read(reinterpret_cast<char *>(&sound_header),sizeof(VK_SoundHeader));

	if(VK_ReadSoundHeader){
		// Force the last byte of the hash to be 0
		sound_header.hash[3] = 0;
		// Make sure it's a valid file
		if(strcmp(sound_header.hash,"SND")!=0){
			VK_ErrorList.push_back(std::string("vkmsfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			name + std::string(" is an invalid format. ") + std::string(sound_header.hash) + " is not a known format");
			return 1;
		}
	}else{
		// Assume 64 sounds
		sound_header.count = 64;
	}
	
	for(int i = 0; i < sound_header.count; i++){
		VK_SoundInfo info;
		soundfile.read(reinterpret_cast<char *>(&info),sizeof(VK_SoundInfo));
		sound_info.push_back(info);
	}

	std::vector<VK_SoundData> sound_data;
	
	// Print all the offsets
	for(auto a : sound_info){
		VK_SoundData data_list;
		// move to location in file
		if(a.offset==0){
			continue;
		}
		if(strcmp(a.name,"__UnNamed__")==0){
			if(!VK_ReadSoundHeader){
				goto skiprestofsounds;
			}
		}
		
		soundfile.seekg(a.offset);
		while(1){
			uint16_t word;
			soundfile.read(reinterpret_cast<char *>(&word),sizeof(uint16_t));
			// read data until 0xFFFF
			if(word==0xFFFF){
				// Stop the party
				break;
			}
			data_list.data.push_back(word);
		}
		sound_data.push_back(data_list);
	}
	skiprestofsounds:;
	std::cout << "Done collecting sound info" << std::endl;
	
	// Generate the sound effects as WAV files
	VKFM_Waveform sound_wave;
	sound_wave.VK_WriteWaveHeader();
	
	std::ofstream sndlist;
	std::string listname = VK_FileDirectory+"/sounds/LIST.TXT";
	sndlist.open(listname,std::ios::binary);
	if(!sndlist.is_open()){
		VK_ErrorList.push_back(std::string("vkmsfx: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + listname);
		return 1;
	}
	
	for ( int i = 0; i < sound_info.size(); i++){
		sndlist << i << " - " << sound_info[i].name << "\n";
	}
	
	sndlist.close();
	
	listname = VK_FileDirectory+"/sounds/exports/VK_SOUNDS.h";
	sndlist.open(listname,std::ios::binary);
	if(!sndlist.is_open()){
		VK_ErrorList.push_back(std::string("vkmsfx: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + listname);
		return 1;
	}
	sndlist << "// Sound Definitions for VortaKeen exported by vkmake\n";
	for ( int i = 0; i < sound_info.size(); i++){
		if(std::string(sound_info[i].name).compare("__UnNamed__")==0)
			break;
		std::string putmestr = sound_info[i].name;
	    for(auto& x: putmestr)
			x = toupper(x);
		sndlist << "#define VKS_" << putmestr << " " << i << "\n";
	}
	
	sndlist.close();
	
	std::vector<uint8_t> square_wav;
	for ( int snd =0; snd <  sound_data.size() ;snd ++){
		// Make sure to remove old data
		square_wav.clear();
		sound_wave.VK_ClearData();

		std::cout << "vkmsfx: " << 
				VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
				"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
				"creating " << "SOUND" << str_to_format(snd) << ".WAV" << std::endl;	

		std::cout << VK_SetConsoleColor(VK_CONSOLE_COLORS::BLUE,VK_CONSOLE_COLORS::DEFAULT) << snd << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) << " - " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::GREEN,VK_CONSOLE_COLORS::DEFAULT) << sound_info[snd].name << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) << " -> data length :" << sound_data[snd].data.size() << std::endl;

		for(uint16_t datum : sound_data[snd].data){
			int inv_freq = 0;
			if(datum>0){
				inv_freq =  (float)1193181 / (float)datum;
			}
			sound_wave.VK_GenerateSquareWave(square_wav, inv_freq, 0x10,7.142857143);
			sound_wave.VK_WriteWaveData(square_wav);
		}
		
		sound_wave.save(VK_FileDirectory+"/sounds/SOUND"+str_to_format(snd)+".WAV");
		
		VK_ExportSound(sound_data[snd],sound_info[snd],snd);
	}
	
	soundfile.close();
	std::cout << "vkmsfx: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;		
	
	return 0;
};

void VK_CompileSounds(){
	if(! (VKMake_Flags&static_cast<int>(VKFLAG::SOUND_ONLY)) ){
		// We didn't want to make the sounds!
		return;
	}
	if(VKMake_Flags&static_cast<int>(VKFLAG::SKIP_SND_HEAD)){
		VK_ReadSoundHeader = false;
	}

	// Check if the directory exist
	std::filesystem::path main_path(VK_FileDirectory);
	if(!std::filesystem::exists(main_path)){
		VK_ErrorList.push_back(std::string("vkmsfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open directory ") + VK_FileDirectory);
		return;
	}
	// Create all the needed directories
	std::filesystem::create_directory(VK_FileDirectory+"/sounds");
	std::filesystem::create_directory(VK_FileDirectory+"/sounds/exports");
	
	VK_LoadSounds();
	
	
};


