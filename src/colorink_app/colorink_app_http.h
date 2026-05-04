#pragma once

#include <cstddef>
#include <cstdint>

bool colorinkAppPostForceUpdate();
bool colorinkAppDownloadImageBmpToPsram(uint8_t **outBuf, size_t *outLen);
