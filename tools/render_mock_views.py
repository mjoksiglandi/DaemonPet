from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(r"C:\Users\juan.cornejo\Desktop\DaemonPet")
OUT_DIR = ROOT / "docs" / "renders"
OUT_DIR.mkdir(parents=True, exist_ok=True)

MOCK_SYSTEM = Path(r"C:\Users\juan.cornejo\Downloads\ChatGPT Image 3 jul 2026, 11_56_08 a.m. (2).png")
MOCK_DOCKER = Path(r"C:\Users\juan.cornejo\Downloads\ChatGPT Image 3 jul 2026, 11_56_08 a.m. (3).png")
MOCK_NETWORK = Path(r"C:\Users\juan.cornejo\Downloads\ChatGPT Image 3 jul 2026, 12_01_38 p.m..png")

FONT_REGULAR = r"C:\Windows\Fonts\segoeui.ttf"
FONT_BOLD = r"C:\Windows\Fonts\segoeuib.ttf"

W = 240
H = 240

COLORS = {
    "white": "#D7DEE5",
    "cyan": "#5FE1E6",
    "violet": "#B39BFF",
    "green": "#61E294",
    "orange": "#FF8A3D",
    "yellow": "#F5B942",
}

SYSTEM_BOXES = {
    "cpu": (88, 50, 57, 19),
    "temp": (89, 90, 55, 18),
    "ram": (86, 128, 59, 16),
    "disk": (88, 163, 56, 19),
}

NETWORK_BOXES = {
    "host": (86, 47, 57, 17),
    "ip": (87, 80, 56, 15),
    "rx": (87, 113, 57, 14),
    "tx": (87, 143, 57, 14),
    "uptime": (87, 175, 55, 15),
}

DOCKER_BOXES = {
    "main": (82, 60, 76, 72),
    "secondary": (94, 155, 52, 36),
}


def font(path: str, size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(path, size)


def resize_mock(path: Path) -> Image.Image:
    return Image.open(path).convert("RGBA").resize((W, H), Image.Resampling.LANCZOS)


def fit_text(draw: ImageDraw.ImageDraw, text: str, box: tuple[int, int, int, int], color: str,
             min_size: int, max_size: int, align: str = "center", pad_x: int = 0,
             offset_x: int = 0, offset_y: int = 0) -> None:
    x, y, w, h = box
    selected = font(FONT_BOLD, min_size)
    for size in range(max_size, min_size - 1, -1):
        candidate = font(FONT_BOLD, size)
        left, top, right, bottom = draw.textbbox((0, 0), text, font=candidate)
        text_w = right - left
        text_h = bottom - top
        if text_w <= w and text_h <= h:
            selected = candidate
            break

    left, top, right, bottom = draw.textbbox((0, 0), text, font=selected)
    text_w = right - left
    text_h = bottom - top

    if align == "left":
        draw_x = x + pad_x
    elif align == "right":
        draw_x = x + w - text_w - pad_x
    else:
        draw_x = x + (w - text_w) / 2 + offset_x

    draw_y = y + (h - text_h) / 2 - top + offset_y
    draw.text((draw_x, draw_y), text, font=selected, fill=color)


def render_system() -> None:
    image = resize_mock(MOCK_SYSTEM)
    draw = ImageDraw.Draw(image)
    fit_text(draw, "23%", SYSTEM_BOXES["cpu"], COLORS["cyan"], 20, 20, "center", offset_x=3)
    fit_text(draw, "48.2C", SYSTEM_BOXES["temp"], COLORS["orange"], 18, 18, "center", offset_x=3)
    fit_text(draw, "3.1G", SYSTEM_BOXES["ram"], COLORS["green"], 18, 18, "center", offset_x=3)
    fit_text(draw, "62%", SYSTEM_BOXES["disk"], COLORS["yellow"], 20, 20, "center", offset_x=3)
    image.save(OUT_DIR / "view-system-mock.png")


def render_network() -> None:
    image = resize_mock(MOCK_NETWORK)
    draw = ImageDraw.Draw(image)
    fit_text(draw, "pi-homelab", NETWORK_BOXES["host"], COLORS["white"], 9, 9, "left", pad_x=8, offset_y=3)
    fit_text(draw, "192.168.1.20", NETWORK_BOXES["ip"], COLORS["white"], 9, 9, "left", pad_x=8, offset_y=3)
    fit_text(draw, "12M", NETWORK_BOXES["rx"], COLORS["cyan"], 9, 9, "left", pad_x=8, offset_y=6)
    fit_text(draw, "2M", NETWORK_BOXES["tx"], COLORS["violet"], 9, 9, "left", pad_x=8, offset_y=6)
    fit_text(draw, "3d 04h", NETWORK_BOXES["uptime"], COLORS["white"], 9, 9, "left", pad_x=8, offset_y=6)
    image.save(OUT_DIR / "view-network-mock.png")


def render_docker() -> None:
    image = resize_mock(MOCK_DOCKER)
    draw = ImageDraw.Draw(image)
    fit_text(draw, "18", DOCKER_BOXES["main"], COLORS["white"], 28, 54, "center")
    fit_text(draw, "0", DOCKER_BOXES["secondary"], COLORS["green"], 22, 40, "center")
    image.save(OUT_DIR / "view-docker-mock.png")


if __name__ == "__main__":
    render_system()
    render_network()
    render_docker()
