# EMOpet Design System

## Foundations

### Color Tokens

| Token | Hex | Use |
| --- | --- | --- |
| `bg.canvas` | `#0B0D10` | fondo principal |
| `bg.surface` | `#141922` | panel central y masas internas |
| `bg.panel` | `#1C232B` | tarjetas y modulos secundarios |
| `line.soft` | `#31404D` | divisores, tracks, bordes tenues |
| `text.primary` | `#D7DEE5` | copy principal |
| `text.muted` | `#9AA6B2` | labels y valores secundarios |
| `accent.violet` | `#5E4B8B` | identidad estructural |
| `accent.cyan` | `#5FE1E6` | estado activo, ojos, foco |
| `state.ok` | `#61E294` | operacion saludable |
| `state.warn` | `#F5B942` | advertencia |
| `state.hot` | `#FF8A3D` | temperatura alta |
| `state.error` | `#E25555` | falla |
| `state.offline` | `#73808C` | desconectado |

### Typography

Para firmware embebido conviene usar tipografia sans geometrica, abierta y de trazos limpios.

Criterios:

- numeros con buena diferenciacion de `1`, `7`, `0`
- altura x generosa
- peso medio para labels
- peso semibold para metricas
- soporte estable en bitmap font o raster export

Sugerencias para prototipado:

- `Space Grotesk`
- `Manrope`
- `Sora`

Para firmware final:

- exportar subset de glifos
- convertir a bitmap o a fuente optimizada compatible con la libreria elegida

### Type Scale for 480x480

| Role | Size | Line height | Use |
| --- | --- | --- | --- |
| `display.hero` | `72-96` | `0.95` | temperatura o contador principal |
| `display.metric` | `44-56` | `1.0` | CPU, Docker, status central |
| `title.view` | `24-28` | `1.1` | ciudad, host, nombre de vista si aplica |
| `label.primary` | `18-20` | `1.2` | labels clave |
| `label.secondary` | `14-16` | `1.2` | metadata, forecast, pager labels |
| `micro` | `11-12` | `1.1` | chips o ayudas puntuales |

### Spacing Scale

| Token | Px |
| --- | --- |
| `space.1` | `4` |
| `space.2` | `8` |
| `space.3` | `12` |
| `space.4` | `16` |
| `space.5` | `20` |
| `space.6` | `24` |
| `space.8` | `32` |

### Circular Safe Zones

| Zone | Radius | Use |
| --- | --- | --- |
| `zone.core` | `0-180` | personaje o metrica principal |
| `zone.content` | `180-208` | datos secundarios |
| `zone.nav` | `208-224` | pager, chips, iconos pequenos |
| `zone.dead` | `224-240` | evitar contenido fino |

## Components

### Status Ring

Anillo principal concentrico usado en home y como patron de estado transversal.

Reglas:

- grosor base `10-16 px`
- track oscuro en `line.soft`
- segmento activo con gradiente simple entre `accent.violet` y color de estado
- gap opcional de `18-28 deg` para indicar dinamica o desconexion

Estados:

- OK: continuo y estable
- Nervous: doble pulso o dash corto
- Alert: segmento dominante amarillo/rojo
- Offline: track parcial o roto

### Progress Bar Arc

- preferir arco curvo sobre barra horizontal
- inicio sugerido entre `210 deg` y `330 deg`
- usar valor numerico grande al centro

### Radial Card

Tarjeta flotante circular o capsula redondeada para datos secundarios.

Reglas:

- opacidad visual baja
- no mas de 2 simultaneas grandes por vista
- usar para forecast, nodos, badges o focos de categoria

### Status Chip

- alto `28-34 px`
- radio completo
- icono pequeno opcional
- fondo en color de estado al `15-20%` de intensidad
- texto en `text.primary`

### Dividers

- lineas `1-2 px`
- color `line.soft`
- largo corto, nunca atraviesan todo el diametro salvo vistas tecnicas

### Pager Dots

- diametro `6 px` inactivo
- `10 px` activo
- ubicacion inferior central
- siempre fuera del area central del personaje

## Layout Patterns

### Home Pattern

- personaje dominante en centro
- anillo completo o semicerrado
- un indicador superior y uno inferior maximo
- copy secundaria fuera del volumen del personaje

### Metric Stack Pattern

- numero principal
- label corto debajo
- una segunda o tercera metrica separada por divisor ligero

### Split Arc Pattern

Para vistas con dos estados contrapuestos, por ejemplo `docker activos` vs `caidos`:

- contador principal en centro
- segundo contador debajo
- anillo o badge de severidad

### Forecast Strip Pattern

- tres nodos pequenos alineados en arco bajo
- icono primero, temp alta/baja despues

## Hierarchy Rules

1. El personaje o metrica principal domina el `60-70%` del peso visual.
2. Un solo color de acento fuerte por vista.
3. Labels tecnicos siempre por debajo del valor, nunca compiten con el numero.
4. Elementos decorativos deben justificar categoria o estado; si no informan, sobran.

## Accessibility and Readability

- contraste minimo visual objetivo: `4.5:1` para texto pequeño
- no confiar solo en color para alertas; combinar con forma, gap o icono
- valores numericos importantes con al menos `44 px`
- no colocar texto fino en el borde exterior del circulo

## Embedded Constraints

- limitar a `3-4` capas visuales por vista
- gradientes suaves solo cuando puedan simularse con pocos pasos o imagen exportada
- iconos preferentemente monoline o duotono simple
- componentes deben poder dibujarse con primitivas basicas:
  - circulos
  - arcos
  - rectangulos redondeados
  - lineas
  - sprites pequenos
