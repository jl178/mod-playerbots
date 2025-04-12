// RaidOnyxiaActions.h
#ifndef _PLAYERBOT_RAIDONYXIAACTIONS_H_
#define _PLAYERBOT_RAIDONYXIAACTIONS_H_

#include "Action.h"
#include "AttackAction.h"
#include "GenericSpellActions.h"
#include "MovementActions.h"

class PlayerbotAI;

class RaidOnyxiaMoveToSideAction : public MovementAction
{
public:
    RaidOnyxiaMoveToSideAction(PlayerbotAI* botAI, std::string const name = "ony move to side")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;
};

class RaidOnyxiaSpreadOutAction : public MovementAction
{
public:
    RaidOnyxiaSpreadOutAction(PlayerbotAI* botAI, std::string const name = "ony spread out")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;
};

class RaidOnyxiaMoveToSafeZoneAction : public MovementAction
{
public:
    RaidOnyxiaMoveToSafeZoneAction(PlayerbotAI* botAI, std::string const name = "ony move to safe zone")
        : MovementAction(botAI, name)
    {
    }
    bool Execute(Event event) override;

private:
    float GetBreathDirectionAngle(uint32 spellId)
    {
        switch (spellId)
        {
            case 17086:
                return 0.0f;  // N to S
            case 18351:
                return M_PI;  // S to N
            case 18576:
                return M_PI_2 * 3;  // E to W
            case 18609:
                return M_PI_2;  // W to E
            case 18564:
                return M_PI_4 * 5;  // SE to NW
            case 18584:
                return M_PI_4;  // NW to SE
            case 18596:
                return M_PI_4 * 7;  // SW to NE
            case 18617:
                return M_PI_4 * 3;  // NE to SW
            default:
                return -1.0f;  // Unknown
        }
    }
};

class RaidOnyxiaKillWhelpsAction : public AttackAction
{
public:
    RaidOnyxiaKillWhelpsAction(PlayerbotAI* botAI, std::string const name = "ony kill whelps")
        : AttackAction(botAI, name)
    {
    }

    bool Execute(Event event) override;
};

#endif
