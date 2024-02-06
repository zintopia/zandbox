#include <stdio.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#include <SDL.h>

#define WIDTH 500
#define HEIGHT 500
#define FALLMS 6000

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

int perimeter[100][100] = {0};

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

int is_empty(int x, int y){
   if(perimeter[x][y] == 0)
      return 1;
   else
      return 0; 
}

void draw_particle(int x, int y){
    SDL_SetRenderDrawColor(renderer, 220, 190, 60, 1);
    SDL_RenderDrawPoint(renderer, x, y);
    perimeter[x][y] = 1;
}

void drop_sand(int deltaX, int deltaY){
    int pemY = deltaY/5, pemX = deltaX/5;
    int stay_flag = 0;
    
    while(stay_flag == 0){
        int down = is_empty(pemX, pemY+1);
        int left = is_empty(pemX-1, pemY+1);
        int right = is_empty(pemX+1, pemY+1);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Turns before pixel into black (1/2)
       
        if(down && pemY < 99){
               SDL_RenderDrawPoint(renderer, pemX, pemY); // Turns before pixel into black (2/2).
               perimeter[pemX][pemY] = 0; // Removes the before pixel reference from the matrix.
               draw_particle(pemX, pemY+1);
        }
        else if(left && pemX > 0 && pemY < 99){
            SDL_RenderDrawPoint(renderer, pemX, pemY);
            perimeter[pemX][pemY] = 0;
            draw_particle(pemX-1, pemY+1);
            pemX--;
        }
        else if(right && pemX < 99 && pemY < 99){
            SDL_RenderDrawPoint(renderer, pemX, pemY);
            perimeter[pemX][pemY] = 0;
            draw_particle(pemX+1, pemY+1);
            pemX++;
        }
        else stay_flag = 1;
        
        pemY++;

        #ifdef _WIN32
            sleep(FALLMS);
        #else 
            usleep(FALLMS);
        #endif
        SDL_RenderPresent(renderer);
    }
}

int main(void){
    init_window();
    SDL_Event event;

    int running = 1;
    while(running == 1){    
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
                /*case SDL_MOUSEMOTION:
                    printf("deltaX: %d\tdeltaY = %d\n", event.motion.x, event.motion.y);
                    break;*/
                case SDL_MOUSEBUTTONDOWN:
                    drop_sand(event.button.x, event.button.y);                   
                    break;
            }
        }
        SDL_Delay(10);           
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
