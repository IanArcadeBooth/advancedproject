/*
 * File:lunar_display.c
 * Author: Lucas Farmer
 * Date: April 13, 2026
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

// World constants
#define WORLD_W    100.0
#define WORLD_H    100.0
#define HALF_WORLD_W 50.0
#define X_MIN 0.0
#define Y_MIN 0.0
#define LZ_CENTER   50.0

// Colour slots
#define C_BLACK  0
#define C_WHITE  1
#define C_GREY 7
#define C_BLUEVIOLET 10

// Line styles
#define LS_SOLID  1
#define LS_DASHED 2

// Text specifiers - keep slant values a 1 and 0 for no slanted text
#define JUST_CENTER 0.5
#define JUST_LEFT 0.0
#define SLANT_DX 1
#define SLANT_DY 0
// Keeps the Overlay Messages text aligned
#define OM_X 50.0
#define OM_TOP_Y 55.0
#define OM_BOTTOM_Y 48.0

// Bar height and width
#define BAR_WIDTH 1.5
#define BAR_HEIGHT 44.0
// Keeps the % bar for the retro and lz-width aligned with the text
#define BAR_X 2.0
#define RETRO_BAR_Y 13.0
#define LZ_BAR_Y 8.0
// Keeps the % values on the screen aligned with each other
#define VAL_PCT 22.0 
// Keeps Retro text aligned
#define RETRO_TEXT_Y 15.5
// Keeps Lz-width text aligned
#define LZ_TEXT_Y 10.5

// Keep the VX and VY text and the value on the same line
#define VX_VY_X 52.0
#define VX_VY_VAL_X 60.0
#define VX_Y 15.5
#define VY_Y 10.5

// Character Height scalers - keep default height at zero
#define CHR_HEIGHT_DEFAULT 0
// Overlay Message
#define OM_TOP 1.6
#define OM_BOTTOM 1.0
// Instrument Panel
#define IP_TEXT_SIZE 0.9




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
    static int linePoints = 2;
    
    double x[] = { x1, x2 };
    double y[] = { y1, y2 };
    plline(linePoints, x, y); //Draws a single line from given coordinates
}

/*
 * draw_stars: Draws small dots scattered around the sky
 * Return: N/A
 */
void draw_stars()
{
    //x positions of stars - arbitrary vales
    static double starX[] = {
         4, 11, 19, 28, 37, 46, 53, 63, 72, 81, 89, 96,
         7, 16, 24, 33, 42, 51, 60, 69, 78, 87, 94,
         10, 21, 31, 41, 55, 65, 75, 85};
    
    //y position of stars - arbitrary vales - Recommended to keep above 60 
    static double starY[] = {
        91, 86, 94, 89, 96, 83, 90, 88, 93, 85, 92, 87,
        76, 79, 73, 81, 77, 74, 80, 72, 78, 75, 70,
        64, 61, 67, 58, 63, 69, 57, 65};
    
    int nStar = (int)(sizeof(starX) / sizeof(starX[0])); //Number of stars
    plcol0(C_WHITE);
    plssym(0.0, 0.35);              //Set the symbol size (height of symbol in mm (0.0 is default), Scaler for actual height (arbitrary))
    plpoin(nStar, starX, starY, 1); // Draw the star(dot) at each point
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
    static int flagPoints = 4;

    /* Rough terrain (gap at landing zone) 
     * Landing zone sits flat with the ground
     * to make it easier to detect the landing
     * Unnamed value are arbitrary
     */
    double terrainX[] = {X_MIN, 6, 12, 18, 24, lzL, lzL, lzR, lzR, 74, 80, 86, 93, WORLD_W};
    double terrainY[] = {SURFACE_Y, 5, 8, 6, 9, 9, SURFACE_Y, SURFACE_Y, 9, 8, 5, 7, 5, SURFACE_Y}; // Keep terrainY value under double SURFACE_Y
    int terrainPoints = (int)(sizeof(terrainX) / sizeof(terrainX[0]));
    plcol0(C_GREY);
    pllsty(LS_SOLID);
    plline(terrainPoints, terrainX, terrainY);

    /* Landing-zone platform 
     * Flat landing zone to make
     * it easier to see
     */
    line2(lzL, SURFACE_Y, lzR, SURFACE_Y);

    // Flag poles (Scaler value are arbitrary)
    line2(lzL, SURFACE_Y, lzL, SURFACE_Y + 4.0); //Left
    line2(lzR, SURFACE_Y, lzR, SURFACE_Y + 4.0); //Right

    // Flag triangles (Scaler value are arbitrary)
    // Left triangle
    double flagLeftX[] = { lzL, lzL, lzL + 1.5, lzL }; 
    double flagLeftY[] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    // Right triangle
    double flagRightX[] = { lzR, lzR, lzR - 1.5, lzR }; 
    double flagRightY[] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    
    // Daws a solid filled polygon
    plfill(flagPoints, flagLeftX, flagLeftY);
    plfill(flagPoints, flagRightX, flagRightY);
}

/*
 * draw_lander: Draws the lunar lander. Uses an array
 *              to set points where the lines will change
 *              directions to draw the parts of the lander.
 *              The points for each line are determined
 *              by scaling the set radius that the lander 
 *              will be drawn in and adding or subtracting
 *              it from the center position of the lander,
 *              depending on which quadrent it should be in.
 * double cx: Center x position of the lander
 * double cy: Center y position of the lander
 * int crashed: Checks if the lander crashed and either draws 
 *              the lander with dashed lines if it crashes and
 *              solid lines if lands of if its flying
 * Return:N/A
 */
void draw_lander(double cx, double cy, int crashed)
{
    /*    ALL SCALER VALUES ARE ARBITRARY - CHANGE AT YOUR OWN RISK    */
    
    double r = LANDER_R;
    static int stageBodyPoints = 5, antDishPoints = 3, engBellPoints = 4; // Shape line points - DO NOT CHANGE
    
    plcol0(C_BLUEVIOLET);
    pllsty(crashed ? LS_DASHED : LS_SOLID); // Line style for the lander based on if its crashed or not

    // Descent stage body (Middle piece)
    double dStageBodyX[] = {cx-r*0.9, cx+r*0.9, cx+r*0.6, cx-r*0.6, cx-r*0.9};
    double dStageBodyY[] = {cy-r*0.2, cy-r*0.2, cy+r*0.5, cy+r*0.5, cy-r*0.2};
    plline(stageBodyPoints, dStageBodyX, dStageBodyY);

    // Ascent stage body (Top piece)
    double aStageBodyX[] = {cx-r*0.4, cx+r*0.4, cx+r*0.4, cx-r*0.4, cx-r*0.4};
    double aStageBodyY[] = {cy+r*0.5, cy+r*0.5, cy+r*1.1, cy+r*1.1, cy+r*0.5};
    plline(stageBodyPoints, aStageBodyX, aStageBodyY);

    // Antenna & dish 
    line2(cx, cy+r*1.1, cx, cy+r*1.5); // Antenna
    double antDishX[] = {cx-r*0.2, cx, cx+r*0.2};
    double antDishY[] = {cy+r*1.5, cy+r*1.7, cy+r*1.5};
    plline(antDishPoints, antDishX, antDishY);

    // Landing legs & foot pads 
    line2(cx-r*0.9, cy-r*0.2, cx-r*1.5, cy-r*1.0); // Right leg
    line2(cx+r*0.9, cy-r*0.2, cx+r*1.5, cy-r*1.0); // Left leg
    
    line2(cx-r*1.7, cy-r*1.0, cx-r*1.3, cy-r*1.0); // Right foot pad
    line2(cx+r*1.3, cy-r*1.0, cx+r*1.7, cy-r*1.0); // left foot pad

    // Engine bell - (bottom picec of the lander where the fire comes out)
    double engBellX[] = {cx-r*0.3, cx+r*0.3, cx+r*0.2, cx-r*0.2};
    double engBellY[] = {cy-r*0.2, cy-r*0.2, cy-r*0.7, cy-r*0.7};
    plline(engBellPoints, engBellX, engBellY);
    line2(engBellX[0], engBellY[0], engBellX[3], engBellY[3]);

    pllsty(LS_SOLID);
}

/*
 * draw_flame: Draws the flame under the engine bell
 *             Flames are created from a large and
 *             small v shape at thebottom of the lander
 * double cx: Center x position of the lander
 * double cy: Center y position of the lander
 * double retro: Curent thrust %, used  to size
 *               the flame
 * Return:N/A
 */
void draw_flame(double cx, double cy, double retro)
{
    static int bigFirePoint = 3, smallFirePoint = 3;
    
    if (retro < 0.01) return; // If theres less then 0.01% thrust then keep bar at 0%
    double r  = LANDER_R;
    double fl = r * retro / 0.55;

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    
    // Creates the larger V shape (Scaler value are arbitrary)
    double bigFireX[] = {cx-r*0.2, cx, cx+r*0.2};
    double bigFireY[] = {cy-r*0.7, cy-r*0.7-fl, cy-r*0.7};
    plline(bigFirePoint, bigFireX, bigFireY);
    
    // Creates the small v shape (Scaler value are arbitrary)
    double smallFireX[] = {cx-r*0.1, cx, cx+r*0.1 };
    double smallFireY[] = {cy-r*0.7, cy-r*0.7-fl*0.55, cy-r*0.7};
    plline(smallFirePoint, smallFireX, smallFireY);
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
    static int borderPoints = 5, borderFillPoints = 4;
    
    plcol0(C_WHITE);
    pllsty(LS_SOLID);

    // Draws the border around the fill area
    double barBorderX[] = {px, px+w, px+w, px, px};
    double barBorderY[] = {py, py, py+h, py+h, py};
    plline(borderPoints, barBorderX, barBorderY);

    // Draws the fill bar
    if (pct > 0.0) {
        double fw = w * (pct > 100.0 ? 100.0 : pct) / 100.0; // Fill Width
        double barBorderFillX[] = {px, px+fw, px+fw, px};
        double barBorderFillY[] = {py, py, py+h, py+h};
        plfill(borderFillPoints, barBorderFillX, barBorderFillY);
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
 * lunar_display_update: Redraws the display each frame
 * double lander_x: lander x position
 * double Lander_y: lander y position
 * double vx: horizontal velocity of lander
 * double vy: vertical velocity
 * double retro_pct: retro thrust
 * double lz_pct: landing zone
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
    char   buf[64];
    double lz_hw = lz * WORLD_W / 4.0;
    double retro_display = retro_pct * 100.0;
    double lz_display = lz * 100.0;
    double bottomVportMaxY = WORLD_H * 0.20;

    plclear(); // Clears the screen to be redraw each frame

    // Space viewport (top) 
    plvpor(0.02, 0.98, 0.30, 0.93);         // Define the normalized coordinates (0.0 - 1.0) of the viewport - DO NOT CHANGE
    plwind(X_MIN, WORLD_W, Y_MIN, WORLD_H); // Set the world coordinates for the edges of the viewport

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    
    // Creates a box with no tick marks on the X or Y axis
    // Zeros are for auto tick spaceing, but without the "t" or "x" option they won't be drawn
    //"bc", b - Draws the bottom and left edge of the box
    //"bc", c - Draws the top and right edge of the box 
    plbox("bc", 0, 0, "bc", 0, 0); 

    plschr(CHR_HEIGHT_DEFAULT, 1.2);
    plmtex("t", 0.9, 0.5, JUST_CENTER, "LUNAR LANDER");
    plschr(CHR_HEIGHT_DEFAULT, 1.0); // Reset character height to default

    // Draws the stars and surface terrain
    draw_stars();
    draw_surface(lz_hw);
    // Draws the lander based on if its crashed or not
    draw_lander(lander_x, lander_y, status == LANDER_CRASHED);
    if (status == LANDER_FLYING){
        draw_flame(lander_x, lander_y, retro_pct);
    }

    // Overlay messages when the lander lands
    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    if (status == LANDER_LANDED) { 
        plschr(CHR_HEIGHT_DEFAULT, OM_TOP);                                     
        plptex(OM_X, OM_TOP_Y, SLANT_DX, SLANT_DY, JUST_CENTER, "** GOOD LANDING! **");
        plschr(CHR_HEIGHT_DEFAULT, OM_BOTTOM);
        plptex(OM_X, OM_BOTTOM_Y, SLANT_DX, SLANT_DY, JUST_CENTER, "Press Start to restart");
    }
    if (status == LANDER_CRASHED) {
        plschr(CHR_HEIGHT_DEFAULT, OM_TOP);
        plptex(OM_X, OM_TOP_Y, SLANT_DX, SLANT_DY, JUST_CENTER, "** CRASHED! **");
        plschr(CHR_HEIGHT_DEFAULT, OM_BOTTOM);
        plptex(OM_X, OM_BOTTOM_Y, SLANT_DX, SLANT_DY, JUST_CENTER, "Press Start to restart");
    }

    // Instrument panel (bottom) - Shows simulation information in the bottom 20% of the screen
    plvpor(0.02, 0.98, 0.01, 0.28);                 // Define the normalized coordinates (0.0 - 1.0) of the viewport - DO NOT CHANGE
    plwind(X_MIN, WORLD_W, Y_MIN, bottomVportMaxY); // Set the world coordinates for the edges of the viewport

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    
    // Creates a box with no tick marks on the X or Y axis
    // Zeros are for auto tick spaceing, but without the "t" or "x" option they won't be drawn
    // "bc", b - Draws the bottom and left edge of the box
    // "bc", c - Draws the top and right edge of the box 
    plbox("bc", 0, 0, "bc", 0, 0);
    
    plschr(CHR_HEIGHT_DEFAULT, IP_TEXT_SIZE);

    // Divider - bottom display area to seperate the instrument panels
    line2(HALF_WORLD_W, 1.0, HALF_WORLD_W, bottomVportMaxY);

    // Retro - Shows the thrust percentage in value and visual form on the left side of instrument panel
    plptex(BAR_X, RETRO_TEXT_Y, SLANT_DX, SLANT_DY, JUST_LEFT, "Retro");
    snprintf(buf, sizeof(buf), "%.0lf %%", retro_display);
    plptex(VAL_PCT, RETRO_TEXT_Y, SLANT_DX, SLANT_DY, JUST_LEFT, buf);
    draw_bar(BAR_X, RETRO_BAR_Y, BAR_HEIGHT, BAR_WIDTH, retro_display);

    // LZ-Width - Shows the landing zone percentage in value and visual form on the left side of instrument panel
    plptex(BAR_X, LZ_TEXT_Y, SLANT_DX, SLANT_DY, JUST_LEFT, "LZ-Width");
    snprintf(buf, sizeof(buf), "%.0lf %%", lz_display);
    plptex(VAL_PCT, LZ_TEXT_Y, SLANT_DX, SLANT_DY, JUST_LEFT, buf);
    draw_bar(BAR_X, LZ_BAR_Y, BAR_HEIGHT, BAR_WIDTH, (double)lz_display);

    // Vx / Vy - Shows the X & Y velocity of the lander on the right side of the instrument panel
    plptex(VX_VY_X, VX_Y, SLANT_DX, SLANT_DY, JUST_LEFT, "Vx");
    snprintf(buf, sizeof(buf), "%+.2lf  m/sec", vx);
    plptex(VX_VY_VAL_X, VX_Y, SLANT_DX, SLANT_DY, JUST_LEFT, buf);

    plptex(VX_VY_X, VY_Y, SLANT_DX, SLANT_DY, JUST_LEFT, "Vy");
    snprintf(buf, sizeof(buf), "%+.2lf  m/sec", vy);
    plptex(VX_VY_VAL_X, VY_Y, SLANT_DX, SLANT_DY, JUST_LEFT, buf);

    // Status for bottom right corner of screen
    if (status == LANDER_LANDED || status == LANDER_CRASHED) {
        plschr(CHR_HEIGHT_DEFAULT, 1.1);
        plptex(86.0, 5.5, SLANT_DX, SLANT_DY, JUST_CENTER, status == LANDER_LANDED ? "Good Job" : "Crashed!");
    }

    plschr(CHR_HEIGHT_DEFAULT, 1.0); // Reset character height to default

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
