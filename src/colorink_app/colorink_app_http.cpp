#include "colorink_app/colorink_app_http.h"

#include "colorink_app/colorink_app_config.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <esp_heap_caps.h>

static void slog(const char *line) {
  Serial.println(line);
  Serial.flush();
}

static bool postForceUpdate(WiFiClient &client) {
  HTTPClient http;
  if (!http.begin(client, String(kColorinkAppHttpBaseUrl))) {
    Serial.println("colorink: POST begin failed");
    return false;
  }
  http.addHeader(kColorinkAppHttpHeaderClientId, COLORINK_DEVICE_ID);
  http.addHeader(String("Content-Type"), String("application/json"));
  const int code = http.POST(String("{\"forceUpdate\":true}"));
  Serial.printf("colorink: POST base → %d\n", code);
  Serial.flush();
  http.end();

  return code >= 200 && code < 300;
}

static bool downloadImageBmpToPsram(WiFiClient &client, uint8_t **outBuf,
                                    size_t *outLen) {
  *outBuf = nullptr;
  *outLen = 0;

  HTTPClient http;
  String url(kColorinkAppHttpBaseUrl);
  url += "/image";
  if (!http.begin(client, url)) {
    Serial.println("colorink: GET begin failed");
    return false;
  }
  http.addHeader(kColorinkAppHttpHeaderClientId, COLORINK_DEVICE_ID);

  const int code = http.GET();
  Serial.printf("colorink: GET …/image → %d\n", code);
  Serial.flush();
  if (code != HTTP_CODE_OK) {
    http.end();
    return false;
  }

  const int bodyLen = http.getSize();
  if (bodyLen <= 0) {
    slog("colorink: image needs Content-Length (chunked unsupported)");
    http.end();
    return false;
  }

  Serial.printf("colorink: loading %d B into PSRAM…\n", bodyLen);
  Serial.flush();

  uint8_t *buf = static_cast<uint8_t *>(
      heap_caps_malloc(static_cast<size_t>(bodyLen), MALLOC_CAP_SPIRAM));
  if (!buf) {
    Serial.println("colorink: PSRAM alloc failed");
    http.end();
    return false;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t filled = 0;
  const size_t total = static_cast<size_t>(bodyLen);
  const uint32_t t0_ms = millis();

  while (filled < total) {
    if (millis() - t0_ms > kColorinkAppIoTimeoutMs) {
      Serial.println("colorink: PSRAM fill timeout");
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
      Serial.printf("colorink: %u / %u B\n", unsigned(filled), unsigned(total));
      Serial.flush();
    }
  }

  http.end();

  Serial.printf("colorink: PSRAM read %u B in %lu ms\n", unsigned(filled),
                static_cast<unsigned long>(millis() - t0_ms));
  Serial.flush();

  if (filled != total) {
    Serial.printf("colorink: incomplete body got=%u expected=%u\n",
                  unsigned(filled), unsigned(total));
    heap_caps_free(buf);
    return false;
  }

  *outBuf = buf;
  *outLen = total;
  return true;
}

bool colorinkAppRefreshBmpToPsram(uint8_t **outBuf, size_t *outLen) {
  *outBuf = nullptr;
  *outLen = 0;

  WiFiClient client;
  client.setConnectionTimeout(kColorinkAppIoTimeoutMs);

  if (!postForceUpdate(client)) {
    return false;
  }
  return downloadImageBmpToPsram(client, outBuf, outLen);
}
