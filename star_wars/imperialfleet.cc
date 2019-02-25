#include "imperialfleet.h"

/*** CONSTRUCTORS ***/

ImperialFighter::ImperialFighter(ShieldPoints shield_points, AttackPower attack_power)
    : Starship(shield_points)
    , StarshipFighter(attack_power)
    {}

DeathStar::DeathStar(ShieldPoints shield_points, AttackPower attack_power)
    : ImperialFighter(shield_points, attack_power)
    {}

ImperialDestroyer::ImperialDestroyer(ShieldPoints shield_points, AttackPower attack_power)
    : ImperialFighter(shield_points, attack_power)
    {}

TIEFighter::TIEFighter(ShieldPoints shield_points, AttackPower attack_power)
    : ImperialFighter(shield_points, attack_power)
    {}

Squadron::Squadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships)
    : ships(ships)
    {}

Squadron::Squadron(const std::vector<std::shared_ptr<ImperialStarship>>& ships)
    : ships(ships)
    {}

/*** DESTRUCTORS ***/

ImperialStarship::~ImperialStarship() = default;

ImperialFighter::~ImperialFighter() = default;

/*** SQUADRON ***/

AttackPower Squadron::getAttackPower() {
    AttackPower attack_power = 0;

    for (const auto& ship : ships)
        if (!ship->isDestroyed())
            attack_power += ship->getAttackPower();

    return attack_power;
}

ShieldPoints Squadron::getShield() {
    ShieldPoints shield_points = 0;

    for (const auto& ship : ships)
        shield_points += ship->getShield();

    return shield_points;
}

void Squadron::attack(StarshipUnit* target) {
    for (const auto& ship : ships)
        if (!ship->isDestroyed())
            ship->attack(target);
}

void Squadron::takeDamage(AttackPower damage) {
    for (const auto& ship : ships)
        ship->takeDamage(damage);
}

size_t Squadron::getUnitSize() {
    size_t res = 0;
    for (const auto& ship : ships)
        res += (*ship).getUnitSize();

    return res;
}

bool Squadron::isDestroyed() {
    for (const auto& ship : ships)
        if (!ship->isDestroyed())
            return false;

    return true;
}


/*** FABRICATING FUNCTIONS ***/

std::shared_ptr<ImperialStarship> createDeathStar(ShieldPoints shield_points, AttackPower attack_power) {
    return std::make_shared<DeathStar>(shield_points, attack_power);
}

std::shared_ptr<ImperialStarship> createImperialDestroyer(ShieldPoints shield_points, AttackPower attack_power) {
    return std::make_shared<ImperialDestroyer>(shield_points, attack_power);
}

std::shared_ptr<ImperialStarship> createTIEFighter(ShieldPoints shield_points, AttackPower attack_power) {
    return std::make_shared<TIEFighter>(shield_points, attack_power);
}

std::shared_ptr<ImperialStarship> createSquadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships) {
    return std::make_shared<Squadron>(ships);
}

std::shared_ptr<ImperialStarship> createSquadron(const std::vector<std::shared_ptr<ImperialStarship>>& ships) {
    return std::make_shared<Squadron>(ships);
}

