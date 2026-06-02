#ifndef IMAGES_H
#define IMAGES_H

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

LV_IMG_DECLARE(barriere_fermee);
LV_IMG_DECLARE(barriere_ouverte);

#endif /* IMAGES_H */