#ifndef STARWARS_REBELFLEET_H
#define STARWARS_REBELFLEET_H

#include "helper.h"
#include <memory>


class RebelStarship : public Starship {
    Speed speed;

public:
    RebelStarship(ShieldPoints shield_points, Speed speed);
    virtual ~RebelStarship() = 0;

    Speed getSpeed();
};


class RebelFighter : public RebelStarship, public StarshipFighter {
public:
    RebelFighter(ShieldPoints shield_points, Speed speed, AttackPower attack_power);
    virtual ~RebelFighter() = 0;
};

class Explorer : public RebelStarship {
public:
    Explorer(ShieldPoints shield_points, Speed speed);
};

class StarCruiser : public RebelFighter {
public:
    StarCruiser(ShieldPoints shield_points, Speed speed, AttackPower attack_power);
};

class XWing : public RebelFighter {
public:
    XWing(ShieldPoints shield_points, Speed speed, AttackPower attack_power);
};


/*** FABRICATING FUNCTIONS ***/

std::shared_ptr<RebelStarship> createExplorer(ShieldPoints shield_points, Speed speed);

std::shared_ptr<RebelStarship> createXWing(ShieldPoints shield_points, Speed speed, AttackPower attack_power);

std::shared_ptr<RebelStarship> createStarCruiser(ShieldPoints shield_points, Speed speed, AttackPower attack_power);

#endif //STARWARS_REBELFLEET_H
