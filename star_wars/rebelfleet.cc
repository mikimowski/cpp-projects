#include "rebelfleet.h"
#include <cassert>

namespace {
    constexpr Speed MIN_REBEL_SPEED = 299796;
    constexpr Speed MAX_REBEL_SPEED = 2997960;
    constexpr Speed MIN_STARCRUISER_SPEED = 99999;
    constexpr Speed MAX_STARCRUISER_SPEED = 299795;
}

/*** CONSTRUCTORS ***/

RebelStarship::RebelStarship(ShieldPoints shield_points, Speed speed)
    : Starship(shield_points)
    , speed(speed)
    {}

RebelFighter::RebelFighter(ShieldPoints shield_points, Speed speed, AttackPower attack_power)
    : RebelStarship(shield_points, speed)
    , StarshipFighter(attack_power)
    {}

Explorer::Explorer(ShieldPoints shield_points, Speed speed)
    : RebelStarship(shield_points, speed)
    {
        assert(speed >= MIN_REBEL_SPEED && speed <= MAX_REBEL_SPEED);
    }

StarCruiser::StarCruiser(ShieldPoints shield_points, Speed speed, AttackPower attack_power)
    : RebelFighter(shield_points, speed, attack_power)
    {
        assert(speed >= MIN_STARCRUISER_SPEED && speed <= MAX_STARCRUISER_SPEED);
    }

XWing::XWing(ShieldPoints shield_points, Speed speed, AttackPower attack_power)
    : RebelFighter(shield_points, speed, attack_power)
    {
        assert(speed >= MIN_REBEL_SPEED && speed <= MAX_REBEL_SPEED);
    }

/*** DESCTRUCTORS ***/

RebelStarship::~RebelStarship() = default;

RebelFighter::~RebelFighter() = default;


/*** GETTERS ***/

Speed RebelStarship::getSpeed() {
    return speed;
}

/*** FABRICATING FUNCTIONS ***/

std::shared_ptr<RebelStarship> createExplorer(ShieldPoints shield_points, Speed speed) {
    return std::make_shared<Explorer>(shield_points, speed);
}

std::shared_ptr<RebelStarship> createXWing(ShieldPoints shield_points, Speed speed, AttackPower attack_power) {
    return std::make_shared<XWing>(shield_points, speed, attack_power);
}

std::shared_ptr<RebelStarship> createStarCruiser(ShieldPoints shield_points, Speed speed, AttackPower attack_power) {
    return std::make_shared<StarCruiser>(shield_points, speed, attack_power);
}
