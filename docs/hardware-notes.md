# Hardware Notes

## Base confirmada

- ESP32-S3 con `16 MB Flash` y `16 MB PSRAM`
- Display `GC9A01` circular `240x240`
- Bus paralelo `I80/8080` de 8 bits
- Touch de la familia `CST816`
- I2C principal en `GPIO8/9`
- Backlight en `GPIO42`
- Reset LCD en `GPIO21`
- Touch reset en `GPIO0`
- `TCA6408` en `0x20`, INT en `GPIO45`

## Pines del display

- `DC GPIO18`
- `CS GPIO2`
- `WR GPIO3`
- `D0..D7 = GPIO10..GPIO17`

## Integrados detectados

- IMU `QMI8658`
- WS2812 en `GPIO46`
- microSD con `CS GPIO40`, `SCK GPIO41`, `MOSI GPIO47`, `MISO GPIO48`

## Pendiente

- confirmar el modelo exacto del touch
- validar orientacion y calibracion en placa
