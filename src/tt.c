#if 0
#!/bin/sh
gcc -Wall -I ../../gals/src/ `sdl2-config --cflags` ../../gals/src/sdl/gals.c tt.c -o xtt `sdl2-config --libs` -lSDL2_net
exit
#endif

#include <assert.h>
#include <SDL2/SDL.h>
#include "gals.h"

enum {
    EVT_NONE, EVT_UP, EVT_DOWN, EVT_RIGHT, EVT_LEFT, EVT_STOP
};

void _assert (int x) {}

int main (int argc, char** argv) {
    assert(argc == 2);
	assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window*   win = SDL_CreateWindow("SDL", 0,0, 400,400, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    int self = gals_connect(atoi(argv[1]), 20);
    printf(">>> %d\n", self);

    int x = 10;
    int y = 10;
    int xdir = 0;
    int ydir = 0;
    uint64_t prv = 0;

	while (1) {
        uint64_t now;
        int evt;
        gals_wait(&now, &evt);
        //printf("now=%ld evt=%d\n", now, evt);

        SDL_SetRenderDrawColor(ren, 0xFF,0xFF,0xFF,0xFF);
        SDL_RenderClear(ren);

        switch (evt) {
            case EVT_LEFT:  { xdir=-1; ydir=0; break; }
            case EVT_RIGHT: { xdir= 1; ydir=0; break; }
            case EVT_UP:    { ydir=-1; xdir=0; break; }
            case EVT_DOWN:  { ydir= 1; xdir=0; break; }
            case EVT_STOP:  { ydir= 0; xdir=0; printf("PAUSE: t=%ld, xy=(%d,%d)\n",now,x,y); break; }
        }

        SDL_Rect r = { x, y, 10, 10 };
        SDL_SetRenderDrawColor(ren, 0xFF,0x00,0x00,0xFF);
        SDL_RenderFillRect(ren, &r);

        if (now!=prv && evt==0) {
            x += 5 * xdir;
            y += 5 * ydir;
        }
        prv = now;


        {
            SDL_Event inp;
            while (SDL_PollEvent(&inp)) {
                uint32_t n = 0;
                if (inp.type == SDL_QUIT) {
                    exit(0);
                }
                if (inp.type == SDL_KEYDOWN) {
                    switch (inp.key.keysym.sym) {
                        case SDLK_LEFT:  { n=EVT_LEFT;  break; }
                        case SDLK_RIGHT: { n=EVT_RIGHT; break; }
                        case SDLK_UP:    { n=EVT_UP;    break; }
                        case SDLK_DOWN:  { n=EVT_DOWN;  break; }
                        case SDLK_SPACE: { n=EVT_STOP;  break; }
                    }
                }
                if (n != 0) {
                    SDL_Rect r = { 190, 190, 20, 20 };
                    SDL_SetRenderDrawColor(ren, 0x77,0x77,0x77,0x77);
                    SDL_RenderFillRect(ren, &r);
                    gals_emit(n);
                }
            }
        }

        SDL_RenderPresent(ren);
	}

	gals_disconnect();
	return 0;
}

