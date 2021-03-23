#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

void getPlacePosition(int &field);
bool checkForWin();
bool checkForDraw();
void drawGrid();
bool drawPlace();

SDL_Window *window = SDL_CreateWindow("Tic-Tac-Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 666, 666, 0);

SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

SDL_AudioSpec place_wav_spec;
Uint32 place_wav_length;
Uint8 *place_wav_buffer;
SDL_AudioDeviceID deviceId;

SDL_Texture *grid_texture;
SDL_Texture *place_texture;

int mouse_x, mouse_y;
int pos_x = 0;
int pos_y = 0;
int player_on_turn = 1;

int grid[9] = 
	{ 
		0, 0, 0,
		0, 0, 0,
		0, 0, 0 
	};

int main() 
{
    SDL_Event event;
    bool quit = false;
    bool is_playing;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    IMG_Init(IMG_INIT_PNG);

    if (SDL_LoadWAV("../res/sfx/place.wav", &place_wav_spec, &place_wav_buffer, &place_wav_length) == NULL) {
        fprintf(stderr, "Could not open test.wav: %s\n", SDL_GetError());
    }

    deviceId = SDL_OpenAudioDevice(NULL, 0, &place_wav_spec, NULL, 0);

    while(!quit) {
        drawGrid();
        player_on_turn = 1;

        for (int i = 0; i < 9; i++) {
            grid[i] = 0;
        }

        is_playing = true;

        while(is_playing) {
            SDL_WaitEvent(&event);

            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (!drawPlace()) {
                            break;
                        }

                        if (checkForWin()) {
                            std::cout << "Player " << player_on_turn << " won!" << std::endl;
                            is_playing = false;
                            break;
                        }
                        else if (checkForDraw()) {
                            std::cout << "Draw!" << std::endl;
                            is_playing = false;
                            break;
                        }

                        player_on_turn = (player_on_turn == 1) ? 2 : 1;
                    }
                    break;
                case SDL_QUIT:
                    is_playing = false;
                    quit = true;
                    break;
            }
        }
    }

    SDL_DestroyTexture(grid_texture);
    SDL_DestroyTexture(place_texture);
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(place_wav_buffer);
    IMG_Quit();
    SDL_Quit();
    return 0;
}

void getPlacePosition(int &field)
{
    int grid_positions [9][2] {
        {000, 000}, {222, 000}, {444, 000},
        {000, 222}, {222, 222}, {444, 222},
        {000, 444}, {222, 444}, {444, 444}
    };
    
    for (int i = 0; i < 9; i++) {
        // std::cout << "Grid Position: " << grid_positions[i][0] << ", " << grid_positions[i][1] << " | " << "Mouse Position: " << mouse_pos_x << ", " << mouse_pos_y << std::endl;
        if (mouse_x > grid_positions[i][0] && mouse_x < (grid_positions[i][0] + 222) && mouse_y > grid_positions[i][1] && mouse_y < (grid_positions[i][1] + 222)) {
            pos_x = grid_positions[i][0];
            pos_y = grid_positions[i][1];
            field = i;
        }
    }
}

bool checkForWin() {
	if (grid[0] == player_on_turn && grid[1] == player_on_turn && grid[2] == player_on_turn) {return true;}
	
	else if (grid[3] == player_on_turn && grid[4] == player_on_turn && grid[5] == player_on_turn) {return true;}
	
	else if (grid[6] == player_on_turn && grid[7] == player_on_turn && grid[8] == player_on_turn) {return true;}
	
	else if (grid[0] == player_on_turn && grid[3] == player_on_turn && grid[6] == player_on_turn) {return true;}
	
	else if (grid[1] == player_on_turn && grid[4] == player_on_turn && grid[7] == player_on_turn) {return true;}
	
	else if (grid[2] == player_on_turn && grid[5] == player_on_turn && grid[8] == player_on_turn) {return true;}
	
	else if (grid[0] == player_on_turn && grid[4] == player_on_turn && grid[8] == player_on_turn) {return true;}
	
	else if (grid[2] == player_on_turn && grid[4] == player_on_turn && grid[6] == player_on_turn) {return true;}
	
	return false;
}

bool checkForDraw() {
    int counter = 0;

    for (int i = 0; i < 9; i++) {
        if (grid[i] == 1 || grid[i] == 2) {
            counter++;
        }
    }

    if (counter == 9) {
        return true;
    }

    return false;
}

void drawGrid()
{
    SDL_RenderClear(renderer);

    SDL_Surface *grid_surface;
    grid_surface = IMG_Load("../res/graphics/grid.png");

    if(!grid_surface) {
        printf("IMG_load: %s\n", IMG_GetError());
    }

    grid_texture = SDL_CreateTextureFromSurface(renderer, grid_surface);
    SDL_FreeSurface(grid_surface);

    SDL_RenderCopy(renderer, grid_texture, NULL, NULL);

    SDL_RenderPresent(renderer);
}

bool drawPlace()
{
    int field = -1;

    SDL_GetMouseState(&mouse_x, &mouse_y);
    getPlacePosition(field);

    // std::cout << "Field: " << field << std::endl;
    // std::cout << "Grid field: " << grid[field] << std::endl;
    // std::cout << "Player: " << player_on_turn << std::endl;

    if (grid[field] != 0 || field == -1) {
        return false;
    }

    int success = SDL_QueueAudio(deviceId, place_wav_buffer, place_wav_length);
    SDL_PauseAudioDevice(deviceId, 0);

    SDL_Surface *place_surface = (player_on_turn == 1) ? IMG_Load("../res/graphics/cross.png") : IMG_Load("../res/graphics/circle.png");

    if(!place_surface) {
        printf("IMG_load: %s\n", IMG_GetError());
    }

    place_texture = SDL_CreateTextureFromSurface(renderer, place_surface);
    SDL_FreeSurface(place_surface);

    SDL_Rect place_rect;
    place_rect.x = pos_x+16;
    place_rect.y = pos_y+16;
    place_rect.w = 190;
    place_rect.h = 190;

    SDL_RenderCopy(renderer, place_texture, NULL, &place_rect);

    SDL_RenderPresent(renderer);

    grid[field] = player_on_turn;

    return true;
}