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
SDName: boss_proffesor_putricide
SD%Complete: 0%
SDComment: Mutated Transformation requires vehicle support. So, no slime puddles will be created, since the transformation is required to remove it. As well, adds will be slowed to 50% of normal speed. And, the add spells are non-functional.
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

// NOT WORKING:
// Mutated Transformation - vehicle support
// Gaseous Bloat - roots caster, not applying stacks
// Volatile Ooze Adhesive - roots caster
// Mutated Strength - procs wrong spells
// All m_creature model changes. (Guzzle Potion, Create Concoction) (turns into a pig)

//... I think blizz pulled out a lot of hacks for this encounter.

enum Spells
{
    SPELL_BERSERK                       = 47008,
    SPELL_CHOKING_GAS_BOMB              = 71255,
    SPELL_CHOKING_GAS_BOMB_DAMAGE_AURA  = 71259,
    SPELL_CHOKING_GAS_BOMB_EXPLODE_AURA = 71280,
    SPELL_SLIME_PUDDLE                  = 70341,    // Note: cast 70343 on summoned NPC [NPC_SLIME_PUDDLE]. Do range check BEFORE damage apply (NPC script?)
    SPELL_MALLEABLE_GOO                 = 70852,
    SPELL_UNSTABLE_EXPERIMENT           = 71968,
    SPELL_TEAR_GAS                      = 71617,
    SPELL_GUZZLE_POTION                 = 73122,    // Turns professor into a pig. Unknown modelid. Stacks Mutated Plague in target every melee, should be on self and be Mutated Strength
    SPELL_CREATE_CONCOCTION             = 71621,    // Turns professor into a pig. Unknown modelid.
    SPELL_MUTATED_STRENGTH              = 71603,    // See SPELL_GUZZLE_POTION - triggered by it.
    SPELL_MUTATED_PLAGUE                = 72672,
    SPELL_UNBOUND_PLAGUE                = 72855,

    // Gas Cloud
    SPELL_GASEOUS_BLOAT                 = 70672,    // Roots caster-> appears to be a channeled spell, in which the caster can move. Also does not stack Gaseous Bloat on target.
    SPELL_EXPUNGE_GAS                   = 70701,    // Core fix needed. Damage based on stacks of Gaseous Bloat. Problem : gaseous bloat NOT STACKING!!

    // Volatile Ooze
    SPELL_VOLATILE_OOZE_ADHESIVE        = 70447,    // Same as SPELL_GASEOUS_BLOAT
    SPELL_OOZE_ERUPTION                 = 70492,
};

enum NPCS
{
    NPC_CHOKING_GAS_BOMB        = 38159,
    NPC_TEAR_GAS_TARGET         = 38317,
    NPC_SLIME_PUDDLE            = 70342,

    NPC_GAS_CLOUD               = 37562,
    NPC_VOLATILE_OOZE           = 37697,
};

enum Says
{
    SAY_AGGRO                   = -1300494,
    SAY_AIRLOCK                 = -1300495,
    SAY_HEROIC_PHASE_CHANGE     = -1300496,
    SAY_TRANSFORM1              = -1300497,
    SAY_TRANSFORM2              = -1300498,
    SAY_KILLED_PLAYER1          = -1300499,
    SAY_KILLED_PLAYER2          = -1300500,
    SAY_BERSERK                 = -1300501,
    SAY_DEATH                   = -1300502,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_UNSTABLE_EXPERIMENT,
    EVENT_CHOKING_GAS,
    EVENT_MALLEABLE_GOO,
    EVENT_TEAR_GAS,
    EVENT_MUTATED_PLAGUE,
    EVENT_UNBOUND_PLAGUE,
    EVENT_START_NO_MOVEMENT,
    EVENT_START_MOVEMENT,
};

enum Phases
{
    PHASE_ONE = 1,
    PHASE_TWO,
    PHASE_THREE,
    PMASK_ONE       = bit_mask<PHASE_ONE, PHASE_TWO>::value,
    PMASK_TWO       = bit_mask<PHASE_TWO, PHASE_THREE>::value,
    PMASK_THREE     = bit_mask<PHASE_THREE>::value,
};

static const float CenterPosition[2]     =   { 4357.0f, 3211.5f };
static const float SummonPositions[2][2] =
{
    { 4383.1f, 3209.4f },
    { 4330.9f, 3210.2f },
};

#define FLOOR_HEIGHT    389.4f

#define TIMER_BERSERK               10*MINUTE*IN_MILLISECONDS
#define TIMER_UNSTABLE_EXPERIMENT   38*IN_MILLISECONDS
#define TIMER_CHOKING_GAS           35.5*IN_MILLISECONDS
#define TIMER_MALLEABLE_GOO         25*IN_MILLISECONDS
#define TIMER_MUTATED_PLAGUE        10*IN_MILLISECONDS
#define TIMER_UNBOUND_PLAGUE        2*MINUTE*IN_MILLISECONDS        // guessed value
#define TIMER_START_NO_MOVEMENT     3*IN_MILLISECONDS
#define TIMER_START_MOVEMENT        23*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_proffesor_putricideAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    bool LastSummonSideIsOrange :1;
    bool HasDoneIntro :1;

    boss_proffesor_putricideAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(m_creature),
        LastSummonSideIsOrange(true),
        HasDoneIntro(false)
    {
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.SetPhase(PHASE_ONE);
        LastSummonSideIsOrange = true;
        m_BossEncounter = IN_PROGRESS;
        SCHEDULE_EVENT(BERSERK);
        if (m_bIsHeroic)
            SCHEDULE_EVENT(UNBOUND_PLAGUE);
        SCHEDULE_EVENT(UNSTABLE_EXPERIMENT, 0, 0, PMASK_ONE);
        SCHEDULE_EVENT(CHOKING_GAS, 0, 0, PMASK_TWO);
        SCHEDULE_EVENT(MALLEABLE_GOO, 0, 0, PMASK_TWO);
        SCHEDULE_EVENT(MUTATED_PLAGUE, 0, 0, PMASK_THREE);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!HasDoneIntro && pWho && pWho->GetPositionZ() > FLOOR_HEIGHT - 1)   // prevent intro on Rotface/Festergut encounter
        {
            HasDoneIntro = true;
            DoScriptText(SAY_AIRLOCK, m_creature);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void KilledUnit(Unit* pWho)
    {
        DoScriptText(urand(0, 1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_DEATH, m_creature);
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void JustSummoned(Creature *pSumm)
    {
        if (!pSumm)
            return;
        if (pSumm->GetEntry() == NPC_CHOKING_GAS_BOMB)           // Not directly summomed by SummonMgr
        {
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
            pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE);
            pSumm->CastSpell(pSumm, SPELL_CHOKING_GAS_BOMB_DAMAGE_AURA, true);
            pSumm->CastSpell(pSumm, SPELL_CHOKING_GAS_BOMB_EXPLODE_AURA, true);
            pSumm->ForcedDespawn(10500);
        }
    }

    void SpellHitTarget(Unit *pVictim, const SpellEntry *pSpell)
    {
        if (pSpell->SpellDifficultyId == 2119)  // Unstable Experiment
        {
            Creature *pSumm;
            if (LastSummonSideIsOrange)
                pSumm = SummonMgr.SummonCreature(NPC_VOLATILE_OOZE, SummonPositions[0][0], SummonPositions[0][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN);
            else
                pSumm = SummonMgr.SummonCreature(NPC_GAS_CLOUD, SummonPositions[1][0], SummonPositions[1][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN);
            if (!pSumm)
                return;
            pSumm->SetInCombatWithZone();
            pSumm->SetSpeedRate(MOVE_RUN, 0.6f);    // because Mutated Transformation is not working.
            Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1);
            if (pTarget)
            {
                pSumm->AddThreat(pTarget, 50000.0f);
                //pSumm->CastSpell(pTarget, LastSummonSideIsOrange ? SPELL_VOLATILE_OOZE_ADHESIVE : SPELL_GASEOUS_BLOAT, false);
            }
            LastSummonSideIsOrange = !LastSummonSideIsOrange;
        }
    }

    void ChangePhase()
    {
        if (Events.GetPhase() >= PHASE_THREE)
            return;
        Events.SetPhase(Events.GetPhase()+1);
        if (m_bIsHeroic)
        {
            SummonMgr.SummonCreature(NPC_VOLATILE_OOZE, SummonPositions[0][0], SummonPositions[0][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN);
            SummonMgr.SummonCreature(NPC_GAS_CLOUD, SummonPositions[1][0], SummonPositions[1][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN);
            DoScriptText(SAY_HEROIC_PHASE_CHANGE, m_creature);
        }
        else
        {
            Creature *TearTarget = SummonMgr.SummonCreature(NPC_TEAR_GAS_TARGET, CenterPosition[0], CenterPosition[1], FLOOR_HEIGHT, 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS);
            if (TearTarget)
                DoCast(TearTarget, SPELL_TEAR_GAS, true);
            DoScriptText(urand(0, 1) ? SAY_TRANSFORM1 : SAY_TRANSFORM2, m_creature);
        }
        Events.SetCooldown(TIMER_START_MOVEMENT);
        //DoCast(m_creature, Events.GetPhase() == PHASE_TWO ? SPELL_CREATE_CONCOCTION : SPELL_GUZZLE_POTION);
        m_creature->MonsterMove(4356.7f, 3265.5f, 389.4f, TIMER_START_NO_MOVEMENT);
        Events.ScheduleEvent(EVENT_START_NO_MOVEMENT, TIMER_START_NO_MOVEMENT, 0, 0, 1);
        Events.ScheduleEvent(EVENT_START_MOVEMENT, TIMER_START_MOVEMENT, 0, 0, 1);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        if ((Events.GetPhase() == PHASE_ONE && m_creature->GetHealthPercent() < 80.0f) ||
            (Events.GetPhase() == PHASE_TWO && m_creature->GetHealthPercent() < 35.0f))
            ChangePhase();

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BERSERK, m_creature);
                    break;
                case EVENT_UNSTABLE_EXPERIMENT:
                    DoCast(m_creature, SPELL_UNSTABLE_EXPERIMENT);
                    break;
                case EVENT_CHOKING_GAS:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_CHOKING_GAS_BOMB);
                    break;
                case EVENT_MALLEABLE_GOO:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_MALLEABLE_GOO);
                    break;
                case EVENT_MUTATED_PLAGUE:
                    DoCast(m_creature->getVictim(), SPELL_MUTATED_PLAGUE);
                    break;
                case EVENT_UNBOUND_PLAGUE:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                        DoCast(target, SPELL_UNBOUND_PLAGUE, false);
                    break;
                case EVENT_START_NO_MOVEMENT:
                    DoStartNoMovement(m_creature->getVictim());
                    break;
                case EVENT_START_MOVEMENT:
                    DoStartMovement(m_creature->getVictim());
                    break;
                default:
                    break;
            }

        if (!Events.GetCooldownTime(0))
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_putricide_addAI: public ScriptedAI
{
    mob_putricide_addAI(Creature *pCreature): ScriptedAI(pCreature)
    {
    }

    void Reset() {}

    void SpellHitTarget(Unit *pVictim, const SpellEntry *pSpell)
    {
        if (!pVictim)
            return;

        //if (pSpell->SpellDifficultyId == 2241 || pSpell->SpellDifficultyId == 2176)     // Ooze adhesive, Gaseous bloat
        //    m_creature->InterruptSpell(CURRENT_GENERIC_SPELL);      // Not working.
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!pWho)
            return;
        if (pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 5.0f)) //&& (pWho->HasAuraByDifficulty(SPELL_VOLATILE_OOZE_ADHESIVE) || pWho->HasAuraByDifficulty(SPELL_GASEOUS_BLOAT)))
        {
            DoCast(pWho, m_creature->GetEntry() == NPC_GAS_CLOUD ? SPELL_EXPUNGE_GAS : SPELL_OOZE_ERUPTION, true);
            m_creature->ForcedDespawn(500);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

void AddSC_boss_proffesor_putricide()
{
    Script *newscript;

    REGISTER_SCRIPT(mob_putricide_add);
    REGISTER_SCRIPT(boss_proffesor_putricide);
}
