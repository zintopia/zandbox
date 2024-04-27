#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define WIDTH 500
#define HEIGHT 500
#define G_VAL 0.1f

#define empty_material_id (Uint8)0
#define sand_material_id (Uint8)1
#define rock_material_id (Uint8)2 
#define water_material_id (Uint8)3
#define oil_material_id (Uint8)4

#define empty_color_scheme (SDL_Color){0, 0, 0, 255}
#define sand_color_scheme (SDL_Color){245, 200, 100, 0.8}
#define rock_color_scheme (SDL_Color){105, 105, 100, 0.8}
#define water_color_scheme (SDL_Color){0, 65, 255, 0.8}
#define oil_color_scheme (SDL_Color){55, 50, 10, 0.8}

typedef struct{
    Uint8 id;
    SDL_Color color_scheme;
    float density;
    float vspeed;
}ptcl_t;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

ptcl_t area[100][100] = {0};

ptcl_t empty_p = {empty_material_id, empty_color_scheme, 0.f, 0.f};
ptcl_t sand_p = {sand_material_id, sand_color_scheme, 2.f, 1.f};
ptcl_t rock_p = {rock_material_id, rock_color_scheme, 2.f, 1.f};
ptcl_t water_p = {water_material_id, water_color_scheme, 1.f, 1.f};
ptcl_t oil_p = {oil_material_id, oil_color_scheme, 0.5f, 1.f};

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

void display_text(const char *txt, TTF_Font *font, Uint8 y, Uint8 x, Uint8 w, Uint8 h){
    SDL_Surface *surfmssg = TTF_RenderText_Solid(font, txt, (SDL_Color){255,255,255,255});
    SDL_Texture *texmssg = SDL_CreateTextureFromSurface(renderer, surfmssg);
    
    SDL_FreeSurface(surfmssg);
    
    SDL_Rect rect = {0};
    rect.y = y;
    rect.x = x;
    rect.w = w;
    rect.h = h;
    
    SDL_RenderCopy(renderer, texmssg, NULL, &rect);
}

void show_current_particle(Uint8 id){
    TTF_Font *arial = TTF_OpenFont("font_src/Arial.ttf", 20);

    switch(id){
        case sand_material_id:
            display_text("Current: Sand", arial, 0, 0, 25, 10);            
            break;
        case rock_material_id:
            display_text("Current: Rock", arial, 0, 0, 25, 10);
            break;
        case water_material_id:
            display_text("Current: Water", arial, 0, 0, 27, 10);
            break;
        case oil_material_id:
            display_text("Current: Oil", arial, 0, 0, 21, 10);
            break;
    }
    TTF_CloseFont(arial);
}

int is_in_borders(int y, int x){
    return (y >= 0 && y < 100 && x >= 0 && x < 100);
}
int is_empty(int y, int x){
      return (area[y][x].id == empty_material_id && is_in_borders(y, x)); 
}

ptcl_t get_particle_info(Uint8 y, Uint8 x){
    ptcl_t p = area[y][x];
    return p;
}

ptcl_t get_particle_type(Uint8 id){
    switch(id){
        case sand_material_id: return sand_p; break;
        case water_material_id: return water_p; break;
        case rock_material_id: return rock_p; break;
        case oil_material_id: return oil_p; break;
        default: return empty_p; break;
    }
}

void draw_particle(int y, int x, SDL_Color colors){
    SDL_SetRenderDrawColor(renderer, colors.r, colors.g, colors.b, colors.a);
    SDL_RenderDrawPoint(renderer, x, y);
}

Uint8 rand_value(){
    return (rand() % 20);
}

Uint8 max_down(Uint8 y, Uint8 x, ptcl_t p){
	Uint8 pspeed = (Uint8)p.vspeed;
	Uint8 counter = 0;
	
	for(int i = 1; i <= pspeed; i++){
		if(is_empty(y+i, x))
			counter++;
		else
			break;
	}
	return counter;
}

void powder_move_into_density(Uint8 y, Uint8 x){
    ptcl_t referencep = get_particle_type(area[y][x].id);
    Uint8 val = rand_value();
    Uint8 d_left = is_in_borders(y+1, x-1);
    Uint8 d_right = is_in_borders(y+1, x+1);

    if(area[y][x].density > area[y+1][x].density){
        ptcl_t targetp = area[y+1][x];
        area[y][x] = empty_p;
        area[y+1][x] = empty_p;
        area[y+1][x] = referencep;
        area[y][x] = targetp;
    }
     else if(d_left && area[y][x].density > area[y+1][x-1].density && val <= 10){
        ptcl_t targetp = get_particle_type(area[y+1][x-1].id);
        area[y][x] = empty_p;
        area[y+1][x-1] = empty_p;
        area[y+1][x-1] = referencep;
        area[y][x] = targetp;
    }
     else if(d_right && area[y][x].density > area[y+1][x+1].density && val >= 10){
        ptcl_t targetp = get_particle_type(area[y+1][x+1].id);
        area[y][x] = empty_p;
        area[y+1][x+1] = empty_p;
        area[y+1][x+1] = referencep;
        area[y][x] = targetp;
    }
}

void liquid_move_into_density(Uint8 y, Uint8 x){
    ptcl_t referencep = get_particle_type(area[y][x].id);
    Uint8 val = rand_value();
    Uint8 left = is_in_borders(y, x-1);
    Uint8 right = is_in_borders(y, x+1);
    Uint8 d_left = is_in_borders(y+1, x-1);
    Uint8 d_right = is_in_borders(y+1, x+1);

    if(area[y][x].density > area[y+1][x].density){
        ptcl_t targetp = get_particle_type(area[y+1][x].id);
        area[y][x] = empty_p;
        area[y+1][x] = empty_p;
        area[y+1][x] = referencep;
        area[y][x] = targetp;
    }
    else if(d_left && area[y][x].density > area[y+1][x-1].density && val <= 10){
        ptcl_t targetp = get_particle_type(area[y+1][x-1].id);
        area[y][x] = empty_p;
        area[y+1][x-1] = empty_p;
        area[y+1][x-1] = referencep;
        area[y][x] = targetp;
    }
     else if(d_right && area[y][x].density > area[y+1][x+1].density && val >= 10){
        ptcl_t targetp = get_particle_type(area[y+1][x+1].id);
        area[y][x] = empty_p;
        area[y+1][x+1] = empty_p;
        area[y+1][x+1] = referencep;
        area[y][x] = targetp;
    }
    else if(left && area[y][x].density > area[y][x-1].density && val <= 10){
        ptcl_t targetp = get_particle_type(area[y][x-1].id);
        area[y][x] = empty_p;
        area[y][x-1] = empty_p;
        area[y][x-1] = referencep;
        area[y][x] = targetp;
    }
    else if(right && area[y][x].density > area[y][x+1].density && val >= 10){
        ptcl_t targetp = get_particle_type(area[y][x+1].id);
        area[y][x] = empty_p;
        area[y][x+1] = empty_p;
        area[y][x+1] = referencep;
        area[y][x] = targetp;
    }
}

void upt_sand(int y, int x){
    draw_particle(y, x, sand_color_scheme);

    ptcl_t reference = get_particle_info(y, x);
    Uint8 val = rand_value();
    
    reference.vspeed += G_VAL;

    Uint8 down = max_down(y, x, reference);
    Uint8 d_left = is_empty(y+1, x-1);
    Uint8 d_right = is_empty(y+1, x+1);
    
    if(down){
        area[y][x] = empty_p;
        area[y+down][x] = reference;
    }
    else if(d_left && val >= 10){
        area[y][x] = empty_p;
        area[y+1][x-1] = reference;
    }
    else if(d_right && val <= 10){
        area[y][x] = empty_p;
        area[y+1][x+1] = reference;
    }
    else powder_move_into_density(y, x);
}

void upt_water(int y, int x){
    draw_particle(y, x, water_color_scheme);

    Uint8 val = rand_value();
    ptcl_t reference = get_particle_info(y, x);

    reference.vspeed += G_VAL;

    Uint8 down = max_down(y, x, reference);
    Uint8 left = is_empty(y, x-1);
    Uint8 right = is_empty(y, x+1);
    Uint8 d_left = is_empty(y+1, x-1);
    Uint8 d_right = is_empty(y+1, x+1);
    
    if(down){
        area[y][x] = empty_p;
        area[y+down][x] = reference;
    }
    else if(d_left && val <= 10){
        area[y][x] = empty_p;
        area[y+1][x-1] = reference;
    }
    else if(d_right && val >= 10){
        area[y][x] = empty_p;
        area[y+1][x+1] = reference;
    }
    else if(left && val <= 10){
        area[y][x] = empty_p;
        area[y][x-1] = reference;
    }
    else if(right && val >= 10){
        area[y][x] = empty_p;
        area[y][x+1] = reference;
    }
    else liquid_move_into_density(y, x);
}

void upt_oil(int y, int x){
    draw_particle(y, x, oil_color_scheme);
    
    ptcl_t reference = get_particle_info(y, x);
    Uint8 val = rand_value();
    
    reference.vspeed += G_VAL;

    Uint8 down = max_down(y, x, reference);
    Uint8 left = is_empty(y, x-1);
    Uint8 right = is_empty(y, x+1);
    Uint8 d_left = is_empty(y+1, x-1);
    Uint8 d_right = is_empty(y+1, x+1);

    if(down){
        area[y][x] = empty_p;
        area[y+down][x] = reference;
    }
    else if(d_left && val <= 10){
        area[y][x] = empty_p;
        area[y+1][x-1] = reference;
    }
    else if(d_right && val >= 10){
        area[y][x] = empty_p;
        area[y+1][x+1] = reference;
    }
    else if(left && val <= 10){
        area[y][x] = empty_p;
        area[y][x-1] = reference;
    }
    else if(right && val >= 10){
        area[y][x] = empty_p;
        area[y][x+1] = reference;
    }
    else liquid_move_into_density(y, x);
}

void update_particles(){
    for(int y = 100-1; y>=0; y--){
        for(int x = 100-1; x>=0; x--){
            switch(get_particle_info(y, x).id){
                case sand_material_id:
                    upt_sand(y, x);
                    break;
		        case water_material_id:
                    upt_water(y, x);
                    break;
                case oil_material_id:
                    upt_oil(y, x);
                    break;
                case rock_material_id:
                    draw_particle(y, x, rock_color_scheme); // doesn't create a particular instance in the matrix; to fix. (or not)
                    break;
            }
        }
    }
}

void place_particles(Uint8 y, Uint8 x, ptcl_t p){
    Uint8 bottom = is_empty(y, x);
    Uint8 up = is_empty(y-1, x);
    Uint8 down = is_empty(y+1, x);
    Uint8 left = is_empty(y, x-1);
    Uint8 right = is_empty(y, x+1);

    if(down) area[y+1][x] = p;
    if(up) area[y-1][x] = p;
    if(left) area[y][x-1] = p;
    if(right) area[y][x+1] = p;
    if(bottom) area[y][x] = p;
}

void erase_particles(Uint8 y, Uint8 x){
    area[y][x] = empty_p;
    area[y+1][x] = empty_p;
    area[y-1][x] = empty_p;
    area[y][x-1] = empty_p;
    area[y][x+1] = empty_p;
}

int main(void){
    srand(time(NULL));
    init_window();
    TTF_Init();
    //TTF_Font *arial = TTF_Open("font_src/Arial.ttf", 25);
    SDL_Event event;
    
    int running = 1;
    int count = 0;
    Uint16 y, x;
    Uint8 is_right_pressed = 0;
    Uint8 is_left_pressed = 0;
    Uint8 id_reference = 1; /* initiates with sand as default. */

    while(running == 1){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_MOUSEMOTION:
                    Uint16 refy = event.motion.y/5, refx = event.motion.x/5;
                    printf("Y:%d\tX:%d\n", refy, refx);

                    if(refy < 99 && refy > 0 && refx < 99 && refx > 0)  
                        y = refy, x = refx;
                    else is_left_pressed = 0; // mantains in 0.
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT)
                        is_left_pressed = 1;
                    if(event.button.button == SDL_BUTTON_RIGHT)
                        is_right_pressed = 1; 
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(is_left_pressed)
                        is_left_pressed = 0;
                    else if(is_right_pressed)
                        is_right_pressed = 0;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == 'e'){
                        id_reference++;
                        if(id_reference > 4) id_reference = 1;
                    }
                    break;
            }    
        }

        if(is_left_pressed && area[y][x].id == empty_material_id){
            ptcl_t p = get_particle_type(id_reference);
    	    place_particles(y, x, p); 
        }
        else if(is_right_pressed){
            erase_particles(y, x);
        }
        
        update_particles();
        show_current_particle(id_reference);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);           
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
