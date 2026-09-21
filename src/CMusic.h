#ifndef _CMUSIC_H
#define _CMUSIC_H

#include <SDL3_mixer/SDL_mixer.h>
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
  void setVolume(int vol); //0-10

private:
  void startTrack(eMusic m, int loops); //(re)starts m from the beginning on the music track; loops = -1 forever, 0 = once
  bool trackPlaying() const;

  MIX_Mixer* mixer;      //the audio device
  MIX_Track* track;      //the one track music plays on; switching songs just points it at another MIX_Audio
  MIX_Audio* music[5];
  eMusic currentSong;
  bool currentIsOneShot; //currentSong was started by playSongOnce(), so it isn't looping
  bool loaded[5];
  float gain;            //master volume, 0.0-1.0 (remembered so setVolume() works before the mixer exists)

};

#endif
