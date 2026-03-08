/**
 * @file lv_conf.h
 * Minimal LVGL v9.5 configuration for Mochi Desk Robot
 * Waveshare ESP32-S3-Touch-AMOLED-1.32 (466x466)
 */

#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
   COLOR SETTINGS
 *====================*/
#define LV_COLOR_DEPTH 16

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#define LV_MEM_SIZE (64 * 1024U)

/*====================
   HAL SETTINGS
 *====================*/
#define LV_DEF_REFR_PERIOD  33      /* ~30 fps */
#define LV_DPI_DEF          200

/*====================
   LOG SETTINGS
 *====================*/
#define LV_USE_LOG 0

/*====================
   DISPLAY
 *====================*/
#define LV_USE_DRAW_SW 1

#endif /* LV_CONF_H */
