# DaemonPet Roadmap

## Vision

DaemonPet quiere ser un display remoto pequeno, expresivo y util para homelab: una mezcla entre panel tecnico y mascota virtual que te avisa rapidamente cuando algo esta mal.

## Principles

- Primero funcional, despues vistoso.
- Hardware real antes que suposiciones.
- Arquitectura modular para crecer sin reescribir.
- Legibilidad por encima de densidad visual.

## Phase 0: Baseline Hardware

Objetivo: confirmar y fijar la base de hardware.

- [x] Revisar archivos del fabricante incluidos en el proyecto.
- [x] Identificar display `GC9A01` y resolucion `240x240`.
- [x] Confirmar base `ESP32-S3` con `16 MB Flash` y `16 MB PSRAM`.
- [x] Identificar el bus `I80/8080` y pines principales.
- [x] Identificar familia touch `CST816`.
- [ ] Confirmar sobre placa el modelo exacto del touch.
- [ ] Confirmar orientacion tactil y necesidad de calibracion.

## Phase 1: Minimum Viable Firmware

Objetivo: tener una primera version util en el display.

- [x] Crear proyecto PlatformIO con framework Arduino.
- [x] Inicializar display y backlight.
- [x] Mostrar splash screen.
- [x] Implementar conexion WiFi modular.
- [x] Implementar cliente HTTP configurable.
- [x] Implementar parseo JSON modular.
- [x] Implementar reglas de estado de mascota.
- [x] Crear vistas base: mascota, sistema, red, docker.
- [x] Soportar rotacion automatica de vistas.
- [x] Preparar soporte touch sin acoplar la UI.
- [x] Documentar configuracion y arquitectura inicial.

## Phase 2: Bring-Up on Real Hardware

Objetivo: validar que lo compilado se comporta bien en la placa fisica.

- [ ] Flashear la placa y verificar boot estable.
- [ ] Confirmar deteccion de PSRAM en runtime.
- [ ] Verificar orientacion correcta del display.
- [ ] Medir brillo util y consumo.
- [ ] Validar lectura tactil real.
- [ ] Corregir mapeo de coordenadas si hace falta.
- [ ] Verificar estabilidad de polling HTTP en WiFi real.

## Phase 3: UX and Polish

Objetivo: mejorar la experiencia sin aumentar innecesariamente la complejidad.

- [ ] Mejorar composicion visual para pantalla circular.
- [ ] Animar expresiones simples de la mascota.
- [ ] Mostrar metricas criticas con mejor jerarquia visual.
- [ ] Agregar pantalla de error mas clara para WiFi/HTTP/JSON.
- [ ] Agregar vista secundaria con uptime e IP mas limpia.
- [ ] Evaluar cambio manual de brillo.

## Phase 4: Interaction and Inputs

Objetivo: aprovechar mejor la interfaz de la placa.

- [ ] Touch con cambio de vista confiable.
- [ ] Gestos simples o toques por zona.
- [ ] Soporte para botones fisicos si aplica.
- [ ] Feedback visual al tocar.

## Phase 5: Server Integration

Objetivo: hacer mas robusta la fuente de datos.

- [ ] Documentar el generador recomendado de `status.json`.
- [ ] Crear ejemplo de script para Raspberry Pi o Linux server.
- [ ] Validar compatibilidad con Docker, temperatura y disco en distintos hosts.
- [ ] Definir contrato estable del JSON.
- [ ] Manejar campos opcionales y versiones futuras del payload.

## Phase 6: Advanced Features

Objetivo: evolucionar sin perder simplicidad.

- [ ] MQTT o WebSocket en lugar de polling.
- [ ] OTA updates.
- [ ] Integracion con Home Assistant.
- [ ] LVGL si se necesita una UI mas avanzada.
- [ ] Mas estados emocionales y animaciones.
- [ ] Sonido o feedback adicional si aporta valor real.

## Definition of Done for v0.1

La version `v0.1` se considera lista cuando:

- Compila limpio en PlatformIO.
- Arranca en la placa real.
- Se conecta a WiFi.
- Consulta el endpoint sin bloquearse.
- Interpreta bien el JSON esperado.
- Renderiza la mascota y las vistas base.
- Muestra estados correctos segun temperatura, CPU y Docker.
- Tiene documentacion suficiente para que otra persona pueda montarlo.
