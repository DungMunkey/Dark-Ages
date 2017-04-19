#Set these variables if needed
C = gcc
CC = g++
FLAGS = -std=c++11 -O3 -Wno-unused-result -Wno-write-strings


#Do not touch these variables
LIBS = -lSDL2 -lSDL2_ttf -lsfml-system -lsfml-audio -lpthread
INCLUDE = -I/usr/include/SDL2

#Do not touch these variables
DA = CBattle.o CCamera.o CDarkages.o CDisplay.o CFont.o CGfxCollection.o CGraphic.o CInput.o CMap.o CMusic.o CPlayer.o CWorld.o

#Make statements
darkages : Darkages.cpp $(DA)
	$(CC) $(FLAGS) $(INCLUDE) $(DA) Darkages.cpp $(LIBS) -o darkages

clean:
	rm *.o darkages

%.o : %.cpp
	$(CC) $(FLAGS) $(INCLUDE) $< -c
