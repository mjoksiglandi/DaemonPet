#pragma once

#include <Arduino.h>
#include "system_status.h"

class StatusClient {
 public:
  bool fetch(SystemStatus& status, String& errorMessage);
};
