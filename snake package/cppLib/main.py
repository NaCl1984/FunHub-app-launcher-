import time
from conback import ConsoleBackend, Alignment, PixelStyle, Key, ColorRGB
import conback

def main():
    context = ConsoleBackend()
    context.init()

    context.set_alignment(Alignment.Center)
    context.set_pixel_style(PixelStyle.Small)
    context.set_cursor_visibility(False)
    context.clear_screen()

    mode = 0
    target = 0
    width = 32

    prev_w, prev_h = 0, 0

    animation = [0] * (32 * 32)
    output = [ColorRGB() for _ in range(len(animation))]
    output2 = [0] * len(animation)

    while True:
        t_width, t_height = context.get_terminal_size()

        # Очищаем экран при изменении размеров окна
        if t_height != prev_h or t_width != prev_w:
            context.clear_screen()
            prev_h = t_height
            prev_w = t_width

        # Обновляем состояние кадров анимации
        for i in range(len(animation)):
            if i == target:
                animation[i] = 15
            else:
                animation[i] = max(1, animation[i] - 1)

            val = animation[i] * 17

            output[i] = ColorRGB(val, val, val)
            output2[i] = animation[i]

        target = (target + 1) % len(animation)

        # Отрисовка в зависимости от выбранного режима
        if mode == 0:
            context.print_img_true_color(output, width)
        elif mode == 1:
            context.print_img_16_colors(output2, width)

        # Вывод отладочной информации и подсказок
        context.print_str(f"{target}   ", x=0, y=0)
        context.print_str("Esc - exit, ←/→ - change mode", x=0, y=t_height - 1)

        context.set_cursor_pos(0, 0)

        # Обработка ввода
        if context.is_key_just_pressed(Key.Right):
            context.clear_screen()
            mode = (mode + 1) % 2
        elif context.is_key_just_pressed(Key.Left):
            context.clear_screen()
            mode = (mode - 1) % 2
        elif context.is_key_pressed(Key.Escape):
            break

        context.update_input()

        # Небольшая пауза для снижения нагрузки на ЦП (в C++ у тебя был заголовок <thread>)
        time.sleep(0.01)

    context.clear_screen()
    context.shutdown()
    context.close()

if __name__ == "__main__":
    main()