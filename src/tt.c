#if 0
#!/bin/sh
gcc -Wall -I ../../gals/src/ -I ../../pico-sdl/src/ `sdl2-config --cflags` ../../gals/src/sdl/gals.c ../../pico-sdl/src/pico.c tt.c -o xtt `sdl2-config --libs` -lSDL2_net -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include "pico.h"
#include "gals.h"

enum {
    EVT_TIME, EVT_DRAG
};

void _assert (int x) {}

int main (int argc, char** argv) {
    assert(argc == 2);
    pico_open();
    int self = gals_connect(atoi(argv[1]), 20);
    printf(">>> %d\n", self);

    int x = 0;
    int y = 0;

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
        int pay;
        gals_wait(&now, &evt, &pay);
        //printf("now=%ld evt=%d\n", now, evt);

        pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });

        if (evt == EVT_DRAG) {
            x = pay;
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
                .tag = PICO_OUTPUT_DRAW_RECT,
                .Rect={{x,y},{5,9}}
            }
        });

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

                static int drag_is = 0;
                static SDL_Point drag_src;

                switch (inp.type) {
                    case SDL_QUIT:
                        exit(0);
                    case SDL_MOUSEBUTTONUP:
                        if (drag_is) {
                            gals_emit(EVT_DRAG, x + (inp.button.x-drag_src.x));
                            drag_is = 0;
                        }
                        break;
                    case SDL_MOUSEBUTTONDOWN: {
                        SDL_Point pt = {inp.button.x, inp.button.y};
                        if (pico_isPointVsRect(pt, (SDL_Rect)  {x,y,5,9})) {
                            drag_is = 1;
                            drag_src = pt;
                        }
                        break;
                    }
                }
            }
        }
	}

	gals_disconnect();
	return 0;
}

