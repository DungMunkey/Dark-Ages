#include "CDarkages.h"
#include "CDisplay.h"
#include "CInput.h"
#include "Structs.h"
#include <time.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <wchar.h>
#define da_mkdir(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <unistd.h>
#define da_mkdir(path) mkdir(path, 0755)
#endif

using namespace std;

//The graphics, music, mods, saves and darkages.cfg are all found relative to the working directory, so make that the
//folder the game lives in. It then works no matter how it was started (shortcut, terminal, file manager).
static void changeToExeFolder(){
#ifdef _WIN32
  //Ask Windows directly rather than using SDL_GetBasePath(): the 2.0.12 version returns the wrong folder when the exe's
  //path is long (roughly 130+ characters, e.g. a zip extracted deep in a folder tree), and its UTF-8 result wouldn't
  //suit _chdir for folder names with non-ASCII characters anyway.
  vector<wchar_t> path(MAX_PATH);
  for(;;){
    DWORD len = GetModuleFileNameW(NULL, &path[0], (DWORD)path.size());
    if(len == 0) return;                //can't tell where we are; stay put
    if(len < path.size()) break;        //the whole path fit
    if(path.size() >= 32768) return;    //longer than Windows allows; give up
    path.resize(path.size() * 2);       //it was cut off: try again with a bigger buffer
  }
  wchar_t* lastSlash = wcsrchr(&path[0], L'\\');
  if(lastSlash == NULL) return;
  *lastSlash = 0;                       //drop "Darkages.exe", leaving the folder
  _wchdir(&path[0]);
#else
  char* basePath = SDL_GetBasePath();
  if(basePath != NULL){
    int ignored = chdir(basePath);
    (void)ignored;
    SDL_free(basePath);
  }
#endif
}

int main(int argc, char* args[]) {

  changeToExeFolder();

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