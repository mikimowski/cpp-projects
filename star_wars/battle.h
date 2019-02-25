#ifndef STARWARS_BATTLE_H
#define STARWARS_BATTLE_H

#include "imperialfleet.h"
#include "rebelfleet.h"

/*** ATTACKSTRATEGY ***/
class AttackStrategy {
public:
    AttackStrategy() = default;
    virtual ~AttackStrategy() = default;

    virtual void simulateAction(std::vector<std::shared_ptr<RebelStarship>>& rebel_ships,
                                std::vector<std::shared_ptr<ImperialStarship>>& imperial_ships,
                                Time time_step) = 0;
};


/*** ATTACKSTRATEGYON2OR3NOT5 ***/
class AttackStrategyOn2Or3Not5 : public AttackStrategy {
protected:
    Time current_time;
    Time max_time;

public:
    AttackStrategyOn2Or3Not5(Time start_time, Time max_time);

    void simulateAction(std::vector<std::shared_ptr<RebelStarship>>& rebel_ships,
                        std::vector<std::shared_ptr<ImperialStarship>>& imperial_ships,
                        Time time_step) override;

    bool isAttackTime();

    void conductAttack(std::vector<std::shared_ptr<RebelStarship>>& rebel_ships,
            std::vector<std::shared_ptr<ImperialStarship>>& imperial_ships);

};

/// Fabricating function
std::shared_ptr<AttackStrategy> createAttackStrategyOn2Or3Not5(Time start_time, Time max_time);

/*** SPACEBATTLE ***/

class SpaceBattle {
public:
    class BattleStats {
        size_t cnt_rebel_ships;
        size_t cnt_imperial_ships;
        bool ongoing;
        std::string info;

    public:
        BattleStats(size_t cnt_rebel_ships, size_t cnt_imperial_ships);

        void setOngoing(bool val);
        void setInfo(const std::string& new_info);
        void setCntRebelShips(size_t new_amount);
        void setCntImperialShips(size_t new_amount);

        size_t getCntRebelFleet();
        size_t getCntImperialFleet();
        const std::string& getInfo();

        bool isOngoing();
    };

protected:
    std::vector<std::shared_ptr<RebelStarship>> rebel_ships;
    std::vector<std::shared_ptr<ImperialStarship>> imperial_ships;
    std::shared_ptr<AttackStrategy> attack_strategy;
    BattleStats battle_stats;

    size_t recountRebelFleet();
    size_t recountImperialFleet();
    void updateBattleStats();

    /**
    * Method tries to settle the battle.
    * Checks whether the battle can be ended - at least one of the sides lost all its ships.
    * If so sets info and ends the battle.
    */
    void tryToSettle();

public:
    SpaceBattle(std::vector<std::shared_ptr<RebelStarship>>& rebel_ships,
            std::vector<std::shared_ptr<ImperialStarship>>& imperial_ships,
            std::shared_ptr<AttackStrategy>& attack_strategy);

    void tick(Time time_step);
    size_t countRebelFleet();
    size_t countImperialFleet();

    class Builder {
        std::vector<std::shared_ptr<RebelStarship>> rebel_ships;
        std::vector<std::shared_ptr<ImperialStarship>> imperial_ships;
        Time start_time = 0;
        Time max_time = 25;
        std::shared_ptr<AttackStrategy> attack_strategy = nullptr;

    public:
        Builder() = default;

        /// Adds ship to rebel's ships.
        Builder& ship(const std::shared_ptr<RebelStarship>& ship);
        /// Adds ship to imperial's ships.
        Builder& ship(const std::shared_ptr<ImperialStarship>& ship);
        /// Sets start_time of the battle.
        Builder& startTime(Time start_time);
        /// Sets max_time of the battle.
        Builder& maxTime(Time max_time);
        /// Sets attack_strategy for the battle.
        Builder& attackStrategy(std::shared_ptr<AttackStrategy> attack_strategy);

        SpaceBattle build();
    };
};



#endif //STARWARS_BATTLE_H
