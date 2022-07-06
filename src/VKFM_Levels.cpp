/*
 * VKFM_Levels.cpp
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/

#include "VKFM_Heads.h"


const int MAX_LEVEL_TILES = 160-14;

// Not used (For unlzexe'ed exes)
const int CK1_TileInfo_Offset = 0x130F8;
const int CK2_TileInfo_Offset = 0x17828;
const int CK3_TileInfo_Offset = 0x198C8;

// Number of tiles in each episode
const int CK1_NumberOfTiles = 611;
const int CK2_NumberOfTiles = 689;
const int CK3_NumberOfTiles = 715;

std::vector<int> VK_LevelList = {
	 1,  2,  3,  4,  5,  6,  7,  8,  9,
	10, 11, 12, 13, 14, 15, 16, 80, 81, 90,
};

// Level header
 
struct CK_LevelHeader {
	uint16_t level_width;
	uint16_t level_height;
	uint16_t num_of_planes;
	uint32_t blank;
	uint32_t TED_variables;
	uint16_t plane_size;
	uint32_t unknown[4];
};

struct tile_const {
	std::string name;
	int value;
};

std::vector<tile_const> const_tiles;

int VK_PartCompareString(std::string a,std::string b){
	if(b.length() > a.length()){
		return 1;
	}
	for(int i = 0; i < b.length(); i++){
		if(a[i]!=b[i]){
			return -1;
		}
	}
	return 0;
};


void VK_ExpandSwapped(std::vector<uint16_t> &dst, std::vector<uint8_t> &src, uint16_t key){
	const int WORDSIZE = 2;
	
	size_t finalsize, count;
	uint16_t value;
	size_t inc = 0;
	
	finalsize = (src[3]<<24) | (src[2]<<16) | (src[1]<<8) | src[0];
	finalsize >>= 1;
		
	for(size_t i = (WORDSIZE<<1), elementnum = 0; elementnum < finalsize; i+= inc){
		// Read data word
		value = (src[i+1]<<8) | src[i];
		// If word is 0xFEFE/0xABCD (aka key)
		if(value == key){
			// Read count (word)
			count = (src[i+3]<<8) | src[i+2];
			value = (src[i+5]<<8) | src[i+4];
			
			// Copy byte count times
			for (uint32_t j = 0; j < count; j++, elementnum ++){
				dst.push_back(value);
			}
			
			inc = 3*WORDSIZE;
		}else{
			// Just write the value
			dst.push_back(value);
			elementnum ++;
			inc = WORDSIZE;
		}
	}
	
};



std::vector<uint16_t> VK_LevelTiles;
std::vector<uint16_t> VK_LevelTInfo[6];


int VK_LoadTileset(std::string name){
	std::ifstream tinfofile;

	// Open the file
	tinfofile.open(name,std::ios::binary);
	
	if(!tinfofile.is_open()){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	
	int count = 0;
	
	if(VK_KeenEpisode.compare("CK1")==0){
		count = CK1_NumberOfTiles;
	}else
	if(VK_KeenEpisode.compare("CK2")==0){
		count = CK2_NumberOfTiles;
	}else
	if(VK_KeenEpisode.compare("CK3")==0){
		count = CK3_NumberOfTiles;
	}
	
	for(int i = 0; i < 6; i++){
		for(int j = 0; j < count; j++){
			uint16_t b = 0;
			tinfofile.read(reinterpret_cast<char *>(&b),sizeof(uint16_t));
			VK_LevelTInfo[i].push_back(b);
		}
	}
		
	tinfofile.close();

	return 0;
};

int VK_RoundToN16(int val){
	int modu = val%16;
	if(modu==0) return val;
	return val + (16-modu);
};

int VK_LoadLevel(int level_id){
	
	std::ifstream levelfile;
	
	std::string name = VK_FileDirectory+"/levels/LEVEL"+str_to_format(level_id)+"."+VK_KeenEpisode;
	
	VK_LoadTileset(VK_FileDirectory+"/TILEINFO.TLI");
	
	// Open the file
	levelfile.open(name,std::ios::binary);
	
	if(!levelfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	
	std::cout << "vkmlvl: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"opened " << name << std::endl;	


	// Load the level data
	std::vector<uint8_t> level_data(
         (std::istreambuf_iterator<char>(levelfile)),
         (std::istreambuf_iterator<char>()));

    std::vector<uint16_t> map_data;

	// Decompress the data
    VK_ExpandSwapped(map_data, level_data, 0xFEFE);

	// Copy the header over
	CK_LevelHeader level_head;

	level_head.level_width = map_data[0];
	level_head.level_height = map_data[1];
	level_head.num_of_planes = map_data[2];
	level_head.blank = (map_data[4]<<8) | map_data[3];
	level_head.TED_variables = (map_data[6]<<8) | map_data[5];
	level_head.plane_size = map_data[7] >> 1; // Plane size is in bytes so shift by 1
	level_head.unknown[0] = (map_data[9]<<8) | map_data[8];
	level_head.unknown[1] = (map_data[11]<<8) | map_data[10];
	level_head.unknown[2] = (map_data[13]<<8) | map_data[12];
	level_head.unknown[3] = (map_data[15]<<8) | map_data[14];
	
    // Remove the header from the map data and last byte
    //map_data.erase(map_data.begin(),map_data.begin()+(sizeof(level_head)>>1));
    
    //level_head.plane_size = level_head.level_width*level_head.level_height;
//    level_head.plane_size = VK_RoundToN16(2*(level_head.level_width*level_head.level_height));
        
    VK_LevelTiles.clear();
    
    std::vector<uint16_t> tile_animations;
    
    // Find all the animation locations
	for(int c = 0; c < VK_LevelTInfo[0].size(); c++){
		if(VK_LevelTInfo[0][c]>1){
			for(int i = 0; i < VK_LevelTInfo[0][c]; i++){
				tile_animations.push_back(i);
			}
		}else{
			tile_animations.push_back(0);
		}
		c += VK_LevelTInfo[0][c]-1;
	}
    

	int tile_offset = 0;


	// Set background tiles first
	for(auto t : const_tiles){
		if(VK_PartCompareString(t.name,"BACKGROUND")==0){
			int addnew = 1;
			if(t.value==9999)
				addnew = 0;
			else{
				for(auto c : VK_LevelTiles){
					if(c == t.value) addnew = 0;
				}
				if(addnew){
					VK_LevelTiles.push_back(t.value);
				}
			}
			break;
		}
	}
	
	for (int bi = 0; bi < level_head.plane_size; bi++){
		uint16_t b = map_data[bi+16];
		int addnew = 1;

		tile_offset ++;
		if(tile_offset>=(level_head.level_width*level_head.level_height)){
			std::cout << "vkmlvl: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"done collecting tiles " << std::to_string((int)bi) << std::endl;
			break;
		}
		//if(VK_LevelTInfo[1][b]==25||VK_LevelTInfo[1][b]==26){
		//}

		for(auto t : const_tiles){
			
			if(t.name.compare("SWITCH")==0){
				if(b == (t.value)){
					addnew = 1;
					for(auto c : VK_LevelTiles){
						if(c == t.value) addnew = 0;
					}
					if(addnew)
						VK_LevelTiles.push_back(t.value); // Add the switch tile here
					addnew = 1;
					for(auto c : VK_LevelTiles){
						if(c == (b+1)) addnew = 0;
					}
					if(addnew)
						VK_LevelTiles.push_back(b+1); // Add the switch tile here
						
					for(auto B : const_tiles){
						if(B.name.compare("BRIDGE")==0){
							addnew = 1;
							for(auto c : VK_LevelTiles){
								if(c == B.value) addnew = 0;
							}
							if(addnew)
								VK_LevelTiles.push_back(B.value); // Add the bridge tile here
						}
					}
					
				}
			}
			
			if(t.name.compare("STATUE")==0){
				if(b == (t.value+13)){
					addnew = 1;
					for(auto c : VK_LevelTiles){
						if(c == t.value) addnew = 0;
					}
					if(addnew)
						VK_LevelTiles.push_back(t.value); // Add the bottom tile here
				}
			}
		}
		addnew = 1;
		for(auto c : VK_LevelTiles){
			if(c == b) addnew = 0;
		}
		if(addnew){
			// Add the other animation tiles
			if(b >= tile_animations.size()){
				std::cout << "error: bad animation" << std::endl;
				VK_ErrorList.push_back(std::string("vkmlvl: ") +
				VK_SetConsoleColor(VK_CONSOLE_COLORS::RED,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("bad animation ") + std::to_string((int)b));

				return 1;
			}
			if(b < VK_LevelTInfo[0].size()){
				/*
				int tilecount = 0;
				if(VK_LevelTInfo[0][b]>1){
					for(int i = 1; i < 64; i++){
						if(VK_LevelTInfo[0][b-i] == VK_LevelTInfo[0][b]){
							tilecount ++;
						}
					}
				}
				tilecount %= 4;*/

				for(int i = 0; i < VK_LevelTInfo[0][b-tile_animations[b]]; i++){
					VK_LevelTiles.push_back((b-tile_animations[b])+i);//(b-tilecount));
				}
					
			}else{
				VK_ErrorList.push_back(std::string("vkmlvl: ") +
				VK_SetConsoleColor(VK_CONSOLE_COLORS::RED,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("bad tile ") + std::to_string((int)b));

				return 1;
			}
		}
	}

	
	// Make sure there aren't too many tiles in the level
	if(VK_LevelTiles.size() > MAX_LEVEL_TILES){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("too many unique tiles in level"));

		return 1;
	}

	
	std::cout << "Width:" << level_head.level_width << std::endl;
	std::cout << "Height:" << level_head.level_height << std::endl;
	std::cout << "Plane Size: " << level_head.plane_size << std::endl;
	
	levelfile.close();
	std::cout << "vkmlvl: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;	


	// Open 1TIL0000.BMP
	// And Create new tileset per level
	
	VKFM_Bitmap tileset_bmp;
	std::string tilesetname = "TIL0000.BMP";
	/*
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		std::string tilesetname = "HDTIL0000.BMP";
	}*/
	
	VK_BMPERRORS bitmap_error = tileset_bmp.load(VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+tilesetname);
	
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	
	// Copy tileset	

	VKFM_Bitmap new_tileset;
	tileset_bmp.copy_heads(new_tileset);

	VKFM_Bitmap tile_bmp;

	int tileset_x = 0;
	int tileset_y = 0;
	
	int place_x = 0;
	int place_y = 0;
	
	auto placetile = [&](int t) { 
		tileset_y = (t) / 13;
		tileset_x = (t) - (tileset_y*13);
		
		tileset_bmp.copy(tile_bmp);
		tile_bmp.crop_bmp(tile_bmp,tileset_x*16,tileset_y*16,16,16);
		new_tileset.stitch(tile_bmp,place_x*16,place_y*16);
		
		place_x ++;
		if(place_x >= 8){
			place_x = 0;
			place_y ++;
		}
	};

	if(level_id==80){
		for(auto t : const_tiles){
			if(t.name.compare("DONE_LARGE")==0||
				t.name.compare("TELEPORT_D")==0||
				t.name.compare("TELEPORT_S")==0){
				for(int i = 0; i < 4; i++){
					VK_LevelTiles.push_back(t.value+i);
				}
			}else{
				if(t.name.compare("DONE_SMALL")==0){
					VK_LevelTiles.push_back(t.value);
				}
			}
		}
	}
	if(level_id==90){
		for(auto t : const_tiles){
			if(t.name.compare("ITEM_1")==0||
				t.name.compare("ITEM_2")==0||
				t.name.compare("ITEM_3")==0||
				t.name.compare("ITEM_4")==0){
				for(int i = 0; i < 4; i++){
					VK_LevelTiles.push_back(t.value+i);
				}
			}
		}
	}
	

	for(auto tile : VK_LevelTiles){
		placetile(tile);		
	}

	/*
	for(int e = 0 ; e < 16; e++){
		for(int i = 0 ; i < 16; i++){
			
			tileset_x += 16;
			if(tileset_x > tileset_bmp.dib_head.bitmap_width){
				tileset_x = 0;
				tileset_y += 16;
			}
			if(tileset_y > tileset_bmp.dib_head.bitmap_height){
				tileset_y = 0; // Save the program!
			}
		}
	}*/

	bitmap_error = new_tileset.save(VK_FileDirectory+"/levels/tilesets/LEVEL"+str_to_format(level_id)+"_TILESET.BMP");
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error saving bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	
	// Export the tileset
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		bitmap_error = new_tileset.exportrgb(VK_FileDirectory+"/levels/tilesets/exports/LEVEL"+str_to_format(level_id)+"_TILESET.h",std::string("level") + str_to_format(level_id) + std::string("_tileset"));
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}
	}else{
		bitmap_error = new_tileset.exportb(VK_FileDirectory+"/levels/tilesets/exports/LEVEL"+str_to_format(level_id)+"_TILESET.h",std::string("level") + str_to_format(level_id) + std::string("_tileset"));
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}
	}
	
	
	// Save the level in a useable format
	std::ofstream exportfile;
	
	name = VK_FileDirectory+"/levels/exports/LEVEL"+str_to_format(level_id)+".h";
		
	// Open the file
	exportfile.open(name,std::ios::binary);
	
	if(!exportfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	
	exportfile << "// Vortakeen Level exported by vkmake\n" << 
		"const unsigned short level" << str_to_format(level_id) << "_width = " << level_head.level_width << ";\n" <<
		"const unsigned short level" << str_to_format(level_id) << "_height = " << level_head.level_height << ";\n" <<
		"const unsigned short level" << str_to_format(level_id) << "_size = " << (level_head.level_width*level_head.level_height) << ";\n" <<
		"const unsigned short level" << str_to_format(level_id) << "_num_tiles = " << (VK_LevelTiles.size()) << ";\n" <<		
		"const unsigned short level" << str_to_format(level_id) << "_data[] = {\n";
	
	for(int e = 0; e < level_head.level_height; e++){
		exportfile << "\t";
		for(int i = 0; i < level_head.level_width; i++){ 
			uint16_t b = -1;
			for ( int c = 0; c < VK_LevelTiles.size(); c++){
				if(VK_LevelTiles[c] == map_data[16+(e*(level_head.level_width))+i]){
					b = c;
					break;
				}
			}
			if(b==-1){
				// Error in level!
				VK_ErrorList.push_back(std::string("vkmlvl: ") +
				VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
				std::string("bad tile to find. tile id - ") + std::to_string(map_data[16+(e*(level_head.level_width))+i]));
				return 1;
			}
			exportfile << "0x" << std::hex << std::setfill('0') << std::setw(4) << b << ", ";
		}
		exportfile << "\n";
	}

	exportfile << "\n// Sprite layer\n\n";

	for(int e = 0; e < level_head.level_height; e++){
		exportfile << "\t";
		for(int i = 0; i < level_head.level_width; i++){ 
			uint16_t b = map_data[16+level_head.plane_size+(e*(level_head.level_width))+i];
			exportfile << "0x" << std::hex << std::setfill('0') << std::setw(4) << b << ", ";
		}
		exportfile << "\n";
	}
	exportfile << "};\n";


	exportfile.close();
	std::cout << "vkmlvl: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;	


	// Save the tile info in a useable format
	exportfile;
	
	name = VK_FileDirectory+"/levels/exports/LEVEL"+str_to_format(level_id)+"_INFO.h";
		
	// Open the file
	exportfile.open(name,std::ios::binary);
	
	if(!exportfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
		VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
		std::string("could not open ") + name);
		return 1;
	}
	
	exportfile << "// Vortakeen Tile Info exported by vkmake\n" << 
		"const unsigned short level" << str_to_format(level_id) << "_tinfo_size = " << std::dec << VK_LevelTiles.size() << ";\n" <<
		"const unsigned short level" << str_to_format(level_id) << "_tinfo[] = {\n";

		int ticnt = 0;
		for ( int ti = 0; ti < VK_LevelTiles.size(); ti ++){
			for(int i = 0; i < 6; i++){
				uint16_t tinfo = VK_LevelTInfo[i][VK_LevelTiles[ti]];
				if(i==0){
					if(ticnt==0){
						if(tinfo == 2){
							ticnt = 1;
						}
						if(tinfo == 4){
							ticnt = 3;
						}
					}else{
						if(tinfo == 2){
							tinfo |= (2-ticnt)<<4;
						}
						if(tinfo == 4){
							tinfo |= (4-ticnt)<<4;
						}
						ticnt -= 1;
					}
				}
				exportfile << "0x" << std::hex << std::setfill('0') << std::setw(4) << tinfo << ", ";
			}
			exportfile << "\n";
		}
	exportfile << "};\n";

	exportfile.close();
	std::cout << "vkmlvl: " << 
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) << 
			"info: " << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"closed " << name << std::endl;	

	return 0;
};

std::string VK_IsLevelFile(std::string name){
	std::string out = "";
	bool is_file = false;
	for(int i = name.length()-1; i >= 0; i--){
		if(name[i] == '.'){
			// It's a file
			is_file = true;
		}
		if(name[i] == '/' || name[i] == '\\'){
			if(is_file){
				return out;
			}else{
				return "";
			}
		}
		out.insert (0, 1, name[i]);
	}
	return "";
};

int VK_ComparePart(std::string a, std::string b){
	if(b.length() > a.length()){
		return -1;
	}
	for(int c = 0; c < b.length(); c++){
		if(b[c] != a[c]){
			return 1;
		}
	}
	return 0;
};

int VK_FindLevelID(std::string name){
	int value = 0;
	if(VK_ComparePart(name,"LEVEL")==0){
		for(int i = 5; i < 8; i++){
			if(name[i]>='0'&&name[i]<='9'){
				value *= 10;
				value += name[i]-'0';
				//std::cout << value << std::endl;
			}
		}
	}
	return value;
};


int VK_SaveSpecialTiles(){
	// Load the special tiles
	std::ifstream tilesfile(VK_FileDirectory+"/Special_Tiles.txt");
	if(!tilesfile.is_open()){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open "+VK_FileDirectory+"/Special_Tiles.txt"));
		return 1;		
	}
	
	while(!tilesfile.eof()){
		tile_const t;
		tilesfile >> t.name >> t.value;
		if(t.name.length()){
			const_tiles.push_back(t);
		}
	}
	
	// Open 1TIL0000.BMP
	// And Create new tileset for special tiles
	
	VKFM_Bitmap tileset_bmp;
	std::string tilesetname = "TIL0000.BMP";
	/*
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		std::string tilesetname = "HDTIL0000.BMP";
	}*/
		
	VK_BMPERRORS bitmap_error = tileset_bmp.load(VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+tilesetname);
	
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	
	// Copy tileset	

	VKFM_Bitmap new_tileset;
	tileset_bmp.copy_heads(new_tileset);

	VKFM_Bitmap tile_bmp;

	int tileset_x = 0;
	int tileset_y = 0;
	
	int place_x = 0;
	int place_y = 0;
	
	for(auto t : const_tiles){
		std::cout << t.name << " - " << t.value << std::endl;
		auto placetile = [&](int tile) { 
			tileset_y = (tile) / 13;
			tileset_x = (tile) - (tileset_y*13);
			
			tileset_bmp.copy(tile_bmp);
			tile_bmp.crop_bmp(tile_bmp,tileset_x*16,tileset_y*16,16,16);
			new_tileset.stitch(tile_bmp,place_x*16,place_y*16);
			
			place_x ++;
			if(place_x >= 8){
				place_x = 0;
				place_y ++;
			}
		};

		if(t.name.compare("INFO_KEYS")==0||
			t.name.compare("INFO_NO_ITEMS")==0||
			t.name.compare("INFO_ITEMS")==0){
			for(int i = 0; i < 4; i++){
				placetile(t.value+i);
			}
		}else{
			if(t.name.compare("INFO_GUN")==0||
			t.name.compare("INFO_POGO")==0){
				placetile(t.value);
			}
		}

		
	}
	
	bitmap_error = new_tileset.save(VK_FileDirectory+"/graph/misc/SPECIAL_TILES.BMP");
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error saving bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	// Export the tileset
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		bitmap_error = new_tileset.exportrgb(VK_FileDirectory+"/graph/misc/exports/SPECIAL_TILES.h",std::string("SPECIAL_TILES"));
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}
	}else{
		bitmap_error = new_tileset.exportb(VK_FileDirectory+"/graph/misc/exports/SPECIAL_TILES.h",std::string("SPECIAL_TILES"));
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}

	}
	
	
	return 0;
};
	

void VK_CompileLevels(){
	if(! (VKMake_Flags&static_cast<int>(VKFLAG::LEVELS_ONLY)) ){
		// We didn't want to make the levels!
		return;
	}
	std::string level_dir = VK_FileDirectory+"/levels";
	// Check if the directory exist
	std::filesystem::path level_path(level_dir);
	if(!std::filesystem::exists(level_path)){
		VK_ErrorList.push_back(std::string("vkmlvl: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open directory ") + level_dir);
		return;
	}

	// Create all the needed directories
	std::filesystem::create_directory(VK_FileDirectory+"/graph/sheets");
	std::filesystem::create_directory(VK_FileDirectory+"/levels/exports");
	std::filesystem::create_directory(VK_FileDirectory+"/levels/tilesets");
	std::filesystem::create_directory(VK_FileDirectory+"/levels/tilesets/exports");

	
	// Find all levels in directory
	std::string path = VK_FileDirectory+"/levels";
	std::vector<std::string> files;
	for (const auto & entry : std::filesystem::directory_iterator(path)){
		std::string n = VK_IsLevelFile(entry.path());
		if(n.length())
			files.push_back(n);
	}
	if(files.size()){
		VK_LevelList.clear();
		for( auto f : files){
			// Strip the names
			VK_LevelList.push_back(VK_FindLevelID(f));
		}
	}
	

	VK_SaveSpecialTiles();
	
	for(int level : VK_LevelList){
		if(VK_LoadLevel(level)){
			continue;
		}
	}
	
};

