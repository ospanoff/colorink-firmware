#include "ui/wake_screen.h"

#include <cstdio>

void drawWakeBatteryScreen(EPaper &display, bool wakeup_from_rtc_timer,
                           int32_t battery_mv, int battery_percent) {
  display.fillScreen(TFT_WHITE);
  display.setTextColor(TFT_BLACK, TFT_WHITE);

  display.setTextDatum(MC_DATUM);
  const int base_size = display.width() >= 900 ? 6 : 3;
  display.setTextSize(base_size);

  const int cx = display.width() / 2;
  const int h = display.height();
  const int y_timer = h / 5;
  const int y_volt = (2 * h) / 5;
  const int y_pct = (3 * h) / 5;

  display.drawString(wakeup_from_rtc_timer ? "Timer" : "Force", cx, y_timer);

  char volts_buf[72];
  if (battery_mv >= 0) {
    const unsigned frac_mv = static_cast<unsigned>(battery_mv % 1000);
    snprintf(volts_buf, sizeof(volts_buf), "%u.%03u V (%ld mV)",
             static_cast<unsigned>(battery_mv / 1000), frac_mv,
             static_cast<long>(battery_mv));
  } else {
    snprintf(volts_buf, sizeof(volts_buf), "Battery: ---");
  }
  display.drawString(volts_buf, cx, y_volt);

  char pct_buf[32];
  if (battery_percent >= 0) {
    snprintf(pct_buf, sizeof(pct_buf), "~%d%%", battery_percent);
  } else {
    snprintf(pct_buf, sizeof(pct_buf), "---");
  }
  display.drawString(pct_buf, cx, y_pct);

  display.update();
}
