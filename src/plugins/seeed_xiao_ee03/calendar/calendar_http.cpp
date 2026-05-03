#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03

#include "calendar_http.h"

#include "calendar_config.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

static void slog(const char *line) {
  Serial.println(line);
  Serial.flush();
}

bool calendarPostForceUpdate() {
  WiFiClient client;
  client.setConnectionTimeout(kCalendarIoTimeoutMs);

  HTTPClient http;
  if (!http.begin(client, String(kCalendarHttpBaseUrl))) {
    Serial.println("calendar: POST begin failed");
    return false;
  }
  http.addHeader(kCalendarDeviceHeader, kCalendarDeviceId);
  http.addHeader(String("Content-Type"), String("application/json"));
  const int code =
      http.POST(String("{\"forceUpdate\":true}"));
  Serial.printf("calendar: POST …/calendar → %d\n", code);
  Serial.flush();
  http.end();

  return code >= 200 && code < 300;
}

bool calendarDownloadImageBmpToPsram(uint8_t **outBuf, size_t *outLen) {
  *outBuf = nullptr;
  *outLen = 0;

  WiFiClient client;
  client.setConnectionTimeout(kCalendarIoTimeoutMs);

  HTTPClient http;
  String url(kCalendarHttpBaseUrl);
  url += "/image";
  if (!http.begin(client, url)) {
    Serial.println("calendar: GET begin failed");
    return false;
  }
  http.addHeader(kCalendarDeviceHeader, kCalendarDeviceId);

  const int code = http.GET();
  Serial.printf("calendar: GET …/image → %d\n", code);
  Serial.flush();
  if (code != HTTP_CODE_OK) {
    http.end();
    return false;
  }

  const int bodyLen = http.getSize();
  if (bodyLen <= 0) {
    slog("calendar: image needs Content-Length (chunked unsupported)");
    http.end();
    return false;
  }

  Serial.printf("calendar: loading %d B into PSRAM…\n", bodyLen);
  Serial.flush();

  uint8_t *buf = static_cast<uint8_t *>(heap_caps_malloc(
      static_cast<size_t>(bodyLen), MALLOC_CAP_SPIRAM));
  if (!buf) {
    Serial.println("calendar: PSRAM alloc failed");
    http.end();
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t filled = 0;
  const size_t total = static_cast<size_t>(bodyLen);
  const uint32_t t0_ms = millis();

  while (filled < total) {
    if (millis() - t0_ms > kCalendarIoTimeoutMs) {
      Serial.println("calendar: PSRAM fill timeout");
      heap_caps_free(buf);
      http.end();
      return false;
    }

    const int avail = stream->available();
    if (avail <= 0) {
      if (!http.connected()) {
        break;
      }
      delay(2);
      continue;
    }

    size_t want = total - filled;
    if (static_cast<size_t>(avail) < want) {
      want = static_cast<size_t>(avail);
    }
    const size_t rd =
        stream->readBytes(reinterpret_cast<char *>(buf + filled), want);
    if (rd == 0) {
      delay(2);
      continue;
    }
    filled += rd;
    if ((filled & ((size_t{1} << 18) - 1)) == 0 && filled != 0) {
      Serial.printf("calendar: %u / %u B\n", unsigned(filled), unsigned(total));
      Serial.flush();
    }
  }

  http.end();

  Serial.printf("calendar: PSRAM read %u B in %lu ms\n", unsigned(filled),
                static_cast<unsigned long>(millis() - t0_ms));
  Serial.flush();

  if (filled != total) {
    Serial.printf("calendar: incomplete body got=%u expected=%u\n",
                  unsigned(filled), unsigned(total));
    heap_caps_free(buf);
    return false;
  }

  *outBuf = buf;
  *outLen = total;
  return true;
}

#endif
