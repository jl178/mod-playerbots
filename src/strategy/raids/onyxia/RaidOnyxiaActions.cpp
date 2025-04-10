// RaidOnyxiaActions.cpp
#include "RaidOnyxiaActions.h"

#include "GenericSpellActions.h"
#include "LastMovementValue.h"
#include "MovementActions.h"
#include "Playerbots.h"
#include "PositionAction.h"

bool RaidOnyxiaMoveToSideAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    float angleToBot = boss->GetAngle(bot);
    float bossFacing = boss->GetOrientation();
    float diff = fabs(angleToBot - bossFacing);
    if (diff > M_PI)
        diff = 2 * M_PI - diff;

    float distance = bot->GetDistance(boss);

    // Too close (30 yards) and either in front or behind
    if (distance <= 30.0f && (diff < M_PI / 4 || diff > 3 * M_PI / 4))
    {
        float offsetAngle = bossFacing + M_PI_2;  // 90° to the right
        float offsetDist = 15.0f;

        float sideX = boss->GetPositionX() + offsetDist * cos(offsetAngle);
        float sideY = boss->GetPositionY() + offsetDist * sin(offsetAngle);

        // bot->Yell("Too close to front or tail — moving to side of Onyxia!", LANG_UNIVERSAL);
        return MoveTo(boss->GetMapId(), sideX, sideY, boss->GetPositionZ(), false, false, false, false,
                      MovementPriority::MOVEMENT_COMBAT);
    }

    return false;
}

bool RaidOnyxiaSpreadOutAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");

    if (!boss)
        return false;

    Player* target = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_targets.GetUnitTarget()->ToPlayer();
    if (target != bot)
        return false;

    // bot->Yell("Spreading out — I'm the Fireball target!", LANG_UNIVERSAL);
    return MoveFromGroup(9.0f);  // move 9 yards
}

bool RaidOnyxiaMoveToSafeZoneAction::Execute(Event event)
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss)
        return false;

    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell)
        return false;

    uint32 spellId = currentSpell->m_spellInfo->Id;
    Position bossPos = boss->GetPosition();

    float safeX = bossPos.GetPositionX();
    float safeY = bossPos.GetPositionY();
    constexpr float OFFSET = 30.0f;

    switch (spellId)
    {
        case 17086:
        case 18351:  // N <-> S
            safeX += urand(0, 1) ? OFFSET : -OFFSET;
            break;
        case 18576:
        case 18609:  // E <-> W
            safeY += urand(0, 1) ? OFFSET : -OFFSET;
            break;
        default:
            safeX += OFFSET * cos(boss->GetOrientation() + M_PI_2);
            safeY += OFFSET * sin(boss->GetOrientation() + M_PI_2);
            break;
    }

    float groundZ = bot->GetMap()->GetHeight(bot->GetPhaseMask(), safeX, safeY, bossPos.GetPositionZ());

    bot->Yell("Moving to Safe Zone!", LANG_UNIVERSAL);
    return MoveTo(boss->GetMapId(), safeX, safeY, groundZ, false, false, false, false,
                  MovementPriority::MOVEMENT_COMBAT);
}
