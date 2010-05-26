/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss Northrend Beasts
SD%Complete: 10
SDComment: vehicle support is needed to properly implement the 'throw snobold' event
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_gormok
boss_acidmaw
boss_dreadscale
boss_icehowl
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum GormokSpells
{
    // Gormok
    SPELL_IMPALE_N10            = 66331,    // "Requires Polearms"
    SPELL_IMPALE_N25            = 67477,
    SPELL_IMPALE_H10            = 67478,
    SPELL_IMPALE_H25            = 67479,
    SPELL_STAGGERING_STOMP_N10  = 66330,
    SPELL_STAGGERING_STOMP_N25  = 67647,
    SPELL_STAGGERING_STOMP_H10  = 67648,
    SPELL_STAGGERING_STOMP_H25  = 67649,
    // Snobold Vassal
    SPELL_SNOBOLLED             = 66406,    // dummy effect, target vehicle
    SPELL_RISING_ANGER          = 66636,    // target Gormok
    SPELL_BATTER                = 66408,
    SPELL_FIRE_BOMB             = 66313,
    SPELL_HEAD_CRACK            = 66407,
};

enum GormokAdds
{
    NPC_SNOBOLD_VASSAL          = 34800,
};

enum GormokEvents
{
    // Gormok
    EVENT_IMPALE = 1,
    EVENT_STAGGERING_STOMP,
    EVENT_THROW_SNOBOLD,
    // Snobold Vassal
    EVENT_BATTER,
    EVENT_FIRE_BOMB,
    EVENT_HEAD_CRACK,
};

#define TIMER_IMPALE            10*IN_MILLISECONDS
#define TIMER_STAGGERING_STOMP  urand(20, 25)*IN_MILLISECONDS
#define TIMER_THROW_SNOBOLD     urand(20, 45)*IN_MILLISECONDS

#define MAX_SNOBOLD             5

struct MANGOS_DLL_DECL boss_gormokAI: public boss_trial_of_the_crusaderAI
{
    uint32 m_uiSnoboldCount;

    typedef std::list<uint64> GuidList;
    GuidList m_Snobolds;

    boss_gormokAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        m_uiSnoboldCount(0)
    {
    }

    void Reset()
    {
        DespawnSummons();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        Events.RescheduleEvent(EVENT_IMPALE, TIMER_IMPALE);
        Events.RescheduleEvent(EVENT_STAGGERING_STOMP, TIMER_STAGGERING_STOMP);
        Events.RescheduleEvent(EVENT_THROW_SNOBOLD, TIMER_THROW_SNOBOLD);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon && pSummon->GetEntry() == NPC_SNOBOLD_VASSAL)
        {
            m_Snobolds.push_back(pSummon->GetGUID());
            pSummon->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_IMPALE:
                    DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_IMPALE));
                    Events.ScheduleEvent(EVENT_IMPALE, TIMER_IMPALE);
                    break;
                case EVENT_STAGGERING_STOMP:
                    DoCast(m_creature, DIFFICULTY(SPELL_STAGGERING_STOMP));
                    Events.ScheduleEvent(EVENT_STAGGERING_STOMP, TIMER_STAGGERING_STOMP);
                    break;
                case EVENT_THROW_SNOBOLD:
                    DoSpawnCreature(NPC_SNOBOLD_VASSAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                    if (++m_uiSnoboldCount < MAX_SNOBOLD)
                        Events.ScheduleEvent(EVENT_THROW_SNOBOLD, TIMER_THROW_SNOBOLD);
                    break;
                default:
                    break;
            }
        DoMeleeAttackIfReady();
    }

    void DespawnSummons()
    {
        for (GuidList::const_iterator i = m_Snobolds.begin(); i != m_Snobolds.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        m_Snobolds.clear();
        m_uiSnoboldCount = 0;
    }
};

enum JormungarSpells
{
    // Acidmaw
    SPELL_ACIDIC_SPEW           = 66818,    //triggers 66819, there is 67609, 67610, 67611 related and currently unused, maybe core should auto-select them?
    SPELL_PARALYTIC_BITE_N10    = 66824,
    SPELL_PARALYTIC_BITE_N25    = 67612,
    SPELL_PARALYTIC_BITE_H10    = 67613,
    SPELL_PARALYTIC_BITE_H25    = 67614,
    SPELL_ACID_SPIT_N10         = 66880,
    SPELL_ACID_SPIT_N25         = 67606,
    SPELL_ACID_SPIT_H10         = 67607,
    SPELL_ACID_SPIT_H25         = 67608,
    SPELL_PARALYTIC_SPRAY_N10   = 66901,
    SPELL_PARALYTIC_SPRAY_N25   = 67615,
    SPELL_PARALYTIC_SPRAY_H10   = 67616,
    SPELL_PARALYTIC_SPRAY_H25   = 67617,
    // Dreadscale
    SPELL_MOLTEN_SPEW           = 66821,    //triggers 66820, related and unused 67635, 67636, 67637
    SPELL_BURNING_BITE_N10      = 66879,
    SPELL_BURNING_BITE_N25      = 67624,
    SPELL_BURNING_BITE_H10      = 67625,
    SPELL_BURNING_BITE_H25      = 67626,
    SPELL_FIRE_SPIT_N10         = 66796,
    SPELL_FIRE_SPIT_N25         = 67632,
    SPELL_FIRE_SPIT_H10         = 67633,
    SPELL_FIRE_SPIT_H25         = 67634,
    SPELL_BURNING_SPRAY_N10     = 66902,
    SPELL_BURNING_SPRAY_N25     = 67627,
    SPELL_BURNING_SPRAY_H10     = 67628,
    SPELL_BURNING_SPRAY_H25     = 67629,
    // all bosses
    SPELL_SLIME_POOL_N10        = 66883,
    SPELL_SLIME_POOL_N25        = 67641,
    SPELL_SLIME_POOL_H10        = 67642,
    SPELL_SLIME_POOL_H25        = 67643,
    SPELL_SWEEP_N10             = 66794,
    SPELL_SWEEP_N25             = 67644,
    SPELL_SWEEP_H10             = 67645,
    SPELL_SWEEP_H25             = 67646,
    SPELL_ENRAGE                = 68335,
    SPELL_SUBMERGE              = 56422, //i'm using Nerubian Submerge here, don't know correct spell
    SPELL_EMERGE                = 65982, //same here, using another spell till i understand what is wrong 66947, 66949
};

enum JormungarDisplayId
{
    DISPLAY_ROOTED              = 29815,
    DISPLAY_MOBILE              = 24564,
};

enum JormungarEvents
{
    EVENT_PHASE_SWITCH = 1,     // mobile <-> rooted
    EVENT_SPEW,                 // mobile
    EVENT_BITE,                 // mobile
    EVENT_SPIT,                 // rooted
    EVENT_SPRAY,                // rooted
    EVENT_SLIME_POOL,           // mobile
    EVENT_SWEEP,                // rooted
    EVENT_MOVE_UNDERGROUND,     // underground, after submerging
};

enum JormungarPhases
{
    PHASE_ROOTED = 1,
    PHASE_ON_GROUND,
};

static const float RoomCenter[] = { 563.67f, 139.57f, 393.83f };

#define PHASE_TIMER 45*IN_MILLISECONDS
#define SPEW_TIMER  urand(10, 20)*IN_MILLISECONDS
#define BITE_TIMER  urand(10, 15)*IN_MILLISECONDS
#define SPIT_TIMER  1200
#define SPRAY_TIMER urand(10, 20)*IN_MILLISECONDS
#define SLIME_TIMER urand(10, 15)*IN_MILLISECONDS
#define SWEEP_TIMER urand(15, 20)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_acidmawAI: public boss_trial_of_the_crusaderAI
{
    boss_acidmawAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    bool m_bIsRooted;
    uint32 m_uiStep;

    void Aggro(Unit *pWho)
    {
        m_uiStep = 0;
        m_bIsRooted = true;
        Events.SetPhase(PHASE_ROOTED);
        Events.RescheduleEvent(EVENT_SPIT,  SPIT_TIMER,  0, PHASE_ROOTED);
        Events.RescheduleEvent(EVENT_SPRAY, SPRAY_TIMER, 0, PHASE_ROOTED);
        Events.RescheduleEvent(EVENT_SWEEP, SWEEP_TIMER, 0, PHASE_ROOTED);
        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER);
        SetCombatMovement(false);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void JustDied(Unit* pSlayer)
    {
        Unit* pDreadScale = GET_CREATURE(TYPE_DREADSCALE);
        if (pDreadScale && pDreadScale->isAlive())
        {
            pDreadScale->CastSpell(pDreadScale, SPELL_ENRAGE, true);
            pDreadScale->MonsterTextEmote("As its companion perishes, Dreadscale becomes Enraged!", pDreadScale->GetGUID(), true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_PHASE_SWITCH:
                    //submerge
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (m_bIsRooted)
                    {
                        Events.Reset();
                        Events.SetPhase(PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER + 10*IN_MILLISECONDS);
                        Events.ScheduleEvent(EVENT_SPEW,  SPEW_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_SLIME_POOL, SLIME_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_BITE,  BITE_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ON_GROUND);
                        m_bIsRooted = false;
                    }
                    else
                    {
                        Events.Reset();
                        Events.SetPhase(PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND,0);
                        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER + 10*IN_MILLISECONDS);
                        Events.ScheduleEvent(EVENT_SPIT,  SPIT_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_SPRAY, SPRAY_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_SWEEP, SWEEP_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ROOTED);
                        m_bIsRooted = true;
                    }
                    break;
                case EVENT_SPEW:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_ACIDIC_SPEW, CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SPEW, SPEW_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_BITE:
                    m_creature->CastSpell(m_creature->getVictim(), DIFFICULTY(SPELL_PARALYTIC_BITE), false);
                    Events.RescheduleEvent(EVENT_BITE, BITE_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_SPIT:
                    m_creature->CastSpell(m_creature->getVictim(), DIFFICULTY(SPELL_ACID_SPIT), false);
                    Events.RescheduleEvent(EVENT_SPIT, SPIT_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_SPRAY:
                    if (Unit* pTarget = m_creature->SelectRandomUnfriendlyTarget(NULL, 50.0f))
                        m_creature->CastSpell(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), DIFFICULTY(SPELL_PARALYTIC_SPRAY), true);
                    Events.RescheduleEvent(EVENT_SPRAY, SPRAY_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_SLIME_POOL:
                    DoCastSpellIfCan(m_creature, DIFFICULTY(SPELL_SLIME_POOL), CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SLIME_POOL, SLIME_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_SWEEP:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SWEEP), CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SWEEP, SWEEP_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_MOVE_UNDERGROUND:
                {
                    switch (m_uiStep)
                    {
                        case 0:
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->InterruptSpell(CURRENT_GENERIC_SPELL, false);
                            m_creature->StopMoving();
                            m_creature->RemoveAllAttackers();
                            m_creature->CastSpell(m_creature, SPELL_SUBMERGE, false);
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 1000);
                            m_uiStep++;
                            break;
                        case 1:
                            m_creature->GetMotionMaster()->MovePoint(NULL, RoomCenter[0]+(rand()%80 - 40), RoomCenter[1]+(rand()%80 - 40), RoomCenter[2]);
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 7500);
                            m_uiStep++;
                            break;
                        case 2:
                            m_creature->SetDisplayId(m_bIsRooted ? DISPLAY_ROOTED : DISPLAY_MOBILE);
                            m_creature->StopMoving();
                            m_uiStep++;
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 500);
                            break;
                        case 3: //emerge
                            m_creature->CastSpell(m_creature, SPELL_EMERGE, true);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_uiStep++;
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 1500);
                            break;
                        case 4: 
                            if (m_bIsRooted)
                                DoStartNoMovement(m_creature->getVictim());
                            else 
                                DoStartMovement(m_creature->getVictim());
                            m_uiStep = 0;
                            break;
                    }
                    break;
                }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_dreadscaleAI: public boss_trial_of_the_crusaderAI
{
    boss_dreadscaleAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    bool m_bIsRooted;
    uint8 m_uiStep;

    void Aggro(Unit *pWho)
    {
        m_uiStep = 0;
        m_bIsRooted = false;
        Events.SetPhase(PHASE_ON_GROUND);
        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER);
        Events.ScheduleEvent(EVENT_SPEW, SPEW_TIMER, 0, PHASE_ON_GROUND);
        Events.ScheduleEvent(EVENT_BITE, BITE_TIMER, 0, PHASE_ON_GROUND);
        Events.ScheduleEvent(EVENT_SLIME_POOL, SLIME_TIMER, 0, PHASE_ON_GROUND);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void JustDied(Unit* pSlayer)
    {
        Unit* pAcidMaw = GET_CREATURE(TYPE_ACIDMAW);
        if (pAcidMaw && pAcidMaw->isAlive())
        {
            pAcidMaw->CastSpell(pAcidMaw, SPELL_ENRAGE, true);
            pAcidMaw->MonsterTextEmote("As its companion perishes, Acidmaw becomes Enraged!", NULL, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_PHASE_SWITCH:
                    //submerge
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (m_bIsRooted)
                    {
                        Events.Reset();
                        Events.SetPhase(PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER + 10*IN_MILLISECONDS);
                        Events.ScheduleEvent(EVENT_SPEW,  SPEW_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_SLIME_POOL, SLIME_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ON_GROUND);
                        Events.ScheduleEvent(EVENT_BITE,  BITE_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ON_GROUND);
                        DoStartMovement(m_creature->getVictim());
                        m_bIsRooted = false;
                    }
                    else
                    {
                        Events.Reset();
                        Events.SetPhase(PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND,0);
                        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER + 10*IN_MILLISECONDS);
                        Events.ScheduleEvent(EVENT_SPIT,  SPIT_TIMER + 10*IN_MILLISECONDS,  0, PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_SPRAY, SPRAY_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ROOTED);
                        Events.ScheduleEvent(EVENT_SWEEP, SWEEP_TIMER + 10*IN_MILLISECONDS, 0, PHASE_ROOTED);
                        DoStartNoMovement(m_creature->getVictim());
                        m_bIsRooted = true;
                    }
                    break;
                case EVENT_SPEW:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOLTEN_SPEW, CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SPEW, SPEW_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_BITE:
                    m_creature->CastSpell(m_creature->getVictim(), DIFFICULTY(SPELL_BURNING_BITE), false);
                    Events.RescheduleEvent(EVENT_BITE, BITE_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_SPIT:
                    m_creature->CastSpell(m_creature->getVictim(), DIFFICULTY(SPELL_FIRE_SPIT), false);
                    Events.RescheduleEvent(EVENT_SPIT, SPIT_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_SPRAY:
                    if (Unit* pTarget = m_creature->SelectRandomUnfriendlyTarget(NULL, 50.0f))
                         m_creature->CastSpell(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), DIFFICULTY(SPELL_BURNING_SPRAY), true);
                    Events.RescheduleEvent(EVENT_SPRAY, SPRAY_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_SLIME_POOL:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SLIME_POOL), CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SLIME_POOL, SLIME_TIMER, 0, PHASE_ON_GROUND);
                    break;
                case EVENT_SWEEP:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SWEEP), CAST_TRIGGERED);
                    Events.RescheduleEvent(EVENT_SWEEP, SWEEP_TIMER, 0, PHASE_ROOTED);
                    break;
                case EVENT_MOVE_UNDERGROUND:
                {
                    switch (m_uiStep)
                    {
                        case 0:
                            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_creature->InterruptSpell(CURRENT_GENERIC_SPELL, false);
                            m_creature->StopMoving();
                            m_creature->RemoveAllAttackers();
                            m_creature->CastSpell(m_creature, SPELL_SUBMERGE, false);
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 1000);
                            m_uiStep++;
                            break;
                        case 1:
                            m_creature->GetMotionMaster()->MovePoint(NULL, RoomCenter[0]+(rand()%80 - 40), RoomCenter[1]+(rand()%80 - 40), RoomCenter[2]);
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 7500);
                            m_uiStep++;
                            break;
                        case 2:
                            m_creature->SetDisplayId(m_bIsRooted ? DISPLAY_ROOTED : DISPLAY_MOBILE);
                            m_creature->StopMoving();
                            m_uiStep++;
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 500);
                            break;
                        case 3: //emerge
                            m_creature->CastSpell(m_creature, SPELL_EMERGE, true);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            m_uiStep++;
                            Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 1500);
                            break;
                        case 4: 
                            if (m_bIsRooted)
                                DoStartNoMovement(m_creature->getVictim());
                            else 
                                DoStartMovement(m_creature->getVictim());
                            m_uiStep = 0;
                            break;
                    }
                    break;
                }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

enum IcehowlSpells
{
    SPELL_BERSERK               = 26662,
    SPELL_FEROCIOUS_BUTT_N10    = 66770,
    SPELL_FEROCIOUS_BUTT_N25    = 67654,
    SPELL_FEROCIOUS_BUTT_H10    = 67655,
    SPELL_FEROCIOUS_BUTT_H25    = 67656,
    SPELL_ARCTIC_BREATH_N10     = 66689,
    SPELL_ARCTIC_BREATH_N25     = 67650,
    SPELL_ARCTIC_BREATH_H10     = 67651,
    SPELL_ARCTIC_BREATH_H25     = 67652,
    SPELL_WHIRL_N10             = 67345,
    SPELL_WHIRL_N25             = 67663,
    SPELL_WHIRL_H10             = 67664,
    SPELL_WHIRL_H25             = 67665,
    SPELL_MASSIVE_CRASH_N10     = 66683,
    SPELL_MASSIVE_CRASH_N25     = 67660,
    SPELL_MASSIVE_CRASH_H10     = 67661,
    SPELL_MASSIVE_CRASH_H25     = 67662,
    SPELL_FROTHING_RAGE_N10     = 66759,
    SPELL_FROTHING_RAGE_N25     = 67657,
    SPELL_FROTHING_RAGE_H10     = 67658,
    SPELL_FROTHING_RAGE_H25     = 67659,
    SPELL_TRAMPLE               = 66734,    // just the dmg part
    SPELL_STAGGERED_DAZE        = 66758,    // part of the Trample effect
};

enum IcehowlEvents
{
    EVENT_BERSERK = 1,
};

#define TIMER_BERSERK           15*MINUTE*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_icehowlAI: public boss_trial_of_the_crusaderAI
{
    boss_icehowlAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        Events.RescheduleEvent(EVENT_BERSERK, TIMER_BERSERK);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_northrend_beasts()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_gormok);
    REGISTER_SCRIPT(boss_acidmaw);
    REGISTER_SCRIPT(boss_dreadscale);
    REGISTER_SCRIPT(boss_icehowl);
}
