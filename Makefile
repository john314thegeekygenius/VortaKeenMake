
CFLAGS = -DCOLOR_CONSOLE -DLINUX -std=c++20

SRC_FILES = src/VKFM_Console.cpp src/VKFM_Main.cpp src/VKFM_Bitmap.cpp src/VKFM_Graphics.cpp src/VKFM_Levels.cpp src/VKFM_Sounds.cpp src/VKFM_Text.cpp src/VKFM_Waveform.cpp 

vkmake:
	g++ $(CFLAGS) -o out/vkmake $(SRC_FILES)

