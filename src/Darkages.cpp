#include "CDarkages.h"
#include "CDisplay.h"
#include "CInput.h"
#include "Structs.h"
#include <time.h>

#ifdef _WIN32
#include <direct.h>
#define da_chdir _chdir
#define da_mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define da_chdir chdir
#define da_mkdir(path) mkdir(path, 0755)
#endif

using namespace std;

int main(int argc, char* args[]) {

  //The graphics, music, mods, saves and darkages.cfg are all found relative to the working directory, so make that
  //the folder the game lives in. It then works no matter how it was started (shortcut, terminal, file manager).
  char* basePath = SDL_GetBasePath();
  if(basePath != NULL){
    da_chdir(basePath);
    SDL_free(basePath);
  }

  //Saves/ isn't shipped with the game; create it if it's missing (does nothing when it already exists).
  da_mkdir("Saves");

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