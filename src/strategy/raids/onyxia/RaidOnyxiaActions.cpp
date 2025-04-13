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

        bot->Yell("Too close to front or tail — moving to side of Onyxia!", LANG_UNIVERSAL);
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

    bot->Yell("Spreading out — I'm the Fireball target!", LANG_UNIVERSAL);
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
    float angle = GetBreathDirectionAngle(spellId);
    Position bossPos = boss->GetPosition();

    // Move perpendicular to breath (left or right randomly)
    float sideOffset = M_PI_2 * (urand(0, 1) ? 1 : -1);
    float safeAngle = angle + sideOffset;

    float distance = 30.0f + urand(0, 10);  // Randomize for spread
    float safeX = bot->GetPositionX() + distance * cos(safeAngle);
    float safeY = bot->GetPositionY() + distance * sin(safeAngle);
    float safeZ = bot->GetPositionZ();  // Stay on ground hopefully?

    bot->Yell("Moving to Safe Zone!", LANG_UNIVERSAL);
    return MoveTo(boss->GetMapId(), safeX, safeY, safeZ, false, false, false, false, MovementPriority::MOVEMENT_COMBAT);
    return true;
}

bool RaidOnyxiaKillWhelpsAction::Execute(Event event)
{
    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (ObjectGuid guid : npcs)
    {
        if (!guid.IsCreature())
            continue;

        Creature* creature = botAI->GetCreature(guid);
        if (!creature || !creature->IsAlive() || !creature->IsInWorld())
            continue;

        if (creature->GetEntry() == 11262)
        {
            bot->Yell("ATTACKING WHELPS!!! AHHHHH", LANG_UNIVERSAL);
            context->GetValue<Unit*>("current target")->Set(creature->ToUnit());
            return Attack(creature->ToUnit());
        }
    }

    return false;
}
