/*
 * lunar_display.c  –  Black & White
 *
 * 
 * Ver. 1
 */

#include "lunar_display.h"

#include <stdio.h>
#include <math.h>
#include <plplot/plplot.h>

/* ── world / layout constants ── */
#define WORLD_W    100.0
#define WORLD_H    100.0
#define SURFACE_Y    5.0
#define LZ_CENTER   50.0
#define LANDER_R     2.8

/* ── colour slots ── */
#define C_BLACK  0
#define C_WHITE  1

/* ── line styles ── */
#define LS_SOLID  1
#define LS_DASHED 2

/*
 *  Internal helpers
 */

static void line2(double x1, double y1, double x2, double y2)
{
    double x[2] = { x1, x2 };
    double y[2] = { y1, y2 };
    plline(2, x, y);
}

static void draw_stars(void)
{
    static double sx[] = {
         4, 11, 19, 28, 37, 46, 53, 63, 72, 81, 89, 96,
         7, 16, 24, 33, 42, 51, 60, 69, 78, 87, 94,
        10, 21, 31, 41, 55, 65, 75, 85
    };
    static double sy[] = {
        91, 86, 94, 89, 96, 83, 90, 88, 93, 85, 92, 87,
        76, 79, 73, 81, 77, 74, 80, 72, 78, 75, 70,
        64, 61, 67, 58, 63, 69, 57, 65
    };
    int n = (int)(sizeof(sx) / sizeof(sx[0]));
    plcol0(C_WHITE);
    plssym(0.0, 0.35);
    plpoin(n, sx, sy, 1);
}

static void draw_surface(double lz_hw)
{
    double lzL = LZ_CENTER - lz_hw;
    double lzR = LZ_CENTER + lz_hw;

    /* rough terrain (gap at landing zone) */
    double tx[] = {  0,  6, 12, 18, 24, lzL, lzL,       lzR, lzR,  74, 80, 86, 93, 100 };
    double ty[] = {  5,  5,  8,  6,  9,   9, SURFACE_Y, SURFACE_Y,   9,   8,  5,  7,  5,   5 };
    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    plline(14, tx, ty);

    /* landing-zone platform */
    line2(lzL, SURFACE_Y, lzR, SURFACE_Y);

    /* flag poles */
    line2(lzL, SURFACE_Y, lzL, SURFACE_Y + 4.0);
    line2(lzR, SURFACE_Y, lzR, SURFACE_Y + 4.0);

    /* flag triangles */
    double fl1x[4] = { lzL, lzL, lzL + 1.5, lzL };
    double fl1y[4] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    double fl2x[4] = { lzR, lzR, lzR - 1.5, lzR };
    double fl2y[4] = { SURFACE_Y+4, SURFACE_Y+2, SURFACE_Y+3, SURFACE_Y+4 };
    plfill(4, fl1x, fl1y);
    plfill(4, fl2x, fl2y);
}

/*
 * crashed = 0 → solid lines
 * crashed = 1 → dashed lines (visual distinction without colour)
 */
static void draw_lander(double cx, double cy, int crashed)
{
    double r = LANDER_R;
    plcol0(C_WHITE);
    pllsty(crashed ? LS_DASHED : LS_SOLID);

    /* descent-stage body */
    double bx[5] = { cx-r*0.9, cx+r*0.9, cx+r*0.6, cx-r*0.6, cx-r*0.9 };
    double by[5] = { cy-r*0.2, cy-r*0.2, cy+r*0.5, cy+r*0.5, cy-r*0.2 };
    plline(5, bx, by);

    /* ascent stage */
    double ax[5] = { cx-r*0.4, cx+r*0.4, cx+r*0.4, cx-r*0.4, cx-r*0.4 };
    double ay[5] = { cy+r*0.5, cy+r*0.5, cy+r*1.1, cy+r*1.1, cy+r*0.5 };
    plline(5, ax, ay);

    /* antenna + dish */
    line2(cx, cy+r*1.1, cx, cy+r*1.5);
    double dx[3] = { cx-r*0.2, cx, cx+r*0.2 };
    double dy[3] = { cy+r*1.5, cy+r*1.7, cy+r*1.5 };
    plline(3, dx, dy);

    /* landing legs + foot pads */
    line2(cx-r*0.9, cy-r*0.2, cx-r*1.5, cy-r*1.0);
    line2(cx+r*0.9, cy-r*0.2, cx+r*1.5, cy-r*1.0);
    line2(cx-r*1.7, cy-r*1.0, cx-r*1.3, cy-r*1.0);
    line2(cx+r*1.3, cy-r*1.0, cx+r*1.7, cy-r*1.0);

    /* engine bell */
    double ex[4] = { cx-r*0.3, cx+r*0.3, cx+r*0.2, cx-r*0.2 };
    double ey[4] = { cy-r*0.2, cy-r*0.2, cy-r*0.7, cy-r*0.7 };
    plline(4, ex, ey);
    line2(ex[0], ey[0], ex[3], ey[3]);

    pllsty(LS_SOLID);
}

static void draw_flame(double cx, double cy, double retro)
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

/* outline + white fill proportional to pct */
static void draw_bar(double px, double py, double w, double h, double pct)
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


void lunar_display_init(void)
{
   
    plsdev("xcairo");
    plsetopt("db", "");        /* double-buffer: no flicker */
    plsetopt("bg", "000000");  /* black background          */
    plinit();

    /* monochrome palette */
    plscol0(C_BLACK,   0,   0,   0);
    plscol0(C_WHITE, 255, 255, 255);
}

void lunar_display_update(double lander_x,
                          double lander_y,
                          double vx,
                          double vy,
                          double retro_pct,
                          int    lz_pct,
                          int    status)
{
    double lz_hw = lz_pct * WORLD_W / 200.0;
    char   buf[64];

    plbop();

    /* ── Space viewport (top 70 %) ── */
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

    /* overlay messages */
    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    if (status == LANDER_LANDED) {
        plschr(0, 1.6);
        plptex(50, 55, 1, 0, 0.5, "** GOOD LANDING! **");
        plschr(0, 1.0);
        plptex(50, 48, 1, 0, 0.5, "Press R to restart");
    }
    if (status == LANDER_CRASHED) {
        plschr(0, 1.6);
        plptex(50, 55, 1, 0, 0.5, "** CRASHED! **");
        plschr(0, 1.0);
        plptex(50, 48, 1, 0, 0.5, "Press R to restart");
    }

    /* ── Instrument panel (bottom 28 %) ── */
    plvpor(0.02, 0.98, 0.01, 0.28);
    plwind(0.0, 100.0, 0.0, 20.0);

    plcol0(C_WHITE);
    pllsty(LS_SOLID);
    plbox("bc", 0, 0, "bc", 0, 0);

    /* divider */
    line2(50.0, 1.0, 50.0, 19.0);

    /* Retro */
    plschr(0, 0.9);
    plptex(2.0, 15.5, 1, 0, 0.0, "Retro");
    snprintf(buf, sizeof(buf), "%.0f %%", retro_pct);
    plptex(22.0, 15.5, 1, 0, 0.0, buf);
    draw_bar(2.0, 13.0, 44.0, 1.5, retro_pct);

    /* LZ-Width */
    plptex(2.0, 10.5, 1, 0, 0.0, "LZ-Width");
    snprintf(buf, sizeof(buf), "%d %%", lz_pct);
    plptex(22.0, 10.5, 1, 0, 0.0, buf);
    draw_bar(2.0, 8.0, 44.0, 1.5, (double)lz_pct);

    /* Vx / Vy */
    plptex(52.0, 15.5, 1, 0, 0.0, "Vx");
    snprintf(buf, sizeof(buf), "%+.2f  m/sec", vx);
    plptex(60.0, 15.5, 1, 0, 0.0, buf);

    plptex(52.0, 10.5, 1, 0, 0.0, "Vy");
    snprintf(buf, sizeof(buf), "%+.2f  m/sec", vy);
    plptex(60.0, 10.5, 1, 0, 0.0, buf);

    /* status */
    if (status == LANDER_LANDED || status == LANDER_CRASHED) {
        plschr(0, 1.1);
        plptex(86.0, 5.5, 1, 0, 0.5,
               status == LANDER_LANDED ? "Good Job" : "Crashed!");
    }

    plschr(0, 1.0);
    pleop();
    plflush();
}

void lunar_display_close(void)
{
    plend();
}
