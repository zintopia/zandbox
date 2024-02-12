#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL.h>

#define WIDTH 500
#define HEIGHT 500
#define FALLMS 1000

#define powder_material 1
#define unmv_solid_material 2 

#define sand_color_scheme (SDL_Color){245, 200, 100, 0.8}
#define rock_color_scheme (SDL_Color){105, 105, 100, 0.8}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int area[100][100] = {0};

void init_window(){
    SDL_Init(SDL_INIT_VIDEO);    
    SDL_CreateWindowAndRenderer(
            WIDTH, 
            HEIGHT, 
            0, 
            &window,
            &renderer
    );
    SDL_RenderSetScale(renderer, 5, 5);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_RenderPresent(renderer);
}   

int is_empty(int y, int x){
      return (area[y][x] == 0 && y >= 0 && y < 100
                              && x >= 0 && x < 100); 
}

void draw_particle(int y, int x, SDL_Color colors){
    SDL_SetRenderDrawColor(renderer, colors.r, colors.g, colors.b, colors.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

int rand_value(){
    return (rand() % 10);
}

void uptsand(int y, int x){
    int val = rand_value();
    const int down = is_empty(y+1, x);
    const int d_left = is_empty(y+1, x-1);
    const int d_right = is_empty(y+1, x+1);
    
    if(down){
        area[y][x] = 0;
        area[y+1][x] = 1;
    }
    else if(d_left && val >= 5){
        area[y][x] = 0;
        area[y+1][x-1] = 1;
    }
    else if(d_right && val <= 5){
        area[y][x] = 0;
        area[y+1][x+1] = 1;
    }
}

void update_particles(){
    for(int y = 100-1; y>=0; y--){
        for(int x = 100-1; x>=0; x--){
            switch(area[y][x]){
                case powder_material:
                    draw_particle(y, x, sand_color_scheme);
                    uptsand(y, x);
                    break;
                case unmv_solid_material:
                    draw_particle(y, x, rock_color_scheme);
                    break;
            }
        }
    }
}

int main(void){
    srand(time(NULL));
    init_window();
    SDL_Event event;
    
    int running = 1;
    int id_reference = 1; /* initiates with sand as default. */

    while(running == 1){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                /*case SDL_MOUSEMOTION:
                    printf("deltaX: %d\tdeltaY = %d\n", event.motion.x, event.motion.y);
                    break;*/
                case SDL_MOUSEBUTTONDOWN:
                    int y = event.button.y/5;
                    int x = event.button.x/5;
                    area[y][x] = id_reference;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == 'e')
                        id_reference++;
                        if(id_reference > 2) id_reference = 1;
                    break;
            }
        }
        update_particles();
        SDL_RenderPresent(renderer);
        SDL_Delay(10);           
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
