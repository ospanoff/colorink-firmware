#pragma once

#include <driver/gpio.h>

#if defined(USE_XIAO_EPAPER_DISPLAY_BOARD_EE02) ||                             \
    defined(USE_XIAO_EPAPER_DISPLAY_BOARD_EE03)

// Battery sense (esp32-photoframe driver_seeedstudio_xiao_ee02): divider on
// D0/GPIO1, enable on GPIO6.
inline constexpr gpio_num_t kBatAdcSensePin = GPIO_NUM_1;
inline constexpr gpio_num_t kBatAdcEnablePin = GPIO_NUM_6;
inline constexpr float kBatDividerRatio = 2.0f;

#else
#error "Define battery sense GPIOs for this board preset."
#endif
