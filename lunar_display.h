/*
 * lunar_display.h
 *
 * Public API for the PLplot Lunar Lander display module.
 * Your simulation program calls these three functions:
 *
 *   1. lunar_display_init()          – call once at startup
 *   2. lunar_display_update(...)     – call every frame with current values
 *   3. lunar_display_close()         – call once on exit
 */


/* ── Status codes passed to lunar_display_update() ─── */
#define LANDER_FLYING   0   /* simulation still running  */
#define LANDER_LANDED   1   /* successful touchdown       */
#define LANDER_CRASHED -1   /* crash                      */

/*
 * lunar_display_init
 * ------------------
 * Initialise PLplot window.  Call once before the simulation loop.
 */
void lunar_display_init(void);

/*
 * lunar_display_update
 * --------------------
 * Render one frame.  Call this every tick from your simulation loop.
 *
 * Parameters
 * ----------
 * lander_x   : horizontal position  (world units 0–100)
 * lander_y   : vertical position    (world units 0–100, surface ≈ 5)
 * vx         : horizontal velocity  (m/s, any sign)
 * vy         : vertical velocity    (m/s, negative = falling)
 * retro_pct  : retro-thrust setting (0–100 %)
 * lz_pct     : landing-zone half-width as % of world width (e.g. 10)
 * status     : LANDER_FLYING / LANDER_LANDED / LANDER_CRASHED
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
 * -------------------
 * Shut down PLplot.  Call once after the simulation loop ends.
 */
void lunar_display_close(void);


