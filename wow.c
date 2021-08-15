// Using libs SDL, glibc
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>

#define SCREEN_WIDTH 1100    //window height
#define SCREEN_HEIGHT SCREEN_WIDTH*0.8  //window width

#define BOARD_WIDTH SCREEN_WIDTH*0.5
#define BOARD_HEIGHT BOARD_WIDTH

#define RADAR_WIDTH SCREEN_WIDTH*0.2
#define RADAR_HEIGHT RADAR_WIDTH

#define BOARD_DIM 11
#define FRAME_TIME 200

//function prototypes
//initilise SDL
int init(int w, int h, int argc, char *args[]);

typedef struct player_s {
    
    int posx, posy;     // position in game board
    int w, h;           // player image dimensions
    int lives, points; 
    int direction;      // needed to control the player image and shooting
    /* Diretions:
    0 = up
    1 = right
    2 = down
    3 = left
    */
} player_t;


// Program globals
static player_t player;
int level;

SDL_Window* window = NULL;	//The window we'll be rendering to
SDL_Renderer *renderer;		//The renderer SDL will use to draw to the screen

//surfaces
static SDL_Surface *screen;
static SDL_Surface *title;
static SDL_Surface *player_image;
static SDL_Surface *lives;
static SDL_Surface *numbermap;

//textures
SDL_Texture *screen_texture;

/*
Description: determines if a given position on the board is disables for player
and enemies (used currently by walls)

Inputs: posx, posy: coordinates os the searched position.

Output: 1 if the given position corresponds to a wall and 0 if not.
*/
static int is_disabled_block(int posx, int posy) {
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

/*
Description: A function that draws the game board once it begins, it draws a diffent
one depending on the level
*/
static void draw_board() {

    SDL_Rect board, border;

    board.x = (screen->w/2) - (BOARD_WIDTH/2);
    board.y = (screen->h/3) - (BOARD_HEIGHT/2);
    board.w = BOARD_WIDTH;
    board.h = BOARD_HEIGHT;

    border.x = board.x - 10;
    border.y = board.y - 10;
    border.w = board.w + 20;
    border.h = board.h + 20;

    int r_2 = SDL_FillRect(screen, &border, SDL_MapRGB(screen->format, 143, 143, 143));
    int r_1 = SDL_FillRect(screen, &board, SDL_MapRGB(screen->format, 0, 0, 0));
		
    if (r_1 !=0 || r_2 != 0){
    
        printf("fill rectangle failed in func draw_board()");
    }

    SDL_Rect src;
    int i;

    for (i = 0; i < BOARD_DIM; i++) {
        int j;

        for (j = 0; j < BOARD_DIM; j++) {
            
            if (is_disabled_block(i, j)) {

                src.x = board.x + (BOARD_WIDTH/BOARD_DIM)*i;
                src.y = board.y + (BOARD_HEIGHT/BOARD_DIM)*j;
                src.w = BOARD_WIDTH/BOARD_DIM;
                src.h = BOARD_HEIGHT/BOARD_DIM;

                int color = SDL_FillRect(screen, &src, SDL_MapRGB(screen->format, 143, 143, 143));
            }
        }
    }

}

/*
Description: function that draws the radar box and enemy positions
*/
static void draw_radar() {

    SDL_Rect radar, radar_border;

    radar.x = (screen->w/2) - (RADAR_WIDTH/2);
    radar.y = (5*screen->h/6) - (RADAR_HEIGHT/2);
    radar.w = RADAR_WIDTH;
    radar.h = RADAR_HEIGHT;

    radar_border.x = (screen->w/2) - (RADAR_WIDTH/2) - 10;
    radar_border.y = (5*screen->h/6) - (RADAR_HEIGHT/2) - 10;
    radar_border.w = RADAR_WIDTH + 20;
    radar_border.h = RADAR_HEIGHT + 20;

    int r_1 = SDL_FillRect(screen, &radar_border, SDL_MapRGB(screen->format, 143, 143, 143));
    int r_2 = SDL_FillRect(screen, &radar, SDL_MapRGB(screen->format, 0, 0, 0));

    if (r_1 != 0 || r_2 != 0) {

        printf("fill rectangle failed in func draw_radar()");
    }

}

/*
Description: function that draws the player's amount of lives
*/
static void draw_player_lives() {

    SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = lives->w;
	src.h = lives->h;

	dest.x = 3*screen->w/4;
	dest.y = 3*screen->h/4;
	dest.w = lives->w;
	dest.h = lives->h;

	SDL_BlitSurface(lives, &src, screen, &dest);

    SDL_Rect src_num;
	SDL_Rect dest_num;

	src_num.x = 0;
	src_num.y = 0;
	src_num.w = 64;
	src_num.h = 64;

	dest_num.x = 3*screen->w/4;
	dest_num.y = 3*screen->h/4 + lives->h;
	dest_num.w = 64;
	dest_num.h = 64;

	if (player.lives > 0 && player.lives < 10) {
		
		src_num.x += src_num.w * player.lives;
	}
	
	SDL_BlitSurface(numbermap, &src_num, screen, &dest_num);
}

/*
Description: a function that loads the menu image.
*/
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

/*
Description: this function changes the player's position or orientation,
depending on direction chosen by the player.

Input: d -> direction chosen by the player, where 0 = up, 1 = right, 
2 = down and 3 = left.
*/
static void move_player(int d) {

    // moving up
    if (d == 0) {

        if (player.direction == 0) {
            
            if (player.posy-1 >= 0 && 
                !is_disabled_block(player.posx, player.posy-1)) {

                player.posy -= 1;
            }
        } else {

            player.direction = 0;
        }
    }

    // moving right
    if (d == 1) {

        if (player.direction == 1) {
            
            if (player.posx+1 < BOARD_DIM && 
                !is_disabled_block(player.posx+1, player.posy)) {

                player.posx += 1;
            }
        } else {
            
            player.direction = 1;
        }
    }

    // moving down
    if (d == 2) {

        if (player.direction == 2) {
            
            if (player.posy+1 < BOARD_DIM && 
                !is_disabled_block(player.posx, player.posy+1)) {

                player.posy += 1;
            }
        } else {
            
            player.direction = 2;
        }
    }

    // moving left
    if (d == 3) {

        if (player.direction == 3) {
            
            if (player.posx-1 >= 0 && 
                !is_disabled_block(player.posx-1, player.posy)) {

                player.posx -= 1;
            }
        } else {
            
            player.direction = 3;
        }
    }
}

/*
Description: This function displays the player image in the screen wiht the current
position and direction.
*/
static void draw_player() {

    SDL_Rect src;
    SDL_Rect dest;

    src.x = 0;
    src.y = 0;
    src.w = player_image->w;
    src.h = player_image->h;

    int board_corner_posx = (screen->w/2) - (BOARD_WIDTH/2);
    int board_corner_posy = (screen->h/3) - (BOARD_HEIGHT/2);
    dest.x = board_corner_posx + (BOARD_WIDTH/BOARD_DIM) * player.posx;
    dest.y = board_corner_posy + (BOARD_WIDTH/BOARD_DIM) * player.posy;
    dest.w = player.w;
    dest.h = player.h;

    // The player image depends on the current direction
    switch (player.direction) {

        case 0:
            player_image = SDL_LoadBMP("player_up.bmp");
            break;

        case 1:
            player_image = SDL_LoadBMP("player_right.bmp");
            break;
        
        case 2:
            player_image = SDL_LoadBMP("player_down.bmp");
            break;

        case 3:
            player_image = SDL_LoadBMP("player_left.bmp");
            break;

    }

    if (player_image == NULL) {

        printf("Could not load the player image! SDL_Error: %s\n", SDL_GetError());
    }

    SDL_BlitSurface(player_image, &src, screen, &dest);
}

/*
Description: Function that initializes the main variables in the game
*/
static void init_game() {

    // Initialize player constants
    player.posx = 0;
    player.posy = 0;
    player.w = 50;
    player.h = 50;
    player.lives = 3;
    player.points = 0;
    player.direction = 1;

    // Initial level
    level = 3;

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

    // Initialize player position
    init_game();

    while (quit == 0)
    {
        //check for new events every frame
		SDL_PumpEvents();

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);

        if (keystate[SDL_SCANCODE_ESCAPE]) {
		
			quit = 1;
		}

        if (keystate[SDL_SCANCODE_W]) {

            move_player(0); // move player up
        }

        if (keystate[SDL_SCANCODE_D]) {

            move_player(1); // move player right
        }

        if (keystate[SDL_SCANCODE_S]) {

            move_player(2); // move player down
        }

        if (keystate[SDL_SCANCODE_A]) {

            move_player(3); // move player left
        }
        
        //draw background
		SDL_RenderClear(renderer);
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

        //display main menu
        if (state == 0){

            if (keystate[SDL_SCANCODE_SPACE]){
                state = 1;
            }

            draw_menu();

        // display the game
        } else if (state = 1){

            // draw game board
            draw_board();

            // draw the game radar
            draw_radar();

            // draw player
            draw_player();

            // draw lives count
            draw_player_lives();

        }

        SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->w * sizeof (Uint32));
		SDL_RenderCopy(renderer, screen_texture, NULL, NULL);

		//draw to the display
		SDL_RenderPresent(renderer);

		SDL_Delay(FRAME_TIME);
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

    // Load the player left image
    player_image = SDL_LoadBMP("player_right.bmp");

    if (player_image == NULL) {

        printf("Could not load the player right image! SDL_Error: %s\n", SDL_GetError());
    }

    // load de lives image
    lives = SDL_LoadBMP("lives.bmp");

    if (lives == NULL) {

        printf("Could not load the lives image! SDL_Error: %s\n", SDL_GetError());
    }

    // load numbermap
    numbermap = SDL_LoadBMP("numbermap.bmp");

    if (lives == NULL) {

        printf("Could not load the numbermap image! SDL_Error: %s\n", SDL_GetError());
    }

    // ...
    // Code to load images
    // ...

    // Set the title colourkey. 
	Uint32 colorkey = SDL_MapRGB(title->format, 255, 0, 255);
    SDL_SetColorKey(numbermap, SDL_TRUE, colorkey);
}