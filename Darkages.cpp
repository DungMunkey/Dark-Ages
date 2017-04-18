#include "CDarkages.h"
#include "CDisplay.h"
#include "CInput.h"
#include "SFML/Audio.hpp"
#include <time.h>

int main(int argc, char* args[]) {

  srand(time(NULL));

	//Start up SDL and create window
	CDisplay display;
	if (!display.init())	{
		printf("Failed to initialize!\n");
		return -1;
	}

	CInput inp;

  CDarkages game(&display);
  game.title();
  //game.run();

	return 0;
}