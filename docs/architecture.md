# Firmware Architecture

## Scaffolding

El proyecto ahora usa una estructura clasica de PlatformIO:

- `src/`: entrypoint del firmware
- `include/`: headers del proyecto
- `lib/`: implementacion modular
- `docs/`: notas de soporte
- `test/`: espacio para pruebas futuras

## Modulos

- `main.cpp`: loop principal y orquestacion
- `wifi_manager`: gestion de WiFi y reconexion
- `status_client`: cliente HTTP y parseo base
- `system_status`: modelo de datos del endpoint
- `pet_state`: reglas de estado emocional
- `ui`: splash, vistas y touch simple
- `config`: parametros configurables
- `display_config`: pines y constantes de hardware
