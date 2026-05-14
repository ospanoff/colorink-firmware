#pragma once

#include <cstddef>
#include <cstdint>

#if defined(ED103TC2_DRIVER)

#include "boot_display_error.h"
#include <TFT_eSPI.h>

BootDisplayError displayBmpEe03Ed103(EPaper &epaper, const uint8_t *bmpRam,
                                     size_t bmpLen);

#endif
