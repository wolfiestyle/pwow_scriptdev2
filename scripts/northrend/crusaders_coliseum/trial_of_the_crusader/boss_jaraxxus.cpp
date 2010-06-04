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
SD%Complete: 0
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
    SPELL_FEL_FIREBALL_N10      = 66532,
    SPELL_FEL_FIREBALL_N25      = 66963,
    SPELL_FEL_FIREBALL_H10      = 66964,
    SPELL_FEL_FIREBALL_H25      = 66965,
    SPELL_FEL_LIGHTNING_N10     = 66528,
    SPELL_FEL_LIGHTNING_N25     = 67029,
    SPELL_FEL_LIGHTNING_H10     = 67030,
    SPELL_FEL_LIGHTNING_H25     = 67031,
    SPELL_INCINERATE_FLESH_N10  = 66237,
    SPELL_INCINERATE_FLESH_N25  = 67049,
    SPELL_INCINERATE_FLESH_H10  = 67050,
    SPELL_INCINERATE_FLESH_H25  = 67051,
    SPELL_LEGION_FLAME_N10      = 66197,
    SPELL_LEGION_FLAME_N25      = 68123,
    SPELL_LEGION_FLAME_H10      = 68124,
    SPELL_LEGION_FLAME_H25      = 68125,
    SPELL_INFERNAL_ERUPTION_N10 = 66258,
    SPELL_INFERNAL_ERUPTION_N25 = 67901,
    SPELL_INFERNAL_ERUPTION_H10 = 67902,
    SPELL_INFERNAL_ERUPTION_H25 = 67903,
    SPELL_INFERNAL_ERUPT_N10    = 66252,
    SPELL_INFERNAL_ERUPT_N25    = 67067,
    SPELL_INFERNAL_ERUPT_H10    = 67068,
    SPELL_INFERNAL_ERUPT_H25    = 67069,
    SPELL_NETHER_PORTAL_N10     = 66269,
    SPELL_NETHER_PORTAL_N25     = 67898,
    SPELL_NETHER_PORTAL_H10     = 67899,
    SPELL_NETHER_PORTAL_H25     = 67900,
    SPELL_NETHER_PORTAL_SUMMON_N10 = 66263,
    SPELL_NETHER_PORTAL_SUMMON_N25 = 67103,
    SPELL_NETHER_PORTAL_SUMMON_H10 = 67104,
    SPELL_NETHER_PORTAL_SUMMON_H25 = 67105,
    SPELL_NETHER_POWER          = 67009,
};

enum Says
{
    SAY_AGGRO                           = -1300311,
    SAY_INCINERATE                      = -1300312,
    SAY_SUMMON_MISTRESS_OF_PAIN         = -1300313,
    SAY_SUMMON_INFERNO                  = -1300314,
    SAY_KILLED_PLAYER1                  = -1300315,
    SAY_KILLED_PLAYER2                  = -1300316,
    SAY_DEATH                           = -1300317,
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

#define BUFF_TIMER          urand(30,45)*IN_MILLISECONDS
#define PORTAL_TIMER        2*MINUTE*IN_MILLISECONDS
#define BERSERK_TIMER       10*MINUTE*IN_MILLISECONDS
#define VOLCANO_TIMER       2*MINUTE*IN_MILLISECONDS
#define FIREBALL_TIMER      urand(10,15)*IN_MILLISECONDS
#define LIGHTNING_TIMER     urand(15,20)*IN_MILLISECONDS
#define INCINERATE_TIMER    urand(25,30)*IN_MILLISECONDS
#define LEGION_FLAME_TIMER  urand(30,40)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_jaraxxusAI: public boss_trial_of_the_crusaderAI
{
    boss_jaraxxusAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    typedef std::list<uint64> GuidList;
    GuidList m_Summoners;   // portals - volcanoes

    void Aggro(Unit *pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        Events.ScheduleEvent(EVENT_FEL_FIREBALL, FIREBALL_TIMER);
        Events.ScheduleEvent(EVENT_FEL_LIGHTNING, LIGHTNING_TIMER);
        Events.ScheduleEvent(EVENT_INCINERATE_FLESH, INCINERATE_TIMER);
        Events.ScheduleEvent(EVENT_LEGION_FLAME, LEGION_FLAME_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_VOLCANO, 90*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SUMMON_PORTAL, 30*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_BUFF, BUFF_TIMER);
        m_BossEncounter = IN_PROGRESS;
    }

    void Reset()
    {
        DespawnSummons();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (pSummon)
        {
            if (!m_bIsHeroic && (pSummon->GetEntry() == NPC_NETHER_PORTAL || pSummon->GetEntry() == NPC_VOLCANO))
            {
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            switch (pSummon->GetEntry())
            {
                case NPC_NETHER_PORTAL:
                    pSummon->GetMotionMaster()->MoveIdle();
                    pSummon->SetDisplayId(30039);
                    pSummon->CastSpell(pSummon, DIFFICULTY(SPELL_NETHER_PORTAL_SUMMON), false);
                    m_creature->MonsterTextEmote("Lord Jaraxxus creates a Nether Portal!", 0, true);
                    m_Summoners.push_back(pSummon->GetGUID());
                    break;
                case NPC_VOLCANO:
                    pSummon->GetMotionMaster()->MoveIdle();
                    pSummon->CastSpell(pSummon, DIFFICULTY(SPELL_INFERNAL_ERUPT), false);
                    m_Summoners.push_back(pSummon->GetGUID());
                    break;
                default:
                    break;
            }
        }
    }

    void DespawnSummons() // despawns portals and volcanoes only
    {
        for (GuidList::const_iterator i = m_Summoners.begin(); i != m_Summoners.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        m_Summoners.clear();
    }

    void JustDied(Unit* pSlayer)
    {
        DoScriptText(SAY_DEATH, m_creature, NULL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FEL_FIREBALL:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_FEL_FIREBALL));
                    Events.ScheduleEvent(EVENT_FEL_FIREBALL, FIREBALL_TIMER);
                    break;
                case EVENT_FEL_LIGHTNING:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(pTarget, DIFFICULTY(SPELL_FEL_LIGHTNING));
                    Events.ScheduleEvent(EVENT_FEL_LIGHTNING, LIGHTNING_TIMER);
                    break;
                case EVENT_INCINERATE_FLESH:
                    DoScriptText(SAY_INCINERATE, m_creature);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        m_creature->CastSpell(pTarget, DIFFICULTY(SPELL_INCINERATE_FLESH), false);
                    Events.ScheduleEvent(EVENT_INCINERATE_FLESH, INCINERATE_TIMER);
                    break;
                case EVENT_LEGION_FLAME:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCastSpellIfCan(pTarget, DIFFICULTY(SPELL_LEGION_FLAME));
                    Events.ScheduleEvent(EVENT_LEGION_FLAME, LEGION_FLAME_TIMER);
                    break;
                case EVENT_SUMMON_VOLCANO:
                    DoScriptText(SAY_SUMMON_INFERNO, m_creature);
                    m_creature->CastSpell(m_creature, DIFFICULTY(SPELL_INFERNAL_ERUPTION), false);
                    Events.ScheduleEvent(EVENT_SUMMON_VOLCANO, VOLCANO_TIMER);
                    break;
                case EVENT_SUMMON_PORTAL:
                    DoScriptText(SAY_SUMMON_MISTRESS_OF_PAIN, m_creature);
                    m_creature->CastSpell(m_creature, DIFFICULTY(SPELL_NETHER_PORTAL), false);
                    Events.ScheduleEvent(EVENT_SUMMON_PORTAL, PORTAL_TIMER);
                    break;
                case EVENT_BERSERK:
                    DoCastSpellIfCan(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_BUFF:
                    DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER);
                    Events.ScheduleEvent(EVENT_BUFF, BUFF_TIMER);
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
    SPELL_SHIVAN_SLASH_N10      = 66378,
    SPELL_SHIVAN_SLASH_N25      = 67097,
    SPELL_SHIVAN_SLASH_H10      = 67098,
    SPELL_SHIVAN_SLASH_H25      = 67099,
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

#define KISS_TIMER      urand(20,30)*IN_MILLISECONDS
#define SLASH_TIMER     urand(30,45)*IN_MILLISECONDS
#define SPIKE_TIMER     urand(15,30)*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_mistress_of_painAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsHeroic;
    bool m_bIs10Man;
    EventMap Events;
    InstanceVar<uint32> m_AchievementCounter;

    mob_mistress_of_painAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_AchievementCounter(DATA_ACHIEVEMENT_COUNTER, m_pInstance)
    {
        Difficulty diff = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
        m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
    }

    void Reset()
    {
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEvent(EVENT_KISS, KISS_TIMER);
        Events.ScheduleEvent(EVENT_SHIVAN_SLASH, SLASH_TIMER);
        Events.ScheduleEvent(EVENT_SPIKE, SPIKE_TIMER, 4000);
        ++m_AchievementCounter;
    }

    void JustDied(Unit* pSlayer)
    {
        if (!m_pInstance)
            return;
        --m_AchievementCounter;
        m_creature->ForcedDespawn();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->isTemporarySummon())
                static_cast<TemporarySummon*>(m_creature)->UnSummon();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_KISS:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        m_creature->CastSpell(pTarget, SPELL_MISTRESS_KISS, false);
                    Events.ScheduleEvent(EVENT_KISS, KISS_TIMER);
                    break;
                case EVENT_SHIVAN_SLASH:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SHIVAN_SLASH));
                    Events.ScheduleEvent(EVENT_SHIVAN_SLASH, SLASH_TIMER);
                    break;
                case EVENT_SPIKE:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        m_creature->CastSpell(pTarget, SPELL_SPINNING_PAIN_SPIKE, false);
                    Events.ScheduleEvent(EVENT_SPIKE, SPIKE_TIMER, 4000);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_jaraxxus_add_summonerAI: public Scripted_NoMovementAI
{
    mob_jaraxxus_add_summonerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
    }

    ScriptedInstance *m_pInstance;

    void Reset()
    {
    }

    void UpdateAI(uint32 const uiDiff)
    {
    }

    void Aggro(Unit *pWho)
    {
    }

    void JustSummoned(Creature *pSummon)
    {
        if (Unit* Jaraxxus = GET_CREATURE(TYPE_JARAXXUS))
            ((Creature*)Jaraxxus)->AI()->JustSummoned(pSummon);
        pSummon->SetInCombatWithZone();
    }

    void JustDied(Unit* pSlayer)
    {
        m_creature->ForcedDespawn();
    }
};

#define FELSTREAK_TIMER urand(15,30)*IN_MILLISECONDS
#define FEL_INFERNO_TIMER urand(15,30)*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_felflame_infernalAI: public ScriptedAI
{
    EventMap Events;

    mob_felflame_infernalAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
    }

    void Reset()
    {
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEvent(EVENT_FEL_INFERNO, FEL_INFERNO_TIMER, 6000);
        Events.ScheduleEvent(EVENT_FELSTREAK, FELSTREAK_TIMER, 7500);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->isTemporarySummon())
                static_cast<TemporarySummon*>(m_creature)->UnSummon();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FELSTREAK:
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        m_creature->CastSpell(pTarget, SPELL_FEL_STREAK, false);
                    Events.ScheduleEvent(EVENT_FELSTREAK, FELSTREAK_TIMER, 7500);
                    break;
                case EVENT_FEL_INFERNO:
                    m_creature->StopMoving();
                    m_creature->CastSpell(m_creature, SPELL_FEL_INFERNO, false);
                    Events.ScheduleEvent(EVENT_FEL_INFERNO, FEL_INFERNO_TIMER, 6000);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pSlayer)
    {
        m_creature->ForcedDespawn();
    }
};

struct MANGOS_DLL_DECL mob_legion_flameAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;

    mob_legion_flameAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_creature->SetDisplayId(11686);
        m_creature->CastSpell(m_creature, SPELL_LEGION_FLAMES, false);
    }

    void Reset()
    {
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_pInstance->IsEncounterInProgress())
            m_creature->ForcedDespawn();
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
