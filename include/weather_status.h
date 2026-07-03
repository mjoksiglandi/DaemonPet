#pragma once

#include <Arduino.h>

struct WeatherForecastDay {
  String date;
  int weatherCode = -1;
  float tempMax = 0.0f;
  float tempMin = 0.0f;
};

struct WeatherStatus {
  String locationLabel;
  float currentTemp = 0.0f;
  int currentWeatherCode = -1;
  WeatherForecastDay forecast[4];
  bool valid = false;
  String lastError;
};
