#include "helper.h"
#include <cassert>

/*** STARSHIPUNIT ***/

/// Basic StarshipUnit takes no action - only fighters can for instance counter attack
void StarshipUnit::defend(StarshipUnit* attacker) {
    (void)attacker;
}

/*** STARSHIP ***/

Starship::Starship(ShieldPoints shield_points)
    : shield_points(shield_points)
    {
        assert(shield_points >= 0);
    }

void Starship::takeDamage(AttackPower damage) {
    if (this->shield_points > damage)
        this->shield_points -= damage;
    else
        this->shield_points = 0;
}

ShieldPoints Starship::getShield() {
    return this->shield_points;
}

bool Starship::isDestroyed() {
    return this->getShield() == 0;
}

/// Basic unit has 1 ship, therefore it's size equals one if it's alive.
size_t Starship::getUnitSize() {
    return !isDestroyed();
}

Starship::~Starship() = default;


/*** STARSHIPFIGHTER ***/
StarshipFighter::StarshipFighter(AttackPower attack_power)
    : attack_power(attack_power)
    {
        assert(attack_power >= 0);
    }

AttackPower StarshipFighter::getAttackPower() {
    return attack_power;
}

void StarshipFighter::attack(StarshipUnit* target) {
    target->takeDamage(this->attack_power);
    target->defend(this);
}

void StarshipFighter::defend(StarshipUnit* attacker) {
    attacker->takeDamage(this->attack_power);
}

StarshipFighter::~StarshipFighter() = default;