#pragma once

#include "boot_display_error.h"
#include <TFT_eSPI.h>

const char *wakeOverlayErrorShortLabel(BootDisplayError e);

/** Short error label at the top-right (opaque text; does not clear the
 * panel). Caller runs epaper.update() after all painting. On multigray EPaper,
 * uses gray indices compatible with the 4bpp sprite buffer. */
void drawWakeErrorOverlayTopRight(EPaper &display, BootDisplayError error);

/** When percent is 0–19, draw "N%" at the bottom-right. Skips unknown values
 * (negative). Caller runs epaper.update() after all painting. */
void drawWakeLowBatteryBottomRight(EPaper &display, int battery_percent);
