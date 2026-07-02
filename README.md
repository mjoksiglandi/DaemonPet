# DaemonPet

> Mascota virtual y panel de estado para homelab sobre ESP32-S3 con pantalla circular tactil.

DaemonPet convierte una placa ESP32-S3 con display circular en un monitor remoto para tu homelab. Consulta un endpoint HTTP en tu red local, interpreta las metricas del servidor y muestra una cara de mascota que cambia de expresion segun el estado general del sistema.

## Why This Exists

Mirar dashboards completos para saber si el homelab "esta bien" es demasiado para una consulta rapida. DaemonPet busca darte una senal visual inmediata y, al mismo tiempo, mantener a mano las metricas clave del servidor desde un display pequeno, silencioso y siempre visible.

## Current Status

La version actual ya compila en PlatformIO y cubre la primera meta funcional:

- Inicializa el display circular GC9A01.
- Muestra pantalla de arranque y UI simple por vistas.
- Conecta por WiFi a tu red local.
- Consulta un endpoint HTTP configurable.
- Parsea JSON con ArduinoJson.
- Calcula el estado de la mascota con reglas de prioridad.
- Rota vistas automaticamente y deja touch preparado.
- Maneja errores de WiFi, HTTP y JSON.

## Hardware Baseline

La carpeta de referencia del fabricante y el ejemplo `FullFunctionTest.ino` indican esta base para la placa incluida en el proyecto:

- MCU: ESP32-S3 con `16 MB Flash` y `16 MB PSRAM`.
- Display: `GC9A01`, `240x240`, bus paralelo `I80/8080 de 8 bits`.
- Touch: familia `CST816` en I2C.
- Backlight: `GPIO42`.
- Reset LCD: `GPIO21`.
- I2C: `SDA GPIO8`, `SCL GPIO9`.
- Touch reset: `GPIO0`.
- Expansor de interrupciones: `TCA6408` en `0x20`, INT en `GPIO45`.
- Bus LCD:
  - `DC GPIO18`
  - `CS GPIO2`
  - `WR GPIO3`
  - `D0..D7 = GPIO10..GPIO17`

Perifericos detectados en el paquete del fabricante:

- IMU `QMI8658`
- WS2812 en `GPIO46`
- microSD (`CS GPIO40`, `SCK GPIO41`, `MOSI GPIO47`, `MISO GPIO48`)

## Project Layout

```text
DaemonPet/
|-- platformio.ini
|-- README.md
|-- ROADMAP.md
|-- src/
|   |-- main.cpp
|   |-- config.h
|   |-- display_config.h
|   |-- wifi_manager.h
|   |-- wifi_manager.cpp
|   |-- status_client.h
|   |-- status_client.cpp
|   |-- system_status.h
|   |-- pet_state.h
|   |-- pet_state.cpp
|   |-- ui.h
|   `-- ui.cpp
`-- ESP32S3-NxxRxx-128I80T_开发板 V1.01 (1)/
```

## Quick Start

### 1. Prerequisites

Necesitas:

- PlatformIO Core o VS Code + PlatformIO.
- Una placa compatible con la base `ESP32-S3 N16R16`.
- Un endpoint HTTP local que responda JSON con el formato esperado.

### 2. Configure WiFi and Endpoint

Edita `src/config.h`:

```cpp
static constexpr char WIFI_SSID[] = "TU_WIFI";
static constexpr char WIFI_PASSWORD[] = "TU_PASSWORD";
static constexpr char STATUS_ENDPOINT[] = "http://192.168.1.20/status.json";
```

Tambien puedes ajustar:

- `HTTP_REFRESH_MS`
- `WIFI_RETRY_MS`
- `VIEW_ROTATION_MS`
- `CPU_HIGH_THRESHOLD`
- `CPU_SLEEPY_THRESHOLD`
- `TEMP_HOT_THRESHOLD`

### 3. Build

```powershell
pio run
```

Si `pio` no esta en tu `PATH`, puedes usar:

```powershell
$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe run
```

### 4. Upload

```powershell
pio run --target upload
```

### 5. Serial Monitor

```powershell
pio device monitor
```

## Expected JSON

DaemonPet espera un JSON simple como este:

```json
{
  "host": "pi-homelab",
  "ip": "192.168.1.20",
  "cpu": 23,
  "temp": 48.2,
  "ram_used": 3.1,
  "ram_total": 8,
  "disk": 62,
  "docker_up": 18,
  "docker_exit": 0,
  "rx": "12M",
  "tx": "2M",
  "uptime": "3d 04h"
}
```

## Pet State Rules

DaemonPet aplica estas prioridades:

1. Desconectada: no hay WiFi o el endpoint no responde.
2. Acalorada: temperatura mayor a `TEMP_HOT_THRESHOLD`.
3. Alerta: `docker_exit > 0`.
4. Nerviosa: CPU mayor a `CPU_HIGH_THRESHOLD`.
5. Somnolienta: CPU menor a `CPU_SLEEPY_THRESHOLD` y todo lo demas OK.
6. Normal: estado estable.

## Views

### Mascota

- Cara principal.
- Estado general.
- Metrica critica o resumen corto.

### Sistema

- CPU
- Temperatura
- RAM
- Disco

### Red

- Host
- IP
- RX
- TX
- Uptime

### Docker

- Contenedores activos
- Contenedores caidos

## Touch and Rotation

La arquitectura actual soporta dos caminos:

- Si el touch responde, tocar cambia de vista.
- Si el touch no esta listo o no se quiere usar aun, las vistas rotan automaticamente.

Esto permite iterar despues sobre calibracion, gestos o navegacion mas compleja sin reescribir la UI.

## Documentation Notes

- Este proyecto usa `Arduino_GFX` porque coincide con la inicializacion observada en los ejemplos del fabricante para GC9A01 sobre bus I80.
- La deteccion exacta del touch queda pendiente de validacion fisica en placa para confirmar si el chip es `CST816S`, `CST816T` o `CST816D`.
- Los PDF y esquemas del fabricante se conservan en el repo como referencia de hardware.

## Roadmap

El plan de evolucion vive en `ROADMAP.md`.

## Validation

La base actual fue compilada con exito en este entorno con:

```powershell
pio run
```

Resultado de referencia de la build:

- RAM usada: `21.6%`
- Flash usada: `18.5%`

## Next Steps

- Probar la orientacion real del touch en hardware.
- Subir firmware a placa y validar brillo, rotacion y respuesta tactil.
- Afinar layout para legibilidad sobre display circular.
- Agregar iconografia o animaciones simples de mascota.
- Evaluar MQTT, OTA y Home Assistant en una segunda fase.
