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
SD%Complete: 85%
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

/*######
## boss_gormok
######*/

enum GormokSpells
{
    // Gormok
    SPELL_IMPALE                = 66331,    // "Requires Polearms"
    SPELL_STAGGERING_STOMP      = 66330,
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
#define TIMER_STAGGERING_STOMP  20*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define TIMER_THROW_SNOBOLD     20*IN_MILLISECONDS, 45*IN_MILLISECONDS

#define MAX_SNOBOLD             5

struct MANGOS_DLL_DECL boss_gormokAI: public boss_trial_of_the_crusaderAI
{
    SummonManager SummonMgr;
    uint32 m_uiSnoboldCount;

    boss_gormokAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        SummonMgr(pCreature),
        m_uiSnoboldCount(0)
    {
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        m_uiSnoboldCount = 0;
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        Events.ScheduleEvent(EVENT_IMPALE, TIMER_IMPALE, TIMER_IMPALE);
        Events.ScheduleEventInRange(EVENT_STAGGERING_STOMP, TIMER_STAGGERING_STOMP, TIMER_STAGGERING_STOMP);
        Events.ScheduleEventInRange(EVENT_THROW_SNOBOLD, TIMER_THROW_SNOBOLD, TIMER_THROW_SNOBOLD);
        m_BossEncounter = IN_PROGRESS;
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon)
            pSummon->SetInCombatWithZone();
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
                    DoCast(m_creature->getVictim(), SPELL_IMPALE);
                    break;
                case EVENT_STAGGERING_STOMP:
                    DoCast(m_creature, SPELL_STAGGERING_STOMP);
                    break;
                case EVENT_THROW_SNOBOLD:
                    SummonMgr.SummonCreatureAt(m_creature, NPC_SNOBOLD_VASSAL, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                    DoCast(m_creature, SPELL_RISING_ANGER);
                    if (++m_uiSnoboldCount >= MAX_SNOBOLD)
                        Events.CancelEvent(EVENT_THROW_SNOBOLD);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

#define TIMER_BATTER        10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_FIRE_BOMB     20*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define TIMER_HEAD_CRACK    30*IN_MILLISECONDS, 45*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_snobold_vassalAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance* m_pInstance;
    InstanceVar<uint32> m_AchievementCounter;

    mob_snobold_vassalAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_AchievementCounter(DATA_ACHIEVEMENT_COUNTER, m_pInstance)
    {
    }

    void Reset()
    {
    }

    void Aggro(Unit *pWho)
    {
        Events.RescheduleEventInRange(EVENT_BATTER, TIMER_BATTER, TIMER_BATTER);
        Events.RescheduleEventInRange(EVENT_FIRE_BOMB, TIMER_FIRE_BOMB, TIMER_FIRE_BOMB);
        Events.RescheduleEventInRange(EVENT_HEAD_CRACK, TIMER_HEAD_CRACK, TIMER_HEAD_CRACK);
        ++m_AchievementCounter;
    }

    void JustDied(Unit* pSlayer)
    {
        --m_AchievementCounter;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {   
                case EVENT_BATTER:
                    DoCast(m_creature->getVictim(), SPELL_BATTER);
                    break;
                case EVENT_FIRE_BOMB:
                    DoCast(m_creature->getVictim(), SPELL_FIRE_BOMB);
                    break;
                case EVENT_HEAD_CRACK:
                    DoCast(m_creature->getVictim(), SPELL_HEAD_CRACK);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_acidmaw
######*/

enum JormungarSpells
{
    // Acidmaw
    SPELL_ACIDIC_SPEW           = 66818,    //triggers 66819, there is 67609, 67610, 67611 related and currently unused, maybe core should auto-select them?
    SPELL_PARALYTIC_BITE        = 66824,
    SPELL_ACID_SPIT             = 66880,
    SPELL_PARALYTIC_SPRAY       = 66901,
    // Dreadscale
    SPELL_MOLTEN_SPEW           = 66821,    //triggers 66820, related and unused 67635, 67636, 67637
    SPELL_BURNING_BITE          = 66879,
    SPELL_FIRE_SPIT             = 66796,
    SPELL_BURNING_SPRAY         = 66902,
    // all bosses
    SPELL_SLIME_POOL            = 66883,
    SPELL_SWEEP                 = 66794,
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
    PHASE_ROOTED    = 1,
    PHASE_ON_GROUND,
    PMASK_ROOTED    = bit_mask<PHASE_ROOTED>::value,
    PMASK_ON_GROUND = bit_mask<PHASE_ON_GROUND>::value,
};

static const float RoomCenter[] = { 563.67f, 139.57f, 393.83f };

#define PHASE_TIMER 45*IN_MILLISECONDS
// rooted
#define SPIT_TIMER  1200
#define SPRAY_TIMER 10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define SWEEP_TIMER 15*IN_MILLISECONDS, 20*IN_MILLISECONDS
// movile
#define SPEW_TIMER  10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define SLIME_TIMER 15*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define BITE_TIMER  15*IN_MILLISECONDS, 20*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_acidmawAI: public boss_trial_of_the_crusaderAI
{
    bool m_bIsRooted;
    uint32 m_uiStep;

    boss_acidmawAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        m_creature->CastSpell(m_creature, SPELL_EMERGE, false);
        m_uiStep = 0;
        m_bIsRooted = true;
        Events.SetPhase(PHASE_ROOTED);
        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER, PHASE_TIMER + 10*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SPIT,  SPIT_TIMER,  SPIT_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SPRAY, SPRAY_TIMER, SPRAY_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SWEEP, SWEEP_TIMER, SWEEP_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SPEW,  SPEW_TIMER,  SPEW_TIMER, 0, 0, PMASK_ON_GROUND);
        Events.ScheduleEventInRange(EVENT_SLIME_POOL, SLIME_TIMER, SLIME_TIMER, 0, 0, PMASK_ON_GROUND);
        Events.ScheduleEventInRange(EVENT_BITE,  BITE_TIMER,  BITE_TIMER, 0, 0, PMASK_ON_GROUND);
        SetCombatMovement(false);
        m_BossEncounter = IN_PROGRESS;
    }

    void JustDied(Unit* pSlayer)
    {
        Unit* pDreadScale = GET_CREATURE(TYPE_DREADSCALE);
        if (pDreadScale && pDreadScale->isAlive())
        {
            pDreadScale->CastSpell(pDreadScale, SPELL_ENRAGE, true);
            pDreadScale->MonsterTextEmote("As its companion perishes, Dreadscale becomes Enraged!", NULL, true);
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
                        Events.SetPhase(PHASE_ON_GROUND);
                        Events.DelayEvents(10*IN_MILLISECONDS, PMASK_ON_GROUND);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        m_bIsRooted = false;
                    }
                    else
                    {
                        Events.SetPhase(PHASE_ROOTED);
                        Events.DelayEvents(10*IN_MILLISECONDS, PMASK_ROOTED);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        m_bIsRooted = true;
                    }
                    break;
                case EVENT_SPEW:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_ACIDIC_SPEW, CAST_TRIGGERED);
                    break;
                case EVENT_BITE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_PARALYTIC_BITE, false);
                    break;
                case EVENT_SPIT:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_ACID_SPIT, false);
                    break;
                case EVENT_SPRAY:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), SPELL_PARALYTIC_SPRAY, true);
                    break;
                case EVENT_SLIME_POOL:
                    DoCastSpellIfCan(m_creature, SPELL_SLIME_POOL, CAST_TRIGGERED);
                    break;
                case EVENT_SWEEP:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWEEP, CAST_TRIGGERED);
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
                            m_creature->GetMotionMaster()->MovePoint(0, RoomCenter[0]+(rand()%80 - 40), RoomCenter[1]+(rand()%80 - 40), RoomCenter[2]);
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

        if (!m_uiStep) //prevent melee attacks from underground
            DoMeleeAttackIfReady();
    }
};

/*######
## boss_dreadscale
######*/

struct MANGOS_DLL_DECL boss_dreadscaleAI: public boss_trial_of_the_crusaderAI
{
    bool m_bIsRooted;
    uint32 m_uiStep;

    boss_dreadscaleAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        m_uiStep = 0;
        m_bIsRooted = false;
        Events.SetPhase(PHASE_ON_GROUND);
        Events.ScheduleEvent(EVENT_PHASE_SWITCH, PHASE_TIMER, PHASE_TIMER + 10*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SPIT,  SPIT_TIMER,  SPIT_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SPRAY, SPRAY_TIMER, SPRAY_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SWEEP, SWEEP_TIMER, SWEEP_TIMER, 0, 0, PMASK_ROOTED);
        Events.ScheduleEventInRange(EVENT_SPEW, SPEW_TIMER, SPEW_TIMER, 0, 0, PMASK_ON_GROUND);
        Events.ScheduleEventInRange(EVENT_SLIME_POOL, SLIME_TIMER, SLIME_TIMER, 0, 0, PMASK_ON_GROUND);
        Events.ScheduleEventInRange(EVENT_BITE, BITE_TIMER, BITE_TIMER, 0, 0, PMASK_ON_GROUND);
        m_BossEncounter = IN_PROGRESS;
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
                        Events.SetPhase(PHASE_ON_GROUND);
                        Events.DelayEvents(10*IN_MILLISECONDS, PMASK_ON_GROUND);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        DoStartMovement(m_creature->getVictim());
                        m_bIsRooted = false;
                    }
                    else
                    {
                        Events.SetPhase(PHASE_ROOTED);
                        Events.DelayEvents(10*IN_MILLISECONDS, PMASK_ROOTED);
                        Events.ScheduleEvent(EVENT_MOVE_UNDERGROUND, 0);
                        DoStartNoMovement(m_creature->getVictim());
                        m_bIsRooted = true;
                    }
                    break;
                case EVENT_SPEW:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOLTEN_SPEW, CAST_TRIGGERED);
                    break;
                case EVENT_BITE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_BURNING_BITE, false);
                    break;
                case EVENT_SPIT:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_FIRE_SPIT, false);
                    break;
                case EVENT_SPRAY:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                         m_creature->CastSpell(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), SPELL_BURNING_SPRAY, true);
                    break;
                case EVENT_SLIME_POOL:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLIME_POOL, CAST_TRIGGERED);
                    break;
                case EVENT_SWEEP:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SWEEP, CAST_TRIGGERED);
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
                            m_creature->GetMotionMaster()->MovePoint(0, RoomCenter[0]+(rand()%80 - 40), RoomCenter[1]+(rand()%80 - 40), RoomCenter[2]);
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

        if (!m_uiStep) //prevent melee attacks from underground
            DoMeleeAttackIfReady();
    }
};

#undef PHASE_TIMER

/*######
## boss_icehowl
######*/

/* TODO: merge this
enum
{
    EMOTE_MASSIVE_CRASH                 = -1649039,
};
*/

enum IcehowlSpells
{
    SPELL_BERSERK               = 26662,
    SPELL_FEROCIOUS_BUTT        = 66770,
    SPELL_ARCTIC_BREATH         = 66689,
    SPELL_WHIRL                 = 67345,
    SPELL_MASSIVE_CRASH         = 66683,
    SPELL_FROTHING_RAGE         = 66759,
    SPELL_TRAMPLE               = 66734,    // just the dmg part
    SPELL_STAGGERED_DAZE        = 66758,    // part of the Trample effect
    SPELL_SURGE_OF_ADRENALINE   = 68667,
};

enum IcehowlEvents
{
    EVENT_BERSERK = 1,
    EVENT_SPECIAL,
    EVENT_WHIRL,
    EVENT_BREATH,
    EVENT_BUTT,
    EVENT_PHASE_CHANGE,
};

enum IcehowlPhases
{
    PHASE_NORMAL = 1,
    PHASE_SPECIAL,
    PMASK_NORMAL    = bit_mask<PHASE_NORMAL>::value,
    PMASK_SPECIAL   = bit_mask<PHASE_SPECIAL>::value,
};

#define TIMER_BERSERK   4*MINUTE*IN_MILLISECONDS
#define PHASE_TIMER     30*IN_MILLISECONDS, 45*IN_MILLISECONDS
#define SPECIAL_TIMER   60*IN_MILLISECONDS
#define WHIRL_TIMER     10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define BREATH_TIMER    15*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define BUTT_TIMER      20*IN_MILLISECONDS, 30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_icehowlAI: public boss_trial_of_the_crusaderAI
{
    Unit *pTarget;
    float pTargetX, pTargetY;
    uint32 m_uiStep;

    boss_icehowlAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        pTarget(NULL),
        pTargetX(0.0f), pTargetY(0.0f),
        m_uiStep(0)
    {
    }

    void Aggro(Unit *pWho)
    {
        pTarget = NULL;
        pTargetX = 0.0f;
        pTargetY = 0.0f;
        m_uiStep = 0;
        m_creature->SetSpeedRate(MOVE_WALK, 2.0f, true);
        m_creature->SetSpeedRate(MOVE_RUN,  2.0f, true);
        Events.SetPhase(PHASE_NORMAL);
        if (m_bIsHeroic)
            Events.ScheduleEvent(EVENT_BERSERK, TIMER_BERSERK);
        Events.ScheduleEventInRange(EVENT_PHASE_CHANGE, PHASE_TIMER, SPECIAL_TIMER);
        Events.ScheduleEventInRange(EVENT_WHIRL, WHIRL_TIMER, WHIRL_TIMER, 0, 0, PMASK_NORMAL);
        Events.ScheduleEventInRange(EVENT_BUTT, BUTT_TIMER, BUTT_TIMER, 0, 0, PMASK_NORMAL);
        Events.ScheduleEventInRange(EVENT_BREATH, BREATH_TIMER, BREATH_TIMER, 7500, 0, PMASK_NORMAL);
        m_BossEncounter = IN_PROGRESS;
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
                case EVENT_WHIRL:
                    m_creature->CastSpell(m_creature, SPELL_WHIRL, false);
                    break;
                case EVENT_BUTT:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_FEROCIOUS_BUTT, false);
                    break;
                case EVENT_BREATH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(target, SPELL_ARCTIC_BREATH, false);
                    break;
                case EVENT_PHASE_CHANGE:
                    Events.SetPhase(PHASE_SPECIAL);
                    Events.ScheduleEvent(EVENT_SPECIAL, 0, 0, 0, 0, PMASK_SPECIAL);
                    break;
                case EVENT_SPECIAL:
                    switch(m_uiStep)
                    {
                        case 0:
                            DoStartNoMovement(m_creature->getVictim());
                            m_creature->SetSpeedRate(MOVE_RUN, 10.0f, true);
                            m_creature->GetMotionMaster()->MovePoint(0, RoomCenter[0], RoomCenter[1], RoomCenter[2]);
                            Events.ScheduleEvent(EVENT_SPECIAL, 2000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 1:
                            m_creature->GetMotionMaster()->MoveIdle();
                            m_creature->CastSpell(m_creature, SPELL_MASSIVE_CRASH, false);
                            Events.ScheduleEvent(EVENT_SPECIAL, 3000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 2:
                            pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                            if (pTarget)
                            {
                                pTargetX = pTarget->GetPositionX();
                                pTargetY = pTarget->GetPositionY();
                                m_creature->SetFacingTo(m_creature->GetAngle(pTargetX, pTargetY));
                                m_creature->MonsterTextEmote("Icehowl glares at $N and lets out a bellowing roar!", pTarget, true);
                            }
                            Events.ScheduleEvent(EVENT_SPECIAL, 1000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 3:
                            m_creature->HandleEmote(EMOTE_ONESHOT_BATTLEROAR); //FIXME: doesn't display animation
                            Events.ScheduleEvent(EVENT_SPECIAL, 1500, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 4:
                            if (pTarget)
                            {
                                float x, y;
                                float angle = m_creature->GetAngle(pTarget) + M_PI;
                                float radius = m_creature->GetDistance2d(pTarget);
                                x = radius*cos(angle) + RoomCenter[0];
                                y = radius*sin(angle) + RoomCenter[1];
                                m_creature->GetMotionMaster()->MovePoint(0, x, y, m_creature->GetPositionZ());
                            }
                            Events.ScheduleEvent(EVENT_SPECIAL, 1000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 5:
                            m_creature->SetFacingTo(m_creature->GetAngle(pTargetX, pTargetY));
                            if (!m_bIsHeroic)
                            {
                                Map *pMap = m_creature->GetMap();
                                if (pMap && pMap->IsDungeon())
                                {
                                    Map::PlayerList const &PlayerList = pMap->GetPlayers();
                                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                        if (i->getSource()->isAlive())
                                            i->getSource()->CastSpell(i->getSource(), SPELL_SURGE_OF_ADRENALINE, true);
                                }
                            }
                            Events.ScheduleEvent(EVENT_SPECIAL, 1000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 6:
                            m_creature->SetSpeedRate(MOVE_RUN, 10.0f, true);
                            m_creature->SetSpeedRate(MOVE_WALK, 10.0f, true);
                            m_creature->GetMotionMaster()->MovePoint(0, pTargetX, pTargetY, m_creature->GetPositionZ());
                            Events.ScheduleEvent(EVENT_SPECIAL, 2500, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        case 7:
                        {
                            Map* pMap = m_creature->GetMap();
                            bool m_bHit = false;
                            if (pMap && pMap->IsDungeon())
                            {
                                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i) 
                                    if (m_creature->IsWithinDistInMap(i->getSource(), 12.0f))
                                    {
                                        m_bHit = true;
                                        break;
                                    }
                            }
                            m_creature->CastSpell(m_creature, SPELL_TRAMPLE, true);
                            if (!m_bHit)
                            {
                                m_creature->CastSpell(m_creature, SPELL_STAGGERED_DAZE, false);
                                m_creature->MonsterTextEmote("Icehowl crashes into the Coliseum Wall and is stunned!", 0, true);
                            }
                            else
                                m_creature->CastSpell(m_creature, SPELL_FROTHING_RAGE, false);
                            Events.ScheduleEvent(EVENT_SPECIAL, 1000, 0, 0, 0, PMASK_SPECIAL);
                            m_uiStep++;
                            break;
                        }
                        case 8:
                            m_creature->SetSpeedRate(MOVE_RUN, 2.0f, true);
                            m_creature->SetSpeedRate(MOVE_WALK, 2.0f, true);
                            DoStartMovement(m_creature->getVictim());
                            m_uiStep = 0;
                            Events.SetPhase(PHASE_NORMAL);
                            break;
                    }
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
    REGISTER_SCRIPT(mob_snobold_vassal);
    REGISTER_SCRIPT(boss_acidmaw);
    REGISTER_SCRIPT(boss_dreadscale);
    REGISTER_SCRIPT(boss_icehowl);
}
