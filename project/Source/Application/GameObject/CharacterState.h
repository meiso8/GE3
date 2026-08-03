#pragma once

struct HPs {
    int maxHp;
    int hp;
};

struct CharacterState {
    bool isHit;
    bool isDead;

    HPs hps;
};