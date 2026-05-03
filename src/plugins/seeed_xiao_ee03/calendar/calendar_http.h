#pragma once

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03

#include <cstddef>
#include <cstdint>

bool calendarPostForceUpdate();
bool calendarDownloadImageBmpToPsram(uint8_t **outBuf, size_t *outLen);

#endif
