/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss Anub'arak
SD%Complete: 70
SDComment: most add timers need verification, pursuing spikes not working (no idea about how it works), Spider Frenzy buffs players instead of adds
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_anubarak_trial
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

//evidently, azjol-nerub was merely a set-back

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1649064,
    SAY_SLAY_1                          = -1649065,
    SAY_SLAY_2                          = -1649066,
    SAY_DEATH                           = -1649067,
    SAY_BERSERK                         = -1649068,
    SAY_SUBMERGE                        = -1649069,
    SAY_LEECHING_SWARM                  = -1649070,
};
*/

enum Spells
{
    // Anub'arak
    SPELL_BERSERK               = 26662,
    SPELL_FREEZING_SLASH        = 66012,
    SPELL_PENETRATING_COLD      = 66013,
    SPELL_LEECHING_SWARM        = 66118,
    SPELL_PURSUING_SPIKES       = 65922,  // only part of the effect, probably used by dummy creature
    SPELL_SUBMERGE_ANUBARAK     = 65981,
    SPELL_UNDERGROUND_VISUAL    = 65921,
    SPELL_PURSUED_BY_ANUBARK    = 67574,

    // Frost Sphere
    SPELL_PERMAFROST            = 66193,
    // Nerubian Burrower
    SPELL_SUBMERGE_BURROWER     = 68394,
    SPELL_SHADOW_STRIKE         = 66134, //only heroic
    SPELL_SUBMERGE_ATTEMPT      = 67322,
    SPELL_EXPOSE_WEAKNESS       = 67720,
    SPELL_SPIDER_FRENZY         = 66128,
    // Swarm Scarab
    SPELL_SWARM_SCARAB_PASSIVE  = 65774,
    SPELL_DETERMINATION         = 66092,
};

enum AddIds
{
    NPC_FROST_SPHERE            = 34606,
    NPC_NERUBIAN_BURROWER       = 34607,
    NPC_SWARM_SCARAB            = 34605,
};

enum Says
{
    SAY_ANUBARAK_ANUBARAK_INTRO6 = -1300354,

    SAY_AGGRO                   = -1300355,
    SAY_SUBMERGE                = -1300356,
    SAY_LEECHING_SWARM          = -1300357,
    SAY_KILLED_PLAYER1          = -1300358,
    SAY_KILLED_PLAYER2          = -1300359,
    SAY_DEATH                   = -1300360,
    SAY_BERSERK                 = -1300361,
};

enum Events
{
    // Anub'arak
    EVENT_BERSERK = 1,
    EVENT_FREEZING_SLASH,
    EVENT_PENETRATING_COLD,
    EVENT_SUBMERGE,
    EVENT_SUMMON_BURROWER,
    EVENT_SUMMON_SWARM_SCARAB,
    EVENT_PURSUING_SPIKES,
    EVENT_UNSUBMERGE,
    // Nerubian Burrower
    EVENT_ATTEMPT_SUBMERGE,
    EVENT_EXPOSE_WEAKNESS,
    EVENT_SHADOW_STRIKE,
    // Swarm Scarab
    EVENT_DETERMINATION,
};

enum Phases
{
    PHASE_ABOVEGROUND = 1,
    PHASE_BELOWGROUND,
    PHASE_THREE,
    PMASK_ABOVE         = bit_mask<PHASE_ABOVEGROUND>::value,
    PMASK_BELOW         = bit_mask<PHASE_BELOWGROUND>::value,
    PMASK_ABOVE_THREE   = bit_mask<PHASE_ABOVEGROUND, PHASE_THREE>::value,
};

// Anub'arak
#define TIMER_BERSERK               10*MINUTE*IN_MILLISECONDS
#define TIMER_SUMMON_SWARM_SCARAB   4*IN_MILLISECONDS
#define TIMER_SUMMON_BURROWER       45*IN_MILLISECONDS
#define TIMER_PENETRATING_COLD      18*IN_MILLISECONDS           //not sure about this one
#define TIMER_FREEZING_SLASH        19*IN_MILLISECONDS, 21*IN_MILLISECONDS
#define TIMER_SUBMERGE              75*IN_MILLISECONDS
#define TIMER_UNSUBMERGE            65*IN_MILLISECONDS
// Nerubian Burrower
#define TIMER_ATTEMPT_SUBMERGE      50*IN_MILLISECONDS           //not sure
#define TIMER_EXPOSE_WEAKNESS       3*IN_MILLISECONDS, 10*IN_MILLISECONDS   //unsure
#define TIMER_SHADOW_STRIKE         31*IN_MILLISECONDS
// Swarm Scarab
#define TIMER_DETERMINATION         30*IN_MILLISECONDS, 400*IN_MILLISECONDS //unsure (wowwiki says "random chance")

#define MAX_ALIVE_FROST_SPHERES     6
#define FLOOR_HEIGHT                143.0f
#define WALK_SPEED                  1.0f
#define RUN_SPEED                   1.14286f

static const float SummonPositions[4][2] = 
{
    {740.5f, 189.1f},
    {692.9f, 184.8f},
    {688.2f, 102.8f},
    {735.4f, 75.3f}
};

static const float FrostSpherePos[3] = {741.0f, 133.6f, 163.0f};

struct MANGOS_DLL_DECL boss_anubarak_trialAI: public boss_trial_of_the_crusaderAI
{
    SummonManager SummonMgr;
    bool m_bSaidBeginningStuff;

    boss_anubarak_trialAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        SummonMgr(pCreature),
        m_bSaidBeginningStuff(false)
    {
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bSaidBeginningStuff)
        {
            m_bSaidBeginningStuff = true;
            DoScriptText(SAY_ANUBARAK_ANUBARAK_INTRO6, m_creature);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        SummonMgr.UnsummonAll();
        m_creature->RemoveAllAuras();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        Events.Reset();
        Events.SetPhase(PHASE_ABOVEGROUND);

        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(SUMMON_BURROWER);
        SCHEDULE_EVENT(PENETRATING_COLD, 0, 0, PMASK_ABOVE_THREE);
        SCHEDULE_EVENT_R(FREEZING_SLASH, 0, 0, PMASK_ABOVE_THREE);
        SCHEDULE_EVENT(SUMMON_SWARM_SCARAB, 0, 0, PMASK_BELOW);
        Events.ScheduleEvent(EVENT_SUBMERGE, TIMER_SUBMERGE, 0, 0, 0, PMASK_ABOVE);

        SummonAdds(NPC_FROST_SPHERE, MAX_ALIVE_FROST_SPHERES);

        DoScriptText(SAY_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void JustSummoned(Creature *pSumm)
    {
        if (!pSumm)
            return;

        switch (pSumm->GetEntry())
        {
            case NPC_FROST_SPHERE:
                pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                pSumm->GetMotionMaster()->MoveIdle();
                pSumm->SetSplineFlags(SPLINEFLAG_UNKNOWN7); //Fly
                break;
            case NPC_SWARM_SCARAB:
            case NPC_NERUBIAN_BURROWER:
                pSumm->SetInCombatWithZone();
                break;
        }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (!pSumm)
            return;

        if (pSumm->GetEntry() == NPC_FROST_SPHERE)
        {
            pSumm->MonsterMove(pSumm->GetPositionX(), pSumm->GetPositionY(), FLOOR_HEIGHT, 1);  //move to ground
            pSumm->CastSpell(pSumm, SPELL_PERMAFROST, true);
        }
        else
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void SummonAdds(uint32 entry, uint32 num = 1)
    {
        if (entry == NPC_FROST_SPHERE)
        {
            for (int i = 0; i < num; i++)
            {
                float x, y;
                GetRandomPointInCircle(x, y, 54.0f, FrostSpherePos[0], FrostSpherePos[1]);
                SummonMgr.SummonCreature(NPC_FROST_SPHERE, x, y, FrostSpherePos[2], 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            }
        }
        else    // scarabs and burrowers
        {
            for (int i = 0; i < num; i++)
            {
                uint32 RandPos = urand(0,3);
                SummonMgr.SummonCreature(entry, SummonPositions[RandPos][0], SummonPositions[RandPos][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsHeroic)
        {
            uint32 AliveFrostSpheres = SummonMgr.GetSummonCount(NPC_FROST_SPHERE);
            if (AliveFrostSpheres < MAX_ALIVE_FROST_SPHERES)
                SummonAdds(NPC_FROST_SPHERE, MAX_ALIVE_FROST_SPHERES - AliveFrostSpheres);
        }

        if (Events.GetPhase() != PHASE_THREE && m_creature->GetHealthPercent() < 30.0f)
        {
            Events.SetPhase(PHASE_THREE);
            m_creature->InterruptNonMeleeSpells(false);
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_ANUBARAK);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            if (!m_bIsHeroic)
                Events.CancelEvent(EVENT_SUMMON_BURROWER);
            DoScriptText(SAY_LEECHING_SWARM, m_creature);
            DoCast(m_creature, SPELL_LEECHING_SWARM);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_FREEZING_SLASH:
                    DoCast(m_creature->getVictim(), SPELL_FREEZING_SLASH);
                    break;
                case EVENT_PENETRATING_COLD:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_PENETRATING_COLD);
                    break;
                case EVENT_SUMMON_BURROWER:
                    /* 1 will spawn on 10man normal
                     * 2 will spawn on 25man normal
                     * 2 will spawn on 10man heroic (and keep spawning even during phase 3)
                     * 4 will spawn on 25man heroic (and keep spawning even during phase 3)
                     */
                    SummonAdds(NPC_NERUBIAN_BURROWER, DIFF_SELECT(1, 2, 2, 4));
                    break;
                case EVENT_SUMMON_SWARM_SCARAB:
                    SummonAdds(NPC_SWARM_SCARAB);
                    break;
                case EVENT_SUBMERGE:
                {
                    Events.SetPhase(PHASE_BELOWGROUND);
                    DoScriptText(SAY_SUBMERGE, m_creature);
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature, SPELL_SUBMERGE_ANUBARAK, true);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    //DoCast(m_creature, SPELL_UNDERGROUND_VISUAL, true); //doesn't show
                    DoResetThreat();
                    Events.ScheduleEvent(EVENT_UNSUBMERGE, TIMER_UNSUBMERGE, 0, 0, 0, PMASK_BELOW);
                    break;
                }
                case EVENT_UNSUBMERGE:
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_ANUBARAK);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    Events.SetPhase(PHASE_ABOVEGROUND);
                    Events.ScheduleEvent(EVENT_SUBMERGE, TIMER_SUBMERGE, 0, 0, 0, PMASK_ABOVE);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *pKiller)
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        SummonMgr.UnsummonAll();
        Events.Reset();
        DoScriptText(SAY_DEATH, m_creature);
        //only non-summoned boss, so to streamline things its easier to do this
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSEY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

struct MANGOS_DLL_DECL mob_toc_nerubian_burrowerAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsHeroic :1;
    bool m_bInPermafrost :1;
    bool m_bIsSlowed :1;

    mob_toc_nerubian_burrowerAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_Difficulty(pCreature->GetMap()->GetDifficulty()),
        m_bIsHeroic(m_Difficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_Difficulty == RAID_DIFFICULTY_25MAN_HEROIC),
        m_bInPermafrost(false),
        m_bIsSlowed(false)
    {
    }

    void Reset()
    {
        Events.Reset();
    }

    void SpellHit(Unit *who, const SpellEntry *Spell) 
    {
        if (Spell->Id == SPELL_SUBMERGE_ATTEMPT && !m_bInPermafrost)
        {
            DoCast(m_creature, SPELL_SUBMERGE_BURROWER);
            m_creature->SetHealth(m_creature->GetMaxHealth());
            Events.ScheduleEvent(EVENT_UNSUBMERGE, 1*IN_MILLISECONDS);
        }
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        SCHEDULE_EVENT(ATTEMPT_SUBMERGE);
        SCHEDULE_EVENT_R(EXPOSE_WEAKNESS);
        //DoCast(m_creature, SPELL_SPIDER_FRENZY, true);
        if (m_bIsHeroic)
            SCHEDULE_EVENT(SHADOW_STRIKE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DynamicObject *Permafrost = GetClosestDynamicObjectWithEntry(m_creature, GetSpellIdWithDifficulty(SPELL_PERMAFROST, m_Difficulty), 8.0f);
        if (Permafrost)
        {
            m_bInPermafrost = true;
            if (!m_bIsSlowed)
            {
                // since Permafrost doesn't want to affect mobs, we'll slow them manually (hack)
                m_bIsSlowed = true;
                float speed_mod = 1.0f;
                SpellEntry const* spellInfo = GetSpellStore()->LookupEntry(Permafrost->GetSpellId());
                if (spellInfo)
                    speed_mod = float(100 + spellInfo->EffectBasePoints[Permafrost->GetEffIndex()]) / 100.0f;
                m_creature->SetSpeedRate(MOVE_WALK, WALK_SPEED * speed_mod, true);
                m_creature->SetSpeedRate(MOVE_RUN,  RUN_SPEED * speed_mod,  true);
            }
        }
        else
        {
            m_bInPermafrost = false;
            if (m_bIsSlowed)
            {
                m_bIsSlowed = false;
                m_creature->SetSpeedRate(MOVE_WALK, WALK_SPEED, true);
                m_creature->SetSpeedRate(MOVE_RUN,  RUN_SPEED,  true);
            }
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ATTEMPT_SUBMERGE:
                    DoCast(m_creature, SPELL_SUBMERGE_ATTEMPT);
                    break;
                case EVENT_EXPOSE_WEAKNESS:
                    DoCast(m_creature->getVictim(), SPELL_EXPOSE_WEAKNESS);
                    break;
                case EVENT_SHADOW_STRIKE:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_SHADOW_STRIKE);
                    break;
                case EVENT_UNSUBMERGE:
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_BURROWER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_toc_swarm_scarabAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    Difficulty m_Difficulty;
    bool m_bIsSlowed;

    mob_toc_swarm_scarabAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_Difficulty(pCreature->GetMap()->GetDifficulty()),
        m_bIsSlowed(false)
    {
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        DoCast(m_creature, SPELL_SWARM_SCARAB_PASSIVE);
        Events.ScheduleEventInRange(EVENT_DETERMINATION, TIMER_DETERMINATION);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DynamicObject *Permafrost = GetClosestDynamicObjectWithEntry(m_creature, GetSpellIdWithDifficulty(SPELL_PERMAFROST, m_Difficulty), 8.0f);
        if (Permafrost)
        {
            if (!m_bIsSlowed)
            {
                // since Permafrost doesn't want to affect mobs, we'll slow them manually (hack)
                m_bIsSlowed = true;
                float speed_mod = 1.0f;
                SpellEntry const* spellInfo = GetSpellStore()->LookupEntry(Permafrost->GetSpellId());
                if (spellInfo)
                    speed_mod = float(100 + spellInfo->EffectBasePoints[Permafrost->GetEffIndex()]) / 100.0f;
                m_creature->SetSpeedRate(MOVE_WALK, WALK_SPEED * speed_mod, true);
                m_creature->SetSpeedRate(MOVE_RUN,  RUN_SPEED * speed_mod,  true);
            }
        }
        else
        {
            if (m_bIsSlowed)
            {
                m_bIsSlowed = false;
                m_creature->SetSpeedRate(MOVE_WALK, WALK_SPEED, true);
                m_creature->SetSpeedRate(MOVE_RUN,  RUN_SPEED,  true);
            }
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_DETERMINATION:
                    DoCast(m_creature, SPELL_DETERMINATION);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_anubarak_trial()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_anubarak_trial);
    REGISTER_SCRIPT(mob_toc_nerubian_burrower);
    REGISTER_SCRIPT(mob_toc_swarm_scarab);
}
