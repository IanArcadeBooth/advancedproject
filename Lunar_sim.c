/*
 * file: lunar_sim.c
 * Author: Lucas Farmer
 * date:
 * Description: Intakes simulation data such as the 
 *              x and y positions of the lander, the
 *              horizontal and vertical velocities,
 *              the retor thurst and the landing zone.
 *              This data is taken and used to draw  the 
 *              lander.
 */



//Build:  gcc -O2 -Wall sim.c lunar_display.c $(pkg-config --cflags --libs plplot) -lm -o lunar_lander


#include <stdio.h>
#include <time.h>
#include "lunar_display.h"


int main(void)
{
   
    lunar_display_init();

    double lander_x = 20.0;
    double lander_y = 80.0;
    double vx       =  2.0;
    double vy       = -0.5;
    double retro    = 40.0;  
    int    lz_pct   = 10;     
    int    status   = LANDER_FLYING;

    while (status == LANDER_FLYING) {

        /* 
         * PHYSICS GOES HERE
        */

        lunar_display_update(lander_x, 
                             lander_y,
                             vx, 
                             vy,
                             retro, 
                             lz_pct,
                             status);
    }

    lunar_display_update(lander_x, 
                         lander_y,
                         vx, 
                         vy,
                         retro, 
                         lz_pct,
                         status);

    lunar_display_close();
    return 0;
}
