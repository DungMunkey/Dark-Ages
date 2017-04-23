#include "CDisplay.h"

CDisplay::CDisplay(){

  screenHeight = 800; // 1050;
  screenWidth =  1280; // 1680;
  currentScreenMode=0;
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

bool CDisplay::init(sConf& conf) {
	//Initialization flag
	bool success = true;

	//Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}	else	{

    int display_count = 0, display_index = 0, mode_index = 0;
    SDL_DisplayMode mode ={SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0};

    SDL_GetCurrentDisplayMode(0, &mode);
    Uint32 pFormat=mode.format;
    int rRate=mode.refresh_rate;
    sDAVidMode vm;
    char str[32];

    for(int i=SDL_GetNumDisplayModes(0)-1; i >=0; i--){
      if(SDL_GetDisplayMode(0, i, &mode) != 0) {
        SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
      } else {
        if(mode.format != pFormat || mode.refresh_rate != rRate) continue;
        vm.h=mode.h;
        vm.w=mode.w;
        sprintf(str, "%dx%d", mode.w, mode.h);
        vm.name=str;
        if(conf.w==vm.w && conf.h==vm.h){
          currentScreenMode=screenModes.size();
        }
        screenModes.push_back(vm);
        //printf("%d\tSDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i, %ihz\n", i,SDL_BITSPERPIXEL(mode.format), mode.w, mode.h,mode.refresh_rate);
      }
    }
    conf.w=screenModes[currentScreenMode].w;
    conf.h=screenModes[currentScreenMode].h;

    //for(int i=0; i < screenModes.size();i++){
    //  printf("%s\n", &screenModes[i].name[0]);
    //}
    screenWidth=screenModes[currentScreenMode].w;
    screenHeight=screenModes[currentScreenMode].h;
    int wf;
    if(conf.fullScreen) {
      wf=SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
    } else {
      wf=SDL_WINDOW_SHOWN;
    }

    //Set texture filtering to nearest neighbor. Pixels rule!!!
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) ) {
			printf( "Warning: nearest neighbor texture filtering not enabled!" );
		}

		//Create window
		window = SDL_CreateWindow( "Dark Ages: The Continents", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, wf );
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