# EMOpet Visual Spec

## Scope

Este documento define la identidad visual de EMOpet para una pantalla circular de `480x480`.
La propuesta se basa en:

- el brief adjunto del proyecto
- los renders actuales en [docs/renders](/C:/Users/juan.cornejo/Desktop/DaemonPet/docs/renders)
- las restricciones tecnicas de un `ESP32-S3`

Las referencias mencionadas como `references/emopet_robot.png`, `references/ui_dashboard_style.png` y `references/weather_reference.png` no estan presentes en el workspace actual. Por eso, la reinterpretacion se apoya en la descripcion textual del brief y en los renders ya generados.

## Proposed Direction

EMOpet debe sentirse como un `companion node` de homelab: una mascota robotica pequena, sobria y expresiva que funciona como lectura emocional del sistema antes de convertirse en dashboard.

La direccion visual se resume en cinco principios:

1. `Cute-tech, no toy`: formas suaves y amables, pero con construccion tecnica y control visual.
2. `Dark industrial calm`: fondo oscuro, materiales grafito, acentos frios y brillo moderado.
3. `Circular first`: toda composicion nace desde un centro vivo y un anillo de estado perimetral.
4. `Status by expression`: el personaje comunica antes por ojos, brillo, postura y ritmo que por texto.
5. `Embedded realism`: nada depende de texturas pesadas, sombras complejas o efectos imposibles de portar.

## Personality

EMOpet no es humano. Es un companion robot compacto con:

- cabeza frontal oscura, amplia y legible
- ojos cyan/turquesa como principal punto emocional
- cuerpo pequeno respecto de la cabeza para reforzar ternura sin infantilizar
- postura estable, de escritorio, como dispositivo tecnico que "vive" junto al rack

Personalidad percibida:

- observador
- amigable
- sereno en estado normal
- atento cuando hay actividad
- vulnerable cuando la conexion cae

## Visual Language

### Silhouette

- Cabeza principal: volumen rectangular-suave o capsula ancha.
- Visor: masa oscura unica donde viven ojos y microexpresion.
- Cuerpo: modulo pequeno, centrado, sin extremidades complejas.
- Base: apoyo corto y ancho para transmitir estabilidad.

### Surface Treatment

- Superficies oscuras en `Obsidian Void` y `Graphite Alloy`.
- Borde interno o halo fino para separar planos en pantallas oscuras.
- Brillos puntuales solo en ojos, acentos de estado y anillo principal.
- Sin chrome, sin glassmorphism, sin neones exagerados.

### Motion Intent

La mascota debe sugerir animacion aunque el render sea estatico:

- ojos con posibilidad de parpadeo simple
- pequenas inclinaciones de cabeza entre estados
- anillo con pulso, gap o barrido suave segun prioridad
- micro rebote vertical al cambiar a `happy`

## Color Rules

### Base Palette

- `Obsidian Void` `#0B0D10`: fondo absoluto y vacio exterior
- `Graphite Alloy` `#1C232B`: superficies, tarjetas, visor secundario
- `Cold Silver` `#D7DEE5`: texto principal, divisores, iconos pasivos
- `Cold Industrial Violet` `#5E4B8B`: acento estructural y tono identitario

### Accent Palette

- `Signal Cyan` `#5FE1E6`: ojos, highlights activos, datos en foco
- `Soft Mint` `#61E294`: OK, valores estables, activo saludable
- `Alert Amber` `#F5B942`: advertencia
- `Heat Orange` `#FF8A3D`: temperatura alta
- `Fault Red` `#E25555`: caida critica o contenedor detenido
- `Offline Gray` `#73808C`: desconexion, datos no disponibles

### Usage Ratios

- 65% fondos oscuros
- 20% superficies intermedias
- 10% texto y lineas claras
- 5% acentos de estado

La UI no debe tener mas de un color de alerta dominante al mismo tiempo.

## Circular Composition Rules

La pantalla circular debe leerse por zonas concentricas:

1. `Core` `0-180 px` desde el centro:
   - personaje o metrica principal
2. `Utility ring` `180-208 px`:
   - anillo de estado, progreso o navegacion
3. `Peripheral band` `208-224 px`:
   - indicadores secundarios, pager y alertas discretas
4. `Safety edge` `224-240 px`:
   - no ubicar tipografia fina ni datos criticos

Reglas clave:

- No poner texto sobre la cara o visor de la mascota.
- Mantener el centro libre de ruido cuando la vista principal sea la mascota.
- Evitar que bloques rectangulares toquen el borde del circulo completo.
- Usar alineaciones radiales o ejes verticales simples; no grillas densas.

## Current Render Assessment

Los renders actuales muestran una buena base de:

- composicion circular
- fondo oscuro consistente
- foco claro en una metrica o personaje
- legibilidad aceptable a baja densidad

Pero conviene corregir:

- la mascota actual es demasiado simple y se lee mas como carita plana que como robot companion
- hay textos demasiado cerca del personaje en `view-pet.png`
- las tarjetas circulares moradas/azules parecen decorativas, no sistemicas
- falta un lenguaje unico de componentes entre sistema, red, docker y clima

## Mascot States

### OK / Normal

- ojos ovalados cyan suaves
- postura simetrica
- anillo violeta con segmento cyan estable
- brillo bajo a medio

### Happy

- ojos mas altos y luminosos
- base corporal levemente expandida
- anillo mas abierto y limpio
- posible microdestello cyan

### Sleepy

- ojos semicerrados o lineales cortos
- cabeza apenas inclinada
- brillo reducido
- anillo tenue con ritmo lento

### Nervous / High Load

- ojos mas pequenos o tensos
- leve inclinacion o compresion del cuerpo
- anillo con pulso discontinuo
- cyan mas duro con soporte violeta

### Hot / High Temperature

- ojos tensos con detalle calido secundario
- anillo con seccion naranja
- micro indicador termico fuera del personaje

### Alert / Container Down

- ojos asimetricos o mas fijos
- acento amarillo/rojo controlado
- anillo con gap visible o segmento de alerta
- indicador periferico de falla

### Offline

- ojos grisaceos o apagados
- anillo roto o incompleto
- cuerpo mas bajo visualmente
- sin highlights activos

## Asset Strategy

Para firmware embebido, la mascota debe existir en dos niveles:

1. `SVG master` para documentacion, refinamiento y export.
2. `Sprite-friendly interpretation` para firmware:
   - capas simples
   - rellenos planos
   - pocos nodos
   - posibilidad de exportar a `RGB565`

Se recomienda separar logica visual en:

- base del cuerpo
- ojos por estado
- anillo por estado
- indicadores perifericos opcionales

## Implementation Notes

- Diseñar contra `480x480` aunque el firmware actual mencione `240x240`; la especificacion debe escalar con factor `0.5` si el hardware final sigue en `240`.
- Convertir SVG a primitivas o sprites precalculados en build time, no parsear SVG en runtime.
- Mantener cada asset raster final dentro de presupuestos razonables de PSRAM.
- Priorizar animaciones por interpolacion simple de posicion, alpha o frame swap.
