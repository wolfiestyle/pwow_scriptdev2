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
SDName: Boss Jaraxxus
SD%Complete: 90
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_jaraxxus
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "TemporarySummon.h"

enum Spells
{
    SPELL_BERSERK               = 64238,
    SPELL_FEL_FIREBALL          = 66532,
    SPELL_FEL_LIGHTNING         = 66528,
    SPELL_INCINERATE_FLESH      = 66237,
    SPELL_LEGION_FLAME          = 66197,
    SPELL_INFERNAL_ERUPTION     = 66258,
    SPELL_INFERNAL_ERUPT        = 66252,
    SPELL_NETHER_PORTAL         = 66269,
    SPELL_NETHER_PORTAL_SUMMON  = 66263,
    SPELL_NETHER_POWER          = 67009,
};

enum Says
{
    SAY_AGGRO                   = -1300311,
    SAY_INCINERATE              = -1300312,
    SAY_SUMMON_MISTRESS_OF_PAIN = -1300313,
    SAY_SUMMON_INFERNO          = -1300314,
    SAY_KILLED_PLAYER1          = -1300315,
    SAY_KILLED_PLAYER2          = -1300316,
    SAY_DEATH                   = -1300317,
};

enum Adds
{
    NPC_NETHER_PORTAL           = 34825,
    NPC_VOLCANO                 = 34813,
};

enum Events
{
    EVENT_FEL_FIREBALL = 1,
    EVENT_FEL_LIGHTNING,
    EVENT_INCINERATE_FLESH,
    EVENT_LEGION_FLAME,
    EVENT_SUMMON_VOLCANO,
    EVENT_SUMMON_PORTAL,
    EVENT_BERSERK,
    EVENT_BUFF,
};

#define BERSERK_TIMER       10*MINUTE*IN_MILLISECONDS
#define PORTAL_TIMER        2*MINUTE*IN_MILLISECONDS
#define VOLCANO_TIMER       2*MINUTE*IN_MILLISECONDS
#define FIREBALL_TIMER      10*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define LIGHTNING_TIMER     15*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define INCINERATE_TIMER    25*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define LEGION_FLAME_TIMER  30*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define BUFF_TIMER          30*IN_MILLISECONDS, 45*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_jaraxxusAI: public boss_trial_of_the_crusaderAI
{
    SummonManager SummonMgr;              // portals - volcanoes
    InstanceVar<uint32> m_bIsInTalkPhase; // used to avoid Reset and despawn during intro

    boss_jaraxxusAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        SummonMgr(pCreature),
        m_bIsInTalkPhase(DATA_IN_TALK_PHASE, m_pInstance)
    {
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_bIsInTalkPhase)
            return;
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit *pWho)
    {
        if (m_bIsInTalkPhase)
            return;
        DoScriptText(SAY_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_PORTAL, 30*IN_MILLISECONDS, PORTAL_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_VOLCANO, 90*IN_MILLISECONDS, VOLCANO_TIMER);
        Events.ScheduleEventInRange(EVENT_FEL_FIREBALL, FIREBALL_TIMER, FIREBALL_TIMER);
        Events.ScheduleEventInRange(EVENT_FEL_LIGHTNING, LIGHTNING_TIMER, LIGHTNING_TIMER);
        Events.ScheduleEventInRange(EVENT_INCINERATE_FLESH, INCINERATE_TIMER, INCINERATE_TIMER);
        Events.ScheduleEventInRange(EVENT_LEGION_FLAME, LEGION_FLAME_TIMER, LEGION_FLAME_TIMER);
        Events.ScheduleEventInRange(EVENT_BUFF, BUFF_TIMER, BUFF_TIMER);
        m_BossEncounter = IN_PROGRESS;
    }

    void Reset()
    {
        if (m_bIsInTalkPhase)
            return;
        SummonMgr.UnsummonAll();    // despawns portals and volcanoes only
        boss_trial_of_the_crusaderAI::Reset();
    }

    void KilledUnit(Unit *who)
    {
        boss_trial_of_the_crusaderAI::KilledUnit(who);
        if ((!who || who->GetTypeId() != TYPEID_PLAYER) && !m_bIsInTalkPhase)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (!pSummon)
            return;
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());

        if (!m_bIsHeroic && (pSummon->GetEntry() == NPC_NETHER_PORTAL || pSummon->GetEntry() == NPC_VOLCANO))
            pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

        switch (pSummon->GetEntry())
        {
            case NPC_NETHER_PORTAL:
                pSummon->GetMotionMaster()->MoveIdle();
                pSummon->SetDisplayId(30039);
                pSummon->CastSpell(pSummon, SPELL_NETHER_PORTAL_SUMMON, false);
                m_creature->MonsterTextEmote("Lord Jaraxxus creates a Nether Portal!", 0, true);
                break;
            case NPC_VOLCANO:
                pSummon->GetMotionMaster()->MoveIdle();
                pSummon->CastSpell(pSummon, SPELL_INFERNAL_ERUPT, false);
                break;
            default:
                break;
        }
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (!pSummon)
            return;
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
    }

    void JustDied(Unit* pSlayer)
    {
        DoScriptText(SAY_DEATH, m_creature);
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
                    m_creature->CastSpell(m_creature, SPELL_BERSERK, false);
                    break;
                case EVENT_INCINERATE_FLESH:
                    DoScriptText(SAY_INCINERATE, m_creature);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(pTarget, SPELL_INCINERATE_FLESH, false);
                    break;
                case EVENT_FEL_FIREBALL:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEL_FIREBALL);
                    break;
                case EVENT_FEL_LIGHTNING:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCastSpellIfCan(pTarget, SPELL_FEL_LIGHTNING);
                    break;
                case EVENT_LEGION_FLAME:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCastSpellIfCan(pTarget, SPELL_LEGION_FLAME);
                    break;
                case EVENT_SUMMON_VOLCANO:
                    DoScriptText(SAY_SUMMON_INFERNO, m_creature);
                    m_creature->CastSpell(m_creature, SPELL_INFERNAL_ERUPTION, false);
                    break;
                case EVENT_SUMMON_PORTAL:
                    DoScriptText(SAY_SUMMON_MISTRESS_OF_PAIN, m_creature);
                    m_creature->CastSpell(m_creature, SPELL_NETHER_PORTAL, false);
                    break;
                case EVENT_BUFF:
                    DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

enum AddSpells
{
    SPELL_MISTRESS_KISS         = 67077,
    SPELL_SHIVAN_SLASH          = 66378,
    SPELL_SPINNING_PAIN_SPIKE   = 66283,
    SPELL_FEL_INFERNO           = 67047, // Spell trigger is 66517.
    SPELL_FEL_STREAK            = 66493,
    SPELL_LEGION_FLAMES         = 66201,
};

enum AddEvents
{
    EVENT_KISS = 1,
    EVENT_SHIVAN_SLASH,
    EVENT_SPIKE,
    EVENT_FEL_INFERNO,
    EVENT_FELSTREAK,
    EVENT_BURN,
};

#define KISS_TIMER      20*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define SLASH_TIMER     30*IN_MILLISECONDS, 45*IN_MILLISECONDS
#define SPIKE_TIMER     15*IN_MILLISECONDS, 30*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_mistress_of_painAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;
    InstanceVar<uint32> m_AchievementCounter;
    bool m_bIsHeroic;

    mob_mistress_of_painAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_AchievementCounter(DATA_ACHIEVEMENT_COUNTER, m_pInstance)
    {
        Difficulty diff = m_creature->GetMap()->GetDifficulty();
        m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
    }

    void Reset()
    {
        Events.Reset();
    }

    void KilledUnit(Unit *pWho)
    {
        if (m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            m_pInstance->SetData(DATA_IMMORTAL, 0);
    }

    void Aggro(Unit* pWho)
    {
        if (m_bIsHeroic)
            Events.ScheduleEventInRange(EVENT_KISS, KISS_TIMER, KISS_TIMER);
        Events.ScheduleEventInRange(EVENT_SHIVAN_SLASH, SLASH_TIMER, SLASH_TIMER);
        Events.ScheduleEventInRange(EVENT_SPIKE, SPIKE_TIMER, SPIKE_TIMER, 4000);
        ++m_AchievementCounter;
    }

    void JustDied(Unit* pSlayer)
    {
        --m_AchievementCounter;
        DESPAWN_CREATURE;
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            DESPAWN_CREATURE;
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_KISS:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(pTarget, SPELL_MISTRESS_KISS, false);
                    break;
                case EVENT_SHIVAN_SLASH:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHIVAN_SLASH);
                    break;
                case EVENT_SPIKE:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(pTarget, SPELL_SPINNING_PAIN_SPIKE, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_jaraxxus_add_summonerAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;

    mob_jaraxxus_add_summonerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
    }

    void Reset()
    {
    }

    void KilledUnit(Unit* pWho)
    {
        if (m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            m_pInstance->SetData(DATA_IMMORTAL, 0);
    }

    void Aggro(Unit *pWho)
    {
    }

    void JustSummoned(Creature *pSummon)
    {
        pSummon->SetInCombatWithZone();
    }

    void JustDied(Unit* pSlayer)
    {
        DESPAWN_CREATURE;
    }

    void UpdateAI(uint32 const uiDiff)
    {
    }
};

#define FELSTREAK_TIMER     15*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define FEL_INFERNO_TIMER   15*IN_MILLISECONDS, 30*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_felflame_infernalAI: public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance *m_pInstance;

    mob_felflame_infernalAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
    }

    void Reset()
    {
        Events.Reset();
    }

    void KilledUnit(Unit *pWho)
    {
        if (m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            m_pInstance->SetData(DATA_IMMORTAL, 0);
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_FEL_INFERNO, FEL_INFERNO_TIMER, FEL_INFERNO_TIMER, 6000);
        Events.ScheduleEventInRange(EVENT_FELSTREAK, FELSTREAK_TIMER, FELSTREAK_TIMER, 7500);
    }

    void JustDied(Unit* pSlayer)
    {
        DESPAWN_CREATURE;
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            DESPAWN_CREATURE;
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FELSTREAK:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        m_creature->CastSpell(pTarget, SPELL_FEL_STREAK, false);
                    break;
                case EVENT_FEL_INFERNO:
                    m_creature->StopMoving();
                    m_creature->CastSpell(m_creature, SPELL_FEL_INFERNO, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

#define FLAME_DESPAWN_TIMER 60*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_legion_flameAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    uint32 m_uiDespawnTimer;

    mob_legion_flameAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_uiDespawnTimer(0)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetDisplayId(11686);
        m_creature->CastSpell(m_creature, SPELL_LEGION_FLAMES, false);
    }

    void Reset()
    {
    }

    void KilledUnit(Unit *pWho)
    {
        if (m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            m_pInstance->SetData(DATA_IMMORTAL, 0);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        m_uiDespawnTimer += uiDiff;

        if (m_uiDespawnTimer > FLAME_DESPAWN_TIMER || !m_pInstance->IsEncounterInProgress())
            DESPAWN_CREATURE;
    }
};

void AddSC_boss_jaraxxus()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_jaraxxus);
    REGISTER_SCRIPT(mob_mistress_of_pain);
    REGISTER_SCRIPT(mob_felflame_infernal);
    REGISTER_SCRIPT(mob_legion_flame);
    REGISTER_SCRIPT(mob_jaraxxus_add_summoner);
}
