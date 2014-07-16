#include "Main.h"

#include "options.h"

Options::Options(TouhouGame touhou_game, Difficulty difficulty, Character character, Shot shot) :
                 touhou_game(touhou_game), difficulty(difficulty), character(character), shot(shot) {
}

Options::~Options() {
}

TouhouGame Options::get_touhou_game() const {
    return touhou_game;
}

Difficulty Options::get_difficulty() const {
    return difficulty;
}

Character Options::get_character() const {
    return character;
}

Shot Options::get_shot() const {
    return shot;
}