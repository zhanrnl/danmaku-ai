#include "Main.h"

#include "navigation.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "keyboard.h"
#include "options.h"

const Character playable_characters_eosd[] = {
    REIMU, MARISA, END_OF_LIST,
};
const Character playable_characters_pcb[] = {
    REIMU, MARISA, SAKUYA, END_OF_LIST,
};
const Character playable_characters_in[] = {
    REIMU_AND_YUKARI, MARISA_AND_ALICE, SAKUYA_AND_REMILIA, YOUMU_AND_YUYUKO,
    REIMU, YUKARI, MARISA, ALICE, SAKUYA, REMILIA, YOUMU, YUYUKO, END_OF_LIST,
};
const Character playable_characters_mof[] = {
    REIMU, MARISA, END_OF_LIST,
};
const Character playable_characters_sa[] = {
    REIMU, MARISA, END_OF_LIST,
};
const Character playable_characters_ufo[] = {
    REIMU, MARISA, SANAE, END_OF_LIST,
};
const Character playable_characters_td[] = {
    REIMU, MARISA, SANAE, YOUMU, END_OF_LIST,
};
const Character playable_characters_ddc[] = {
    REIMU, MARISA, SAKUYA, END_OF_LIST,
};

bool is_character_selection_left_right(TouhouGame game) {
    switch (game) {
        case TOUHOU_EOSD:
        case TOUHOU_PCB:
        case TOUHOU_IN:
        case TOUHOU_MOF:
        case TOUHOU_SA:
        case TOUHOU_TD:
        case TOUHOU_DDC:
            return true;
        case TOUHOU_UFO:
            return false;
        default:
            assert(false);
            fprintf(stderr, "Invalid game option: %d\n", game);
            exit(1); 
            return false;
    }
}

void select_difficulty(Difficulty difficulty) {
    if (difficulty == EXTRA) {
        make_key_tap(KEYCODE_DOWN);
    } else {
        make_key_tap(KEYCODE_ENTER);

        switch (difficulty) {
            case EASY:
                make_key_tap(KEYCODE_UP);
                break;
            case MEDIUM:
                break;
            case HARD:
                make_key_tap(KEYCODE_DOWN);
                break;
            case LUNATIC:
                make_key_tap(KEYCODE_DOWN);
                make_key_tap(KEYCODE_DOWN);
                break;
            default:
                assert(false);
                fprintf(stderr, "Invalid difficulty option: %d\n", difficulty);
                exit(1);
                break;
        }
    }
    make_key_tap(KEYCODE_ENTER);
}

void select_character(TouhouGame game, Character character) {
    const Character *character_list = NULL;

    switch (game) {
        case TOUHOU_EOSD:
            character_list = playable_characters_eosd;
            break;
        case TOUHOU_PCB:
            character_list = playable_characters_pcb;
            break;
        case TOUHOU_IN:
            character_list = playable_characters_in;
            break;
        case TOUHOU_MOF:
            character_list = playable_characters_mof;
            break;
        case TOUHOU_SA:
            character_list = playable_characters_sa;
            break;
        case TOUHOU_UFO:
            character_list = playable_characters_ufo;
            break;
        case TOUHOU_TD:
            character_list = playable_characters_td;
            break;
        case TOUHOU_DDC:
            character_list = playable_characters_ddc;
            break;
        default:
            assert(false);
            fprintf(stderr, "Invalid game option: %d\n", game);
            exit(1);
            break;
    }

    const Character *current_character = character_list;
    while (*current_character != character) {
        if (*current_character == END_OF_LIST) {
            assert(false);
            fprintf(stderr, "Invalid character option: %d\n", character);
            exit(1);
        }
        
        bool is_left_right_selection = is_character_selection_left_right(game);
        make_key_tap(is_left_right_selection ? KEYCODE_RIGHT : KEYCODE_DOWN);
        current_character++;
    }

    make_key_tap(KEYCODE_ENTER);
}

void select_shot(TouhouGame game, Shot shot) {
    // Sanity checking.
    switch (game) {
        case TOUHOU_EOSD:
        case TOUHOU_PCB:
        case TOUHOU_UFO:
        case TOUHOU_DDC:
            if (shot != SHOT_A && shot != SHOT_B) {
                assert(false);
                fprintf(stderr, "Invalid shot option: %d\n", shot);
                exit(1);
            }
            break;
        case TOUHOU_MOF:
        case TOUHOU_SA:
            if (shot != SHOT_A && shot != SHOT_B && shot != SHOT_C) {
                assert(false);
                fprintf(stderr, "Invalid shot option: %d\n", shot);
                exit(1);
            }
            break;
        case TOUHOU_IN:
        case TOUHOU_TD:
            if (shot != SHOT_NONE) {
                assert(false);
                fprintf(stderr, "Invalid shot option: %d\n", shot);
                exit(1);
            }
            break;
    }

    // If there's no shots to choose from, we are done.
    if (shot == SHOT_NONE) {
        return;
    }

    switch (shot) {
        case SHOT_C:
            make_key_tap(KEYCODE_DOWN);
            // Fall-through
        case SHOT_B:
            make_key_tap(KEYCODE_DOWN);
            // Fall-through
        case SHOT_A:
            break;
        default:
            assert(false);
            fprintf(stderr, "Invalid shot option: %d\n", shot);
            exit(1);
            break;
    }
    make_key_tap(KEYCODE_ENTER);
}

void Navigation::start_game(Options options) {
    TouhouGame game = options.get_touhou_game();

    // Select the difficulty level.
    select_difficulty(options.get_difficulty());

    // Select the character.
    select_character(game, options.get_character());

    // Select the shot.
    select_shot(game, options.get_shot());
}