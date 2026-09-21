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
  void playSongOnce(eMusic m); //restarts m from the beginning and plays it through a single time (no looping)
  double getSongDuration(eMusic m) const; //length of m in seconds, or -1.0 if it isn't loaded or its length can't be determined
  void setVolume(int vol);

private:

  Mix_Music* music[5];
  eMusic currentSong;
  bool currentIsOneShot; //currentSong was started by playSongOnce(), so it isn't looping
  bool loaded[5];

};

#endif
