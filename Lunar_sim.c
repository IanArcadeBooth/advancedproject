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



//Build:  gcc -O2 -Wall Lunar_sim.c lunar_display.c $(pkg-config --cflags --libs plplot) -lm -o lunar_lander


#include <stdio.h>
#include <time.h>
#include "lunar_display.h"


int main(void)
{
   
    lunar_display_init();
    
    FILE* file;

    double Lander_x;
    double Lander_y;
    double vx;
    double vy;
    double Retro;  
    double Lz;     
    int status = LANDER_FLYING;
    
    while((file = fopen("rocketInfo.txt", "r")) == NULL){}
    fscanf(file, "%lf, %lf, %lf, %lf, %lf, %lf", &Lander_x, &Lander_y, &vx, &vy, &Retro, &Lz);
    fclose(file);
    remove("rocketInfo.txt");
    
    if(Lander_y <= 5 && vy < 10)
    {
        status = LANDER_LANDED;
    }
    else if(Lander_y <= 5 && vy >= 10)
    {
        status = LANDER_FLYING;
    }

    while (status == LANDER_FLYING) 
    {
        lunar_display_update(Lander_x, 
                             Lander_y,
                             vx, 
                             vy,
                             Retro, 
                             Lz,
                             status);
    }

    lunar_display_update(Lander_x, 
                         Lander_y,
                         vx, 
                         vy,
                         Retro, 
                         Lz,
                         status);

    lunar_display_close();
    return 0;
}
