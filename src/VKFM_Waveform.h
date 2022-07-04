#pragma once


struct WaveHeader {
	uint8_t riff_checksum[4]; // 4 byte checksum
	uint32_t chunk_length; // length of the file minus the checksum
	uint8_t wave_checksum[4]; // 4 byte checksum
	uint8_t format_checksum[4]; // 4 byte checksum
	uint32_t format_length; // length of this chunk
	uint16_t format_type; // type of format
	uint16_t num_channels; // number of channels
	uint32_t sample_rate; // sample rate of wave file
	uint32_t sample_count; // (Sample Rate * BitsPerSample * Channels) / 8.
	uint16_t sample_format; // (BitsPerSample * Channels) / 8.  1 - 8 bit mono  2 - 8 bit stereo/16 bit mono    4 - 16 bit stereo
	uint16_t bits_per_sample; // bits per sample
	uint8_t data_checksum[4]; // 4 byte checksum
	uint32_t data_length; // length of the data

	void write_bytes(std::ofstream &file){
		for(int i = 0; i < 4; i++){
			file.write(reinterpret_cast<char *>(&riff_checksum[i]), sizeof(uint8_t));
		}
		file.write(reinterpret_cast<char *>(&chunk_length), sizeof(uint32_t));
		for(int i = 0; i < 4; i++){
			file.write(reinterpret_cast<char *>(&wave_checksum[i]), sizeof(uint8_t));
		}
		for(int i = 0; i < 4; i++){
			file.write(reinterpret_cast<char *>(&format_checksum[i]), sizeof(uint8_t));
		}
		file.write(reinterpret_cast<char *>(&format_length), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&format_type), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&num_channels), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&sample_rate), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&sample_count), sizeof(uint32_t));
		file.write(reinterpret_cast<char *>(&sample_format), sizeof(uint16_t));
		file.write(reinterpret_cast<char *>(&bits_per_sample), sizeof(uint16_t));
		for(int i = 0; i < 4; i++){
			file.write(reinterpret_cast<char *>(&data_checksum[i]), sizeof(uint8_t));
		}
		file.write(reinterpret_cast<char *>(&data_length), sizeof(uint32_t));
	};

};


enum class VK_WAVERRORS {
	NO_ERROR 		= 0x00,
	FAILED_OPEN 	= 0x01,
};


class VKFM_Waveform {
public:
	WaveHeader wavehead;

	std::vector<uint8_t> wave_data;
	
	VK_WAVERRORS load(std::string name){
				
		return VK_WAVERRORS::NO_ERROR;
	};
	
	VK_WAVERRORS save(std::string name){
		
		std::ofstream wave_file;
		
		wave_file.open(name, std::ios::binary);
		
		if(!wave_file.is_open()){
			// Bad file!
			return VK_WAVERRORS::FAILED_OPEN;
		}
		
		// Write the wave header
		wavehead.write_bytes(wave_file);
		
		// Write the wave form
		for(auto form : wave_data){
			wave_file.write(reinterpret_cast<char *>(&form), sizeof(uint8_t));
		}
		
		wave_file.close();
		
		return VK_WAVERRORS::NO_ERROR;
	};
	
	
	void VK_WriteWaveHeader(){

		wavehead.riff_checksum[0] = 0x52;
		wavehead.riff_checksum[1] = 0x49;
		wavehead.riff_checksum[2] = 0x46;
		wavehead.riff_checksum[3] = 0x46;

		//uint32_t chunk_length; // length of the file

		wavehead.wave_checksum[0] = 0x57;
		wavehead.wave_checksum[1] = 0x41;
		wavehead.wave_checksum[2] = 0x56;
		wavehead.wave_checksum[3] = 0x45;

		wavehead.format_checksum[0] = 0x66;
		wavehead.format_checksum[1] = 0x6D;
		wavehead.format_checksum[2] = 0x74;
		wavehead.format_checksum[3] = 0x20;

		wavehead.format_length = 0x10;

		wavehead.format_type = 0x1;
		wavehead.num_channels = 0x1;

		wavehead.sample_rate = 0x5622;
		wavehead.sample_count = 0x5622;
		wavehead.sample_format = 0x1;
		wavehead.bits_per_sample = 0x8;

		wavehead.data_checksum[0] = 0x64;
		wavehead.data_checksum[1] = 0x61;
		wavehead.data_checksum[2] = 0x74;
		wavehead.data_checksum[3] = 0x61;

	};
	
	void VK_ClearData(){
		wave_data.clear();
	};
	
	void VK_WriteWaveData(std::vector<uint8_t> &data){
		// Always start with no sound
		//wave_data.push_back(0x80);

		// Add the data to the wave
		wave_data.insert(wave_data.end(), data.begin(), data.end());

		wavehead.data_length = wave_data.size();

		wavehead.chunk_length = (sizeof(WaveHeader) - 4)+ (wavehead.data_length);

	};
	
	void VK_GenerateSquareWave(std::vector<uint8_t> &wave_data, int freq, int amp,float len){
		// Remove any old data (Warning! Will remove any and all data)
		wave_data.clear();
		
		// Generate wave data
		int rate_count = 0;
		int rate_form = 0;
		int sample_length = (float)(wavehead.sample_rate) * (len/1000.0f);

		
		if(freq==0){
			// Generate no sound
			for(int i = 0; i < sample_length; i++){
				// Generate silence
				wave_data.push_back(0x7f);

			}
			return;
		}
		int sample_freq = wavehead.sample_rate / freq;

		
		for(int i = 0; i < sample_length; i++){
			// Generate a square wave
			rate_count += 1;
			if(rate_count < (sample_freq/2)){
				rate_form = 0x7f-amp;
			}else
			if(rate_count < (sample_freq)){
				rate_form = 0xff-(0x7f-amp);
			}else{
				rate_count = 0;
				rate_form = 0x7f-amp;
			}
			wave_data.push_back(rate_form);

		}
	};

};


