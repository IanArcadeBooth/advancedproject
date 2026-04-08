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
#include <unistd.h>



int main(void)
{
    setbuf(stdout,NULL);
   
    lunar_display_init();
    
    FILE* file;

    double Lander_x;
    double Lander_y;
    double vx;
    double vy;
    double Retro;  
    double Lz;     
    int status = LANDER_FLYING;
    
    
    while (1) 
    {
    printf("waiting for rocketinfo\n");
    while((file = fopen("rocketInfo.txt", "r")) == NULL){
        usleep(10000);
        }
    
    printf("opened rocket info\n");
    if(fscanf(file, "%lf %lf %lf %lf %lf %lf", &Lander_x, &Lander_y, &vx, &vy, &Retro, &Lz) !=6){
        printf("bad read\n");
    }
    else{
        printf("read ok: y=%lf vy=%lf\n", Lander_y, vy);
    }
    fclose(file);
    if (    remove("rocketInfo.txt") == 0)
    {
        printf("file deleted\n");
    }
    else {
        printf("error\n");
    }
    
    if(Lander_y <= 5.0 && vy < 10.0)
    {
        status = LANDER_LANDED;
    }
    else if(Lander_y <= 5 && vy >= 10)
    {
        status = LANDER_CRASHED;
    }

    printf("here\n");
        lunar_display_update(Lander_x, 
                             Lander_y,
                             vx, 
                             vy,
                             Retro, 
                             Lz,
                             status);
        usleep(30000);                    
        printf("here2\n");

        printf("status: %d\n", status);
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
