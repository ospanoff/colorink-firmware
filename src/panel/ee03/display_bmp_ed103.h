#pragma once

#include <cstddef>
#include <cstdint>

#if defined(ED103TC2_DRIVER)

#include "ui/wake_screen.h"

bool displayBmpEe03Ed103(EPaper &epaper, const uint8_t *bmpRam, size_t bmpLen);

#endif
