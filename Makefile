#Linux build. NOTE: this is not maintained yet - the game currently uses Windows-only calls (see src/CMods.cpp), so it
#will not compile here until the Linux port is done (see docs/future-work.md). It only knows the new folder layout.

#Set these variables if needed
C = gcc
CC = g++
FLAGS = -std=c++11 -O3 -Wno-unused-result -Wno-write-strings


#Do not touch these variables
LIBS = -lSDL2 -lSDL2_ttf -lSDL2_mixer -lpthread
INCLUDE = -I/usr/include/SDL2 -Isrc

#Do not touch these variables
DA = build/CBattle.o build/CCamera.o build/CDarkages.o build/CDisplay.o build/CFont.o build/CGfxCollection.o build/CGraphic.o build/CInput.o build/CLoadSave.o build/CMap.o build/CMods.o build/CMusic.o build/COptions.o build/CPlayer.o build/CTitle.o build/CWindow.o build/CWorld.o

#Make statements: the executable is built into game/, the folder that holds everything it loads at run time
darkages : src/Darkages.cpp $(DA)
	$(CC) $(FLAGS) $(INCLUDE) $(DA) src/Darkages.cpp $(LIBS) -o game/darkages

clean:
	rm -f build/*.o game/darkages

build/%.o : src/%.cpp
	mkdir -p build
	$(CC) $(FLAGS) $(INCLUDE) $< -c -o $@
