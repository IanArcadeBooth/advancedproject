/*
 * file: lunar_sim.c
 * Author: Lucas Farmer
 * Date: April 13, 2026
 * Description: Intakes simulation data such as the
 *              x and y positions of the lander, the
 *              horizontal and vertical velocities,
 *              the retor thurst and the landing zone.
 *              This data is taken and used to draw  the
 *              lander.
 */

// Build for graphics only:  gcc -O2 -Wall Lunar_sim.c lunar_display.c $(pkg-config --cflags --libs plplot) -lm -o lunar_lander

#include "lunar_display.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define WORLD_W 100.0
#define LZ_CENTER 50.0

#define FILEOPENWAIT 10000

int main(void)
{
    setbuf(stdout, NULL);
    lunar_display_init(); // Initialzes the plplot window

    while (1) //keep restarting the simulation after each completed run
    {
        FILE *file;
        double Lander_x;
        double Lander_y;
        double vx;
        double vy;
        double Retro;
        double Lz;
        double lz_hw;
        double lz_left;
        double lz_right;
        int status = LANDER_FLYING;

        while (status == LANDER_FLYING) //continue updating until the lander is on the ground
        {
            while ((file = fopen("rocketInfo.txt", "r")) == NULL) // Open the file with all the value from the physics program
            {
                usleep(FILEOPENWAIT);
            }
            
            // Reads all the values from the physics file
            fscanf(file, "%lf %lf %lf %lf %lf %lf", 
                        &Lander_x, &Lander_y, &vx, &vy, &Retro, &Lz);
            
            // Close and delete the file
            fclose(file);
            remove("rocketInfo.txt");

            // Calculates the half width of the landing zone, and the left and right edges
            lz_hw = Lz * WORLD_W / 4.0;
            lz_left = LZ_CENTER - lz_hw;
            lz_right = LZ_CENTER + lz_hw;
            
            // Checks if the lander is in the landing zone and moving less then 10.0 m/s
            if (Lander_y <= SURFACE_Y + LANDER_R &&
                vy > -10.0 &&
                Lander_x >= lz_left &&
                Lander_x <= lz_right)
            {
                status = LANDER_LANDED;
            }
            else if (Lander_y <= SURFACE_Y + LANDER_R)
            {
                status = LANDER_CRASHED;
            }

            lunar_display_update(Lander_x, Lander_y, vx, vy, Retro, Lz, status); // Updates the simulation information every frame
            
        }

        lunar_display_update(Lander_x, Lander_y, vx, vy, Retro, Lz, status); // Final frame for the overlay message 
        usleep(FILEOPENWAIT);
    }

    lunar_display_close();
    return 0;
}
