/*
 * VKFM_Bitmap.cpp
 *
 *  Created on: 06.16.2022
 *      Author: John314
 *
*/

#include "VKFM_Heads.h"



std::string str_to_format(const int number,int width,char fill ) {
	// From: https://stackoverflow.com/questions/35608291/formatting-how-to-convert-1-to-01-2-to-02-3-to-03-and-so-on
    std::stringstream ss;
    ss << std::setw(width) << std::setfill(fill) << number;
    return ss.str();
};

void fix_uint16(uint16_t &i){
	// Reorder the bytes to little-endian
	uint16_t t = i;
	i = ((t&0xFF)<<8) | ((t>>8)&0xFF);

	//std::cout << " 0x" << std::hex << (i&0xFF) << " 0x" << ((i>>8)&0xFF) << std::endl;
};

void fix_uint32(uint32_t &i){
	// Reorder the bytes little-endian
	uint32_t t = i;
	/*
	 * ABCD -> DCBA
	*/
	//printf("%u\n",t);
	//std::cout << " 0x" << std::hex << (unsigned int)(t) << std::endl;
	i = ((t&0xFF)<<24) | (((t>>8)&0xFF) << 16);
	i |= (((t>>16)&0xFF)<<8) | (((t>>24)&0xFF));
	
};

/*
void read_bytes(std::ifstream &file,char * data, int len){
	if(!file.is_open()) return ; // Can't help you
	
	for(int i = 0; i < len; i++){
		char b = 0;
		file.read(&b,1);
		std::cout << std::hex << "0x" << (int)(b) << " "; 
		data[i] = b;
	}
};
*/
