#include "status_client.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "config.h"

bool StatusClient::fetch(SystemStatus& status, String& errorMessage) {
  HTTPClient http;
  http.setTimeout(4000);

  if (!http.begin(AppConfig::STATUS_ENDPOINT)) {
    errorMessage = "No se pudo abrir URL";
    return false;
  }

  const int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    errorMessage = "HTTP " + String(httpCode);
    http.end();
    return false;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getStream());
  if (err) {
    errorMessage = "JSON invalido";
    http.end();
    return false;
  }

  status.host = doc["host"] | "-";
  status.ip = doc["ip"] | "-";
  status.cpu = doc["cpu"] | 0;
  status.temp = doc["temp"] | 0.0f;
  status.ramUsed = doc["ram_used"] | 0.0f;
  status.ramTotal = doc["ram_total"] | 0.0f;
  status.disk = doc["disk"] | 0;
  status.dockerUp = doc["docker_up"] | 0;
  status.dockerExit = doc["docker_exit"] | 0;
  status.rx = doc["rx"] | "-";
  status.tx = doc["tx"] | "-";
  status.uptime = doc["uptime"] | "-";
  status.valid = true;
  status.lastError = "";

  http.end();
  errorMessage = "";
  return true;
}
