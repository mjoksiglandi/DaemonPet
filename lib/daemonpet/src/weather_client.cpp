#include "weather_client.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "config.h"

namespace {

String buildWeatherUrl() {
  String url = "https://api.open-meteo.com/v1/forecast?latitude=";
  url += String(AppConfig::WEATHER_LATITUDE, 4);
  url += "&longitude=";
  url += String(AppConfig::WEATHER_LONGITUDE, 4);
  url += "&current=temperature_2m,weather_code";
  url += "&daily=weather_code,temperature_2m_max,temperature_2m_min";
  url += "&forecast_days=4";
  url += "&timezone=";
  url += AppConfig::WEATHER_TIMEZONE;
  return url;
}

}  // namespace

bool WeatherClient::fetch(WeatherStatus& weather, String& errorMessage) {
  HTTPClient http;
  http.setTimeout(5000);

  if (!http.begin(buildWeatherUrl())) {
    errorMessage = "No se pudo abrir clima";
    return false;
  }

  const int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    errorMessage = "Clima HTTP " + String(httpCode);
    http.end();
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getStream());
  if (err) {
    errorMessage = "Clima JSON invalido";
    http.end();
    return false;
  }

  JsonObject current = doc["current"];
  JsonObject daily = doc["daily"];
  JsonArray dates = daily["time"];
  JsonArray codes = daily["weather_code"];
  JsonArray maxTemps = daily["temperature_2m_max"];
  JsonArray minTemps = daily["temperature_2m_min"];

  if (current.isNull() || daily.isNull() || dates.isNull() || codes.isNull() || maxTemps.isNull() ||
      minTemps.isNull() || dates.size() < 4 || codes.size() < 4 || maxTemps.size() < 4 ||
      minTemps.size() < 4) {
    errorMessage = "Clima incompleto";
    http.end();
    return false;
  }

  weather.locationLabel = AppConfig::WEATHER_LOCATION_LABEL;
  weather.currentTemp = current["temperature_2m"] | 0.0f;
  weather.currentWeatherCode = current["weather_code"] | -1;

  for (size_t i = 0; i < 4; ++i) {
    weather.forecast[i].date = String(dates[i].as<const char*>());
    weather.forecast[i].weatherCode = codes[i] | -1;
    weather.forecast[i].tempMax = maxTemps[i] | 0.0f;
    weather.forecast[i].tempMin = minTemps[i] | 0.0f;
  }

  weather.valid = true;
  weather.lastError = "";
  errorMessage = "";
  http.end();
  return true;
}
