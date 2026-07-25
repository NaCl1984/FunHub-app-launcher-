import ctypes
from enum import IntEnum
import os
import platform
import time
from typing import List, Tuple, Optional


# ==================== C-СТРУКТУРЫ И ENUM ====================

class Alignment(IntEnum):
    LeftTop = 0
    CenterTop = 1
    RightTop = 2
    LeftCenter = 3
    Center = 4
    RightCenter = 5
    LeftBottom = 6
    CenterBottom = 7
    RightBottom = 8


class PixelStyle(IntEnum):
    Small = 0
    Normal = 1
    Big = 2


class Key(IntEnum):
    A = 0; B = 1; C = 2; D = 3; E = 4; F = 5; G = 6; H = 7; I = 8; J = 9; K = 10; L = 11; M = 12
    N = 13; O = 14; P = 15; Q = 16; R = 17; S = 18; T = 19; U = 20; V = 21; W = 22; X = 23; Y = 24; Z = 25
    Num0 = 26; Num1 = 27; Num2 = 28; Num3 = 29; Num4 = 30; Num5 = 31; Num6 = 32; Num7 = 33; Num8 = 34; Num9 = 35
    F1 = 36; F2 = 37; F3 = 38; F4 = 39; F5 = 40; F6 = 41; F7 = 42; F8 = 43; F9 = 44; F10 = 45; F11 = 46; F12 = 47
    Up = 48; Down = 49; Left = 50; Right = 51
    Escape = 52; Space = 53; Shift = 54; Ctrl = 55; Alt = 56; Tab = 57; CapsLock = 58; Enter = 59; Backspace = 60; Tilde = 61
    Insert = 62; Delete = 63; Home = 64; End = 65; PageUp = 66; PageDown = 67
    Count = 68


class ColorRGB(ctypes.Structure):
    _fields_ = [
        ("r", ctypes.c_uint8),
        ("g", ctypes.c_uint8),
        ("b", ctypes.c_uint8),
    ]

    def __init__(self, r: int = 0, g: int = 0, b: int = 0):
        super().__init__(r, g, b)

# ==================== КОНСТАНТЫ ====================

DEFAULT_FG_IDX = 15
DEFAULT_BG_IDX = 0

CONBACK_DEFAULT_PALETTE = [
    ColorRGB(0, 0, 0),       ColorRGB(128, 0, 0),     ColorRGB(0, 128, 0),     ColorRGB(128, 128, 0),
    ColorRGB(0, 0, 128),     ColorRGB(128, 0, 128),   ColorRGB(0, 128, 128),   ColorRGB(192, 129, 129),
    ColorRGB(128, 128, 128), ColorRGB(255, 0, 0),     ColorRGB(0, 255, 0),     ColorRGB(255, 255, 0),
    ColorRGB(0, 0, 255),     ColorRGB(255, 0, 255),   ColorRGB(0, 255, 255),   ColorRGB(255, 255, 255)
]

class C_ConBackChar(ctypes.Structure):
    _fields_ = [
        ("ch", ctypes.c_wchar),
        ("fgIdx", ctypes.c_uint8),
        ("bgIdx", ctypes.c_uint8),
    ]


# ==================== ЗАГРУЗКА И НАСТРОЙКА C-API ====================

def _load_library():
    system = platform.system()
    base_dir = os.path.dirname(os.path.abspath(__file__))
    
    if system == "Windows":
        lib_name = "conback_native.dll"
    elif system == "Linux":
        lib_name = "libconback_native.so"
    else:
        raise RuntimeError(f"Неподдерживаемая ОС: {system}")

    lib_path = os.path.join(base_dir, lib_name)
    if not os.path.exists(lib_path):
        lib_path = lib_name  # пробуем загрузить из системного пути

    return ctypes.CDLL(lib_path)


_lib = _load_library()

# Настройка типов C-функций
_lib.ConsoleBackend_Create.restype = ctypes.c_void_p
_lib.ConsoleBackend_Destroy.argtypes = [ctypes.c_void_p]

_lib.ConsoleBackend_Init.argtypes = [ctypes.c_void_p]
_lib.ConsoleBackend_InitWithConfig.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.POINTER(ColorRGB), ctypes.c_int, ctypes.c_bool]
_lib.ConsoleBackend_Shutdown.argtypes = [ctypes.c_void_p]
_lib.ConsoleBackend_ClearScreen.argtypes = [ctypes.c_void_p]

_lib.ConsoleBackend_PrintStr.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
_lib.ConsoleBackend_PrintStrAt.argtypes = [ctypes.c_void_p, ctypes.c_char_p, ctypes.c_int, ctypes.c_int]
_lib.ConsoleBackend_PrintStr16Colors.argtypes = [ctypes.c_void_p, ctypes.POINTER(C_ConBackChar), ctypes.c_size_t]
_lib.ConsoleBackend_PrintImgTrueColor.argtypes = [ctypes.c_void_p, ctypes.POINTER(ColorRGB), ctypes.c_size_t, ctypes.c_int]
_lib.ConsoleBackend_PrintImg16Colors.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_uint8), ctypes.c_size_t, ctypes.c_int]

_lib.ConsoleBackend_GetTerminalSize.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)]
_lib.ConsoleBackend_SetTerminalSize.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
_lib.ConsoleBackend_SetCursorPos.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int]
_lib.ConsoleBackend_SetCursorVisibility.argtypes = [ctypes.c_void_p, ctypes.c_bool]

_lib.ConsoleBackend_SetAlignment.argtypes = [ctypes.c_void_p, ctypes.c_int]
_lib.ConsoleBackend_SetPalette.argtypes = [ctypes.c_void_p, ctypes.POINTER(ColorRGB)]
_lib.ConsoleBackend_SetFgColorIndex.argtypes = [ctypes.c_void_p, ctypes.c_uint8]
_lib.ConsoleBackend_SetBgColorIndex.argtypes = [ctypes.c_void_p, ctypes.c_uint8]
_lib.ConsoleBackend_SetPixelStyle.argtypes = [ctypes.c_void_p, ctypes.c_int]

_lib.ConsoleBackend_UpdateInput.argtypes = [ctypes.c_void_p]
_lib.ConsoleBackend_IsKeyPressed.argtypes = [ctypes.c_void_p, ctypes.c_int]
_lib.ConsoleBackend_IsKeyPressed.restype = ctypes.c_bool
_lib.ConsoleBackend_IsKeyReleased.argtypes = [ctypes.c_void_p, ctypes.c_int]
_lib.ConsoleBackend_IsKeyReleased.restype = ctypes.c_bool
_lib.ConsoleBackend_IsKeyJustPressed.argtypes = [ctypes.c_void_p, ctypes.c_int]
_lib.ConsoleBackend_IsKeyJustPressed.restype = ctypes.c_bool
_lib.ConsoleBackend_IsKeyJustReleased.argtypes = [ctypes.c_void_p, ctypes.c_int]
_lib.ConsoleBackend_IsKeyJustReleased.restype = ctypes.c_bool


# ==================== ОСНОВНОЙ PYTHON КЛАСС ====================

class ConsoleBackend:
    def __init__(self):
        self._handle = _lib.ConsoleBackend_Create()
        if not self._handle:
            raise RuntimeError("Не удалось создать экземпляр ConsoleBackend")
        c_palette = (ColorRGB * 16)(*CONBACK_DEFAULT_PALETTE)
        _lib.ConsoleBackend_SetPalette(self._handle, c_palette)

    def init(self, alignment: Alignment = Alignment.LeftTop, palette: Optional[List[ColorRGB]] = None, 
             style: PixelStyle = PixelStyle.Normal, cursor_visibility: bool = True):
        if palette is not None and len(palette) == 16:
            palette_arr = (ColorRGB * 16)(*palette)
            _lib.ConsoleBackend_InitWithConfig(self._handle, int(alignment), palette_arr, int(style), cursor_visibility)
        else:
            _lib.ConsoleBackend_Init(self._handle)

    def shutdown(self):
        _lib.ConsoleBackend_Shutdown(self._handle)

    def clear_screen(self):
        _lib.ConsoleBackend_ClearScreen(self._handle)

    # --- Печать ---

    def print_str(self, text: str, x: Optional[int] = None, y: Optional[int] = None):
        c_text = text.encode('utf-8')
        if x is not None and y is not None:
            _lib.ConsoleBackend_PrintStrAt(self._handle, c_text, x, y)
        else:
            _lib.ConsoleBackend_PrintStr(self._handle, c_text)

    def print_str_16_colors(self, chars: List[Tuple[str, int, int]]):
        """Принимает список кортежей: [('a', fg_idx, bg_idx), ...]"""
        c_chars = (C_ConBackChar * len(chars))()
        for idx, (ch, fg, bg) in enumerate(chars):
            c_chars[idx] = C_ConBackChar(ch, fg, bg)
        _lib.ConsoleBackend_PrintStr16Colors(self._handle, c_chars, len(chars))

    def print_img_true_color(self, pixels: List[ColorRGB], width: int):
        c_pixels = (ColorRGB * len(pixels))(*pixels)
        _lib.ConsoleBackend_PrintImgTrueColor(self._handle, c_pixels, len(pixels), width)

    def print_img_16_colors(self, indices: List[int], width: int):
        c_indices = (ctypes.c_uint8 * len(indices))(*indices)
        _lib.ConsoleBackend_PrintImg16Colors(self._handle, c_indices, len(indices), width)

    # --- Управление терминалом и курсором ---

    def get_terminal_size(self) -> Tuple[int, int]:
        w, h = ctypes.c_int(), ctypes.c_int()
        _lib.ConsoleBackend_GetTerminalSize(self._handle, ctypes.byref(w), ctypes.byref(h))
        return w.value, h.value

    def set_terminal_size(self, width: int, height: int):
        _lib.ConsoleBackend_SetTerminalSize(self._handle, width, height)

    def set_cursor_pos(self, x: int, y: int):
        _lib.ConsoleBackend_SetCursorPos(self._handle, x, y)

    def set_cursor_visibility(self, visible: bool):
        _lib.ConsoleBackend_SetCursorVisibility(self._handle, visible)

    # --- Сеттеры ---

    def set_alignment(self, alignment: Alignment):
        _lib.ConsoleBackend_SetAlignment(self._handle, int(alignment))

    def set_palette(self, palette: List[ColorRGB]):
        if len(palette) != 16:
            raise ValueError("Палитра должна содержать ровно 16 цветов")
        c_palette = (ColorRGB * 16)(*palette)
        _lib.ConsoleBackend_SetPalette(self._handle, c_palette)

    def set_fg_color_index(self, fg_idx: int):
        _lib.ConsoleBackend_SetFgColorIndex(self._handle, fg_idx)

    def set_bg_color_index(self, bg_idx: int):
        _lib.ConsoleBackend_SetBgColorIndex(self._handle, bg_idx)

    def set_pixel_style(self, style: PixelStyle):
        _lib.ConsoleBackend_SetPixelStyle(self._handle, int(style))

    # --- Ввод ---

    def update_input(self):
        _lib.ConsoleBackend_UpdateInput(self._handle)

    def is_key_pressed(self, key: Key) -> bool:
        return _lib.ConsoleBackend_IsKeyPressed(self._handle, int(key))

    def is_key_released(self, key: Key) -> bool:
        return _lib.ConsoleBackend_IsKeyReleased(self._handle, int(key))

    def is_key_just_pressed(self, key: Key) -> bool:
        return _lib.ConsoleBackend_IsKeyJustPressed(self._handle, int(key))

    def is_key_just_released(self, key: Key) -> bool:
        return _lib.ConsoleBackend_IsKeyJustReleased(self._handle, int(key))

    # --- Очистка ---

    def close(self):
        if hasattr(self, '_handle') and self._handle:
            _lib.ConsoleBackend_Destroy(self._handle)
            self._handle = None

    def __del__(self):
        self.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()