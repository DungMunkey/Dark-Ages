#ifndef _CMUSIC_H
#define _CMUSIC_H

#include "SFML/Audio.hpp"

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

  void loadMusic();
  void playSong(eMusic m, bool restart=false);
  void setVolume(int vol);

private:

  sf::Music music[5];
  eMusic currentSong;
  bool loaded[5];

};

#endif