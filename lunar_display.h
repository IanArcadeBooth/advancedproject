/*
 * file: lunar_display.h
 * Author: Lucas Farmer
 * Date: April 13, 2026
 * Description: Header file contaning function desclairation
 * 				for lunar_display.c & lunar_sim.c
 */



#define LANDER_FLYING 0     // simulation still running 
#define LANDER_LANDED 1     // successful landing       
#define LANDER_CRASHED -1   // crash      
#define SURFACE_Y 5.0       // Ground level 
#define LANDER_R 1.65       // Lander radius               

/*
 * lunar_display_init
 * Initialise plplot window
 */
void lunar_display_init(void);

/*
 * lunar_display_update
 * Render one frame
 */
void lunar_display_update(double,
                          double,
                          double,
                          double,
                          double,
                          double,
                          int);

/*
 * lunar_display_close
 * Shut down plplot
 */
void lunar_display_close(void);


