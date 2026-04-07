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
    
    FILE* file;

    long lLander_x;
    long lLander_y;
    long lvx;
    long lvy;
    long lRetro;  
    long lLz;     
    int status = LANDER_FLYING;
    
    while((file = fopen("rocketInfo.txt", "r")) == NULL){}
    fscanf(file, "%lf, %lf, %lf, %lf, %lf, %lf", lLander_x, lLander_y, lvx, lvy, lRetro, lLz);
    fclose(file);
    remove("rocketInfo.txt");
    
    double lander_x = (double)lLander_x;
    double lander_y = (double)lLander_y;
    double vx = (double)lvx;
    double vy = (double)lvy;
    double retro = (double)lRetro;  
    double lz_pct = (double)lLz;

    while (status == LANDER_FLYING) {

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
