#include <Arduino.h>
#include <TFT_eSPI.h>

#if defined(ED103TC2_DRIVER)

#include "panel/ee03/display_bmp_ed103.h"

#include "imaging/bmp_decode.h"

#include <esp_heap_caps.h>

bool displayBmpEe03Ed103(EPaper &epaper, const uint8_t *bmpRam, size_t bmpLen) {
  if (bmpRam == nullptr || bmpLen == 0) {
    Serial.println("colorink: BMP decode → failed (no data)");
    Serial.flush();
    return false;
  }

  uint8_t *packedGray4 = nullptr;
  const bool decoded_ok = bmpDecodeGray4PackedFromRam(bmpRam, bmpLen, TFT_WIDTH,
                                                      TFT_HEIGHT, &packedGray4);

  Serial.printf("colorink: BMP decode → %s\n", decoded_ok ? "ok" : "failed");
  Serial.flush();

  if (!decoded_ok) {
    if (packedGray4 != nullptr) {
      heap_caps_free(packedGray4);
    }
    return false;
  }

  Serial.println("epaper: gray push + update (may take tens of seconds)…");
  Serial.flush();
  epaper.initGrayMode(GRAY_LEVEL16);
  epaper.fillScreen(TFT_GRAY_15);
  epaper.pushImage(0, 0, TFT_WIDTH, TFT_HEIGHT,
                   reinterpret_cast<uint16_t *>(packedGray4));
  heap_caps_free(packedGray4);
  epaper.update();
  Serial.println("epaper: update done");
  Serial.flush();
  return true;
}

#endif
