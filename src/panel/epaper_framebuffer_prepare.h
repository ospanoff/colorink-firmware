#pragma once

#include <TFT_eSPI.h>

/** Call once after epaper.begin() before displayBmp and overlays.
 * Multigray panels: 16-level gray RAM buffer cleared to light gray so decode,
 * bitmap paint, and overlay text share one consistent 4bpp framebuffer. No-op
 * when USE_MUTIGRAY_EPAPER is not defined (e.g. color e-paper builds). */
void prepareEpaperFramebuffer(EPaper &epaper);
