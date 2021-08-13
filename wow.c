// Using libs SDL, glibc
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH 1100    //window height
#define SCREEN_HEIGHT SCREEN_WIDTH*0.8  //window width

#define BOARD_WIDTH SCREEN_WIDTH*0.5
#define BOARD_HEIGHT BOARD_WIDTH

#define BOARD_DIM 11

//function prototypes
//initilise SDL
int init(int w, int h, int argc, char *args[]);

//Structures
typedef struct disabled_blocks {
    
    int posx, posy;
} disabled_b;


// Program globals
static disabled_b blocks_level1[9];
static disabled_b blocks_level2[9];
static disabled_b blocks_level3[9];


SDL_Window* window = NULL;	//The window we'll be rendering to
SDL_Renderer *renderer;		//The renderer SDL will use to draw to the screen

//surfaces
static SDL_Surface *screen;
static SDL_Surface *title;

//textures
SDL_Texture *screen_texture;

static int is_disabled_block(int level, int posx, int posy) {
    if (level == 1) {

        if ( (posx == 1 && (posy == 1 || posy == 2 || posy == 3 || posy == 4 || posy == 5 || posy == 7)) ||
             (posx == 2 && (posy == 1)) ||
             (posx == 3 && (posy == 1 || posy == 3 || posy == 4 || posy == 5 || posy == 6 || posy == 7 || posy == 9)) ||
             (posx == 4 && (posy == 1 || posy == 7 || posy == 9)) ||
             (posx == 5 && (posy == 7 || posy == 9)) ||
             (posx == 6 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)) ||
             (posx == 7 && (posy == 3 || posy == 7)) ||
             (posx == 8 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)) ||
             (posx == 10 && (posy == 3 || posy == 7)))
        {
            return 1;
        }
    }

    else if (level == 2) {
        
        if ( (posx == 1 && (posy == 1 || posy == 2 || posy == 3 || posy == 5 || posy == 7 || posy == 8 || posy == 9 || posy == 10)) ||
             (posx == 2 && (posy == 3)) ||
             (posx == 3 && (posy == 1 || posy == 3 || posy == 5 || posy == 6 || posy == 7 || posy == 9)) ||
             (posx == 4 && (posy == 1)) ||
             (posx == 5 && (posy == 7 || posy == 9)) ||
             (posx == 6 && (posy == 1 || posy == 8 || posy == 7 || posy == 9)) ||
             (posx == 7 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)) ||
             (posx == 8 && (posy == 1 || posy == 9)) ||
             (posx == 9 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)) ||
             (posx == 10 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)))
        {
            return 1;
        }
    }

    else if (level == 3) {

        if ( (posx == 1 && (posy == 1 || posy == 5 || posy == 7 || posy == 8)) ||
             (posx == 3 && (posy == 1 || posy == 2 || posy == 3 || posy == 7 || posy == 9)) ||
             (posx == 4 && (posy == 1 || posy == 9)) ||
             (posx == 5 && (posy == 1 || posy == 7 || posy == 8 || posy == 9)) ||
             (posx == 6 && (posy == 1)) ||
             (posx == 7 && (posy == 1 || posy == 3 || posy == 7 || posy == 9 || posy == 2 || posy == 8)) ||
             (posx == 8 && (posy == 1)) ||
             (posx == 9 && (posy == 1 || posy == 3 || posy == 7 || posy == 9)))
        {
            return 1;
        }
    }

    return 0;
}

static void draw_board(int level) {

    SDL_Rect border;

    border.x = (screen->w/2) - (BOARD_WIDTH/2);
    border.y = (screen->h/3) - (BOARD_HEIGHT/2);
    border.w = BOARD_WIDTH;
    border.h = BOARD_HEIGHT;

    int r = SDL_FillRect(screen, &border, 0xffffffff);
		
    if (r !=0){
    
        printf("fill rectangle failed in func draw_board()");
    }

    SDL_Rect src;
    int i;

    for (i = 0; i < BOARD_DIM; i++) {
        int j;

        for (j = 0; j < BOARD_DIM; j++) {
            
            if (is_disabled_block(level, i, j)) {

                src.x = border.x + (BOARD_WIDTH/BOARD_DIM)*i;
                src.y = border.y + (BOARD_HEIGHT/BOARD_DIM)*j;
                src.w = BOARD_WIDTH/BOARD_DIM;
                src.h = BOARD_HEIGHT/BOARD_DIM;

                int color = SDL_FillRect(screen, &src, 0x0000);
            }
        }
    }
    

}

static void draw_menu() {

	SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = title->w;
	src.h = title->h;

	dest.x = (screen->w / 2) - (src.w / 2);
	dest.y = (screen->h / 2) - (src.h / 2);
	dest.w = title->w;
	dest.h = title->h;

	SDL_BlitSurface(title, &src, screen, &dest);
}

int main (int argc, char *args[]){

    //SDL Window setup
	if (init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1) {
		
		return 0;
	}

    int quit = 0;
    int state = 0;
    int sleep = 0;
    Uint32 next_game_tick = SDL_GetTicks();

    while (quit == 0)
    {
        //check for new events every frame
		SDL_PumpEvents();

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);

        if (keystate[SDL_SCANCODE_ESCAPE]) {
		
			quit = 1;
		}
        
        //draw background
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, 0x000000ff);

        //display main menu
        if (state == 0){

            if (keystate[SDL_SCANCODE_SPACE]){
                state = 1;
            }

            draw_menu();

        // display the game
        } else if (state = 1){

            draw_board(3);

        }

        SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof (Uint32));
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

		//draw to the display
		SDL_RenderPresent(renderer);

        //time it takes to render  frame in milliseconds
		next_game_tick += 1000 / 60;
		sleep = next_game_tick - SDL_GetTicks();
	
		if( sleep >= 0 ) {
            				
			SDL_Delay(sleep);
		}
    }
    
    //free loaded images
	SDL_FreeSurface(screen);
	SDL_FreeSurface(title);

    //free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);
	
	//Destroy window 
	SDL_DestroyWindow(window);

	//Quit SDL subsystems 
	SDL_Quit(); 

    return 0;

}

int init(int width, int height, int argc, char *args[]) {

    //Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	} 

    int i;

    for (i = 0; i < argc; i++) {
		
		//Create window	
		if(strcmp(args[i], "-f")) {
			
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
		
		} else {
		
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
		}
	}

    if (window == NULL) { 
		
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	}

    //create the screen sruface where all the elements will be drawn onto (ball, paddles, net etc)
	screen = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
	
	if (screen == NULL) {
		
		printf("Could not create the screen surfce! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

    //create the screen texture to render the screen surface to the actual display
	screen_texture = SDL_CreateTextureFromSurface(renderer, screen);

	if (screen_texture == NULL) {
		
		printf("Could not create the screen_texture! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

    //Load the title image
	title = SDL_LoadBMP("title.bmp");

	if (title == NULL) {
		
		printf("Could not Load title image! SDL_Error: %s\n", SDL_GetError());

		return 1;
	}

    // ...
    // Code to load images
    // ...

}