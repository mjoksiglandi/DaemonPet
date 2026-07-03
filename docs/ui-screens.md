# EMOpet UI Screens

## Screen Model

La UI se organiza en cinco vistas rotativas dentro de un reloj circular:

1. `Home / Pet`
2. `System`
3. `Network`
4. `Docker`
5. `Weather`

Cada vista debe ser legible en menos de `2 segundos` a una distancia de escritorio o rack.

## 1. Home / Pet

### Goal

Entregar el estado global del homelab a traves de EMOpet, no a traves de texto.

### Layout

- fondo circular oscuro
- EMOpet ocupa `68-74%` del alto util
- anillo de estado alrededor del personaje
- un micro indicador superior opcional
- un label inferior discreto opcional con `host` o resumen corto
- pager dots abajo

### Visible Elements

- cabeza/visor del robot
- cuerpo/base
- ojos y expresion segun estado
- anillo radial
- indicador termico o alerta solo si aplica

### Information Priority

1. estado emocional del sistema
2. presencia de alerta o desconexion
3. nombre corto del host o un unico resumen tecnico

### Notes

- no usar globos de dialogo
- no superponer frases sobre la mascota
- si se necesita texto, ubicarlo fuera del personaje y no mas de dos lineas cortas

## 2. System

### Goal

Mostrar salud del host con una mezcla de lectura numerica y color de estado.

### Layout

- CPU como metrica principal grande en la mitad superior
- temperatura como segunda metrica destacada
- RAM y disco en banda inferior
- dos divisores horizontales cortos
- dos nodos decorativo-funcionales tenues maximo

### Visible Elements

- porcentaje CPU
- temperatura actual
- RAM usada/total
- disco %
- iconos pequeños opcionales

### Information Priority

1. CPU
2. temperatura
3. RAM
4. disco

### Behavior

- CPU y temperatura cambian de color por thresholds
- el anillo de borde puede reflejar el peor estado entre CPU y temperatura

## 3. Network

### Goal

Comunicar conectividad y trafico sin parecer tabla de servidor comprimida.

### Layout

- stack vertical de cinco filas dentro del circulo
- label izquierda, valor derecha
- divisores suaves
- badge o icono WiFi en cabecera superior
- pager dots abajo

### Visible Elements

- host
- IP
- RX
- TX
- uptime

### Information Priority

1. host e IP
2. RX/TX
3. uptime

### Behavior

- si no hay red, reemplazar RX/TX por `--`
- el color del badge WiFi debe pasar a `state.offline`

## 4. Docker

### Goal

Responder rapido cuan sano esta el runtime de contenedores.

### Layout

- contador de activos al centro y arriba
- contador de caidos debajo, mas pequeno pero muy claro
- anillo de severidad o borde de alerta
- chip inferior opcional con `healthy`, `degraded` o `down`

### Visible Elements

- activos
- caidos
- badge Docker
- pager dots

### Information Priority

1. contenedores caidos
2. activos
3. estado general

### Behavior

- si `caidos > 0`, el color dominante cambia a `state.warn` o `state.error`
- si todos estan OK, usar `state.ok` y un borde mas estable

## 5. Weather

### Goal

Integrar clima en el sistema visual sin parecer una app aparte.

### Layout

- temperatura actual grande en el cuadrante izquierdo inferior
- ciudad arriba
- condicion al centro inferior
- icono clima principal en cuadrante derecho superior
- mini forecast de 3 dias en arco bajo

### Visible Elements

- ciudad
- temperatura actual
- condicion
- max/min del dia
- tres items de forecast

### Information Priority

1. temperatura actual
2. condicion actual
3. forecast breve

### Behavior

- iconografia suave, con volumen simple
- usar amarillo/naranja solo dentro del contexto meteorologico y con fondo oscuro consistente

## Rotation Behavior

### Auto Rotation

- cambiar vista cada `5-8 s`
- pausar `15-20 s` despues de interaccion tactil
- volver a `Home` tras `30-60 s` de inactividad manual

### Rotation Order

`Home -> System -> Network -> Docker -> Weather -> Home`

### State Overrides

- `Offline`: forzar retorno periodico a `Home` o `Network`
- `Alert`: extender permanencia de `Docker`
- `Hot`: extender permanencia de `System`

## Touch Behavior

### Primary Gestures

- `tap`: siguiente vista
- `swipe left/right`: navegar entre vistas
- `long press home`: abrir overlay tecnico futuro o fijar vista

### Touch Principles

- objetivos tactiles minimos de `44x44 px`
- no ubicar acciones esenciales en el borde muerto
- priorizar navegacion simple, no menus densos

## Implementation Recommendations for PlatformIO + Arduino

### Rendering Strategy

- separar fondo, anillo, contenido y pager como capas logicas
- renderizar solo regiones que cambian cuando sea posible
- usar `sprites` para mascota, iconos clima y componentes complejos
- dibujar arcos, divisores y chips con primitivas del driver grafico

### Asset Pipeline

- conservar SVG en repo como fuente maestra
- exportar PNG o arrays `RGB565` en build step si el firmware final lo necesita
- preparar variantes de ojos y anillo como piezas independientes para reducir memoria

### Performance

- evitar alpha complejo en tiempo real
- limitar animaciones a `2-4 fps` si usan sprites grandes
- preferir tween de posiciones y cambios discretos de color

### UI State Model

Separar al menos estos modelos:

- `system status`
- `pet emotion state`
- `view routing state`
- `animation state`

Eso permite que la mascota no dependa de strings de UI para cambiar expresion.
