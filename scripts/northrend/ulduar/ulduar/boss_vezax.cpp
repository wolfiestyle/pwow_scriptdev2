/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss General Vezax
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_AURA_OF_DESPAIR       = 62692,
    SPELL_SHADOW_CRASH          = 62660,
    SPELL_MARK_OF_THE_FACELESS  = 63276,
    SPELL_SEARING_FLAMES        = 62661,
    SPELL_SURGE_OF_DARKNESS     = 62662,
    SPELL_SARONITE_BARRIER      = 63364
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_SHADOW_CRASH,
    EVENT_MARK_OF_THE_FACELESS,
    EVENT_SEARING_FLAMES,
    EVENT_SURGE_OF_DARKNESS
};

enum Says
{
    SAY_AGGRO               = -1300154,
    SAY_HARD_MODE           = -1300155,
    SAY_SURGE_OF_DARKNESS   = -1300156,
    SAY_KILLED_PLAYER1      = -1300157,
    SAY_KILLED_PLAYER2      = -1300158,
    SAY_BERSERK             = -1300159,
    SAY_DEATH               = -1300160
};

#define BERSERK_TIMER               10*MINUTE*IN_MILLISECONDS
#define SHADOW_CRASH_TIMER          urand(15, 20)*IN_MILLISECONDS
#define MARK_OF_THE_FACELESS_TIMER  urand(15, 20)*IN_MILLISECONDS
#define SEARING_FLAMES_TIMER        urand(20, 30)*IN_MILLISECONDS
#define SURGE_OF_DARKNESS_TIMER     urand(60, 70)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_vezaxAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    boss_vezaxAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_VEZAX)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_SHADOW_CRASH, SHADOW_CRASH_TIMER);
        events.RescheduleEvent(EVENT_MARK_OF_THE_FACELESS, MARK_OF_THE_FACELESS_TIMER);
        events.RescheduleEvent(EVENT_SEARING_FLAMES, SEARING_FLAMES_TIMER);
        events.RescheduleEvent(EVENT_SURGE_OF_DARKNESS, SURGE_OF_DARKNESS_TIMER);
        DoCast(m_creature, SPELL_AURA_OF_DESPAIR, true);
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    Unit* SelectRangedTarget(float fMinDist)
    {
        std::vector<Unit*> melee_targets, ranged_targets;
        ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator it = tlist.begin(); it != tlist.end(); it++)
        {
            Unit *target = m_creature->GetMap()->GetUnit((*it)->getUnitGuid());
            if (target && target->GetTypeId() == TYPEID_PLAYER && m_creature->IsInMap(target))
            {
                if (m_creature->IsWithinDist(target, fMinDist))
                    melee_targets.push_back(target);
                else
                    ranged_targets.push_back(target);
            }
        }
        if (!ranged_targets.empty())
            return ranged_targets[rand() % ranged_targets.size()];
        else if (!melee_targets.empty())
            return melee_targets[rand() % melee_targets.size()];
        else
            return NULL;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_SHADOW_CRASH:
                    if (Unit *target = SelectRangedTarget(15.0f))
                        DoCast(target, SPELL_SHADOW_CRASH);
                    events.ScheduleEvent(EVENT_SHADOW_CRASH, SHADOW_CRASH_TIMER);
                    break;
                case EVENT_MARK_OF_THE_FACELESS:
                    if (Unit *target = SelectRangedTarget(15.0f))
                        DoCast(target, SPELL_MARK_OF_THE_FACELESS);
                    events.ScheduleEvent(EVENT_MARK_OF_THE_FACELESS, MARK_OF_THE_FACELESS_TIMER);
                    break;
                case EVENT_SEARING_FLAMES:
                    DoCast(m_creature->getVictim(), SPELL_SEARING_FLAMES);
                    events.ScheduleEvent(EVENT_SEARING_FLAMES, SEARING_FLAMES_TIMER);
                    break;
                case EVENT_SURGE_OF_DARKNESS:
                    DoScriptText(SAY_SURGE_OF_DARKNESS, m_creature);
                    DoCast(m_creature, SPELL_SURGE_OF_DARKNESS);
                    events.ScheduleEvent(EVENT_SURGE_OF_DARKNESS, SURGE_OF_DARKNESS_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vezax(Creature* pCreature)
{
    return new boss_vezaxAI(pCreature);
}

void AddSC_boss_vezax()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_vezax";
    NewScript->GetAI = &GetAI_boss_vezax;
    NewScript->RegisterSelf();
}
