#include "Main.h"

#include "keyboard.h"

#include <Windows.h>
#include <Winuser.h>
#pragma comment (lib, "user32.lib")

#include <assert.h>
#include <stdio.h>
#include <string.h>

// See:
// http://www.codeproject.com/Articles/7305/Keyboard-Events-Simulation-using-keybd-event-funct
// http://stackoverflow.com/questions/2876275/what-are-the-ascii-values-of-up-down-left-right
//BYTE get_hardware_scan_code(keycode_t keycode) {
//    switch (keycode) {
//        case KEYCODE_LEFT:
//            return 0xCB;
//        case KEYCODE_UP:
//            return 0xC8;
//        case KEYCODE_RIGHT:
//            return 0xCD;
//        case KEYCODE_DOWN:
//            return 0xD0;
//        case KEYCODE_X:
//            return 0xAD;
//        case KEYCODE_Z:
//            return 0xAC;
//        case KEYCODE_SHIFT:
//            return 0xAA;
//        case KEYCODE_CONTROL:
//            return 0x9D;
//        case KEYCODE_ENTER:
//            return 0x9C;
//        default:
//            assert(false);
//            fprintf(stderr, "invalid keycode: %x\n", keycode);
//            exit(1);
//            return 0;
//    }
//}

WORD get_hardware_scan_code(keycode_t keycode) {
    switch (keycode) {
        case KEYCODE_LEFT:
            return 0x04CB;
        case KEYCODE_UP:
            return 0x04C8;
        case KEYCODE_RIGHT:
            return 0x04CD;
        case KEYCODE_DOWN:
            return 0x04D0;
        case KEYCODE_X:
            return 0x042D;
        case KEYCODE_Z:
            return 0x042C;
        case KEYCODE_SHIFT:
            return 0x042A;
        case KEYCODE_CONTROL:
            return 0x041D;
        case KEYCODE_ENTER:
            return 0x041C;
        default:
            assert(false);
            fprintf(stderr, "invalid keycode: %x\n", keycode);
            exit(1);
            return 0;
    }
}

DWORD get_keybd_event_flags(keycode_t keycode, bool is_keydown) {
    DWORD flags = (is_keydown ? 0 : KEYEVENTF_KEYUP);
    switch (keycode) {
        case KEYCODE_LEFT:
        case KEYCODE_RIGHT:
        case KEYCODE_DOWN:
        case KEYCODE_UP:
            flags |= KEYEVENTF_EXTENDEDKEY;
            break;
        default:
            break;
    }
    return flags;
}

void fill_input_information(INPUT *input, keycode_t keycode, bool is_keydown) {
    memset(input, 0, sizeof(INPUT));
    input->type = INPUT_KEYBOARD;
    input->ki.wScan = get_hardware_scan_code(keycode);
    input->ki.time = 0;
    input->ki.dwFlags = get_keybd_event_flags(keycode, is_keydown) | KEYEVENTF_SCANCODE;
    input->ki.dwExtraInfo = 0;
}

void make_key_event(keycode_t keycode, bool is_keydown) {
    INPUT input;
    fill_input_information(&input, keycode, is_keydown);

    UINT inserted = SendInput(1, &input, sizeof(INPUT));

    if (inserted != 1) {
        fprintf(stderr, "Could not insert key event: %02x\n", keycode);
    }
    fprintf(stdout, "made key event: %x\n", keycode);
}

// Note: This is really slow. Don't use this for the actual game player.
void make_key_tap(keycode_t keycode) {
    make_key_event(keycode);
    short_sleep();
    make_key_event(keycode, false);
    long_wait();
}

void short_sleep() {
    Sleep(100);
}

void long_wait() {
    Sleep(500);
}

void really_long_wait() {
    Sleep(5000);
}