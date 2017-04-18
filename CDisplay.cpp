#include "CDisplay.h"

CDisplay::CDisplay(){

  screenHeight = 800; // 1050;
  screenWidth =  1280; // 1680;
  renderer=NULL;
  //screenSurface = NULL;
  window = NULL;

  txtColors[0].r=255;
  txtColors[0].g=255;
  txtColors[0].b=255;
  txtColors[0].a=255;
}

CDisplay::~CDisplay(){
  if(renderer!=NULL) SDL_DestroyRenderer(renderer);
  renderer=NULL;
  //if(screenSurface != NULL) SDL_FreeSurface(screenSurface);
	//screenSurface = NULL;
  if(window != NULL) SDL_DestroyWindow(window);
  window = NULL;
  SDL_Quit();
}

bool CDisplay::init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}	else	{

    //SDL_DisplayMode dm;
    //if(SDL_GetCurrentDisplayMode(0, &dm)!=0){;
    //if(SDL_GetDesktopDisplayMode(0, &dm) != 0) {
      //SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
    //  exit(1);
    //}
    //screenHeight=dm.h;
    //screenWidth=dm.w;

    //Set texture filtering to nearest neighbor. Pixels rule!!!
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) ) {
			printf( "Warning: nearest neighbor texture filtering not enabled!" );
		}

		//Create window
		window = SDL_CreateWindow( "Dark Ages: The Continents", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN );
		if( window == NULL )	{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}	else	{
			//Get window surface
			//screenSurface = SDL_GetWindowSurface(window);

      renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
			if( renderer == NULL ) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			} else {
				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
			}

		}
	}

	return success;
}