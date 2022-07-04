#pragma once

void fix_uint16(uint16_t &i);
void fix_uint32(uint32_t &i);

// Moved from main
std::string str_to_format(const int number,int width = 2,char fill = '0');

//void read_bytes(std::ifstream &file,char * data, int len);

const int BITMAPINFOHEADER = 40;

const int BITMAP_BI_RGB = 0;
const int BITMAP_BI_RLE8 = 1;
const int BITMAP_BI_RLE4 = 2;

struct DIB_BitmapHeader {
	uint32_t size; // size of this header
	uint32_t bitmap_width; // width of the bitmap
	uint32_t bitmap_height; // height of the bitmap
	uint16_t color_planes; // number of color planes (must be 1)
	uint16_t bpp; // number of bits per pixel
	uint32_t comp_meth; // compression method
	uint32_t image_size; // size of the image
	uint32_t h_res; // horizontal resolution of the image
	uint32_t v_res; // vertical resolution of the image
	uint32_t num_colors; // number of colors in color palette
	uint32_t important_colors; // number of important colors used

	void read_bytes(std::ifstream &file){
		file.read(reinterpret_cast<char *>(&size), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&bitmap_width), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&bitmap_height), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&color_planes), sizeof(uint16_t));
		file.read(reinterpret_cast<char *>(&bpp), sizeof(uint16_t));
		file.read(reinterpret_cast<char *>(&comp_meth), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&image_size), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&h_res), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&v_res), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&num_colors), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&important_colors), sizeof(uint32_t));
		
	};

	void write_bytes(std::ofstream &file){
		file.write(reinterpret_cast<char *>(&size), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&bitmap_width), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&bitmap_height), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&color_planes), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&bpp), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&comp_meth), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&image_size), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&h_res), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&v_res), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&num_colors), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&important_colors), sizeof(uint32_t));
		
	};	
	
	void reorder_bytes(){
		fix_uint32(size);
		fix_uint32(bitmap_width);
		fix_uint32(bitmap_height);
		fix_uint16(color_planes);
		fix_uint16(bpp);
		fix_uint32(comp_meth);
		fix_uint32(image_size);
		fix_uint32(h_res);
		fix_uint32(v_res);
		fix_uint32(num_colors);
		fix_uint32(important_colors);
	};
};

struct BitmapHeader {
	uint16_t checksum; // 2 byte checksum
	uint32_t filesize; // size of bitmap file in bytes
	uint32_t reserved; // 2 reserved words
	uint32_t offset; // offset to bitmap pixels
	
	void read_bytes(std::ifstream &file){
		file.read(reinterpret_cast<char *>(&checksum), sizeof(uint16_t));
		file.read(reinterpret_cast<char *>(&filesize), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&reserved), sizeof(uint32_t));
		file.read(reinterpret_cast<char *>(&offset), sizeof(uint32_t));
		
	};

	void write_bytes(std::ofstream &file){
		file.write(reinterpret_cast<char *>(&checksum), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&filesize), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&reserved), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&offset), sizeof(uint32_t));
		
	};
				
	void reorder_bytes(){
		fix_uint16(checksum);
		fix_uint32(filesize);
		fix_uint32(reserved);
		fix_uint32(offset);
	};
};


enum class VK_BMPERRORS {
	NO_ERROR 		= 0x00,
	FAILED_OPEN 	= 0x01,
	BAD_BMP 		= 0x02,
	EOF_EARLY 		= 0x03,
	BAD_DIB_HEADER 	= 0x04,
	BAD_COMPRESSION = 0x05,
	BAD_PIXEL_DATA 	= 0x06,
	BMP_DIV_8       = 0x08,
	BAD_BPP 		= 0x10,
};

struct Bitmap_Color {
	uint8_t r,g,b,a;
};


class VKFM_Bitmap {
public:
	BitmapHeader bmphead;
	DIB_BitmapHeader dib_head;

	bool bits_need_fliped = false;

	std::vector<uint8_t> pixel_data;
	std::vector<Bitmap_Color> palette;
	
	VK_BMPERRORS load(std::string name){
				
		std::ifstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		// Read the Bitmap header
		bmphead.read_bytes(bitmap_file);

		//bitmap_file.read(reinterpret_cast<char *>(&testb), sizeof(testb));
		
		if(bmphead.checksum == 0x424D){
			// Try swaping bytes arround
			bits_need_fliped = true;
		}
		
		if(bits_need_fliped){
			bmphead.reorder_bytes();
		}
		
		// Make sure it's a valid bitmap
		if(bmphead.checksum != 0x4D42){
			// It's a bad bitmap!
			bitmap_file.close();
			return VK_BMPERRORS::BAD_BMP;
		}

		// Read the DIB header
		dib_head.read_bytes(bitmap_file);

		if(bits_need_fliped){
			dib_head.reorder_bytes();
		}

		
		if(bitmap_file.eof()){
			// Problem reading the file
			bitmap_file.close();
			return VK_BMPERRORS::EOF_EARLY;
		}
		
		
		if(dib_head.size > BITMAPINFOHEADER){
			std::cout << "Warning : Bitmap header not correct size! Skipping bytes" << std::endl;
			for(int i = BITMAPINFOHEADER; i < dib_head.size; i++){
				uint8_t j;
				bitmap_file.read(reinterpret_cast<char *>(&j), sizeof(uint8_t));
			}
			
			dib_head.size = BITMAPINFOHEADER;
		}

		if(dib_head.size < BITMAPINFOHEADER){
			// We don't know how to handle anything else!
			// Problem reading the file
			bitmap_file.close();
			return VK_BMPERRORS::BAD_DIB_HEADER;
		}
		
		if(!(dib_head.bpp==4||dib_head.bpp==8)){
			// We don't know how to handle anything else!
			// Problem reading the file
			bitmap_file.close();
			return VK_BMPERRORS::BAD_BPP;
		}
		
		if(dib_head.comp_meth != BITMAP_BI_RGB){
			// We don't know how to handle anything else!
			// Problem reading the file
			bitmap_file.close();
			return VK_BMPERRORS::BAD_COMPRESSION;
		}
		
		
		// Load the palette BGRX
		for(int index = 0; index < (1<<dib_head.bpp); index ++){
			uint8_t blue, green, red, alpha;
			bitmap_file.read(reinterpret_cast<char *>(&blue), sizeof(uint8_t));
			bitmap_file.read(reinterpret_cast<char *>(&green), sizeof(uint8_t));
			bitmap_file.read(reinterpret_cast<char *>(&red), sizeof(uint8_t));
			bitmap_file.read(reinterpret_cast<char *>(&alpha), sizeof(uint8_t));
			
			palette.push_back({red,green,blue,alpha});
		}
		
		int row_size = ceil( (dib_head.bpp * dib_head.bitmap_width) / 32) * 4;
		
		
		for(int row = dib_head.bitmap_height-1; row >= 0; row--){
			for(int x = 0; x < row_size; x++){
				uint8_t pixel;
				// Grab the byte
				bitmap_file.read(reinterpret_cast<char *>(&pixel), sizeof(uint8_t));
				
				if(dib_head.bpp == 8){
					// Push it into the data stream
					pixel_data.push_back(pixel);
				}else if(dib_head.bpp == 4){
					// Split the byte into 2 bytes
					uint8_t pixel_h = (pixel>>4)&0xF;

					// Push them into the data stream
					pixel_data.push_back(pixel_h);
					pixel_data.push_back(pixel&0xF);
				}
			}
		}
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	VK_BMPERRORS save(std::string name){
		BitmapHeader dummy_bmphead;
		DIB_BitmapHeader dummy_dib_head;
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
				
		// Write the Bitmap header
		
		memcpy(&dummy_bmphead,&bmphead,sizeof(bmphead));

		if(bits_need_fliped){
			dummy_bmphead.reorder_bytes();
		}

		// Fix header
		dummy_bmphead.offset = sizeof(DIB_BitmapHeader)+(sizeof(BitmapHeader)-2)+((1<<dib_head.bpp)*4);
		if(dib_head.bpp==4){
			dummy_bmphead.filesize = dummy_bmphead.offset+((dib_head.bitmap_width>>1)*dib_head.bitmap_height);
		}else{
			dummy_bmphead.filesize = dummy_bmphead.offset+(dib_head.bitmap_width*dib_head.bitmap_height);
		}

		dummy_bmphead.write_bytes(bitmap_file);
		
		// Write the DIB header

		memcpy(&dummy_dib_head,&dib_head,sizeof(dib_head));

		if(bits_need_fliped){
			dummy_dib_head.reorder_bytes();
		}
		
		dummy_dib_head.write_bytes(bitmap_file);

		// Write the palette BGRX
		for(auto rgb : palette){
			bitmap_file.write(reinterpret_cast<char *>(&rgb.b), sizeof(uint8_t));
			bitmap_file.write(reinterpret_cast<char *>(&rgb.g), sizeof(uint8_t));
			bitmap_file.write(reinterpret_cast<char *>(&rgb.r), sizeof(uint8_t));
			bitmap_file.write(reinterpret_cast<char *>(&rgb.a), sizeof(uint8_t));
		}
		
		//int row_size = ceil( (dib_head.bpp * dib_head.bitmap_width) / 32) * 4;
		
		if(pixel_data.size()==0){
			// Why is there no data?
			bitmap_file.close();
			return VK_BMPERRORS::BAD_PIXEL_DATA;
		}
		
		for(int row = 0; row < dib_head.bitmap_height; row++){
			for(int x = 0; x < dib_head.bitmap_width; x++){
				uint8_t pixel = 0;
				int offset = (row*dib_head.bitmap_width) + x;
				if(offset >= pixel_data.size()){
					bitmap_file.close();					
					return VK_BMPERRORS::BAD_PIXEL_DATA;
				}

				if(dib_head.bpp==8){
					pixel = pixel_data[offset];
				}else if(dib_head.bpp == 4){
					pixel = ((pixel_data[offset]&0xF)<<4);
					pixel |= pixel_data[offset+1]&0xF;
					x++;
				}
				// Write the byte
				bitmap_file.write(reinterpret_cast<char *>(&pixel), sizeof(uint8_t));
			}
		}
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	VK_BMPERRORS exportr(std::string name, std::string bmp_name){
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		
		// Write some info
		bitmap_file << "// Bitmap exported by vkmake\n" <<
			"const unsigned short " << bmp_name << "_width = " << dib_head.bitmap_width << ";\n" <<
//			"const unsigned short " << bmp_name << "_width = " << ((dib_head.bpp == 8)?dib_head.bitmap_width:(dib_head.bitmap_width>>1)) << ";\n" <<
			"const unsigned short " << bmp_name << "_height = " << dib_head.bitmap_height << ";\n" <<
			"const unsigned short " << bmp_name << "_size = " << (dib_head.bitmap_width*dib_head.bitmap_height) << ";\n" <<
			"const unsigned char " << bmp_name << "_data[] = {\n";

		for(int row = dib_head.bitmap_height-1; row >= 0; row--){
			bitmap_file << "\t";
			for(int x = 0; x < dib_head.bitmap_width; x++){
				int offset = (row*dib_head.bitmap_width) + x;

				if(offset >= pixel_data.size()){
					bitmap_file.close();					
					return VK_BMPERRORS::BAD_PIXEL_DATA;
				}
				//if(dib_head.bpp == 8){
					int pixel = (int)pixel_data[offset];
					if(pixel>15){
						bitmap_file << std::hex << "0x00, ";
					}else{
						pixel += 1;
						bitmap_file << std::hex << "0x" << (int)pixel << ", ";
					}
				/*}else{
					uint8_t pixel = (pixel_data[offset]&0xF) | ((pixel_data[offset+1]&0xF)<<4);
					bitmap_file << std::hex << "0x" << (int)pixel << ", ";
					x ++; // skip the next pixel
				}*/
			}
			bitmap_file << "\n";
		}

		bitmap_file << "\n};\n";
		
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	
	VK_BMPERRORS exportb(std::string name, std::string bmp_name){
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		int ddw = dib_head.bitmap_width / 8;
		int ddh = dib_head.bitmap_height / 8;
		if( (ddw*8 != dib_head.bitmap_width) || (ddh*8 != dib_head.bitmap_height) ){
			// Uh oh!
			return VK_BMPERRORS::BMP_DIV_8;
		}
		
		// Reorder the bytes into GBA 8x8 chunks
		std::vector<uint16_t> gba_chunks;
		int chnk_x = 0, chnk_y = dib_head.bitmap_height-8;
		
		while(chnk_y>=0){

			for(int y = 7; y >= 0; y--){
				for(int x = 0; x < 8; x++){
					int offset = ((y+chnk_y)*dib_head.bitmap_width) + chnk_x + x;

					if(offset >= pixel_data.size()){
						bitmap_file.close();					
						return VK_BMPERRORS::BAD_PIXEL_DATA;
					}
					int pixel = (int)pixel_data[offset];
					if(pixel>15){
						gba_chunks.push_back(0);
					}else{
						pixel += 1;
						gba_chunks.push_back(pixel);
					}
				}
			}
			chnk_x += 8;
			if(chnk_x>=dib_head.bitmap_width){
				chnk_x = 0;
				chnk_y -= 8;
			}
		}
		
		
		// Write some info
		bitmap_file << "// Bitmap exported by vkmake\n" <<
			"const unsigned short " << bmp_name << "_width = " << dib_head.bitmap_width << ";\n" <<
//			"const unsigned short " << bmp_name << "_width = " << ((dib_head.bpp == 8)?dib_head.bitmap_width:(dib_head.bitmap_width>>1)) << ";\n" <<
			"const unsigned short " << bmp_name << "_height = " << dib_head.bitmap_height << ";\n" <<
			"const unsigned short " << bmp_name << "_size = " << (dib_head.bitmap_width*dib_head.bitmap_height) << ";\n" <<
			"const unsigned char " << bmp_name << "_data[] = {\n";
		
		int row = 0;
		bitmap_file << "\t";
		for( uint8_t pixel : gba_chunks){
			bitmap_file << std::hex << "0x" << (int)pixel << ", ";
			if(row++ >= dib_head.bitmap_width){
				row = 0;
				bitmap_file << "\n\t";
			}
		}

		bitmap_file << "\n};\n";
		
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	
	VK_BMPERRORS exportrgb(std::string name, std::string bmp_name){
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		int ddw = dib_head.bitmap_width / 8;
		int ddh = dib_head.bitmap_height / 8;
		if( (ddw*8 != dib_head.bitmap_width) || (ddh*8 != dib_head.bitmap_height) ){
			// Uh oh!
			return VK_BMPERRORS::BMP_DIV_8;
		}
		
		// Reorder the bytes into GBA 8x8 chunks
		std::vector<uint16_t> gba_chunks;
		int chnk_x = 0, chnk_y = dib_head.bitmap_height-8;
		
		while(chnk_y>=0){

			for(int y = 7; y >= 0; y--){
				for(int x = 0; x < 8; x++){
					int offset = ((y+chnk_y)*dib_head.bitmap_width) + chnk_x + x;

					if(offset >= pixel_data.size()){
						bitmap_file.close();					
						return VK_BMPERRORS::BAD_PIXEL_DATA;
					}
					int pixel = (int)pixel_data[offset];
					gba_chunks.push_back(pixel);
				}
			}
			chnk_x += 8;
			if(chnk_x>=dib_head.bitmap_width){
				chnk_x = 0;
				chnk_y -= 8;
			}
		}
		
		
		// Write some info
		bitmap_file << "// Bitmap exported by vkmake\n" <<
			"const unsigned short " << bmp_name << "_width = " << dib_head.bitmap_width << ";\n" <<
//			"const unsigned short " << bmp_name << "_width = " << ((dib_head.bpp == 8)?dib_head.bitmap_width:(dib_head.bitmap_width>>1)) << ";\n" <<
			"const unsigned short " << bmp_name << "_height = " << dib_head.bitmap_height << ";\n" <<
			"const unsigned short " << bmp_name << "_size = " << (dib_head.bitmap_width*dib_head.bitmap_height) << ";\n" <<
			"const unsigned char " << bmp_name << "_data[] = {\n";
		
		int row = 0;
		bitmap_file << "\t";
		for( uint8_t pixel : gba_chunks){
			bitmap_file << std::hex << "0x" << (int)pixel << ", ";
			if(row++ >= dib_head.bitmap_width){
				row = 0;
				bitmap_file << "\n\t";
			}
		}

		bitmap_file << "\n};\n";
		
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	VK_BMPERRORS exportspr(std::string name, std::string bmp_name, int spr_width, int spr_height){
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		int ddw = dib_head.bitmap_width / 8;
		int ddh = dib_head.bitmap_height / 8;
		if( (ddw*8 != dib_head.bitmap_width) || (ddh*8 != dib_head.bitmap_height) ){
			// Uh oh!
			std::cout << "BMP : " << dib_head.bitmap_width << " x " << dib_head.bitmap_height << std::endl;
			return VK_BMPERRORS::BMP_DIV_8;
		}
		
		// Reorder the bytes into GBA 8x8 chunks
		std::vector<uint16_t> gba_chunks;
		int chnk_x = 0, chnk_y = dib_head.bitmap_height-8;
		
		while(chnk_y>=0){

			for(int y = 7; y >= 0; y--){
				for(int x = 0; x < 8; x++){
					int offset = ((y+chnk_y)*dib_head.bitmap_width) + chnk_x + x;

					if(offset >= pixel_data.size()){
						bitmap_file.close();					
						return VK_BMPERRORS::BAD_PIXEL_DATA;
					}
					int pixel = (int)pixel_data[offset];
					if(pixel>15){
						gba_chunks.push_back(0);
					}else{
						pixel += 1;
						gba_chunks.push_back(pixel);
					}
				}
			}
			chnk_x += 8;
			if(chnk_x>=dib_head.bitmap_width){
				chnk_x = 0;
				chnk_y -= 8;
			}
		}
		
		
		// Write some info
		bitmap_file << "// Bitmap exported by vkmake\n" <<
			"const unsigned short " << bmp_name << "_spr_width = " << spr_width << ";\n" <<
			"const unsigned short " << bmp_name << "_spr_height = " << spr_height << ";\n" <<
			"const unsigned short " << bmp_name << "_width = " << dib_head.bitmap_width << ";\n" <<
			"const unsigned short " << bmp_name << "_height = " << dib_head.bitmap_height << ";\n" <<
			"const unsigned short " << bmp_name << "_size = " << (dib_head.bitmap_width*dib_head.bitmap_height) << ";\n" <<
			"const unsigned char " << bmp_name << "_data[] = {\n";
		
		int row = 0;
		bitmap_file << "\t";
		for( uint8_t pixel : gba_chunks){
			bitmap_file << std::hex << "0x" << (int)pixel << ", ";
			if(row++ >= dib_head.bitmap_width){
				row = 0;
				bitmap_file << "\n\t";
			}
		}

		bitmap_file << "\n};\n";
		
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	
	VK_BMPERRORS exportsprgb(std::string name, std::string bmp_name, int spr_width, int spr_height){
				
		std::ofstream bitmap_file;
		
		bitmap_file.open(name, std::ios::binary);
		
		if(!bitmap_file.is_open()){
			// Bad file!
			return VK_BMPERRORS::FAILED_OPEN;
		}
		
		int ddw = dib_head.bitmap_width / 8;
		int ddh = dib_head.bitmap_height / 8;
		if( (ddw*8 != dib_head.bitmap_width) || (ddh*8 != dib_head.bitmap_height) ){
			// Uh oh!
			std::cout << "BMP : " << dib_head.bitmap_width << " x " << dib_head.bitmap_height << std::endl;
			return VK_BMPERRORS::BMP_DIV_8;
		}
		
		// Reorder the bytes into GBA 8x8 chunks
		std::vector<uint16_t> gba_chunks;
		int chnk_x = 0, chnk_y = dib_head.bitmap_height-8;
		
		while(chnk_y>=0){

			for(int y = 7; y >= 0; y--){
				for(int x = 0; x < 8; x++){
					int offset = ((y+chnk_y)*dib_head.bitmap_width) + chnk_x + x;

					if(offset >= pixel_data.size()){
						bitmap_file.close();					
						return VK_BMPERRORS::BAD_PIXEL_DATA;
					}
					int pixel = (int)pixel_data[offset];
					gba_chunks.push_back(pixel);
				}
			}
			chnk_x += 8;
			if(chnk_x>=dib_head.bitmap_width){
				chnk_x = 0;
				chnk_y -= 8;
			}
		}
		
		
		// Write some info
		bitmap_file << "// Bitmap exported by vkmake\n" <<
			"const unsigned short " << bmp_name << "_spr_width = " << spr_width << ";\n" <<
			"const unsigned short " << bmp_name << "_spr_height = " << spr_height << ";\n" <<
			"const unsigned short " << bmp_name << "_width = " << dib_head.bitmap_width << ";\n" <<
			"const unsigned short " << bmp_name << "_height = " << dib_head.bitmap_height << ";\n" <<
			"const unsigned short " << bmp_name << "_size = " << (dib_head.bitmap_width*dib_head.bitmap_height) << ";\n" <<
			"const unsigned char " << bmp_name << "_data[] = {\n";
		
		int row = 0;
		bitmap_file << "\t";
		for( uint8_t pixel : gba_chunks){
			bitmap_file << std::hex << "0x" << (int)pixel << ", ";
			if(row++ >= dib_head.bitmap_width){
				row = 0;
				bitmap_file << "\n\t";
			}
		}

		bitmap_file << "\n};\n";
		
		
		bitmap_file.close();
		
		return VK_BMPERRORS::NO_ERROR;
	};
	
	void copy(VKFM_Bitmap &copy_bmp){

		memcpy(&copy_bmp.bmphead,&bmphead,sizeof(BitmapHeader));
		memcpy(&copy_bmp.dib_head,&dib_head,sizeof(DIB_BitmapHeader));
		copy_bmp.bits_need_fliped = bits_need_fliped;
		copy_bmp.pixel_data = pixel_data;
		copy_bmp.palette = palette;
	};
	
	void copy_heads(VKFM_Bitmap &copy_bmp){
		memcpy(&copy_bmp.bmphead,&bmphead,sizeof(BitmapHeader));
		memcpy(&copy_bmp.dib_head,&dib_head,sizeof(DIB_BitmapHeader));
		copy_bmp.bits_need_fliped = bits_need_fliped;
		copy_bmp.palette = palette;
		
		copy_bmp.dib_head.bitmap_width = 0;
		copy_bmp.dib_head.bitmap_height = 0;
	};
	
	void copy_heads2(VKFM_Bitmap &copy_bmp){
		memcpy(&copy_bmp.bmphead,&bmphead,sizeof(BitmapHeader));
		memcpy(&copy_bmp.dib_head,&dib_head,sizeof(DIB_BitmapHeader));
		copy_bmp.bits_need_fliped = bits_need_fliped;
		copy_bmp.palette = palette;
	};
	
	
	
	void crop(int x, int y, int w, int h){
		if(dib_head.bitmap_width<=0){
			return;
		}
		if(dib_head.bitmap_height<=0){
			return;
		}

		if(x < 0){
			x = 0;
		}
		if(y < 0){
			y = 0;
		}
		if(x+w > dib_head.bitmap_width){
			w -= (dib_head.bitmap_width-x);
		}
		if(y+h > dib_head.bitmap_height){
			h -= (dib_head.bitmap_height-y);
		}
		if(w < 0) return;
		if(h < 0) return;
		
		std::vector<uint8_t> croped_pixels;
		
		y = (dib_head.bitmap_height -h) - y;
				
		// Crop the image to a certan size
		for(int cy = 0; cy < h; cy ++){
			for(int cx = 0; cx < w; cx ++){
				croped_pixels.push_back(pixel_data[((y+cy)*dib_head.bitmap_width) + ((cx) + x)]);
			}
		}
		dib_head.bitmap_width = w;
		dib_head.bitmap_height = h;
		
		pixel_data.clear();
		
		// Set the data
		for(uint8_t pixel : croped_pixels){
			pixel_data.push_back(pixel);
		}
	};
	
	
	void crop_bmp(VKFM_Bitmap &crop_bmp, int x, int y, int w, int h){
		if(crop_bmp.dib_head.bitmap_width<=0){
			return;
		}
		if(crop_bmp.dib_head.bitmap_height<=0){
			return;
		}

		if(x < 0){
			x = 0;
		}
		if(y < 0){
			y = 0;
		}
		if(x+w > crop_bmp.dib_head.bitmap_width){
			w -= (crop_bmp.dib_head.bitmap_width-x);
		}
		if(y+h > crop_bmp.dib_head.bitmap_height){
			h -= (crop_bmp.dib_head.bitmap_height-y);
		}
		if(w < 0) return;
		if(h < 0) return;
		
		std::vector<uint8_t> croped_pixels;
		
		y = (crop_bmp.dib_head.bitmap_height -h) - y;
				
		// Crop the image to a certan size
		for(int cy = 0; cy < h; cy ++){
			for(int cx = 0; cx < w; cx ++){
				croped_pixels.push_back(crop_bmp.pixel_data[((y+cy)*crop_bmp.dib_head.bitmap_width) + ((cx) + x)]);
			}
		}
		dib_head.bitmap_width = w;
		dib_head.bitmap_height = h;
		
		pixel_data.clear();
		
		// Set the data
		for(uint8_t pixel : croped_pixels){
			pixel_data.push_back(pixel);
		}
	};
	
	void resize_bmp(VKFM_Bitmap &resize_bmp, int w, int h, uint16_t fill){
		
		if(w <= resize_bmp.dib_head.bitmap_width && h <= resize_bmp.dib_head.bitmap_height){
			return; // No need to resize
		}
		if(w <= resize_bmp.dib_head.bitmap_width){
			w = resize_bmp.dib_head.bitmap_width;
		}
		if(h <= resize_bmp.dib_head.bitmap_height){
			h = resize_bmp.dib_head.bitmap_height;
		}
		
		std::vector<uint8_t> new_pixels;
				
		// Crop the image to a certan size
		for(int cy = 0; cy < resize_bmp.dib_head.bitmap_height; cy ++){
			for(int cx = 0; cx < resize_bmp.dib_head.bitmap_width; cx ++){
				new_pixels.push_back(resize_bmp.pixel_data[(cy*resize_bmp.dib_head.bitmap_width) + (cx)]);
			}
			for(int cw = resize_bmp.dib_head.bitmap_width; cw < w; cw++){
				new_pixels.push_back(fill);
			}
		}
		for(int ch = resize_bmp.dib_head.bitmap_height; ch < h; ch++){
			for(int cx = 0; cx < resize_bmp.dib_head.bitmap_width; cx ++){
				new_pixels.push_back(fill);
			}
		}
		dib_head.bitmap_width = w;
		dib_head.bitmap_height = h;
		
		pixel_data.clear();
		
		// Set the data
		for(uint8_t pixel : new_pixels){
			pixel_data.push_back(pixel);
		}
	};
	
	void stitch(VKFM_Bitmap &add_bmp, int x, int y){
		std::vector<uint8_t> copied_pixels;
		int copy_w = dib_head.bitmap_width, copy_h = dib_head.bitmap_height;
		copied_pixels = pixel_data;

		if( x >= dib_head.bitmap_width){
			dib_head.bitmap_width += add_bmp.dib_head.bitmap_width;
		}
		if( y >= dib_head.bitmap_height){
			dib_head.bitmap_height += add_bmp.dib_head.bitmap_height;
		}
		
		pixel_data.clear();
		pixel_data.resize(dib_head.bitmap_width*dib_head.bitmap_height,0xFF);
		
		// Reset the copied pixels
		int cpy_y = (dib_head.bitmap_height)-copy_h;
		
		for(int cy = 0; cy < copy_h; cy ++){
			for(int cx= 0; cx < copy_w; cx ++){
				int offset = cx+((cy+cpy_y)*dib_head.bitmap_width);
				pixel_data[offset] = copied_pixels[(cy*copy_w)+cx];
				
			}
		}
		//
		
		y = (dib_head.bitmap_height-y)-add_bmp.dib_head.bitmap_height;
		
		for(int cy = 0; cy < add_bmp.dib_head.bitmap_height; cy ++){
			for(int cx= 0; cx < add_bmp.dib_head.bitmap_width; cx ++){
				int offset = cx+((cy+y)*dib_head.bitmap_width) + x;//((y<<1)*row_size) + (cy*row_size) ;// + (cx<<1) + x;
				pixel_data[offset] = add_bmp.pixel_data[(cy*add_bmp.dib_head.bitmap_width)+cx];
			}
		}
	};
	
};

