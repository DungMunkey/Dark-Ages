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

  void playSong(eMusic m);
  void setVolume(int vol);

private:

  sf::Music music[5];
  eMusic currentSong;

};

#endif