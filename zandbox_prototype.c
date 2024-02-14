#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL.h>

#define WIDTH 500
#define HEIGHT 500

#define sand_material_id 1
#define rock_material_id 2 
#define water_material_id 3

#define sand_color_scheme (SDL_Color){245, 200, 100, 0.8}
#define rock_color_scheme (SDL_Color){105, 105, 100, 0.8}
#define water_color_scheme (SDL_Color){0, 65, 255, 0.8}

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
    return (rand() % 20);
}

void is_sand_in_water(int y, int x){
    if(area[y+1][x] == water_material_id){
        area[y+1][x] = 0;
        area[y][x] = 0;
        area[y+1][x] = sand_material_id;
        area[y][x] = water_material_id;
    }
    else if(area[y+1][x-1] == water_material_id){
        area[y+1][x-1] = 0;
        area[y][x] = 0;
        area[y+1][x-1] = sand_material_id;
        area[y][x] = water_material_id;
    }
    else if(area[y+1][x+1] == water_material_id){
        area[y+1][x+1] = 0;
        area[y][x] = 0;
        area[y+1][x+1] = sand_material_id;
        area[y][x] = water_material_id;
    }
}

void upt_sand(int y, int x){
    int val = rand_value();
    const int down = is_empty(y+1, x);
    const int d_left = is_empty(y+1, x-1);
    const int d_right = is_empty(y+1, x+1);
    
    if(down){
        area[y][x] = 0;
        area[y+1][x] = sand_material_id;
    }
    else if(d_left && val >= 10){
        area[y][x] = 0;
        area[y+1][x-1] = sand_material_id;
    }
    else if(d_right && val <= 10){
        area[y][x] = 0;
        area[y+1][x+1] = sand_material_id;
    }
    else
        is_sand_in_water(y, x);
}

void upt_water(int y, int x){
    int val = rand_value();
    const int down = is_empty(y+1, x);
    const int left = is_empty(y, x-1);
    const int right = is_empty(y, x+1);
    const int d_left = is_empty(y+1, x-1);
    const int d_right = is_empty(y+1, x+1);
    
    if(down){
        area[y][x] = 0;
        area[y+1][x] = water_material_id;
    }
    else if(d_left && val <= 10){
        area[y][x] = 0;
        area[y+1][x-1] = water_material_id;
    }
    else if(d_right && val >= 10){
        area[y][x] = 0;
        area[y+1][x+1] = water_material_id;
    }
    else if(left && val <= 10){
        area[y][x] = 0;
        area[y][x-1] = water_material_id;
    }
    else if(right && val >= 10){
        area[y][x] = 0;
        area[y][x+1] = water_material_id;
    }
}

void update_particles(){
    for(int y = 100-1; y>=0; y--){
        for(int x = 100-1; x>=0; x--){
            switch(area[y][x]){
                case sand_material_id:
                    draw_particle(y, x, sand_color_scheme);
                    upt_sand(y, x);
                    break;
                case water_material_id:
                    draw_particle(y, x, water_color_scheme);
                    upt_water(y, x);
                    break;;
                case rock_material_id:
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
    int count = 0;
    int y, x;
    int is_pressed = 0;
    int id_reference = 1; /* initiates with sand as default. */

    while(running == 1){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEMOTION:
                    int refy = event.motion.y/5, refx = event.motion.x/5;
                    printf("%d\t%d\n", refy, refx);
                    if(refy < 100 && refy >= 0 && refx < 100 && refx >= 0)  
                        y = refy, x = refx;
                    else is_pressed = 0; // mantains in 0.
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT)
                        is_pressed = 1;     
                    break;
                case SDL_MOUSEBUTTONUP:
                    is_pressed = 0;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == 'e')
                        id_reference++;
                        if(id_reference > 3) id_reference = 1;
                    break;
            }
            
        }
        if(area[y][x] == 0 && is_pressed){
            printf("placed into area[%d][%d]\n", y, x);
            if(count != 0 && count%2 == 0)
                area[y][x] = id_reference;
            count++;
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
