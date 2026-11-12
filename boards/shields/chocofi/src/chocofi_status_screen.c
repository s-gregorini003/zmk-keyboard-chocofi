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

struct layer_hud_state {
    uint8_t index;
};

struct layer_ui_data {
    const char *header;
    const char *top_matrix[3][5];
    const char *top_thumbs[3];
    const char *mid_header;
    const char *bot_matrix[3][5];
    const char *bot_thumbs[3];
    const char *footer[3];
};

static const struct layer_ui_data layer_definitions[7] = {
    [0] = {
        .header = "[ BASE ]",
        .top_matrix = {
            {"Q", "W", "E", "R", "T"},
            {"A", "S", "D", "F", "G"},
            {"Z", "X", "C", "V", "B"}
        },
        .top_thumbs = {"MED", "SPC", "MSE"},
        .mid_header = "-- RIGHT HAND --",
        .bot_matrix = {
            {"Y", "U", "I", "O", "P"},
            {"H", "J", "K", "L", "'"},
            {"N", "M", ",", ".", "/"}
        },
        .bot_thumbs = {"SYM", "NUM", "FUN"},
        .footer = {
            "Q+W:ESC  F+J:CAP",
            "X+C:CPY  C+V:PST",
            "X+V:CUT  L+':;  "
        }
    },
    [1] = {
        .header = "[ NAV ]",
        .top_matrix = {
            {"INS", "HOM", "UP",  "END", "PGU"},
            {"CAP", "LFT", "DWN", "RGT", "PGD"},
            {"RDO", "PST", "CPY", "CUT", "UND"}
        },
        .top_thumbs = {"RET", "BSP", "DEL"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"GUI", "ALT", "CTL", "SFT", ""},
            {"", "ALT", "", "", ""}
        },
        .bot_thumbs = {"", "SPC", ""},
        .footer = {
            "HOLD: SPACE     ",
            "^Z:UNDO  ^Y:REDO",
            "^C:COPY  ^V:PST "
        }
    },
    [2] = {
        .header = "[ MOUSE ]",
        .top_matrix = {
            {"", "sU", "mU", "sD", ""},
            {"", "mL", "mD", "mR", ""},
            {"", "sL", "",   "sR", ""}
        },
        .top_thumbs = {"RC", "LC", "MC"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"GUI", "ALT", "CTL", "SFT", ""},
            {"", "ALT", "", "", ""}
        },
        .bot_thumbs = {"", "TAB", ""},
        .footer = {
            "HOLD: TAB       ",
            "LC:LEFT  RC:RGHT",
            "MC:MID   m:MV s:SC"
        }
    },
    [3] = {
        .header = "[ MEDIA ]",
        .top_matrix = {
            {"", "", "", "", ""},
            {"", "PRV", "V-", "V+", "NXT"},
            {"BT0", "BT1", "BT2", "BT3", "CLR"}
        },
        .top_thumbs = {"STP", "PLY", "MUT"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"GUI", "ALT", "CTL", "SFT", ""},
            {"", "ALT", "", "", ""}
        },
        .bot_thumbs = {"ESC", "", ""},
        .footer = {
            "HOLD: ESCAPE    ",
            "BT0-3: PROFILES ",
            "CLR: CLEAR BT   "
        }
    },
    [4] = {
        .header = "[ NUM ]",
        .top_matrix = {
            {"[", "7", "8", "9", "]"},
            {";", "4", "5", "6", "="},
            {"`", "1", "2", "3", "\\"}
        },
        .top_thumbs = {".", "0", "-"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"", "SFT", "CTL", "ALT", "GUI"},
            {"", "", "", "ALT", ""}
        },
        .bot_thumbs = {"", "BSP", ""},
        .footer = {
            "HOLD: BACKSPACE ",
            "7 8 9: TOP ROW  ",
            "4 5 6: HOME ROW "
        }
    },
    [5] = {
        .header = "[ SYM ]",
        .top_matrix = {
            {"{", "&", "*", "(", "}"},
            {":", "$", "%", "^", "+"},
            {"~", "!", "@", "#", "|"}
        },
        .top_thumbs = {"(", ")", "_"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"", "SFT", "CTL", "ALT", "GUI"},
            {"", "", "", "ALT", ""}
        },
        .bot_thumbs = {"RET", "", ""},
        .footer = {
            "HOLD: RETURN    ",
            "& * (: NUM SHIFT",
            "$ % ^: NUM SHIFT"
        }
    },
    [6] = {
        .header = "[ FUN ]",
        .top_matrix = {
            {"F12", "F7", "F8", "F9", "PSC"},
            {"F11", "F4", "F5", "F6", "SLK"},
            {"F10", "F1", "F2", "F3", "BRK"}
        },
        .top_thumbs = {"APP", "SPC", "TAB"},
        .mid_header = "-- HOLD MODS --",
        .bot_matrix = {
            {"", "", "", "", ""},
            {"", "SFT", "CTL", "ALT", "GUI"},
            {"", "", "", "ALT", ""}
        },
        .bot_thumbs = {"", "", "DEL"},
        .footer = {
            "HOLD: DELETE    ",
            "F7-F9: TOP ROW  ",
            "F4-F6: HOME ROW "
        }
    }
};

static const int col_x[5] = {1, 14, 27, 40, 53};
static const int thumb_cols[3] = {1, 2, 3};
static const int top_row_y[3] = {20, 27, 34};
static const int bot_row_y[3] = {66, 73, 80};
static const int footer_y[3] = {102, 110, 118};

#define TOP_THUMB_Y 44
#define MID_HEADER_Y 57
#define BOT_THUMB_Y 89

static lv_obj_t *header_label;
static lv_obj_t *top_matrix_labels[3][5];
static lv_obj_t *top_thumb_labels[3];
static lv_obj_t *mid_header_label;
static lv_obj_t *bot_matrix_labels[3][5];
static lv_obj_t *bot_thumb_labels[3];
static lv_obj_t *footer_labels[3];

static inline int get_slot_x(int col, const char *str) {
    if (!str) {
        return col_x[col];
    }
    size_t len = strlen(str);
    int offset = (len == 1) ? 4 : (len == 2) ? 2 : 0;
    return col_x[col] + offset;
}

static inline int get_centered_x(const char *str) {
    if (!str) {
        return 0;
    }
    int w = (int)strlen(str) * 4 - 1;
    int x = (64 - w) / 2;
    return (x < 0) ? 0 : x;
}

static char fallback_hdr[16];

static void update_hud(uint8_t layer_idx) {
    const struct layer_ui_data *ui;
    static struct layer_ui_data fallback_ui;

    if (layer_idx < 7) {
        ui = &layer_definitions[layer_idx];
    } else {
        snprintf(fallback_hdr, sizeof(fallback_hdr), "[ L: %02u ]", layer_idx);
        fallback_ui.header = fallback_hdr;
        fallback_ui.mid_header = "-- STUDIO --";
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 5; c++) {
                fallback_ui.top_matrix[r][c] = "";
                fallback_ui.bot_matrix[r][c] = "";
            }
        }
        for (int i = 0; i < 3; i++) {
            fallback_ui.top_thumbs[i] = "";
            fallback_ui.bot_thumbs[i] = "";
        }
        fallback_ui.footer[0] = "ZMK STUDIO      ";
        fallback_ui.footer[1] = "ACTIVE LAYER    ";
        fallback_ui.footer[2] = "";
        ui = &fallback_ui;
    }

    if (!header_label) {
        return;
    }

    /* Update Header */
    lv_label_set_text_static(header_label, ui->header);
    lv_obj_set_pos(header_label, get_centered_x(ui->header), 2);

    /* Update Top 5x3 Matrix */
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            const char *txt = ui->top_matrix[r][c] ? ui->top_matrix[r][c] : "";
            lv_label_set_text_static(top_matrix_labels[r][c], txt);
            lv_obj_set_pos(top_matrix_labels[r][c], get_slot_x(c, txt), top_row_y[r]);
        }
    }

    /* Update Top Thumbs */
    for (int i = 0; i < 3; i++) {
        const char *txt = ui->top_thumbs[i] ? ui->top_thumbs[i] : "";
        lv_label_set_text_static(top_thumb_labels[i], txt);
        lv_obj_set_pos(top_thumb_labels[i], get_slot_x(thumb_cols[i], txt), TOP_THUMB_Y);
    }

    /* Update Mid Header */
    lv_label_set_text_static(mid_header_label, ui->mid_header);
    lv_obj_set_pos(mid_header_label, get_centered_x(ui->mid_header), MID_HEADER_Y);

    /* Update Bot 5x3 Matrix */
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            const char *txt = ui->bot_matrix[r][c] ? ui->bot_matrix[r][c] : "";
            lv_label_set_text_static(bot_matrix_labels[r][c], txt);
            lv_obj_set_pos(bot_matrix_labels[r][c], get_slot_x(c, txt), bot_row_y[r]);
        }
    }

    /* Update Bot Thumbs */
    for (int i = 0; i < 3; i++) {
        const char *txt = ui->bot_thumbs[i] ? ui->bot_thumbs[i] : "";
        lv_label_set_text_static(bot_thumb_labels[i], txt);
        lv_obj_set_pos(bot_thumb_labels[i], get_slot_x(thumb_cols[i], txt), BOT_THUMB_Y);
    }

    /* Update Footer */
    for (int i = 0; i < 3; i++) {
        const char *txt = ui->footer[i] ? ui->footer[i] : "";
        lv_label_set_text_static(footer_labels[i], txt);
        lv_obj_set_pos(footer_labels[i], 1, footer_y[i]);
    }
}

static void layer_hud_update_cb(struct layer_hud_state state) {
    update_hud(state.index);
}

static struct layer_hud_state layer_hud_get_state(const zmk_event_t *eh) {
    return (struct layer_hud_state){
        .index = zmk_keymap_highest_layer_active(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_hud, struct layer_hud_state,
                            layer_hud_update_cb, layer_hud_get_state)

ZMK_SUBSCRIPTION(widget_layer_hud, zmk_layer_state_changed);

static lv_obj_t *create_separator(lv_obj_t *parent, lv_coord_t y) {
    lv_obj_t *line = lv_obj_create(parent);
    lv_obj_remove_style_all(line);
    lv_obj_clear_flag(line, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(line, 64, 1);
    lv_obj_set_pos(line, 0, y);
    lv_obj_set_style_bg_color(line, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);
    return line;
}

static lv_obj_t *create_tiny_label(lv_obj_t *parent, const lv_style_t *style) {
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_remove_style_all(lbl);
    lv_obj_clear_flag(lbl, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_style(lbl, style, 0);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_CLIP);
    return lbl;
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_border_width(screen, 0, 0);
    lv_obj_set_size(screen, 64, 128);
    lv_obj_set_pos(screen, 0, 0);
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

    static lv_style_t font_style;
    static bool style_inited = false;
    if (!style_inited) {
        lv_style_init(&font_style);
        lv_style_set_text_font(&font_style, &tom_thumb);
        lv_style_set_text_color(&font_style, lv_color_black());
        lv_style_set_bg_opa(&font_style, LV_OPA_TRANSP);
        lv_style_set_pad_all(&font_style, 0);
        lv_style_set_border_width(&font_style, 0);
        style_inited = true;
    }

    create_separator(screen, 11);
    create_separator(screen, 53);
    create_separator(screen, 98);

    header_label = create_tiny_label(screen, &font_style);

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            top_matrix_labels[r][c] = create_tiny_label(screen, &font_style);
            bot_matrix_labels[r][c] = create_tiny_label(screen, &font_style);
        }
    }

    for (int i = 0; i < 3; i++) {
        top_thumb_labels[i] = create_tiny_label(screen, &font_style);
        bot_thumb_labels[i] = create_tiny_label(screen, &font_style);
        footer_labels[i] = create_tiny_label(screen, &font_style);
    }

    mid_header_label = create_tiny_label(screen, &font_style);

    update_hud(zmk_keymap_highest_layer_active());
    widget_layer_hud_init();

    return screen;
}
