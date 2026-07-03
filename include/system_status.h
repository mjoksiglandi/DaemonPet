#pragma once

#include <Arduino.h>

struct SystemStatus {
  String host;
  String ip;
  int cpu = 0;
  float temp = 0.0f;
  float ramUsed = 0.0f;
  float ramTotal = 0.0f;
  int disk = 0;
  int dockerUp = 0;
  int dockerExit = 0;
  String rx;
  String tx;
  String uptime;
  bool valid = false;
  String lastError;
};
