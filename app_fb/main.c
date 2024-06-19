
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include "lvgl/lvgl.h"
#include "fbdev.h"
#include "tslibinput.h"
#include "ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static pthread_t thread1;

void* tick_thread(void* data) {
    schedule_cpu(2);
    while(1) {
        usleep(1000);
        lv_tick_inc(1);
    }
}


int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  schedule_cpu(3);

  /*Initialize LVGL*/
  lv_init();

  /* initialise input device */
  tslibinput_init();

  /*Initialize the HAL (display, input devices, tick) for LVGL*/
  hal_init();


  //printf("ui_init\n");
  ui_init();

  //printf("tick thread...\n");
  pthread_create(&thread1, NULL, tick_thread, NULL);

  //printf("loop...\n");

  while(1) {
    int s;
    /* Periodically call the lv_task handler.
     * It could be done in a timer interrupt or an OS task too.*/
    int r = lv_timer_handler();
    if (r < 5) {
        r = 5;
    }
    usleep(r * 1000);
  }

  return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for LVGL
 */
static void hal_init(void)
{
  uint32_t width = 0;
  uint32_t height = 0;

  /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
  fbdev_init();
  fbdev_get_sizes(&width, &height);
  printf("fbdev screen: w=%d h=%d\n", width, height);
#if 0
  if (width != 800 || height != 480) {
    width = 800;
    height= 480;
  }
#endif

  /*Create a display buffer*/
  static lv_color_t buf[SDL_HOR_RES * SDL_VER_RES];
  static lv_disp_draw_buf_t disp_draw_buf;
  lv_disp_draw_buf_init(&disp_draw_buf, buf, NULL, SDL_HOR_RES * SDL_VER_RES);

  /*Create a display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv); /*Basic initialization*/
  disp_drv.draw_buf = &disp_draw_buf;
  disp_drv.flush_cb = fbdev_flush;
  disp_drv.hor_res = width;
  disp_drv.ver_res = height;
  lv_disp_drv_register(&disp_drv);

  /* Add Tsouch screen as input device */
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv); /*Basic initialization*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = tslibinput_read;
  lv_indev_drv_register(&indev_drv);
}
