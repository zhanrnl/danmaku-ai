#ifndef GAMEPLAY_H
#define GAMEPLAY_H

typedef int direction_t;

const direction_t DIRECTION_LEFT = 0x1;
const direction_t DIRECTION_RIGHT = 0x2;
const direction_t DIRECTION_UP = 0x4;
const direction_t DIRECTION_DOWN = 0x8;

class Gameplay {

public:
    Gameplay();
    ~Gameplay();

    void move(direction_t direction, bool shoot, bool focus);
    void bomb();
    void skip_dialog();

private:
    bool left_pressed;
    bool right_pressed;
    bool up_pressed;
    bool down_pressed;
    bool z_pressed;
    bool shift_pressed;
    bool control_pressed;
};

#endif  // GAMEPLAY_H