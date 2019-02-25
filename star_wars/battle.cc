#include "battle.h"
#include <cassert>
#include <iostream>


/*** SPACEBATTLE ***/

SpaceBattle::SpaceBattle(std::vector<std::shared_ptr<RebelStarship>>& rebel_ships,
    std::vector<std::shared_ptr<ImperialStarship>>& imperial_ships,
    std::shared_ptr<AttackStrategy>& attack_strategy)
    : rebel_ships(std::move(rebel_ships))
    , imperial_ships(std::move(imperial_ships))
    , attack_strategy(std::move(attack_strategy))
    , battle_stats(recountRebelFleet(), recountImperialFleet())
    {}

void SpaceBattle::tick(Time time_step) {
    tryToSettle();

    if (battle_stats.isOngoing()) {
        attack_strategy->simulateAction(rebel_ships, imperial_ships, time_step);
        updateBattleStats();
    } else {
        std::cout << battle_stats.getInfo() << std::endl;
    }
}

void SpaceBattle::updateBattleStats() {
    this->battle_stats.setCntImperialShips(this->recountImperialFleet());
    this->battle_stats.setCntRebelShips(this->recountRebelFleet());
}

void SpaceBattle::tryToSettle() {
    size_t cnt_imperial_ships = battle_stats.getCntImperialFleet();
    size_t cnt_rebel_ships = battle_stats.getCntRebelFleet();

    if (cnt_imperial_ships == 0 || cnt_rebel_ships == 0)
        battle_stats.setOngoing(false);
    if (cnt_imperial_ships == 0 && cnt_rebel_ships == 0)
        battle_stats.setInfo("DRAW");
    else if (cnt_imperial_ships == 0)
        battle_stats.setInfo("REBELLION WON");
    else if (cnt_rebel_ships == 0)
        battle_stats.setInfo("IMPERIUM WON");
}

size_t SpaceBattle::countRebelFleet() {
    return this->battle_stats.getCntRebelFleet();
}

size_t SpaceBattle::countImperialFleet() {
    return this->battle_stats.getCntImperialFleet();
}

size_t SpaceBattle::recountRebelFleet() {
    size_t cnt_alive = 0;
    for (const auto& ship : rebel_ships)
        cnt_alive += (*ship).getUnitSize();

    return cnt_alive;
}

size_t SpaceBattle::recountImperialFleet() {
    size_t cnt_alive = 0;
    for (const auto& ship : imperial_ships)
        cnt_alive += (*ship).getUnitSize();

    return cnt_alive;
}


/*** BATTLESTATS ***/

SpaceBattle::BattleStats::BattleStats(size_t cnt_rebel_ships, size_t cnt_imperial_ships)
    : cnt_rebel_ships(cnt_rebel_ships)
    , cnt_imperial_ships(cnt_imperial_ships)
    , ongoing(true)
    , info("ONGOING BATTLE")
    {}

void SpaceBattle::BattleStats::setOngoing(bool val) {
    ongoing = val;
}

void SpaceBattle::BattleStats::setInfo(const std::string& new_info) {
    info = new_info;
}

void SpaceBattle::BattleStats::setCntRebelShips(size_t new_amount) {
    cnt_rebel_ships = new_amount;
}

void SpaceBattle::BattleStats::setCntImperialShips(size_t new_amount) {
    cnt_imperial_ships = new_amount;
}

bool SpaceBattle::BattleStats::isOngoing() {
    return ongoing;
}

size_t SpaceBattle::BattleStats::getCntRebelFleet() {
    return cnt_rebel_ships;
}

size_t SpaceBattle::BattleStats::getCntImperialFleet() {
    return cnt_imperial_ships;
}

const std::string& SpaceBattle::BattleStats::getInfo() {
    return info;
}


/*** ATTACKSTRATEGY ***/

AttackStrategyOn2Or3Not5::AttackStrategyOn2Or3Not5(Time start_time, Time max_time)
    : current_time(start_time)
    , max_time(max_time)
    {
        assert(start_time >= 0);
        assert(start_time <= max_time);
    }

void AttackStrategyOn2Or3Not5::simulateAction(std::vector<std::shared_ptr<RebelStarship>> &rebel_ships,
                                              std::vector<std::shared_ptr<ImperialStarship>> &imperial_ships,
                                              Time time_step) {
    if (isAttackTime())
        conductAttack(rebel_ships, imperial_ships);
    current_time = (current_time + time_step) % (max_time + 1);
}

void AttackStrategyOn2Or3Not5::conductAttack(std::vector<std::shared_ptr<RebelStarship>> &rebel_ships,
                                              std::vector<std::shared_ptr<ImperialStarship>> &imperial_ships) {
    for (auto& imperial_ship : imperial_ships) {
        auto rebel_ship_it = rebel_ships.begin();

        while (!(*imperial_ship).isDestroyed() && rebel_ship_it != rebel_ships.end()) {
            if (!(*rebel_ship_it)->isDestroyed())
                (*imperial_ship).attack((*rebel_ship_it).get());
            rebel_ship_it++;
        }
    }
}

bool AttackStrategyOn2Or3Not5::isAttackTime() {
    return (current_time % 2 == 0 || current_time % 3 == 0) && current_time % 5 != 0;
}

std::shared_ptr<AttackStrategy> createAttackStrategyOn2Or3Not5(Time start_time, Time max_time) {
    return std::make_shared<AttackStrategyOn2Or3Not5>(start_time, max_time);
}

/*** BUILDER ***/

SpaceBattle::Builder& SpaceBattle::Builder::ship(const std::shared_ptr<RebelStarship>& ship) {
    rebel_ships.push_back(ship);

    return *this;
}

SpaceBattle::Builder& SpaceBattle::Builder::ship(const std::shared_ptr<ImperialStarship>& ship) {
    imperial_ships.push_back(ship);

    return *this;
}

SpaceBattle::Builder& SpaceBattle::Builder::startTime(Time start_time) {
    this->start_time = start_time;

    return *this;
}

SpaceBattle::Builder& SpaceBattle::Builder::maxTime(Time max_time) {
    this->max_time = max_time;

    return *this;
}

SpaceBattle::Builder& SpaceBattle::Builder::attackStrategy(std::shared_ptr<AttackStrategy> attack_strategy) {
    this->attack_strategy = std::move(attack_strategy);

    return *this;
}

SpaceBattle SpaceBattle::Builder::build() {
    if (attack_strategy == nullptr) // default attack_strategy
        attack_strategy = std::make_shared<AttackStrategyOn2Or3Not5>(start_time, max_time);

    return SpaceBattle(rebel_ships, imperial_ships, attack_strategy);
}