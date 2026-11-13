/*
 * Copyright (c) 2026 Silvio Gregorini
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

LV_FONT_DECLARE(tom_thumb);

static lv_obj_t *hud_label = NULL;
static char hud_buf[512];

struct layer_hud_state {
    uint8_t index;
};

static void set_hud_text(lv_obj_t *label, uint8_t layer_idx) {
    switch (layer_idx) {
    case 0: // BASE
        strcpy(hud_buf,
               "    [ BASE ]\n"
               "cccccccccccccccc\n"
               "a Q a W a E a R a T\n"
               "a A a S a D a F a G\n"
               "a Z a X a C a V a B\n"
               "\n"
               "a   aMEDaSPCaMSE\n"
               "cccccccccccccccc\n"
               "-- RIGHT HAND --\n"
               "a Y a U a I a O a P\n"
               "a H a J a K a L a '\n"
               "a N a M a , a . a /\n"
               "a   aSYMaNUMaFUN\n"
               "cccccccccccccccc\n"
               "aQ+W:ESC  F+J:CAP\n"
               "aX+C:CPY  C+V:PST\n"
               "aX+V:CUT  L+':;");
        break;

    case 1: // NAV
        strcpy(hud_buf,
               "    b[ NAV ]\n"
               "cccccccccccccccc\n"
               "aINSaHOMabUPbaENDaPGU\n"
               "aCAPaLFTaDWNaRGTaPGD\n"
               "aRDOaPSTaCPYaCUTaUND\n"
               "\n"
               "a   aRETaBSPaDEL\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "aGUIaALTaCTLaSFT\n"
               "a   aALT\n"
               "a   a   aSPC\n"
               "cccccccccccccccc\n"
               "aHOLD: SPACE\n"
               "a^Z:UNDO  ^Y:REDO\n"
               "a^C:COPY  ^V:PST");
        break;

    case 2: // MOUSE
        strcpy(hud_buf,
               "   b[ MOUSE ]\n"
               "cccccccccccccccc\n"
               "a   absUbabmUbabsD\n"
               "a   abmLbabmDbabmR\n"
               "a   absLba   absR\n"
               "\n"
               "a   abRCbabLCbabMC\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "aGUIaALTaCTLaSFT\n"
               "a   aALT\n"
               "a   a   aTAB\n"
               "cccccccccccccccc\n"
               "aHOLD: TAB\n"
               "aLC:LEFT  RC:RGHT\n"
               "aMC:MID m:MV s:SC");
        break;

    case 3: // MEDIA
        strcpy(hud_buf,
               "   b[ MEDIA ]\n"
               "cccccccccccccccc\n"
               "\n"
               "a   aPRVabV-babV+baNXT\n"
               "aBT0aBT1aBT2aBT3aCLR\n"
               "\n"
               "a   aSTPaPLYaMUT\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "aGUIaALTaCTLaSFT\n"
               "a   aALT\n"
               "a   aESC\n"
               "cccccccccccccccc\n"
               "aHOLD: ESCAPE\n"
               "aBT0-3: PROFILES\n"
               "aCLR: CLEAR BT");
        break;

    case 4: // NUM
        strcpy(hud_buf,
               "    b[ NUM ]\n"
               "cccccccccccccccc\n"
               "a [ a 7 a 8 a 9 a ]\n"
               "a ; a 4 a 5 a 6 a =\n"
               "a ` a 1 a 2 a 3 a \\\n"
               "\n"
               "a   a . a 0 a -\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "a   aSFTaCTLaALTaGUI\n"
               "a   a   a   aALT\n"
               "a   a   aBSP\n"
               "cccccccccccccccc\n"
               "aHOLD: BACKSPACE\n"
               "a7 8 9: TOP ROW\n"
               "a4 5 6: HOME ROW");
        break;

    case 5: // SYM
        strcpy(hud_buf,
               "    b[ SYM ]\n"
               "cccccccccccccccc\n"
               "a { a & a * a ( a }\n"
               "a : a $ a % a ^ a +\n"
               "a ~ a ! a @ a # a |\n"
               "\n"
               "a   a ( a ) a _\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "a   aSFTaCTLaALTaGUI\n"
               "a   a   a   aALT\n"
               "a   aRET\n"
               "cccccccccccccccc\n"
               "aHOLD: RETURN\n"
               "a& * (: NUM SHIFT\n"
               "a$ % ^: NUM SHIFT");
        break;

    case 6: // FUN
        strcpy(hud_buf,
               "    b[ FUN ]\n"
               "cccccccccccccccc\n"
               "aF12abF7babF8babF9baPSC\n"
               "aF11abF4babF5babF6baSLK\n"
               "aF10abF1babF2babF3baBRK\n"
               "\n"
               "a   aAPPaSPCaTAB\n"
               "cccccccccccccccc\n"
               "b-- HOLD MODS --\n"
               "\n"
               "a   aSFTaCTLaALTaGUI\n"
               "a   a   a   aALT\n"
               "a   a   a   aDEL\n"
               "cccccccccccccccc\n"
               "aHOLD: DELETE\n"
               "aF7-F9: TOP ROW\n"
               "aF4-F6: HOME ROW");
        break;

    default:
        snprintf(hud_buf, sizeof(hud_buf),
                 "    [ L: %02u ]\n"
                 "cccccccccccccccc\n"
                 "\n"
                 "   ZMK STUDIO\n"
                 "  ACTIVE LAYER\n"
                 "cccccccccccccccc",
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
    lv_obj_set_size(hud_label, 68, 128);
    lv_obj_set_pos(hud_label, 0, 1);
    lv_label_set_long_mode(hud_label, LV_LABEL_LONG_CLIP);

    lv_obj_set_style_text_color(hud_label, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(hud_label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_font(hud_label, &tom_thumb, 0);

    widget_layer_hud_init();

    return screen;
}
