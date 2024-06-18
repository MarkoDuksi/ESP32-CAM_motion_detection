#pragma once

#include "Arduino.h"


namespace peripherals::camera {

    [[nodiscard]] bool init();
}

namespace peripherals::SD_card {

    [[nodiscard]] bool init();

    size_t write_bytes(const char* path, const uint8_t* buff, size_t buff_size);
}
