#include "CDarkages.h"
#include "CDisplay.h"
#include "CInput.h"
#include "Structs.h"
#include "SFML/Audio.hpp"
#include <time.h>

int main(int argc, char* args[]) {

  srand(time(NULL));

  //read any configurations
  sConf conf;
  FILE* f;
  f=fopen("darkages.cfg", "rb");
  if(f!=NULL){
    fread(&conf, sizeof(sConf), 1, f);
    fclose(f);
  }

	//Start up SDL and create window
	CDisplay display;
	if (!display.init(conf))	{
		printf("Failed to initialize!\n");
		return -1;
	}

	CInput inp;

  CDarkages game(&display, &conf);
  game.title();
  //game.run();

  f=fopen("darkages.cfg", "wb");
  if(f != NULL){
    fwrite(&conf, sizeof(sConf), 1, f);
    fclose(f);
  }

	return 0;
}