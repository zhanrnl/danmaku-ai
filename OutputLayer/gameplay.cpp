#include "Main.h"

#include "gameplay.h"

#include <assert.h>
#include <stdio.h>

#include "keyboard.h"

Gameplay::Gameplay() :
    left_pressed(false), right_pressed(false), up_pressed(false), down_pressed(false),
    z_pressed(false), shift_pressed(false), control_pressed(false) {
}

Gameplay::~Gameplay() {
}

void Gameplay::move(int direction, bool shoot, bool focus) {
    // Make sure the direction keys are set correctly.
    bool is_valid = true;
    if ((direction & DIRECTION_LEFT) && (direction & DIRECTION_RIGHT)) {
        is_valid = false;
    } else if ((direction & DIRECTION_UP) && (direction & DIRECTION_DOWN)) {
        is_valid = false;
    }

    if (!is_valid) {   
        assert(false);
        fprintf(stderr, "Invalid direction: %d\n", direction);
        exit(1);
    }

    if ((direction & DIRECTION_LEFT) && !left_pressed) {
        make_key_event(KEYCODE_LEFT);
    } else if (!(direction & DIRECTION_LEFT) && left_pressed) {
        make_key_event(KEYCODE_LEFT, false);
    }

    if ((direction & DIRECTION_RIGHT) && !right_pressed) {
        make_key_event(KEYCODE_RIGHT);
    } else if (!(direction & DIRECTION_RIGHT) && right_pressed) {
        make_key_event(KEYCODE_RIGHT, false);
    }

    if ((direction & DIRECTION_UP) && !up_pressed) {
        make_key_event(KEYCODE_UP);
    } else if (!(direction & DIRECTION_UP) && up_pressed) {
        make_key_event(KEYCODE_UP, false);
    }

    if ((direction & DIRECTION_DOWN) && !down_pressed) {
        make_key_event(KEYCODE_DOWN);
    } else if (!(direction & DIRECTION_DOWN) && down_pressed) {
        make_key_event(KEYCODE_DOWN, false);
    }

    if (shoot && !z_pressed) {
        make_key_event(KEYCODE_Z);
    } else if (!shoot && z_pressed) {
        make_key_event(KEYCODE_Z, false);
    }

    if (focus && !shift_pressed) {
        make_key_event(KEYCODE_SHIFT);
    } else if (!focus && shift_pressed) {
        make_key_event(KEYCODE_SHIFT, false);
    }
}

void Gameplay::bomb() {
    make_key_tap(KEYCODE_X);
}

void Gameplay::skip_dialog() {
    make_key_event(KEYCODE_CONTROL);
    long_wait();
    make_key_event(KEYCODE_CONTROL, false);
}