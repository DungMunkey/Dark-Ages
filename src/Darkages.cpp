#include "CDarkages.h"
#include "CDisplay.h"
#include "CInput.h"
#include "Structs.h"
#include <time.h>
#include <SDL3/SDL_main.h> //SDL3 provides the platform entry point (WinMain on Windows) through this header, in the one file that defines main()

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
  //Ask Windows directly rather than using SDL_GetBasePath(): SDL 2.0.12's version returned the wrong folder when the
  //exe's path was long (roughly 130+ characters, e.g. a zip extracted deep in a folder tree), and its UTF-8 result
  //wouldn't suit _chdir for folder names with non-ASCII characters anyway.
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
  const char* basePath = SDL_GetBasePath(); //owned by SDL: it must not be freed
  if(basePath != NULL){
    int ignored = chdir(basePath);
    (void)ignored;
  }
#endif
}

//Used by tools/package.ps1 to make the darkages.cfg that ships in the zip: "Darkages.exe --write-default-config <file> [mod]".
//It writes every setting at its default, except the mod when one is named, then exits. Doing it here (rather than in
//the script) keeps the file's layout and the default values in one place, the sConf struct.
static int writeDefaultConfig(const char* path, const char* modName){
  sConf conf;
  if(modName != NULL){
    strncpy(conf.modName, modName, sizeof(conf.modName) - 1);
    conf.modName[sizeof(conf.modName) - 1] = 0;
  }
  FILE* f = fopen(path, "wb");
  if(f == NULL) return 1;
  size_t written = fwrite(&conf, sizeof(sConf), 1, f);
  fclose(f);
  return (written == 1) ? 0 : 1;
}

int main(int argc, char* args[]) {

  if(argc >= 3 && strcmp(args[1], "--write-default-config") == 0){
    return writeDefaultConfig(args[2], (argc >= 4) ? args[3] : NULL);
  }

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