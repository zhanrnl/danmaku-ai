#ifndef OPTIONS_H
#define OPTIONS_H

enum TouhouGame {
    TOUHOU_EOSD,
    TOUHOU_PCB,
    TOUHOU_IN,
    TOUHOU_MOF,
    TOUHOU_SA,
    TOUHOU_UFO,
    TOUHOU_TD,
    TOUHOU_DDC,
};

enum Difficulty {
    EASY,
    MEDIUM,
    HARD,
    LUNATIC,
    EXTRA,
};

enum Character {
    REIMU,
    MARISA,
    SAKUYA,
    YOUMU,
    YUKARI,
    ALICE,
    REMILIA,
    YUYUKO,
    SANAE,

    REIMU_AND_YUKARI,
    MARISA_AND_ALICE,
    SAKUYA_AND_REMILIA,
    YOUMU_AND_YUYUKO,

    NUM_CHARACTERS,
    END_OF_LIST,
};

enum Shot {
    SHOT_A,
    SHOT_B,
    SHOT_C,
    SHOT_NONE,
};

class Options {
public:
    Options(TouhouGame touhou_game, Difficulty difficulty, Character character, Shot shot);
    ~Options();

    // TODO(kxing): Inline these methods.
    TouhouGame get_touhou_game() const;
    Difficulty get_difficulty() const;
    Character get_character() const;
    Shot get_shot() const;
private:
    TouhouGame touhou_game;
    Difficulty difficulty;
    Character character;
    Shot shot;
};

#endif  // OPTIONS_H