#ifndef STARWARS_HELPER_H
#define STARWARS_HELPER_H

#include <cstdint>
#include <cstddef>

using ShieldPoints = int32_t;
using Speed = int32_t;
using AttackPower = int32_t;
using Time = int32_t;


class StarshipUnit {
public:
    virtual ~StarshipUnit() = default;

    virtual void takeDamage(AttackPower damage) = 0;
    virtual void defend(StarshipUnit* attacker);  /// By default - no action
    virtual ShieldPoints getShield() = 0;
    virtual size_t getUnitSize() = 0;
    virtual bool isDestroyed() = 0;
};


class WarshipUnit : public virtual StarshipUnit {
public:
    virtual void attack(StarshipUnit* target) = 0;
    virtual AttackPower getAttackPower() = 0;
};


class Starship : public virtual StarshipUnit {
    ShieldPoints shield_points;

public:
    Starship(ShieldPoints shield_points);
    virtual ~Starship() = 0;

    void takeDamage(AttackPower damage) override;
    ShieldPoints getShield() override;
    bool isDestroyed() override;
    size_t getUnitSize() override;
};


class StarshipFighter : public virtual WarshipUnit {
    AttackPower attack_power;

public:
    StarshipFighter(AttackPower attack_power);
    virtual ~StarshipFighter() = 0;

    void attack(StarshipUnit* target) override;
    AttackPower getAttackPower() override;
    void defend(StarshipUnit* attacker) override;
};


#endif //STARWARS_HELPER_H
