from __future__ import annotations

from dataclasses import dataclass
from math import cos, pi, sin
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs" / "renders"
SIZE = 240
CENTER = SIZE // 2


BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (244, 67, 54)
BLUE = (76, 157, 255)
CYAN = (99, 214, 255)
GREEN = (90, 212, 120)
YELLOW = (255, 210, 72)
ORANGE = (255, 163, 63)
LIGHTGREY = (196, 203, 214)
DARKGREY = (67, 76, 92)
SKY = (102, 196, 255)
PANEL = (18, 36, 56)
PANEL_ALT = (26, 44, 66)
BG_1 = (10, 16, 28)
BG_2 = (14, 22, 38)
BG_3 = (18, 30, 48)
CHEEK = (255, 188, 196)
PURPLE = (92, 54, 210)
PURPLE_DARK = (68, 42, 168)
LAVENDER = (194, 154, 255)
SOFT_WHITE = (246, 243, 248)


try:
    FONT_SM = ImageFont.truetype("arial.ttf", 12)
    FONT_MD = ImageFont.truetype("arial.ttf", 18)
    FONT_LG = ImageFont.truetype("arial.ttf", 24)
    FONT_XL = ImageFont.truetype("arial.ttf", 40)
except OSError:
    FONT_SM = ImageFont.load_default()
    FONT_MD = ImageFont.load_default()
    FONT_LG = ImageFont.load_default()
    FONT_XL = ImageFont.load_default()


@dataclass
class PetState:
    label: str
    detail: str
    color: tuple[int, int, int]
    mood: str


def img() -> tuple[Image.Image, ImageDraw.ImageDraw]:
    im = Image.new("RGB", (SIZE, SIZE), BLACK)
    d = ImageDraw.Draw(im)
    return im, d


def center_text(d: ImageDraw.ImageDraw, y: int, text: str, font, fill):
    bbox = d.textbbox((0, 0), text, font=font)
    x = (SIZE - (bbox[2] - bbox[0])) // 2
    d.text((x, y), text, font=font, fill=fill)


def center_text_in_box(d: ImageDraw.ImageDraw, box, y: int, text: str, font, fill):
    x0, _, x1, _ = box
    bbox = d.textbbox((0, 0), text, font=font)
    x = x0 + ((x1 - x0) - (bbox[2] - bbox[0])) // 2
    d.text((x, y), text, font=font, fill=fill)


def rounded_panel(d: ImageDraw.ImageDraw, box, outline):
    d.rounded_rectangle(box, radius=14, fill=PANEL, outline=outline, width=2)


def frame(d: ImageDraw.ImageDraw, title: str, accent):
    ambient_background(d, accent, True, title)


def ambient_background(d: ImageDraw.ImageDraw, accent, show_header: bool, title: str = ""):
    d.ellipse((12, 12, 228, 228), fill=BG_1)
    d.ellipse((20, 20, 220, 220), fill=BG_2)
    d.ellipse((42, 32, 94, 84), fill=(48, 38, 96))
    d.ellipse((150, 150, 206, 206), fill=(32, 92, 132))
    d.ellipse((150, 34, 186, 70), fill=(68, 52, 128))
    d.ellipse((2, 2, 238, 238), outline=DARKGREY, width=2)
    d.ellipse((4, 4, 236, 236), outline=accent, width=2)
    if show_header:
        d.rounded_rectangle((68, 8, 172, 30), radius=10, fill=(28, 24, 66), outline=accent, width=2)
        center_text(d, 11, title, FONT_SM, accent)


def view_dots(d: ImageDraw.ImageDraw, active: int, accent):
    for i in range(5):
        x = 94 + i * 13
        r = 4 if i == active else 2
        fill = accent if i == active else DARKGREY
        d.ellipse((x - r, 224 - r, x + r, 224 + r), fill=fill)


def weather_label(code: int) -> str:
    if code == 0:
        return "Despejado"
    if code in (1, 2):
        return "Parcial"
    if code == 3:
        return "Nublado"
    if code in (45, 48):
        return "Niebla"
    if 51 <= code <= 67 or 80 <= code <= 82:
        return "Lluvia"
    if 71 <= code <= 77 or code in (85, 86):
        return "Nieve"
    if code >= 95:
        return "Tormenta"
    return "Variable"


def sun(d, cx, cy, color, large=False):
    r = 18 if large else 10
    ray = 30 if large else 18
    for i in range(8):
        angle = i * pi / 4
        x1 = cx + int((r + 3) * cos(angle))
        y1 = cy + int((r + 3) * sin(angle))
        x2 = cx + int(ray * cos(angle))
        y2 = cy + int(ray * sin(angle))
        d.line((x1, y1, x2, y2), fill=color, width=2)
    d.ellipse((cx - r, cy - r, cx + r, cy + r), fill=color)


def moon(d, cx, cy, large=False):
    radius = 16 if large else 10
    d.ellipse((cx - radius, cy - radius, cx + radius, cy + radius), fill=LAVENDER)
    cut = radius - 3
    d.ellipse((cx + (8 if large else 5) - cut, cy - (3 if large else 2) - cut,
               cx + (8 if large else 5) + cut, cy - (3 if large else 2) + cut), fill=BG_2)


def cloud(d, cx, cy, color, large=False):
    circles = [(-14, 0, 12), (0, -6, 15), (14, 0, 11)] if large else [(-9, 0, 8), (0, -4, 10), (9, 0, 7)]
    for ox, oy, r in circles:
        d.ellipse((cx + ox - r, cy + oy - r, cx + ox + r, cy + oy + r), fill=color)
    w, h = (44, 16) if large else (28, 10)
    d.rounded_rectangle((cx - w // 2, cy, cx + w // 2, cy + h), radius=h // 2, fill=color)


def rain(d, cx, cy):
    for i in (-10, 0, 10):
        d.line((cx + i, cy + 12, cx + i - 4, cy + 20), fill=CYAN, width=2)


def wind(d, cx, cy, compact=False):
    width = 18 if compact else 30
    d.line((cx - width // 2, cy, cx + width // 2, cy), fill=SOFT_WHITE, width=2)
    d.line((cx - width // 2 + 4, cy + 7, cx + width // 2 - 4, cy + 7), fill=SOFT_WHITE, width=2)
    d.line((cx - width // 2 + 10, cy + 14, cx + width // 2 - 8, cy + 14), fill=SOFT_WHITE, width=2)
    r = 3 if compact else 5
    d.ellipse((cx - width // 2 + 4 - r, cy + 6 - r, cx - width // 2 + 4 + r, cy + 6 + r), outline=SOFT_WHITE, width=2)


def snow(d, cx, cy):
    for i in (-10, 0, 10):
        x = cx + i
        y = cy + 14
        d.line((x - 3, y, x + 3, y), fill=WHITE, width=1)
        d.line((x, y - 3, x, y + 3), fill=WHITE, width=1)
        d.line((x - 2, y - 2, x + 2, y + 2), fill=WHITE, width=1)
        d.line((x - 2, y + 2, x + 2, y - 2), fill=WHITE, width=1)


def storm(d, cx, cy):
    d.polygon([(cx + 2, cy + 8), (cx - 4, cy + 20), (cx + 1, cy + 20), (cx - 2, cy + 30), (cx + 8, cy + 16), (cx + 2, cy + 16)], fill=YELLOW)


def tornado(d, cx, cy):
    for r, oy in [(16, 0), (12, 10), (8, 18), (5, 24)]:
        d.ellipse((cx - r, cy + oy - r // 2, cx + r, cy + oy + r // 2), outline=SOFT_WHITE, width=2)


def weather_icon(d, cx, cy, code, large=False):
    if code == 0:
        sun(d, cx, cy, YELLOW, large)
    elif code in (1, 2):
        moon(d, cx + (18 if large else 10), cy - (16 if large else 10), False)
        sun(d, cx - (12 if large else 8), cy - (8 if large else 5), YELLOW, False)
        cloud(d, cx + (4 if large else 2), cy + (2 if large else 0), WHITE, large)
    elif code in (3, 45, 48):
        cloud(d, cx, cy, LIGHTGREY if code == 3 else DARKGREY, large)
        if code in (45, 48):
            wind(d, cx, cy + (18 if large else 10), not large)
    elif 51 <= code <= 67 or 80 <= code <= 82:
        moon(d, cx + (18 if large else 10), cy - (16 if large else 10), False)
        cloud(d, cx, cy, LIGHTGREY, large)
        rain(d, cx, cy + (2 if large else 0))
    elif 71 <= code <= 77 or code in (85, 86):
        cloud(d, cx, cy, LIGHTGREY, large)
        snow(d, cx, cy)
    elif code >= 95:
        tornado(d, cx, cy) if code > 99 else storm(d, cx, cy)
    else:
        cloud(d, cx, cy, LIGHTGREY, large)


def pet_face(d: ImageDraw.ImageDraw, mood: str, color):
    shell = (31, 32, 66)
    face = SOFT_WHITE
    belly = (70, 210, 120)
    d.polygon([(58, 86), (90, 18), (116, 98)], fill=shell)
    d.polygon([(182, 86), (150, 18), (124, 98)], fill=shell)
    d.ellipse((46, 38, 194, 186), fill=shell, outline=color, width=2)
    d.ellipse((68, 60, 172, 164), fill=face)
    d.ellipse((76, 142, 92, 158), fill=CHEEK)
    d.ellipse((148, 142, 164, 158), fill=CHEEK)
    d.ellipse((76, 154, 164, 210), fill=shell)
    d.rounded_rectangle((90, 176, 150, 200), radius=12, fill=belly)
    d.rounded_rectangle((84, 198, 100, 216), radius=6, fill=shell)
    d.rounded_rectangle((140, 198, 156, 216), radius=6, fill=shell)
    if mood == "Disconnected":
        d.line((94, 96, 110, 112), fill=shell, width=3)
        d.line((110, 96, 94, 112), fill=shell, width=3)
        d.line((130, 96, 146, 112), fill=shell, width=3)
        d.line((146, 96, 130, 112), fill=shell, width=3)
        d.line((104, 140, 136, 140), fill=shell, width=3)
    else:
        d.ellipse((93, 101, 107, 115), fill=shell)
        d.ellipse((133, 101, 147, 115), fill=shell)
        if mood == "Overheated":
            d.ellipse((96, 107, 104, 115), fill=RED)
            d.ellipse((136, 107, 144, 115), fill=RED)
            d.line((108, 142, 132, 142), fill=shell, width=2)
            d.ellipse((115, 145, 125, 155), fill=RED)
        elif mood == "Alert":
            d.line((92, 102, 112, 102), fill=shell, width=2)
            d.line((128, 102, 148, 102), fill=shell, width=2)
            d.ellipse((96, 108, 108, 120), fill=shell)
            d.ellipse((132, 108, 144, 120), fill=shell)
            d.line((106, 142, 134, 142), fill=shell, width=2)
        elif mood == "Nervous":
            d.ellipse((86, 96, 110, 120), fill=WHITE)
            d.ellipse((130, 96, 154, 120), fill=WHITE)
            d.ellipse((98, 105, 104, 111), fill=shell)
            d.ellipse((142, 105, 148, 111), fill=shell)
            d.line((106, 142, 134, 142), fill=shell, width=2)
        elif mood == "Sleepy":
            d.line((92, 110, 114, 110), fill=shell, width=2)
            d.line((126, 110, 148, 110), fill=shell, width=2)
            d.arc((106, 128, 134, 156), 0, 180, fill=shell, width=2)
        else:
            d.arc((104, 122, 136, 154), 0, 180, fill=shell, width=2)


def metric_line(d, y, label, value, color=WHITE):
    d.text((30, y), label, font=FONT_SM, fill=LIGHTGREY)
    d.text((110, y), value, font=FONT_SM, fill=color)


def render_pet():
    im, d = img()
    pet = PetState("Normal", "Todo OK", GREEN, "Normal")
    ambient_background(d, pet.color, False)
    d.ellipse((18, 20, 222, 224), fill=(38, 32, 80))
    pet_face(d, pet.mood, pet.color)
    center_text(d, 198, pet.detail, FONT_SM, LIGHTGREY)
    d.line((40, 212, 200, 212), fill=(90, 78, 132), width=1)
    center_text(d, 220, "pi-homelab  ·  23% CPU", FONT_SM, WHITE)
    view_dots(d, 0, pet.color)
    return im


def render_system():
    im, d = img()
    accent = CYAN
    ambient_background(d, accent, False)
    center_text(d, 56, "23%", FONT_XL, CYAN)
    center_text(d, 88, "CPU", FONT_SM, LIGHTGREY)
    center_text(d, 106, "48.2C", FONT_LG, ORANGE)
    center_text(d, 128, "temperatura", FONT_SM, LIGHTGREY)
    center_text(d, 154, "3.1/8.0 GB", FONT_MD, GREEN)
    center_text(d, 176, "memoria", FONT_SM, LIGHTGREY)
    center_text(d, 198, "62% disco", FONT_MD, YELLOW)
    d.line((62, 96, 178, 96), fill=(46, 62, 88), width=1)
    d.line((62, 142, 178, 142), fill=(46, 62, 88), width=1)
    d.line((62, 188, 178, 188), fill=(46, 62, 88), width=1)
    view_dots(d, 1, accent)
    return im


def render_network():
    im, d = img()
    accent = BLUE
    ambient_background(d, accent, False)
    metric_line(d, 56, "Host", "pi-homelab")
    metric_line(d, 86, "IP", "192.168.1.20")
    metric_line(d, 116, "RX", "12M", CYAN)
    metric_line(d, 146, "TX", "2M", GREEN)
    metric_line(d, 176, "Uptime", "3d 04h")
    d.line((34, 76, 206, 76), fill=(46, 62, 88), width=1)
    d.line((34, 106, 206, 106), fill=(46, 62, 88), width=1)
    d.line((34, 136, 206, 136), fill=(46, 62, 88), width=1)
    d.line((34, 166, 206, 166), fill=(46, 62, 88), width=1)
    view_dots(d, 2, accent)
    return im


def render_docker():
    im, d = img()
    accent = YELLOW
    ambient_background(d, accent, False)
    center_text(d, 72, "18", FONT_XL, GREEN)
    center_text(d, 122, "contenedores activos", FONT_SM, LIGHTGREY)
    center_text(d, 154, "0", FONT_XL, LIGHTGREY)
    center_text(d, 194, "caidos", FONT_SM, WHITE)
    d.line((66, 142, 174, 142), fill=(46, 62, 88), width=1)
    view_dots(d, 3, accent)
    return im


def render_weather():
    im, d = img()
    accent = SKY
    ambient_background(d, accent, False)
    d.ellipse((56, 18, 184, 146), fill=(44, 40, 94))
    d.text((86, 34), "Santiago", font=FONT_SM, fill=WHITE)
    d.text((42, 94), "17C", font=FONT_XL, fill=WHITE)
    d.text((50, 132), "H:18  L:9", font=FONT_SM, fill=LIGHTGREY)
    center_text(d, 150, weather_label(61), FONT_SM, SOFT_WHITE)
    weather_icon(d, 158, 78, 61, True)
    d.line((46, 166, 194, 166), fill=(62, 90, 120), width=1)
    forecast = [("Sab", 3, "18/9C"), ("Dom", 61, "16/8C"), ("Lun", 0, "19/10C")]
    for idx, (day, code, temps) in enumerate(forecast):
        text_x = [52, 120, 188][idx]
        weather_icon(d, text_x, 184, code, False)
        center_text_in_box(d, (text_x - 24, 172, text_x + 24, 232), 206, day, FONT_SM, SOFT_WHITE)
        center_text_in_box(d, (text_x - 24, 172, text_x + 24, 232), 220, temps, FONT_SM, WHITE)
    view_dots(d, 4, accent)
    return im


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    renders = {
        "view-pet.png": render_pet(),
        "view-system.png": render_system(),
        "view-network.png": render_network(),
        "view-docker.png": render_docker(),
        "view-weather.png": render_weather(),
    }
    for name, im in renders.items():
        im.save(OUT / name)

    sheet = Image.new("RGB", (SIZE * 3 + 32, SIZE * 2 + 24), (8, 12, 20))
    sheet_draw = ImageDraw.Draw(sheet)
    positions = [
        (8, 8, "Mascota", renders["view-pet.png"]),
        (SIZE + 16, 8, "Sistema", renders["view-system.png"]),
        (SIZE * 2 + 24, 8, "Red", renders["view-network.png"]),
        (8, SIZE + 16, "Docker", renders["view-docker.png"]),
        (SIZE + 16, SIZE + 16, "Clima", renders["view-weather.png"]),
    ]
    for x, y, label, render in positions:
        sheet.paste(render, (x, y))
        sheet_draw.text((x + 8, y + SIZE - 20), label, font=FONT_SM, fill=WHITE)
    sheet.save(OUT / "views-sheet.png")


if __name__ == "__main__":
    main()
