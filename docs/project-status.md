# Project Status

## Resumen ejecutivo

DaemonPet ya tiene una base funcional de firmware que compila y cubre el flujo principal del producto: conectarse por WiFi, obtener datos de un host, traducirlos a un estado emocional y mostrarlos en una UI circular multi-vista.

La situacion actual es positiva para una fase de prototipo, pero todavia no alcanza un estado de hardware validado ni una version lista para uso continuo.

## Estado confirmado al 3 de julio de 2026

### Firmware

- `main.cpp` coordina actualizacion WiFi, polling de estado, polling de clima y render UI.
- `wifi_manager` implementa reconexion periodica en modo `WIFI_STA`.
- `status_client` consume el endpoint local y parsea el payload base.
- `weather_client` consume Open-Meteo con `current` y `daily`.
- `pet_state` aplica reglas de prioridad simples y previsibles.
- `ui` ya renderiza cinco vistas independientes y soporta cambio automatico o por tap.

### Build

Compilacion validada localmente con:

```powershell
& "$env:USERPROFILE\.platformio\penv\Scripts\platformio.exe" run
```

Resultado observado:

- RAM: `21.6%`
- Flash: `18.8%`

### Estructura del proyecto

El repositorio esta en medio de una reorganizacion saludable:

- `src/` quedo reducido al entrypoint.
- `include/` concentra headers del dominio.
- `lib/daemonpet/` concentra implementaciones reutilizables.
- `docs/` y `assets/` crecieron como base de documentacion y direccion visual.

`git status` muestra esta transicion como cambios aun no consolidados en el arbol de trabajo. Eso no es necesariamente un problema, pero si indica que conviene estabilizar la estructura antes de ampliar alcance funcional.

## Capacidades disponibles hoy

### Monitoreo local

- Lectura de estado desde un endpoint HTTP configurable.
- Presentacion de CPU, temperatura, RAM, disco, red y Docker.
- Manejo basico de fallas de WiFi, HTTP y JSON.

### Companion UI

- Mascota con estados `Disconnected`, `Overheated`, `Alert`, `Nervous`, `Sleepy` y `Normal`.
- Rotacion automatica de vistas.
- Interaccion tactil minima por tap.

### Clima

- Temperatura actual.
- Codigo meteorologico actual.
- Forecast diario de 3 dias adicionales.

## Lo que aun no esta validado

### Hardware real

- Flasheo y uso continuo en la placa final.
- Orientacion real del display.
- Modelo exacto del chip touch.
- Coordenadas tactiles y calibracion.
- Brillo y consumo en uso real.

### Calidad de software

- No hay pruebas automatizadas.
- No hay ejemplos ejecutables del backend que produce `status.json`.
- No hay contrato versionado del payload.
- No hay telemetria ni logs estructurados para diagnostico en campo.

## Riesgos tecnicos actuales

### Riesgo 1: contrato de datos fragil

El firmware consume campos JSON por nombre, pero no existe todavia una especificacion versionada del payload. Cualquier cambio del lado del servidor puede romper comportamiento o degradar la UI sin una ruta clara de compatibilidad.

### Riesgo 2: validacion fisica incompleta

Gran parte del producto depende de detalles de placa: touch, orientacion, brillo y estabilidad del display. Mientras eso no se valide en hardware real, la compilacion exitosa no garantiza una experiencia usable.

### Riesgo 3: ausencia de tests

La logica de estados, parsing y reglas de fallback es suficientemente pequena como para probarla rapido. No hacerlo ahora vuelve mas caro cualquier refactor posterior.

## Lectura honesta del proyecto

DaemonPet ya supero la etapa de idea: existe firmware compilable, modular y con una direccion visual bastante definida. Lo que falta no es vision, sino cerrar el ciclo de producto minimo:

1. validar hardware real
2. fijar contrato de datos
3. agregar pruebas
4. consolidar la UX sobre la base ya construida

## Recomendacion de estado

La etiqueta mas precisa hoy es:

`prototipo funcional compilable, pendiente de bring-up fisico y endurecimiento`
