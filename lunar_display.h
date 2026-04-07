/*
 * file: lunar_display.h
 * Author: Lucas Farmer
 * date:
 * Description: Header file contaning function desclairation
 * 				for lanar_display.c
 */



#define LANDER_FLYING   0   // simulation still running 
#define LANDER_LANDED   1   // successful landing       
#define LANDER_CRASHED -1   // crash                      

/*
 * lunar_display_init
 * Initialise PLplot window
 */
void lunar_display_init(void);

/*
 * lunar_display_update
 * Render one frame
 */
void lunar_display_update(double lander_x,
                          double lander_y,
                          double vx,
                          double vy,
                          double retro_pct,
                          int    lz_pct,
                          int    status);

/*
 * lunar_display_close
 * Shut down PLplot
 */
void lunar_display_close(void);


