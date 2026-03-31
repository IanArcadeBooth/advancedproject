/*
 * example_sim.c
 *
 * Minimal stub showing how YOUR simulation calls the display module.
 * Replace the physics here with your own calculations.
 *
 * Build:  gcc -O2 -Wall sim.c lunar_display.c $(pkg-config --cflags --libs plplot) -lm -o lunar_lander
 */

#include <stdio.h>
#include <time.h>
#include "lunar_display.h"

#define DT 0.05   /* seconds per tick – match your simulation step */

int main(void)
{
    /* ── initialise PLplot window ── */
    lunar_display_init();

    /* ── your simulation variables ── */
    double lander_x = 20.0;
    double lander_y = 80.0;
    double vx       =  2.0;
    double vy       = -0.5;
    double retro    = 40.0;   /* % */
    int    lz_pct   = 10;     /* % */
    int    status   = LANDER_FLYING;

    struct timespec ts = { .tv_sec = 0, .tv_nsec = (long)(DT * 1e9) };

    /* ── simulation loop ── */
    while (status == LANDER_FLYING) {

        /* 
         * PHYSICS GOES HERE
        */

        /* ── hand the values to the display ── */
        lunar_display_update(lander_x, lander_y,
                             vx, vy,
                             retro, lz_pct,
                             status);

        nanosleep(&ts, NULL);//can be removed
    }

    /* show final frame (landed / crashed) */
    lunar_display_update(lander_x, lander_y,
                         vx, vy,
                         retro, lz_pct,
                         status);

    nanosleep(&(struct timespec){ .tv_sec = 3, .tv_nsec = 0 }, NULL);

    /*shut down*/
    lunar_display_close();
    return 0;
}
