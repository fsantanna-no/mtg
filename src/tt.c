#if 0
#!/bin/sh
gcc -Wall -I ../../gals/src/ -I ../../pico-sdl/src/ `sdl2-config --cflags` ../../gals/src/sdl/gals.c ../../pico-sdl/src/pico.c tt.c -o xtt `sdl2-config --libs` -lSDL2_net -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include "pico.h"
#include "gals.h"

enum {
    EVT_TIME, EVT_UP, EVT_DOWN, EVT_RIGHT, EVT_LEFT, EVT_STOP
};

void _assert (int x) {}

int main (int argc, char** argv) {
    assert(argc == 2);
    pico_open();
    int self = gals_connect(atoi(argv[1]), 20);
    printf(">>> %d\n", self);

    int x = 0;
    int y = 0;
    int xdir = 0;
    int ydir = 0;
    uint64_t prv = 0;

    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_COLOR,
            .Color = {
                .tag = PICO_OUTPUT_SET_COLOR_CLEAR,
                .Clear = {0xFF,0xFF,0xFF,0xFF}
            }
        }
    });

	while (1) {
        uint64_t now;
        int evt;
        gals_wait(&now, &evt);
        //printf("now=%ld evt=%d\n", now, evt);

        pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });

        switch (evt) {
            case EVT_LEFT:  { xdir=-1; ydir=0; break; }
            case EVT_RIGHT: { xdir= 1; ydir=0; break; }
            case EVT_UP:    { ydir=-1; xdir=0; break; }
            case EVT_DOWN:  { ydir= 1; xdir=0; break; }
            case EVT_STOP:  { ydir= 0; xdir=0; printf("PAUSE: t=%ld, xy=(%d,%d)\n",now,x,y); break; }
        }

        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_COLOR,
                .Color = {
                    .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                    .Draw = {0xFF,0x00,0x00,0xFF}
                }
            }
        });

        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_DRAW,
            .Draw = {
                .tag = PICO_OUTPUT_DRAW_PIXEL,
                .Pixel={x,y}
            }
        });

        if (now!=prv && evt==0) {
            x += 1 * xdir;
            y -= 1 * ydir;
        }
        prv = now;
        //printf(">>> (%d,%d)\n", x, y);

        {
            SDL_Event inp;
            while (1) {
                int has = pico_input(&inp, (Pico_Input){
                    .tag = PICO_INPUT_EVENT,
                    .Event = {
                        .tag = PICO_INPUT_EVENT_POLL,
                        .type = SDL_ANY
                    }
                });
                if (!has) break;

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
                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_SET,
                        .Set = {
                            .tag = PICO_OUTPUT_SET_COLOR,
                            .Color = {
                                .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                                .Draw = {0x77,0x77,0x77,0x77}
                            }
                        }
                    });

                    pico_output((Pico_Output) {
                        .tag = PICO_OUTPUT_DRAW,
                        .Draw = {
                            .tag = PICO_OUTPUT_DRAW_PIXEL,
                            .Pixel={0,0}
                        }
                    });

                    gals_emit(n);
                }
            }
        }
	}

	gals_disconnect();
	return 0;
}

