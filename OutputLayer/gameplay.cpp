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
	if (x_pressed) {
		make_key_event(KEYCODE_X, false);
		x_pressed = false;
	}
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
		left_pressed = true;
    } else if (!(direction & DIRECTION_LEFT) && left_pressed) {
        make_key_event(KEYCODE_LEFT, false);
		left_pressed = false;
    }

    if ((direction & DIRECTION_RIGHT) && !right_pressed) {
        make_key_event(KEYCODE_RIGHT);
		right_pressed = true;
    } else if (!(direction & DIRECTION_RIGHT) && right_pressed) {
        make_key_event(KEYCODE_RIGHT, false);
		right_pressed = false;
    }

    if ((direction & DIRECTION_UP) && !up_pressed) {
        make_key_event(KEYCODE_UP);
		up_pressed = true;
    } else if (!(direction & DIRECTION_UP) && up_pressed) {
        make_key_event(KEYCODE_UP, false);
		up_pressed = false;
    }

    if ((direction & DIRECTION_DOWN) && !down_pressed) {
        make_key_event(KEYCODE_DOWN);
		down_pressed = true;
    } else if (!(direction & DIRECTION_DOWN) && down_pressed) {
        make_key_event(KEYCODE_DOWN, false);
		down_pressed = false;
    }

    if (shoot && !z_pressed) {
        make_key_event(KEYCODE_Z);
		z_pressed = true;
    } else if (!shoot && z_pressed) {
        make_key_event(KEYCODE_Z, false);
		z_pressed = false;
    }

    if (focus && !shift_pressed) {
        make_key_event(KEYCODE_SHIFT);
		shift_pressed = true;
    } else if (!focus && shift_pressed) {
        make_key_event(KEYCODE_SHIFT, false);
		shift_pressed = false;
    }
}

void Gameplay::bomb() {
	if (!x_pressed) {
		make_key_event(KEYCODE_X);
		x_pressed = true;
	}
}

void Gameplay::skip_dialog() {
    make_key_event(KEYCODE_CONTROL);
    long_wait();
    make_key_event(KEYCODE_CONTROL, false);
}