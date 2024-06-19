/**
 * @file sdl.h
 *
 */

#ifndef _TSLIBINPUT_H_
#define _TSLIBINPUT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifndef LV_DRV_NO_CONF
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_drv_conf.h"
#else
#include "../../lv_drv_conf.h"
#endif
#endif

#if USE_MONITOR || USE_TSLIB

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize SDL to be used as display, mouse and mouse wheel drivers.
 */
void tslibinput_init(void);

void tslibinput_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

void schedule_cpu(int cpuIndex);

/**********************
 *      MACROS
 **********************/

#endif /* USE_MONITOR || USE_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TSLIBINPUT_H */
