/*
 * File:lunar_display.c
 * Author: Lucas Farmer
 * Date: 
 * Description: Creates the lunar lander display
 *              using plplot. The program creates
 *              stars, the lander, lander flame, 
 *              the suface, and the simulation 
 *              information.
 */

#include "lunar_display.h"

#include <stdio.h>
#include <math.h>
#include <plplot/plplot.h>

//world constants
#define WORLD_W    100.0
#define WORLD_H    100.0
#define LZ_CENTER   50.0
#define LANDER_R     3.0

//colour slots
#define C_BLACK  0
#define C_WHITE  1
#define C_GREY 7
#define C_BLUEVIOLET  10

//line styles
#define LS_SOLID  1
#define LS_DASHED 2

/*
 * Line2: Draws a straight line between two points
 * double x1: Starting x point
 * double y1: Starting y point
 * double x2: End x point
 * double y2:End y point
 * Return: N/A
 */
void line2(double x1, double y1, double x2, double y2)
{
    double x[2] = { x1, x2 };
    double y[2] = { y1, y2 };
    plline(2, x, y);
}

/*
 * draw_stars: Draws small dots scattered around the sky
 * Return: N/A
 */
void draw_stars()
{
    //x positions of stars
    static double sx[] = {
         4, 11, 19, 28, 37, 46, 53, 63, 72, 81, 89, 96,
         7, 16, 24, 33, 42, 51, 60, 69, 78, 87, 94,
        10, 21, 31, 41, 55, 65, 75, 85
    };
    
    //y position of stars
    static double sy[] = {
        91, 86, 94, 89, 96, 83, 90, 88, 93, 85, 92, 87,
        76, 79, 73, 81, 77, 74, 80, 72, 78, 75, 70,
        64, 61, 67, 58, 63, 69, 57, 65
    };
    
    int n = (int)(sizeof(sx) / sizeof(sx[0])); //Number of stars
    plcol0(C_WHITE);
    plssym(0.0, 0.35);    //Set the size
    plpoin(n, sx, sy, 1); // Draw the star(dot) at each point
}

/*
 * Draw_surface: Draws the lunar surface and the landing zone
 * double lz_hw: The height and width of landing zone
 * Return: N/A
 */
void draw_surface(double lz_hw)
{
    // Finds the left and right edges of the landing zone 
    double lzL = LZ_CENTER - lz_hw;
    double lzR = LZ_CENTER + lz_hw;

    /* Rough terrain (gap at landing zone) 
     * Landing zone sits flat with the ground
     * to make it easier to detect the landing
     */
    double tx[] = {0, 6, 12, 18, 24, lzL, lzL, lzR, lzR, 74, 80, 86, 93, 100 };
    double ty[] = {5, 5, 8, 6, 9, 9, SURFACE_Y, SURFACE_Y, 9, 8, 5, 7, 5, 5 };
    plcol0(C_GREY);
    pllsty(LS_SOLID);
    plline(14, tx, ty);

    /* Landing-zone platform 
     * Flat landing zone to make
     * it easier to see
     */
    line2(lzL, SURFACE_Y, lzR, SURFACE_Y);

    // flag poles 
    line2(lzL, SURFACE_Y, lzL, SURFACE_Y + 4.0); //Left
    line2(lzR, SURFACE_Y, lzR, SURFACE_Y + 4.0); //Right

    // flag triangles 
    double fl1x[4] = { lzL, lzL, lzL + 1.5, lzL };
    double fl1y[4] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    double fl2x[4] = { lzR, lzR, lzR - 1.5, lzR };
    double fl2y[4] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    // Daws a solid filled polygon
    plfill(4, fl1x, fl1y);
    plfill(4, fl2x, fl2y);
}

/*
 * draw_lander: Draws the lunar lander
 * double cx: Center x position of the lander
 * double cy: Center y position of the lander
 * int crashed: Checks if the lander crashed and either 
 * Return:N/A
 */
void draw_lander(double cx, double cy, int crashed)
{
    double r = LANDER_R;
    plcol0(C_BLUEVIOLET);
    pllsty(crashed ? LS_DASHED : LS_SOLID);

    // descent stage body 
    double bx[5] = { cx-r*0.9, cx+r*0.9, cx+r*0.6, cx-r*0.6, cx-r*0.9 };
    double by[5] = { cy-r*0.2, cy-r*0.2, cy+r*0.5, cy+r*0.5, cy-r*0.2 };
    plline(5, bx, by);

    // ascent stage body 
    double ax[5] = { cx-r*0.4, cx+r*0.4, cx+r*0.4, cx-r*0.4, cx-r*0.4 };
    double ay[5] = { cy+r*0.5, cy+r*0.5, cy+r*1.1, cy+r*1.1, cy+r*0.5 };
    plline(5, ax, ay);

    // antenna & dish 
    line2(cx, cy+r*1.1, cx, cy+r*1.5);
    double dx[3] = { cx-r*0.2, cx, cx+r*0.2 };
    double dy[3] = { cy+r*1.5, cy+r*1.7, cy+r*1.5 };
    plline(3, dx, dy);

    // landing legs & foot pads 
    line2(cx-r*0.9, cy-r*0.2, cx-r*1.5, cy-r*1.0);
    line2(cx+r*0.9, cy-r*0.2, cx+r*1.5, cy-r*1.0);
    line2(cx-r*1.7, cy-r*1.0, cx-r*1.3, cy-r*1.0);
    line2(cx+r*1.3, cy-r*1.0, cx+r*1.7, cy-r*1.0);

    // engine bell 
    double ex[4] = { cx-r*0.3, cx+r*0.3, cx+r*0.2, cx-r*0.2 };
    double ey[4] = { cy-r*0.2, cy-r*0.2, cy-r*0.7, cy-r*0.7 };
    plline(4, ex, ey);
    line2(ex[0], ey[0], ex[3], ey[3]);

    pllsty(LS_SOLID);
}

/*
 * draw_flame: Draws the flame under the engine bell
 * double cx: Center x position of the lander
 * double cy: Center y position of the lander
 * double retro: Curent thrust % 
 * Return:N/A
 */
void draw_flame(double cx, double cy, double retro)
{
    if (retro < 1.0) return;
    double r  = LANDER_R;
    double fl = r * retro / 55.0;

    plcol0(C_WHITE);
    pllsty(LS_SOLID);

    double ox[3] = { cx-r*0.2, cx, cx+r*0.2 };
    double oy[3] = { cy-r*0.7, cy-r*0.7-fl, cy-r*0.7 };
    plline(3, ox, oy);

    double ix[3] = { cx-r*0.1, cx, cx+r*0.1 };
    double iy[3] = { cy-r*0.7, cy-r*0.7-fl*0.55, cy-r*0.7 };
    plline(3, ix, iy);
}

/*
 * draw_bar: Draw the thrust % bar in the information display panel
 * double px: Bottom left x position of the bar
 * double py: Bottom left y position of the bar
 * double w: width of the bar
 * double h: height of the bar
 * double pct: fill percentage
 * Return:N/A
 */
void draw_bar(double px, double py, double w, double h, double pct)
{
    plcol0(C_WHITE);
    pllsty(LS_SOLID);

    double bx[5] = { px, px+w, px+w, px, px };
    double by[5] = { py, py,   py+h, py+h, py };
    plline(5, bx, by);

    if (pct > 0.0) {
        double fw = w * (pct > 100.0 ? 100.0 : pct) / 100.0;
        double fx[4] = { px, px+fw, px+fw, px };
        double fy[4] = { py, py,    py+h,  py+h };
        plfill(4, fx, fy);
    }
}

/*
 * lunar_display_init: Initializes plplot and sets the background colour
 * Param: N/A 
 * Return: N/A
 */
void lunar_display_init(void)
{
   
    plsdev("xwin"); //"xwin" or "xcairo"
    //plsetopt("db", "");        // double-buffer: no flicker 
    plsetopt("bg", "000000");  // black background          
    plinit();
    plscol0(C_WHITE, 255, 255, 255);
    
    pladv(0);
}
 /*
void lunar_display_update(double lander_x,
                          double lander_y,
                          double vx,
                          double vy,
                          double retro_pct,
                          double lz,
                          int status)
                          {
                              printf("inside update\n");
                              plclear();
                              printf("after pladv\n");
                          }



 * lunar_display_update: Redraws the display each frame
 * double lander_x: lander x position
 * double Lander_y: lander y position
 * double vx: horizontal velocity of lander
 * double vy: vertical velocity
 * double retro_pct: retro thrust
 * int lz_pct: landing zone
 * int status: status of lander(flying, landed, crashed)
 * Return: N/A
 */


void lunar_display_update(double lander_x,
                          double lander_y,
                          double vx,
                          double vy,
                          double retro_pct,
                          double lz,
                          int status)
{
    
    double lz_hw = lz * WORLD_W / 200.0;
    char   buf[64];

    plclear();

    //Space viewport (top) 
    plvpor(0.02, 0.98, 0.30, 0.98);
    plwind(0.0, WORLD_W, 0.0, WORLD_H);

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    plbox("bc", 0, 0, "bc", 0, 0);

    plschr(0, 1.2);
    plmtex("t", 0.9, 0.5, 0.5, "LUNAR LANDER");
    plschr(0, 1.0);

    draw_stars();
    draw_surface(lz_hw);
    draw_lander(lander_x, lander_y, status == LANDER_CRASHED);
    if (status == LANDER_FLYING)
        draw_flame(lander_x, lander_y, retro_pct);

    // Overlay messages 
    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    if (status == LANDER_LANDED) {
        plschr(0, 1.6);
        plptex(50, 55, 1, 0, 0.5, "** GOOD LANDING! **");
        plschr(0, 1.0);
        plptex(50, 48, 1, 0, 0.5, "Press Start to restart");
    }
    if (status == LANDER_CRASHED) {
        plschr(0, 1.6);
        plptex(50, 55, 1, 0, 0.5, "** CRASHED! **");
        plschr(0, 1.0);
        plptex(50, 48, 1, 0, 0.5, "Press Start to restart");
    }

    // Instrument panel (bottom)
    plvpor(0.02, 0.98, 0.01, 0.28);
    plwind(0.0, 100.0, 0.0, 20.0);

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    plbox("bc", 0, 0, "bc", 0, 0);

    // Divider
    line2(50.0, 1.0, 50.0, 19.0);

    // Retro 
    plschr(0, 0.9);
    plptex(2.0, 15.5, 1, 0, 0.0, "Retro");
    snprintf(buf, sizeof(buf), "%.0lf %%", retro_pct);
    plptex(22.0, 15.5, 1, 0, 0.0, buf);
    draw_bar(2.0, 13.0, 44.0, 1.5, retro_pct);

    // LZ-Width 
    plptex(2.0, 10.5, 1, 0, 0.0, "LZ-Width");
    snprintf(buf, sizeof(buf), "%.0lf %%", lz);
    plptex(22.0, 10.5, 1, 0, 0.0, buf);
    draw_bar(2.0, 8.0, 44.0, 1.5, (double)lz);

    // Vx / Vy
    plptex(52.0, 15.5, 1, 0, 0.0, "Vx");
    snprintf(buf, sizeof(buf), "%+.2lf  m/sec", vx);
    plptex(60.0, 15.5, 1, 0, 0.0, buf);

    plptex(52.0, 10.5, 1, 0, 0.0, "Vy");
    snprintf(buf, sizeof(buf), "%+.2lf  m/sec", vy);
    plptex(60.0, 10.5, 1, 0, 0.0, buf);

    // status
    if (status == LANDER_LANDED || status == LANDER_CRASHED) {
        plschr(0, 1.1);
        plptex(86.0, 5.5, 1, 0, 0.5,
               status == LANDER_LANDED ? "Good Job" : "Crashed!");
    }

    plschr(0, 1.0);

}
/*
 * lunar_display_close: Shut down plplot 
 * Param: N/A
 * return: N/A
 */
void lunar_display_close(void)
{
    plend();
}
