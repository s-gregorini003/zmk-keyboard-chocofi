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

#if IS_ENABLED(CONFIG_LV_FONT_UNSCII_8)
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
               ".------.\n"
               "|CHOCO |\n"
               "|  FI  |\n"
               "'------'\n"
               " [BASE] \n"
               "        \n"
               "THUMBS: \n"
               "ESC: MED\n"
               "SPC: NAV\n"
               "TAB: MSE\n"
               "RET: SYM\n"
               "BSP: NUM\n"
               "DEL: FUN\n"
               "--------\n"
               "Q+W: ESC\n"
               "F+J: CAP");
        break;

    case 1: // NAV
        strcpy(hud_buf,
               ".------.\n"
               "| NAV  |\n"
               "'------'\n"
               " R-HAND:\n"
               " · H ^ E\n"
               " C < v >\n"
               " Y V C X\n"
               "--------\n"
               "THUMBS: \n"
               "RET  BSP\n"
               "DEL     \n"
               "--------\n"
               "HLD: SPC");
        break;

    case 2: // MOUSE
        strcpy(hud_buf,
               ".------.\n"
               "|MOUSE |\n"
               "'------'\n"
               " R-HAND:\n"
               "  sU ^  \n"
               " < v >  \n"
               "  sD ·  \n"
               "--------\n"
               "CLICKS: \n"
               "RC LC MC\n"
               "--------\n"
               "HLD: TAB");
        break;

    case 3: // MEDIA
        strcpy(hud_buf,
               ".------.\n"
               "|MEDIA |\n"
               "'------'\n"
               " AUDIO: \n"
               " V-   V+\n"
               "<<    >>\n"
               "--------\n"
               "BT PROF:\n"
               "0  1 2 3\n"
               "CLR: B4 \n"
               "--------\n"
               "THUMBS: \n"
               "STP  PLY\n"
               "MUTE    \n"
               "--------\n"
               "HLD: ESC");
        break;

    case 4: // NUM
        strcpy(hud_buf,
               ".------.\n"
               "| NUM  |\n"
               "'------'\n"
               " L-HAND:\n"
               " 7 8 9 [\n"
               " 4 5 6 ;\n"
               " 1 2 3 `\n"
               "--------\n"
               "THUMBS: \n"
               " .  0  -\n"
               "--------\n"
               "HLD: BSP");
        break;

    case 5: // SYM
        strcpy(hud_buf,
               ".------.\n"
               "| SYM  |\n"
               "'------'\n"
               " L-HAND:\n"
               " & * ( {\n"
               " $ % ^ :\n"
               " ! @ # ~\n"
               "--------\n"
               "THUMBS: \n"
               " (  )  _\n"
               "--------\n"
               "HLD: RET");
        break;

    case 6: // FUN
        strcpy(hud_buf,
               ".------.\n"
               "| FUN  |\n"
               "'------'\n"
               " L-HAND:\n"
               "F7 F8 F9\n"
               "F4 F5 F6\n"
               "F1 F2 F3\n"
               "--------\n"
               "F10  F11\n"
               "F12  PSC\n"
               "--------\n"
               "THUMBS: \n"
               "APP  SPC\n"
               "TAB     \n"
               "--------\n"
               "HLD: DEL");
        break;

    default:
        snprintf(hud_buf, sizeof(hud_buf),
                 ".------.\n"
                 "|L: %02u |\n"
                 "'------'\n"
                 "        \n"
                 " ACTIVE \n"
                 " CUSTOM \n"
                 " KEYMAP \n"
                 "        \n"
                 " STUDIO \n"
                 " LAYER  \n"
                 "--------\n"
                 "        \n"
                 "        \n"
                 "--------",
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
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    hud_label = lv_label_create(screen);
    lv_obj_clear_flag(hud_label, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(hud_label, 0, 0);
    lv_obj_set_style_border_width(hud_label, 0, 0);
    lv_obj_set_size(hud_label, 64, 128);
    lv_obj_set_pos(hud_label, 0, 0);
    lv_label_set_long_mode(hud_label, LV_LABEL_LONG_CLIP);

    lv_obj_set_style_text_color(hud_label, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(hud_label, LV_OPA_TRANSP, 0);

#if IS_ENABLED(CONFIG_LV_FONT_UNSCII_8)
    lv_obj_set_style_text_font(hud_label, &lv_font_unscii_8, 0);
#endif

    widget_layer_hud_init();

    return screen;
}
