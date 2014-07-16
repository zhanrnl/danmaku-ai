#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Windows.h>
#include <Winuser.h>

enum keycode_t {
    KEYCODE_LEFT = VK_LEFT,
    KEYCODE_UP = VK_UP,
    KEYCODE_RIGHT = VK_RIGHT,
    KEYCODE_DOWN = VK_DOWN,
    KEYCODE_X = 0x58,
    KEYCODE_Z = 0x5A,
    KEYCODE_SHIFT = VK_SHIFT,
    KEYCODE_CONTROL = VK_CONTROL,
    KEYCODE_ENTER = VK_RETURN,
};

void make_key_event(keycode_t keycode, bool is_keydown=true);

void make_key_tap(keycode_t keycode);

void short_sleep();
void long_wait();

void really_long_wait();

#endif  // KEYBOARD_H