# DaemonPet

> Mascota virtual para homelab y panel de estado remoto sobre ESP32-S3 con pantalla circular.

DaemonPet convierte una placa ESP32-S3 con display GC9A01 en un companion de escritorio para tu homelab. Consulta un endpoint HTTP local, resume la salud del host y la presenta como una mascota con estados emocionales, junto con vistas tecnicas de sistema, red, Docker y clima.

## Que resuelve

Un dashboard completo sirve para diagnostico, pero no para una lectura de un segundo. DaemonPet busca cubrir ese hueco: darte una senal inmediata de "todo bien" o "algo esta mal" y, si hace falta, mostrar los datos minimos para entender el problema sin abrir otra pantalla.

## Estado actual

Al 3 de julio de 2026, el proyecto esta en una fase funcional temprana:

- Compila correctamente en PlatformIO para `esp32-s3-devkitc1-n16r16`.
- Ya implementa WiFi, polling HTTP, parseo JSON y estado emocional de la mascota.
- Ya implementa una UI circular con 5 vistas: `Pet`, `System`, `Network`, `Docker` y `Weather`.
- Ya consume clima actual y forecast de 3 dias desde Open-Meteo.
- Todavia no tiene validacion completa en hardware real para touch, orientacion y comportamiento en uso continuo.
- Todavia no tiene pruebas automatizadas.

La build fue verificada en este workspace con:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run
```

Resultado observado el 3 de julio de 2026:

- RAM: `21.6%` (`70840 / 327680 bytes`)
- Flash: `18.8%` (`1228854 / 6553600 bytes`)

## Funcionalidad implementada

- Inicializacion del display circular GC9A01 sobre bus `I80/8080`.
- Splash screen y rotacion automatica de vistas.
- Conexion WiFi con reconexion periodica.
- Consulta HTTP a un endpoint local configurable.
- Parseo de payload JSON de estado del host.
- Reglas de estado de mascota segun conectividad, temperatura, Docker y CPU.
- Vista de sistema con CPU, temperatura, RAM y disco.
- Vista de red con host, IP, RX, TX y uptime.
- Vista Docker con contenedores activos y caidos.
- Vista de clima con temperatura actual, condicion y forecast de 3 dias.
- Tap basico para cambiar de vista cuando el touch responde.

## Arquitectura rapida

El firmware esta organizado para crecer sin concentrar toda la logica en `main.cpp`:

- `src/main.cpp`: orquestacion del loop principal.
- `include/config.h`: configuracion del proyecto y thresholds.
- `include/display_config.h`: pines y constantes de hardware.
- `lib/daemonpet/src/wifi_manager.cpp`: gestion WiFi y reconexion.
- `lib/daemonpet/src/status_client.cpp`: cliente HTTP para el endpoint local.
- `lib/daemonpet/src/weather_client.cpp`: cliente de Open-Meteo.
- `lib/daemonpet/src/pet_state.cpp`: reglas de interpretacion emocional.
- `lib/daemonpet/src/ui.cpp`: render de vistas, layout y touch basico.

## Requisitos

- Windows, macOS o Linux con PlatformIO.
- Una placa compatible con la base `ESP32-S3 N16R16`.
- Red WiFi accesible para la placa.
- Un endpoint HTTP local que entregue el payload esperado.

## Inicio rapido

### 1. Configura credenciales y endpoints

Edita [include/config.h](/C:/Users/juan.cornejo/Desktop/DaemonPet/include/config.h):

```cpp
static constexpr char WIFI_SSID[] = "REPLACE_WITH_WIFI";
static constexpr char WIFI_PASSWORD[] = "REPLACE_WITH_PASSWORD";
static constexpr char STATUS_ENDPOINT[] = "http://192.168.1.20/status.json";
static constexpr char WEATHER_LOCATION_LABEL[] = "Santiago";
static constexpr char WEATHER_TIMEZONE[] = "America/Santiago";
static constexpr float WEATHER_LATITUDE = -33.4489f;
static constexpr float WEATHER_LONGITUDE = -70.6693f;
```

Tambien puedes ajustar:

- `HTTP_REFRESH_MS`
- `WEATHER_REFRESH_MS`
- `WIFI_RETRY_MS`
- `VIEW_ROTATION_MS`
- `CPU_HIGH_THRESHOLD`
- `CPU_SLEEPY_THRESHOLD`
- `TEMP_HOT_THRESHOLD`
- `BACKLIGHT_BRIGHTNESS`

### 2. Compila

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run
```

### 3. Flashea la placa

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run --target upload
```

### 4. Abre el monitor serie

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" device monitor
```

## Contrato JSON esperado

DaemonPet espera un payload simple como este:

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

Campos consumidos hoy:

- `host`
- `ip`
- `cpu`
- `temp`
- `ram_used`
- `ram_total`
- `disk`
- `docker_up`
- `docker_exit`
- `rx`
- `tx`
- `uptime`

## Estados de la mascota

La prioridad de estados actual es:

1. `Disconnected`: sin WiFi, endpoint caido o datos invalidos.
2. `Overheated`: temperatura mayor a `TEMP_HOT_THRESHOLD`.
3. `Alert`: `docker_exit > 0`.
4. `Nervous`: CPU mayor a `CPU_HIGH_THRESHOLD`.
5. `Sleepy`: CPU menor a `CPU_SLEEPY_THRESHOLD`.
6. `Normal`: estado estable.

## Hardware base documentado

La documentacion del fabricante incluida en el repositorio apunta a esta base:

- MCU `ESP32-S3`
- `16 MB Flash`
- `16 MB PSRAM`
- display circular `GC9A01` `240x240`
- bus paralelo `I80/8080` de 8 bits
- touch de la familia `CST816`
- backlight en `GPIO42`
- I2C en `GPIO8` y `GPIO9`

Los detalles estan ampliados en [docs/hardware-notes.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/hardware-notes.md).

## Estructura del repositorio

```text
DaemonPet/
|-- assets/
|-- docs/
|-- include/
|-- lib/
|-- src/
|-- test/
|-- tools/
|-- platformio.ini
|-- README.md
`-- ROADMAP.md
```

## Documentacion

Punto de entrada de documentos:

- [docs/README.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/README.md)

Anexos principales:

- [docs/project-status.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/project-status.md)
- [docs/improvements-and-next-steps.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/improvements-and-next-steps.md)
- [docs/architecture.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/architecture.md)
- [docs/hardware-notes.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/hardware-notes.md)
- [docs/visual-spec.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/visual-spec.md)
- [docs/design-system.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/design-system.md)
- [docs/ui-screens.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/ui-screens.md)

## Limitaciones conocidas

- El touch se detecta, pero su validacion real en placa sigue pendiente.
- La UI esta optimizada para una primera iteracion funcional, no para render incremental avanzado.
- El endpoint local tiene un contrato implicito; aun no existe versionado formal del payload.
- Las credenciales viven hoy en `include/config.h`; no hay gestion mas segura de configuracion.
- No hay pruebas unitarias ni de integracion.

## Siguientes pasos recomendados

- Flashear hardware real y validar orientacion, brillo, touch y estabilidad WiFi.
- Formalizar el contrato del `status.json` y agregar payloads de ejemplo.
- Agregar pruebas para `pet_state` y para parsing de respuestas HTTP/JSON.
- Mejorar la UI circular para acercarla al sistema visual ya documentado en `docs/`.
- Evaluar OTA, MQTT y una integracion futura con Home Assistant.

## Roadmap

El backlog por fases vive en [ROADMAP.md](/C:/Users/juan.cornejo/Desktop/DaemonPet/ROADMAP.md).
