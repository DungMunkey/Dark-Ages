#ifndef _CMUSIC_H
#define _CMUSIC_H

#include <SDL_mixer.h>
#include <string>

enum eMusic{
  BattleSong,
  DungeonSong,
  TitleSong,
  TownSong,
  WorldSong
};

class CMusic{
public:
  CMusic();
  ~CMusic();

  void loadMusic(const std::string& modName);
  void playSong(eMusic m, bool restart=false);
  void setVolume(int vol);

private:

  Mix_Music* music[5];
  eMusic currentSong;
  bool loaded[5];

};

#endif
