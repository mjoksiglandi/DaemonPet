# Improvements and Next Steps

## Objetivo de este documento

Este anexo prioriza mejoras concretas para llevar DaemonPet desde un prototipo compilable a una primera version util y mantenible.

## Prioridad alta

### 1. Bring-up en hardware real

Sin esta validacion, el proyecto no puede considerarse listo ni siquiera para uso personal estable.

Acciones:

- Flashear la placa objetivo.
- Verificar arranque estable y deteccion de PSRAM.
- Confirmar orientacion del display.
- Validar touch real, coordenadas y sensibilidad.
- Revisar brillo y legibilidad en escritorio.
- Observar estabilidad de WiFi durante al menos una sesion prolongada.

Entregable esperado:

- una lista cerrada de correcciones de pines, orientacion o timings
- evidencia en fotos, video o notas de prueba

### 2. Formalizar el contrato del endpoint

Hoy el firmware depende de un `status.json` implicito. Eso funciona al inicio, pero escala mal.

Acciones:

- Documentar el payload oficial.
- Marcar campos obligatorios y opcionales.
- Definir unidades esperadas para cada metrica.
- Agregar al menos un ejemplo valido y un ejemplo degradado.
- Preparar una estrategia simple de versionado si el payload cambia.

Entregable esperado:

- un documento de contrato de payload
- un ejemplo de generador de `status.json` para Linux o Raspberry Pi

### 3. Agregar pruebas automatizadas

Las primeras pruebas deben ir sobre la logica que ya existe y no depende del display.

Acciones:

- tests de `evaluatePetState`
- tests de parsing y fallback para `status_client`
- tests de parsing y validacion para `weather_client`
- fixtures JSON con respuestas buenas y defectuosas

Entregable esperado:

- carpeta `test/` con al menos una suite ejecutable en CI o localmente

## Prioridad media

### 4. Consolidar la UI con la especificacion visual

El proyecto ya tiene una linea visual bastante mejor definida en `docs/` que en el firmware actual. Conviene cerrar esa brecha.

Acciones:

- alinear colores, componentes y jerarquia con `visual-spec.md` y `design-system.md`
- simplificar elementos decorativos que no comuniquen estado
- mejorar la vista `Pet` para que la mascota se lea menos como icono plano y mas como companion
- ajustar layouts para legibilidad en `240x240`

### 5. Mejorar manejo de errores

Acciones:

- distinguir mejor `WiFi offline`, `HTTP error`, `JSON invalido` y `sin datos`
- exponer ultimo error de forma mas util en pantalla o en serial
- definir estados de fallback claros para cada vista

### 6. Preparar configuracion mas limpia

Acciones:

- revisar si `include/config.h` debe seguir siendo la unica fuente de configuracion
- considerar separacion entre secretos, thresholds y configuracion visual
- documentar valores por defecto y razones de cada threshold

## Prioridad baja

### 7. Mejoras de experiencia

- animaciones ligeras de ojos o anillo
- pausa temporal de rotacion tras interaccion
- regreso automatico a `Home` tras inactividad manual
- brillo ajustable

### 8. Integraciones futuras

- OTA
- MQTT o WebSocket
- Home Assistant
- backend companion mas formal

## Siguiente plan recomendado

### Semana 1

- validar hardware real
- capturar hallazgos de touch, orientacion y brillo
- corregir configuracion de hardware si hace falta

### Semana 2

- documentar contrato del payload
- crear ejemplo de backend local
- agregar primeras pruebas unitarias

### Semana 3

- aplicar ajustes visuales de mayor impacto
- mejorar estados de error y UX de navegacion
- dejar lista una base mas estable para iterar

## Criterio practico de avance

DaemonPet deberia considerar completada su siguiente etapa cuando pueda cumplir todo esto sin intervencion manual rara:

- arranca en placa real
- se conecta a WiFi
- obtiene estado y clima
- cambia de vista sin glitches visibles
- refleja correctamente un caso normal, uno offline y uno de alerta
- puede ser configurado por otra persona usando solo la documentacion
