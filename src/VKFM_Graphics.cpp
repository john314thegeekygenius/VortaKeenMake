/*
 * VKFM_Graphics.cpp
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/

#include "VKFM_Heads.h"


int VK_MakeFont(){
	// Open 1FON0000.BMP
	// Move every other 8 rows into row above
	
	/*
	 * aaaaaaaaaaaaaa
	 * bbbbbbbbbbbbbb
	 * 
	 * becomes
	 * 
	 * aaaaaaaaaaaaaabbbbbbbbbbbbbb
	 * 
	*/
	
	VKFM_Bitmap font_bmp;
	
	std::string bitmapname = "FON0000.BMP";
	/*
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		// We want full color!
		bitmapname = "HDFON0000.BMP";
	}*/
	
	VK_BMPERRORS bitmap_error = font_bmp.load(VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+bitmapname);
	
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		VK_ErrorList.push_back(std::string("vkmgfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error loading bitmap: code ") + std::to_string((int)bitmap_error));
		std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	
	// Copy tileset	

	VKFM_Bitmap new_font;
	font_bmp.copy_heads(new_font);

	VKFM_Bitmap fontrow_bmp;
	for(int fontrow = 0; fontrow < 8; fontrow ++){
		font_bmp.copy(fontrow_bmp);
		fontrow_bmp.crop_bmp(fontrow_bmp,0,fontrow*16,128,8);
		new_font.stitch(fontrow_bmp,0,fontrow*8);

		font_bmp.copy(fontrow_bmp);
		fontrow_bmp.crop_bmp(fontrow_bmp,0,8+(fontrow*16),128,8);
		new_font.stitch(fontrow_bmp,128,fontrow*8);
		
	}
	
	bitmap_error = new_font.save(VK_FileDirectory+"/graph/sheets/CK_FONT.BMP");
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		VK_ErrorList.push_back(std::string("vkmgfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error saving bitmap: code ") + std::to_string((int)bitmap_error));
		std::cout << "Error saving bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	
	// Export the tileset
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		bitmap_error = new_font.exportrgb(VK_FileDirectory+"/graph/sheets/exports/CK_FONT.h","ck_font");
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}
	}else{
		bitmap_error = new_font.exportb(VK_FileDirectory+"/graph/sheets/exports/CK_FONT.h","ck_font");
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}
	}
	return 0;
};



int VK_MakeSpriteSheet(int sprstart, int sprend, std::string sprname){
	int place_x = 0;

	// Sheet to write to
	VKFM_Bitmap sprite_sheet;
	VK_BMPERRORS bitmap_error;
	
	int spr_width = 0;
	int spr_height = 0;


	// make the file name
	std::string bmpname = VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+"SPR"+str_to_format(sprstart,4)+".BMP";
	
	// Original sheet
	VKFM_Bitmap tempbmp;
	
	// Open the sprite
	bitmap_error = tempbmp.load(bmpname);
	
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	} 

	tempbmp.copy_heads(sprite_sheet);
	
	// Go through all the bitmaps and find the largest one.
	for(int sprite_num = sprstart; sprite_num <= sprend; sprite_num++){
		// make the file name
		std::string bmpname = VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+"SPR"+str_to_format(sprite_num,4)+".BMP";
		
		// Original sheet
		VKFM_Bitmap bmp_sprite;
		
		// Open the sprite
		bitmap_error = bmp_sprite.load(bmpname);
		
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
			return 1;
		} 			

		if(spr_width < bmp_sprite.dib_head.bitmap_width){
			spr_width = bmp_sprite.dib_head.bitmap_width;
		}
		if(spr_height < bmp_sprite.dib_head.bitmap_height){
			spr_height = bmp_sprite.dib_head.bitmap_height;
		}
	}
	uint32_t fsw = spr_width/2;
	uint32_t fsh = spr_height;
	
	for(int sprite_num = sprstart; sprite_num <= sprend; sprite_num++){
		// make the file name
		std::string bmpname = VK_FileDirectory+"/graph/"+std::to_string(VK_EpisodeNumber)+"SPR"+str_to_format(sprite_num,4)+".BMP";
		
		// Original sheet
		VKFM_Bitmap bmp_sprite;
		
		// Open the sprite
		bitmap_error = bmp_sprite.load(bmpname);
		
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
			return 1;
		} 
		std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			"loaded " << bmpname << std::endl;
			
/*
		if(spr_width == 0){
			spr_width = bmp_sprite.dib_head.bitmap_width;
			spr_height = bmp_sprite.dib_head.bitmap_height;
		}
*/
		uint32_t cw = spr_width/2;
		uint32_t ch = spr_height;
		
		
		if(bmp_sprite.dib_head.bitmap_width<spr_width||bmp_sprite.dib_head.bitmap_height<spr_height){
			std::cout << std::string("vkmgfx: ") <<
				VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("warning: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
				"bitmap is smaller than largest in group - " << bmpname << std::endl << "bitmap will be resized in final image" << std::endl;
			
			bmp_sprite.crop_bmp(bmp_sprite,0,0,(bmp_sprite.dib_head.bitmap_width/2),bmp_sprite.dib_head.bitmap_height);
			if((VKMake_Flags&static_cast<int>(VKFLAG::HD_SPRITES)) ){
				bmp_sprite.resize_bmp(bmp_sprite,cw,ch,0);
			}else{
				bmp_sprite.resize_bmp(bmp_sprite,cw,ch,0x10);
			}
			
		}/*else if(bmp_sprite.dib_head.bitmap_width>spr_width||bmp_sprite.dib_head.bitmap_height>spr_height){
			std::cout << std::string("vkmgfx: ") <<
				VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("warning: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
				"bitmap is larger than first - " << bmpname << std::endl << "bitmap will be cropped in final image" << std::endl;

		}*/
		else{
			bmp_sprite.crop_bmp(bmp_sprite,0,0,cw,ch);
		}
		
		// Fix the size
		int divch = ch/8;
		int divcw = cw/8;
		bool changedsize = false;
		if((divch*8)<ch){
			ch = (divch*8)+8;
			changedsize = true;
			fsh = ch;
		}
		if((divcw*8)<cw){
			cw = (divcw*8)+8;
			changedsize = true;
			fsw = cw;
		}
		if(changedsize){
			bmp_sprite.resize_bmp(bmp_sprite,cw,ch,0x10);
		}
		
		std::cout<< std::dec << cw << " x " << ch << std::endl;
						
		sprite_sheet.stitch(bmp_sprite,place_x,0);

		place_x += cw;
	}
	std::cout << "writing bitmap..." << std::endl;
	
	// Save the bitmap
	bitmap_error = sprite_sheet.save(VK_FileDirectory+"/graph/sheets/"+sprname+".BMP");
	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error saving bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	}
	std::cout << std::string("vkmgfx: ") <<
		VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
		std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
		"saved bitmap " << sprname << std::endl;
			
	// Export the tileset
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_SPRITES)) ){
		bitmap_error = sprite_sheet.exportsprgb(VK_FileDirectory+"/graph/sheets/exports/"+sprname+".h",sprname,fsw,fsh);
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}	
	}else{
		bitmap_error = sprite_sheet.exportspr(VK_FileDirectory+"/graph/sheets/exports/"+sprname+".h",sprname,fsw,fsh);
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}	

	}

	std::cout << std::string("vkmgfx: ") <<
		VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
		std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
		"exported " << sprname << std::endl;
	
	return 0;
};



int VK_ExportBMP(std::string bmpname, std::string sprname){

	// Original sheet
	VKFM_Bitmap bitmap;

	// Open the sprite
	VK_BMPERRORS bitmap_error = bitmap.load(bmpname);

	if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
		std::cout << "Error loading bitmap! Error code " << (int)bitmap_error << std::endl;
		return 1;
	} 
	std::cout << std::string("vkmgfx: ") <<
	VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
		std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
		"loaded " << bmpname << std::endl;

	// Export the bitmap
	if((VKMake_Flags&static_cast<int>(VKFLAG::HD_TEXTURES)) ){
		bitmap_error = bitmap.exportrgb(VK_FileDirectory+"/graph/misc/exports/"+sprname+".h",sprname);
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}	
	}else{
		bitmap_error = bitmap.exportb(VK_FileDirectory+"/graph/misc/exports/"+sprname+".h",sprname);
		if(bitmap_error!=VK_BMPERRORS::NO_ERROR){
			std::cout << "Error saving header! Error code " << (int)bitmap_error << std::endl;
			return 1;
		}	
	}


	std::cout << std::string("vkmgfx: ") <<
		VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
		std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
		"exported " << sprname << std::endl;

	return 0;
};



struct VK_SpriteInfo {
	std::string exportname;
	int start;
	int end;
};


void VK_CompileGraphics(){
	if(! (VKMake_Flags&static_cast<int>(VKFLAG::GRAPHICS_ONLY)) ){
		// We didn't want to make the graphics!
		return;
	}
	std::string graph_dir = VK_FileDirectory+"/graph";
	// Check if the directory exist
	std::filesystem::path graph_path(graph_dir);
	if(!std::filesystem::exists(graph_path)){
		 
		VK_ErrorList.push_back(std::string("vkmgfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open directory ") + graph_dir);
		return;
	}

	// Create all the needed directories
	std::filesystem::create_directory(VK_FileDirectory+"/graph/misc/exports");
	std::filesystem::create_directory(VK_FileDirectory+"/graph/sheets");
	std::filesystem::create_directory(VK_FileDirectory+"/graph/sheets/exports");
	
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("creating font bitmap...") << std::endl;
	VK_MakeFont();
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("done creating font bitmap") << std::endl;
	
	// Open SPRLIST.TXT
	std::ifstream sprlist;
	
	sprlist.open(VK_FileDirectory+"/graph/SPRLIST.TXT");
	
	if(!sprlist.is_open()){
		VK_ErrorList.push_back(std::string("vkmgfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open SPRLIST.TXT "));
	}
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("opened SPRLIST.TXT ") << std::endl;
	
	std::vector<VK_SpriteInfo> sprinfo;
	// Read contents into vector
	while(1){
		VK_SpriteInfo info;
		if(sprlist.eof()){
			break;
		}
		sprlist >> info.exportname >> info.start >> info.end;
		if(info.exportname.length()){
			sprinfo.push_back(info);
		}else{
			break;
		}
	}
	
	sprlist.close();
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("closed SPRLIST.TXT ") << std::endl;

	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("creating sprite sheets...") << std::endl;
	
	for(VK_SpriteInfo i : sprinfo){
		VK_MakeSpriteSheet(i.start,i.end,i.exportname);

		std::cout << std::string("vkmgfx: ") <<
				VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("done creating ") << i.exportname << " bitmap" << std::endl;

	}

	////////////////////////////

	sprinfo.clear();

	// Open BMPLIST.TXT
	std::ifstream bmplist;
	
	bmplist.open(VK_FileDirectory+"/graph/misc/BMPLIST.TXT");
	
	if(!bmplist.is_open()){
		VK_ErrorList.push_back(std::string("vkmgfx: ") +
			VK_SetConsoleColor(VK_CONSOLE_COLORS::PURPLE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("error: ") + VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) +
			std::string("could not open BMPLIST.TXT "));
	}
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("opened BMPLIST.TXT ") << std::endl;
	

	// Read contents into vector
	while(1){
		VK_SpriteInfo info;
		if(bmplist.eof()){
			break;
		}
		bmplist >> info.exportname >> info.start ;
		if(info.exportname.length()){
			sprinfo.push_back(info);
		}else{
			break;
		}
	}
	
	bmplist.close();
	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("closed BMPLIST.TXT ") << std::endl;

	std::cout << std::string("vkmgfx: ") <<
			VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
			std::string("creating bitmap sheets...") << std::endl;
	
	for(VK_SpriteInfo i : sprinfo){
		
		// make the file name
		std::string bmpname = VK_FileDirectory+"/graph/misc/"+std::to_string(VK_EpisodeNumber)+"BMP"+str_to_format(i.start,4)+".BMP";

		VK_ExportBMP(bmpname,i.exportname);


		std::cout << std::string("vkmgfx: ") <<
				VK_SetConsoleColor(VK_CONSOLE_COLORS::YELLOW,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("info: ") << VK_SetConsoleColor(VK_CONSOLE_COLORS::WHITE,VK_CONSOLE_COLORS::DEFAULT) <<
				std::string("done creating ") << i.exportname << " bitmap" << std::endl;

	}
	
	// Export EXT0000.BMP
	
	// make the file name
	std::string bmpname = VK_FileDirectory+"/graph/misc/"+std::to_string(VK_EpisodeNumber)+"EXT0000.BMP";

	VK_ExportBMP(bmpname,"ENDSCREEN");
	

};

