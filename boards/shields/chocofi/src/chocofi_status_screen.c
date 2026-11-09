/*
 * Copyright (c) 2025 Silvio Gregorini
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#include <zmk/display.h>
#include <zmk/display/status_screen.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#if IS_ENABLED(CONFIG_LV_FONT_MONTSERRAT_8)
LV_FONT_DECLARE(lv_font_montserrat_8);
#elif IS_ENABLED(CONFIG_LV_FONT_UNSCII_8)
LV_FONT_DECLARE(lv_font_unscii_8);
#endif

static lv_obj_t *hud_label = NULL;
static char hud_buf[512];

struct layer_hud_state {
    uint8_t index;
};

static void set_hud_text(lv_obj_t *label, uint8_t layer_idx) {
    switch (layer_idx) {
    case 0: // BASE
        strcpy(hud_buf,
               ".----------.\n"
               "| CHOCOFI  |\n"
               "'----------'\n"
               " [BASE L0]  \n"
               "            \n"
               "  L-THUMBS  \n"
               "ESC: MEDIA  \n"
               "SPC: NAV    \n"
               "TAB: MOUSE  \n"
               "            \n"
               "  R-THUMBS  \n"
               "RET: SYM    \n"
               "BSP: NUM    \n"
               "DEL: FUN    \n"
               "------------\n"
               "Q+W:ESC  F+J:CAP");
        break;

    case 1: // NAV
        strcpy(hud_buf,
               ".----------.\n"
               "|   NAV    |\n"
               "'----------'\n"
               "  R-HAND    \n"
               "------------\n"
               " ·  H  ^  E  U\n"
               " C  <  v  >  D\n"
               " Y  V  C  X  Z\n"
               "------------\n"
               "THUMBS:     \n"
               "[RET][BSP][DEL]\n"
               "------------\n"
               "HOLD: [SPC] ");
        break;

    case 2: // MOUSE
        strcpy(hud_buf,
               ".----------.\n"
               "|  MOUSE   |\n"
               "'----------'\n"
               "  R-HAND    \n"
               "------------\n"
               " ·  sU  ^  sD ·\n"
               " ·  <   v   > ·\n"
               " ·  sL  ·  sR ·\n"
               "------------\n"
               "CLICKS:     \n"
               "[RC]  [LC]  [MC]\n"
               "------------\n"
               "HOLD: [TAB] ");
        break;

    case 3: // MEDIA
        strcpy(hud_buf,
               ".----------.\n"
               "|  MEDIA   |\n"
               "'----------'\n"
               "  R-HAND    \n"
               "------------\n"
               " ·   ·   ·   ·  ·\n"
               " ·  <<  V-  V+ >>\n"
               " B0 B1  B2  B3 CLR\n"
               "------------\n"
               "THUMBS:     \n"
               "[STP][PLY][MUT]\n"
               "------------\n"
               "HOLD: [ESC] ");
        break;

    case 4: // NUM
        strcpy(hud_buf,
               ".----------.\n"
               "|   NUM    |\n"
               "'----------'\n"
               "  L-HAND    \n"
               "------------\n"
               " [  7  8  9  ]\n"
               " ;  4  5  6  =\n"
               " `  1  2  3  \\\n"
               "------------\n"
               "THUMBS:     \n"
               "[ . ][ 0 ][ - ]\n"
               "------------\n"
               "HOLD: [BSPC]");
        break;

    case 5: // SYM
        strcpy(hud_buf,
               ".----------.\n"
               "|   SYM    |\n"
               "'----------'\n"
               "  L-HAND    \n"
               "------------\n"
               " {  &  *  (  }\n"
               " :  $  %  ^  +\n"
               " ~  !  @  #  |\n"
               "------------\n"
               "THUMBS:     \n"
               "[ ( ][ ) ][ _ ]\n"
               "------------\n"
               "HOLD: [RET] ");
        break;

    case 6: // FUN
        strcpy(hud_buf,
               ".----------.\n"
               "|   FUN    |\n"
               "'----------'\n"
               "  L-HAND    \n"
               "------------\n"
               " F12 F7 F8 F9 PSC\n"
               " F11 F4 F5 F6 SLK\n"
               " F10 F1 F2 F3 BRK\n"
               "------------\n"
               "THUMBS:     \n"
               "[APP][SPC][TAB]\n"
               "------------\n"
               "HOLD: [DEL] ");
        break;

    default:
        snprintf(hud_buf, sizeof(hud_buf),
                 ".----------.\n"
                 "| LAYER %02u |\n"
                 "'----------'\n"
                 "            \n"
                 "  DYNAMIC   \n"
                 "   LAYER    \n"
                 "            \n"
                 " CONFIGURED \n"
                 "    VIA     \n"
                 " ZMK STUDIO \n"
                 "------------",
                 layer_idx);
        break;
    }

    lv_label_set_text_static(label, hud_buf);
}

static void layer_hud_update_cb(struct layer_hud_state state) {
    if (hud_label) {
        set_hud_text(hud_label, state.index);
    }
}

static struct layer_hud_state layer_hud_get_state(const zmk_event_t *eh) {
    return (struct layer_hud_state){
        .index = zmk_keymap_highest_layer_active(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_hud, struct layer_hud_state,
                            layer_hud_update_cb, layer_hud_get_state)

ZMK_SUBSCRIPTION(widget_layer_hud, zmk_layer_state_changed);

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_size(screen, 64, 128);
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    hud_label = lv_label_create(screen);
    lv_obj_clear_flag(hud_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(hud_label, 0, 0);
    lv_obj_set_style_border_width(hud_label, 0, 0);
    lv_obj_set_size(hud_label, 64, 128);
    lv_obj_set_pos(hud_label, 0, 0);
    lv_label_set_long_mode(hud_label, LV_LABEL_LONG_CLIP);

    lv_obj_set_style_text_color(hud_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(hud_label, LV_OPA_TRANSP, 0);

#if IS_ENABLED(CONFIG_LV_FONT_MONTSERRAT_8)
    lv_obj_set_style_text_font(hud_label, &lv_font_montserrat_8, 0);
#elif IS_ENABLED(CONFIG_LV_FONT_UNSCII_8)
    lv_obj_set_style_text_font(hud_label, &lv_font_unscii_8, 0);
#endif

    widget_layer_hud_init();

    return screen;
}
