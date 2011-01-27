/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_baltharus
SD%Complete:
SDComment:
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum Says
{
    XERESTRASZA_YELL01  = -1620000,
    XERESTRASZA_YELL02  = -1620001,
    XERESTRASZA_SAY01   = -1620002,
    XERESTRASZA_SAY02   = -1620003,
    XERESTRASZA_SAY03   = -1620004,
    XERESTRASZA_SAY04   = -1620005,
    XERESTRASZA_SAY05   = -1620006,
    XERESTRASZA_SAY06   = -1620007,
    XERESTRASZA_SAY07   = -1620008,

    BALTHARUS_AGGRO01   = -1620024,
    BALTHARUS_SLAY01    = -1620025,
    BALTHARUS_SLAY02    = -1620026,
    BALTHARUS_DEATH01   = -1620027,
    BALTHARUS_SPECIAL01 = -1620028,
    BALTHARUS_YELL01    = -1620029,
};

enum Events
{
    EVENT_SPLIT = 1,
    EVENT_TEMPEST,
    EVENT_CLEAVE,
    EVENT_ENERVATING_BRAND,
    EVENT_SPLIT_COMBO,
    EVENT_OUTRO,
};

enum Spells
{
    // main boss spells
    SPELL_CLONE             = 74511,
    SPELL_TEMPEST           = 75125,
    SPELL_CLEAVE            = 40504,
    SPELL_ENERVATING_BRAND  = 74502,
    SPELL_ENERVATE          = 74505,
    SPELL_REPELLING_WAVE    = 74509,
    SPELL_SIPHONED_MIGHT    = 74507,

    // clone spells
    //SPELL_TEMPEST           = 75125,
    //SPELL_CLEAVE            = 40504,
    //SPELL_EVERVATING_BRAND  = 74502,

    // visuals
    SPELL_SIMPLE_TELEPORT   = 70618,
    SPELL_BARRIER_CHANNEL   = 76221,
};

#define TIMER_CLEAVE            5*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define TIMER_TEMPEST           22*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_ENERVATING_BRAND  5*IN_MILLISECONDS, 8*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_xerestraszaAI: public ScriptedAI, ScriptEventInterface
{
    ScriptedInstance* m_pInstance;
    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;

    npc_xerestraszaAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_uiTalkPhase(0),
        m_uiTalkTimer(0)
    {
    }

    void Reset() {}

    void Aggro(Unit* pWho) {}

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_OUTRO:
                    switch(m_uiTalkPhase)
                    {
                        case 0:
                            m_uiTalkTimer = 5000;
                            break;
                        case 1:
                            DoScriptText(XERESTRASZA_YELL02, m_creature);
                            m_creature->GetMotionMaster()->MovePoint(0, 3152.57f, 375.05f, 85.91f);
                            m_uiTalkTimer = 9000;
                            break;
                        case 2:
                            DoScriptText(XERESTRASZA_SAY01, m_creature);
                            m_uiTalkTimer = 8000;
                            break;
                        case 3:
                            DoScriptText(XERESTRASZA_SAY02, m_creature);
                            m_uiTalkTimer = 7500;
                            break;
                        case 4:
                            DoScriptText(XERESTRASZA_SAY03, m_creature);
                            m_uiTalkTimer = 11500;
                            break;
                        case 5:
                            DoScriptText(XERESTRASZA_SAY04, m_creature);
                            m_uiTalkTimer = 10500;
                            break;
                        case 6:
                            DoScriptText(XERESTRASZA_SAY05, m_creature);
                            m_uiTalkTimer = 10500;
                            break;
                        case 7:
                            DoScriptText(XERESTRASZA_SAY06, m_creature);
                            m_uiTalkTimer = 8500;
                            break;
                        case 8:
                            DoScriptText(XERESTRASZA_SAY07, m_creature);
                            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER | UNIT_NPC_FLAG_GOSSIP);
                            // no break
                        default:
                            break;
                    }
                    if (m_uiTalkPhase < 8)
                        Events.ScheduleEvent(EVENT_OUTRO, m_uiTalkTimer);
                    m_uiTalkPhase++;
                    break;
            }
    }
};

struct MANGOS_DLL_DECL boss_baltharusAI: public boss_ruby_sanctumAI
{
    SummonManager SummonMgr;
    uint32 m_uiTalkTimer;
    bool m_bHasDoneIntro :1;
    bool m_bIsIntro :1;
    bool m_bSplit1 :1;
    bool m_bSplit2 :1;

    boss_baltharusAI(Creature* pCreature):
        boss_ruby_sanctumAI(pCreature),
        SummonMgr(pCreature),
        m_uiTalkTimer(0),
        m_bHasDoneIntro(false),
        m_bIsIntro(false),
        m_bSplit1(false),
        m_bSplit2(false)
    {
        if (pCreature->GetEntry() == NPC_BALTHARUS_CLONE)
            DoCast(m_creature, SPELL_SIMPLE_TELEPORT);
        else
            DoCast(m_creature, SPELL_BARRIER_CHANNEL, false);
    }

    void Reset()
    {
        m_bSplit1 = false;
        m_bSplit2 = false;
        SummonMgr.UnsummonAll();
        DoCast(m_creature, SPELL_BARRIER_CHANNEL, false);
        boss_ruby_sanctumAI::Reset();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_creature->GetEntry() == NPC_BALTHARUS_CLONE)
            m_creature->SetInCombatWithZone();
        else if (!m_bHasDoneIntro && !m_bIsIntro && pWho && pWho->GetTypeId() == TYPEID_PLAYER &&
            pWho->IsWithinDist(m_creature, 50.0f) && pWho->isTargetableForAttack())
        {
            if (Creature *Xerestrasza = GET_CREATURE(DATA_XERESTRASZA))
                DoScriptText(XERESTRASZA_YELL01, Xerestrasza);
            m_bIsIntro = true;
            m_uiTalkTimer = 6*IN_MILLISECONDS;
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (m_creature->GetEntry() == NPC_BALTHARUS)
            DoScriptText(BALTHARUS_AGGRO01, m_creature);
        Events.ScheduleEventInRange(EVENT_CLEAVE, TIMER_CLEAVE,TIMER_CLEAVE);
        Events.ScheduleEventInRange(EVENT_TEMPEST, TIMER_TEMPEST,TIMER_TEMPEST);
        Events.ScheduleEventInRange(EVENT_ENERVATING_BRAND, TIMER_ENERVATING_BRAND,TIMER_ENERVATING_BRAND);
        m_BossEncounter = IN_PROGRESS;
    }

    void JustSummoned(Creature* pSumm)
    {
        if (pSumm->GetEntry() == NPC_BALTHARUS_CLONE)
        {
            pSumm->SetHealthPercent(m_creature->GetHealthPercent());
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
        }
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0,1) ? BALTHARUS_SLAY01 : BALTHARUS_SLAY02, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_creature->GetEntry() == NPC_BALTHARUS) // original dies, all get unsummoned
        {
            if (Creature* Guardian = GET_CREATURE(DATA_XERESTRASZA))
                SendEventTo(Guardian, EVENT_OUTRO, 0);
            DoScriptText(BALTHARUS_DEATH01, m_creature);
            SummonMgr.UnsummonAll();
            m_BossEncounter = DONE;
            boss_ruby_sanctumAI::JustDied(pKiller);
        }
    }

    void UpdateTalk(uint32 const uiDiff)
    {
        m_uiTalkTimer -= uiDiff;
        if (m_uiTalkTimer <= uiDiff)
        {
            DoScriptText(BALTHARUS_YELL01, m_creature);
            m_bHasDoneIntro = true;
            m_bIsIntro = false;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bIsIntro && !m_bHasDoneIntro)
            UpdateTalk(uiDiff);
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CLEAVE:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                    break;
                case EVENT_TEMPEST:
                    DoCast(m_creature->getVictim(), SPELL_TEMPEST, false);
                    Events.DelayEventsWithId(EVENT_ENERVATING_BRAND, 7*IN_MILLISECONDS);
                    break;
                case EVENT_ENERVATING_BRAND:
                    if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 2)) // dont target tanks (if possible)
                        DoCast(Target, SPELL_ENERVATING_BRAND, false);
                    break;
                case EVENT_SPLIT_COMBO:
                    m_creature->InterruptNonMeleeSpells(true);
                    m_creature->RemoveAllAuras();
                    DoCast(m_creature, SPELL_REPELLING_WAVE, false);
                    Events.DelayEventsWithId(EVENT_CLEAVE, 10*IN_MILLISECONDS);
                    Events.DelayEventsWithId(EVENT_TEMPEST, 10*IN_MILLISECONDS);
                    Events.DelayEventsWithId(EVENT_ENERVATING_BRAND, 3*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_SPLIT, 1*IN_MILLISECONDS);
                    break;
                case EVENT_SPLIT:
                    DoScriptText(BALTHARUS_SPECIAL01, m_creature);
                    DoCast(m_creature, SPELL_CLONE, true);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (m_creature->GetEntry() == NPC_BALTHARUS_CLONE)
            return;

        if (m_bIs10Man)
        {
            if (!m_bSplit1 && m_creature->GetHealthPercent() <= 50.0f)
            {
                m_bSplit1 = true;
                Events.ScheduleEvent(EVENT_SPLIT_COMBO, 0);
            }
        }
        else
        {
            if (!m_bSplit1 && m_creature->GetHealthPercent() <= 66.67f)
            {
                m_bSplit1 = true;
                Events.ScheduleEvent(EVENT_SPLIT_COMBO, 0);
            }
            if (!m_bSplit2 && m_creature->GetHealthPercent() <= 33.33f)
            {
                m_bSplit2 = true;
                Events.ScheduleEvent(EVENT_SPLIT_COMBO, 0);
            }
        }
    }
};

void AddSC_boss_baltharus()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_baltharus);
    REGISTER_SCRIPT(npc_xerestrasza);
}
