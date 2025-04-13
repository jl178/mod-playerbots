#include "RaidOnyxiaTriggers.h"

#include "GenericTriggers.h"
#include "ObjectAccessor.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"
#include "strategy/values/NearestNpcsValue.h"

OnyxiaDeepBreathTrigger::OnyxiaDeepBreathTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony deep breath warning") {}

bool OnyxiaDeepBreathTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING))
        return false;

    // Check if Onyxia is casting
    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);

    if (!currentSpell)
        return false;

    uint32 spellId = currentSpell->m_spellInfo->Id;

    if (spellId == 17086 ||  // North to South
        spellId == 18351 ||  // South to North
        spellId == 18576 ||  // East to West
        spellId == 18609 ||  // West to East
        spellId == 18564 ||  // Southeast to Northwest
        spellId == 18584 ||  // Northwest to Southeast
        spellId == 18596 ||  // Southwest to Northeast
        spellId == 18617     // Northeast to Southwest
    )
    {
        return true;
    }

    return false;
}

OnyxiaNearTailTrigger::OnyxiaNearTailTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony near tail") {}

bool OnyxiaNearTailTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || botAI->IsTank(bot))
        return false;

    // Skip if Onyxia is in air or transitioning
    if (!boss->IsInCombat() || boss->IsFlying() || !boss->GetVictim())
        return false;

    return true;
}
RaidOnyxiaFireballSplashTrigger::RaidOnyxiaFireballSplashTrigger(PlayerbotAI* botAI)
    : Trigger(botAI, "ony fireball splash incoming")
{
}

bool RaidOnyxiaFireballSplashTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->HasUnitState(UNIT_STATE_CASTING))
        return false;

    // Check if Onyxia is casting Fireball
    Spell* currentSpell = boss->GetCurrentSpell(CURRENT_GENERIC_SPELL);
    if (!currentSpell || currentSpell->m_spellInfo->Id != 18392)  // 18392 is the classic Fireball ID
        return false;

    GuidVector nearbyUnits = AI_VALUE(GuidVector, "nearest friendly players");

    for (ObjectGuid guid : nearbyUnits)
    {
        Unit* unit = botAI->GetUnit(guid);
        if (!unit || unit == bot || !unit->IsAlive())
            continue;

        if (bot->GetDistance(unit) < 8.0f)
            return true;
    }

    return false;
}

RaidOnyxiaWhelpsSpawnTrigger::RaidOnyxiaWhelpsSpawnTrigger(PlayerbotAI* botAI) : Trigger(botAI, "ony whelps spawn") {}

bool RaidOnyxiaWhelpsSpawnTrigger::IsActive()
{
    Unit* boss = AI_VALUE2(Unit*, "find target", "onyxia");
    if (!boss || !boss->IsFlying())
        return false;

    Unit* victim = bot->GetVictim();
    if (victim && victim->GetEntry() == 11262 && victim->IsAlive())
        return false;  // Already attacking whelp

    GuidVector npcs = AI_VALUE(GuidVector, "nearest hostile npcs");

    for (ObjectGuid guid : npcs)
    {
        if (!guid || !guid.IsCreature())
            continue;

        Creature* unit = botAI->GetCreature(guid);
        if (!unit || !unit->IsAlive())
            continue;

        if (unit->GetEntry() == 11262)
        {
            // DPS and tanks attack whelps
            return botAI->IsDps(bot) || botAI->IsTank(bot);
        }
    }

    return false;
}
