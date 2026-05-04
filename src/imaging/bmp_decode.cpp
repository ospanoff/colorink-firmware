#include "imaging/bmp_decode.h"

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include <esp_heap_caps.h>

struct BmpPixelInfo {
  uint32_t bfOffBits = 0;
  uint32_t srcW = 0;
  uint32_t srcH = 0;
  bool bottom_up = true;
  unsigned rowStride = 0;
  uint16_t bpp = 24;
  uint16_t nPal = 0;
  uint8_t palB[256]{};
  uint8_t palG[256]{};
  uint8_t palR[256]{};
};

static uint16_t le16(const uint8_t *p) {
  return static_cast<uint16_t>(p[0]) | static_cast<uint16_t>(p[1] << 8);
}

static uint32_t le32(const uint8_t *p) {
  return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
         (static_cast<uint32_t>(p[2]) << 16) |
         (static_cast<uint32_t>(p[3]) << 24);
}

static inline uint8_t bgrToGray4(uint8_t b, uint8_t g, uint8_t r) {
  const unsigned y =
      (77u * static_cast<unsigned>(r) + 150u * static_cast<unsigned>(g) +
       29u * static_cast<unsigned>(b)) >>
      8;
  return static_cast<uint8_t>((y * 15u + 127u) / 255u);
}

static inline void setPackedGray4(uint8_t *buf, int dstW, int x, int y,
                                  uint8_t nib) {
  const unsigned idx = static_cast<unsigned>(y) * static_cast<unsigned>(dstW) +
                       static_cast<unsigned>(x);
  const size_t bo = idx >> 1u;
  if (idx & 1u) {
    buf[bo] = static_cast<uint8_t>((buf[bo] & 0xF0u) | (nib & 0x0Fu));
  } else {
    buf[bo] = static_cast<uint8_t>((buf[bo] & 0x0Fu) | ((nib & 0x0Fu) << 4u));
  }
}

static inline void letterboxExtents(uint32_t srcW, uint32_t srcH, int dstW,
                                    int dstH, int &scW, int &scH, int &x0,
                                    int &y0) {
  if (srcW == 0 || srcH == 0 || dstW <= 0 || dstH <= 0) {
    scW = scH = x0 = y0 = 0;
    return;
  }
  const double sx = static_cast<double>(dstW) / static_cast<double>(srcW);
  const double sy = static_cast<double>(dstH) / static_cast<double>(srcH);
  const double s = sx < sy ? sx : sy;
  scW = static_cast<int>(static_cast<double>(srcW) * s + 0.5);
  scH = static_cast<int>(static_cast<double>(srcH) * s + 0.5);
  if (scW < 1) {
    scW = 1;
  }
  if (scH < 1) {
    scH = 1;
  }
  if (scW > dstW) {
    scW = dstW;
  }
  if (scH > dstH) {
    scH = dstH;
  }
  x0 = (dstW - scW) / 2;
  y0 = (dstH - scH) / 2;
}

static void sampleBgrAtColumn(const uint8_t *lineBuf, const BmpPixelInfo &info,
                              unsigned sxCol, uint8_t &ob, uint8_t &og,
                              uint8_t &orv) {
  if (info.bpp == 24) {
    const unsigned base = sxCol * 3u;
    ob = lineBuf[base + 0];
    og = lineBuf[base + 1];
    orv = lineBuf[base + 2];
  } else {
    const unsigned pix =
        sxCol >= info.srcW ? 0u : static_cast<unsigned>(lineBuf[sxCol]);
    const unsigned np = static_cast<unsigned>(info.nPal);
    unsigned pj = pix < np ? pix : (np ? np - 1u : 0u);
    ob = info.palB[pj];
    og = info.palG[pj];
    orv = info.palR[pj];
  }
}

static bool parseBmpPalettizedOrRgb24FromRam(const uint8_t *d, size_t len,
                                             uint32_t expectedBmpW,
                                             uint32_t expectedBmpH,
                                             BmpPixelInfo &out) {
  if (len < 54u) {
    Serial.println("bmp(RAM): short header");
    return false;
  }

  if (le16(d + 0) != 0x4D42u) {
    Serial.println("bmp(RAM): magic");
    return false;
  }

  out.bfOffBits = le32(d + 10);
  const uint32_t biSize = le32(d + 14);
  if (out.bfOffBits < 14 + biSize || biSize < 40u) {
    Serial.printf("bmp(RAM): bad dib off=%u biSize=%u\n",
                  unsigned(out.bfOffBits), unsigned(biSize));
    return false;
  }

  const int32_t biWidthSigned = static_cast<int32_t>(le32(d + 18));
  const int32_t biHeightSigned = static_cast<int32_t>(le32(d + 22));
  const uint16_t planes = le16(d + 26);
  const uint16_t bitCount = le16(d + 28);
  const uint32_t compression = le32(d + 30);

  if (biWidthSigned <= 0 || planes != 1 || (bitCount != 8 && bitCount != 24) ||
      compression != 0u) {
    Serial.printf(
        "bmp(RAM): w=%ld bpp=%u comp=%u planes=%u (need bpp 8 or 24)\n",
        static_cast<long>(biWidthSigned), unsigned(bitCount),
        unsigned(compression), unsigned(planes));
    return false;
  }

  out.bpp = bitCount;
  out.srcW = static_cast<uint32_t>(biWidthSigned);
  const int32_t bh = biHeightSigned;
  out.bottom_up = bh > 0;
  out.srcH = static_cast<uint32_t>(out.bottom_up ? bh : -bh);

  if (expectedBmpW > 0 &&
      (out.srcW != expectedBmpW || out.srcH != expectedBmpH)) {
    Serial.printf(
        "bmp(RAM): note bitmap %lux%lu (expected %lux%lu); decoding anyway\n",
        static_cast<unsigned long>(out.srcW),
        static_cast<unsigned long>(out.srcH),
        static_cast<unsigned long>(expectedBmpW),
        static_cast<unsigned long>(expectedBmpH));
  }

  out.rowStride = static_cast<unsigned>(
      ((((unsigned long long)out.srcW * bitCount + 31u) / 32u) * 4u));

  const uint32_t palette_file_ofs = 14u + biSize;

  if (bitCount == 8) {
    const uint32_t clrUsed = le32(d + 46);
    out.nPal = clrUsed != 0u ? static_cast<uint16_t>(clrUsed) : uint16_t{256};
    if (out.nPal > 256) {
      Serial.printf("bmp(RAM): palette too large (%u)\n", unsigned(out.nPal));
      return false;
    }

    const uint32_t palette_end = palette_file_ofs + out.nPal * 4u;
    if (palette_end > out.bfOffBits) {
      Serial.println("bmp(RAM): palette overlaps pixel offset");
      return false;
    }
    if (static_cast<size_t>(palette_end) > len) {
      Serial.println("bmp(RAM): palette past buffer");
      return false;
    }

    const uint8_t *raw = d + palette_file_ofs;
    for (unsigned i = 0; i < out.nPal; ++i) {
      out.palB[i] = raw[i * 4u + 0u];
      out.palG[i] = raw[i * 4u + 1u];
      out.palR[i] = raw[i * 4u + 2u];
    }
    Serial.printf("bmp(RAM): 8 bpp, %u palette colors\n", unsigned(out.nPal));
    Serial.flush();
  }

  const uint64_t rasterEnd =
      static_cast<uint64_t>(out.bfOffBits) +
      static_cast<uint64_t>(out.rowStride) * static_cast<uint64_t>(out.srcH);
  if (rasterEnd > static_cast<uint64_t>(len)) {
    Serial.println("bmp(RAM): raster past buffer");
    return false;
  }

  return true;
}

bool bmpDecodeLetterboxGray4PackedFromRam(const uint8_t *bmpData, size_t bmpLen,
                                          uint32_t expectedBmpW,
                                          uint32_t expectedBmpH, int dstW,
                                          int dstH, uint8_t **outGray4) {
  *outGray4 = nullptr;

  Serial.printf("bmp(RAM): decode gray16 (4 bpp packed) letterbox → %dx%d\n",
                dstW, dstH);
  Serial.flush();

  if (dstW <= 0 || dstH <= 0) {
    return false;
  }
  if ((dstW & 1) != 0) {
    Serial.println("bmp(RAM): gray4 decode needs even dst width");
    return false;
  }

  BmpPixelInfo info;
  if (!parseBmpPalettizedOrRgb24FromRam(bmpData, bmpLen, expectedBmpW,
                                        expectedBmpH, info)) {
    return false;
  }

  const uint8_t *pixBase = bmpData + info.bfOffBits;

  const size_t packedBytes =
      (static_cast<size_t>(dstW) * static_cast<size_t>(dstH)) / 2u;

  auto *buf =
      static_cast<uint8_t *>(heap_caps_malloc(packedBytes, MALLOC_CAP_SPIRAM));
  if (!buf) {
    Serial.println("bmp(RAM): gray4 PSRAM alloc failed");
    return false;
  }

  memset(buf, 0xFFu, packedBytes);

  int scW = 0;
  int scH = 0;
  int x0 = 0;
  int y0 = 0;
  letterboxExtents(info.srcW, info.srcH, dstW, dstH, scW, scH, x0, y0);

  const int denomX = scW > 1 ? scW - 1 : 1;
  const int denomY = scH > 1 ? scH - 1 : 1;

  auto physRowBottomUp = [&](int32_t rowFromTop) -> unsigned {
    if (!info.bottom_up) {
      return static_cast<unsigned>(rowFromTop);
    }
    return static_cast<unsigned>(static_cast<int32_t>(info.srcH) - 1 -
                                 rowFromTop);
  };

  uint8_t *lineBuf = static_cast<uint8_t *>(malloc(info.rowStride));
  if (!lineBuf) {
    heap_caps_free(buf);
    Serial.println("bmp(RAM): line buf malloc failed");
    return false;
  }

  const int sxMax = info.srcW > 1 ? static_cast<int>(info.srcW) - 1 : 0;
  const int syMax = info.srcH > 1 ? static_cast<int>(info.srcH) - 1 : 0;

  for (int py = 0; py < scH; ++py) {
    const int syTop = (syMax * py) / denomY;
    const unsigned fileRow = physRowBottomUp(syTop);
    const size_t rowOff =
        static_cast<size_t>(fileRow) * static_cast<size_t>(info.rowStride);
    const uint64_t rowEnd =
        static_cast<uint64_t>(info.bfOffBits) +
        static_cast<uint64_t>(rowOff + static_cast<size_t>(info.rowStride));
    if (rowEnd > static_cast<uint64_t>(bmpLen)) {
      free(lineBuf);
      heap_caps_free(buf);
      Serial.println("bmp(RAM): row out of bounds");
      return false;
    }
    memcpy(lineBuf, pixBase + rowOff, info.rowStride);

    for (int px = 0; px < scW; ++px) {
      const unsigned sxPixel =
          sxMax <= 0 ? 0u : static_cast<unsigned>((sxMax * px) / denomX);
      uint8_t bp = 0, gp = 0, rp = 0;
      sampleBgrAtColumn(lineBuf, info, sxPixel, bp, gp, rp);
      const uint8_t g4 = bgrToGray4(bp, gp, rp);
      setPackedGray4(buf, dstW, x0 + px, y0 + py, g4);
    }
    if ((py & 0x3F) == 0) {
      yield();
    }
  }

  free(lineBuf);

  *outGray4 = buf;
  Serial.println("bmp(RAM): gray decode done");
  Serial.flush();
  return true;
}

#endif
