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

// Build:  gcc -O2 -Wall Lunar_sim.c lunar_display.c $(pkg-config --cflags
// --libs plplot) -lm -o lunar_lander

#include "lunar_display.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define WORLD_W 100.0
#define LZ_CENTER 50.0

int main(void) {
  setbuf(stdout, NULL);

  lunar_display_init();

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

  while (status == LANDER_FLYING) {
    while ((file = fopen("rocketInfo.txt", "r")) == NULL) {
      usleep(10000);
    }

    fscanf(file, "%lf %lf %lf %lf %lf %lf", &Lander_x, &Lander_y, &vx, &vy,
           &Retro, &Lz);

    fclose(file);
    remove("rocketInfo.txt");

    lz_hw = Lz * WORLD_W / 4.0;
    lz_left = LZ_CENTER - lz_hw;
    lz_right = LZ_CENTER + lz_hw;

    if (Lander_y <= SURFACE_Y + 3.0 && vy > -10.0 && Lander_x >= lz_left &&
        Lander_x <= lz_right) {
      status = LANDER_LANDED;
    } else if (Lander_y <= SURFACE_Y + 3.0) {
      status = LANDER_CRASHED;
    }

    lunar_display_update(Lander_x, Lander_y, vx, vy, Retro, Lz, status);

    usleep(15000);
  }

  /* draw final frame one more time */
  lunar_display_update(Lander_x, Lander_y, vx, vy, Retro, Lz, status);

  /* keep final message visible */
  while (1) {
    usleep(100000);
  }

  lunar_display_close();
  return 0;
}
