#ifndef STARWARS_IMPERIALFLEET_H
#define STARWARS_IMPERIALFLEET_H

#include "helper.h"
#include <memory>
#include <vector>


class ImperialStarship : public virtual WarshipUnit {
public:
    ImperialStarship() = default;
    virtual ~ImperialStarship() = 0;
};


class ImperialFighter : public Starship, public ImperialStarship, public StarshipFighter {
public:
    ImperialFighter(ShieldPoints shield_points, AttackPower attack_power);
    virtual ~ImperialFighter() = 0;
};


class DeathStar : public ImperialFighter {
public:
    DeathStar(ShieldPoints shield_points, AttackPower attack_power);
};


class ImperialDestroyer : public ImperialFighter {
public:
    ImperialDestroyer(ShieldPoints shield_points, AttackPower attack_power);
};


class TIEFighter : public ImperialFighter {
public:
    TIEFighter(ShieldPoints shield_points, AttackPower attack_power);
};


class Squadron : public ImperialStarship {
    std::vector<std::shared_ptr<ImperialStarship>> ships;

public:
    Squadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships);
    Squadron(const std::vector<std::shared_ptr<ImperialStarship>>& ships);

    void attack(StarshipUnit* target) override;
    void takeDamage(AttackPower damage) override;
    ShieldPoints getShield() override;
    AttackPower getAttackPower() override;
    size_t getUnitSize() override;
    bool isDestroyed() override;
};


std::shared_ptr<ImperialStarship> createDeathStar(ShieldPoints shield_points, AttackPower attack_power);

std::shared_ptr<ImperialStarship> createImperialDestroyer(ShieldPoints shield_points, AttackPower attack_power);

std::shared_ptr<ImperialStarship> createTIEFighter(ShieldPoints shield_points, AttackPower attack_power);

std::shared_ptr<ImperialStarship> createSquadron(std::initializer_list<std::shared_ptr<ImperialStarship>> ships);

std::shared_ptr<ImperialStarship> createSquadron(const std::vector<std::shared_ptr<ImperialStarship>>& ships);

#endif //STARWARS_IMPERIALFLEET_H
