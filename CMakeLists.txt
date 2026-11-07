# Copyright (c) 2025 Silvio Gregorini
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20.0)

if(CONFIG_SHIELD_CHOCOFI_SH1107 AND CONFIG_ZMK_DISPLAY_STATUS_SCREEN_CUSTOM)
    target_sources(app PRIVATE boards/shields/chocofi/src/chocofi_status_screen.c)
endif()
