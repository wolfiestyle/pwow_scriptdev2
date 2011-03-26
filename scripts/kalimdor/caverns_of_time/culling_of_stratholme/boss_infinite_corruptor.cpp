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
SDName: Boss Infinite Corruptor
SD%Complete: 0
SDComment: placeholder
SDCategory: Caverns of Time, Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

#define INF_CORR_SAY_AGGRO      "How dare you interfere with our work here!"
#define INF_CORR_SAY_DEPARTURE  "My work here is finished!"
#define INF_CORR_SAY_DEATH      "My time... has run out..."

enum Spells
{
    SPELL_CORRUPTING_BLIGHT     = 60588,
    SPELL_VOID_STRIKE           = 60590,
};

enum Events
{
    EVENT_BLIGHT = 1,
    EVENT_VOID_STRIKE,
};

struct MANGOS_DLL_DECL boss_infinite_corruptorAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;
    bool m_bIsRegularMode;
    bool m_bYelledReturn;

    boss_infinite_corruptorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bYelledReturn = false;
        Reset();
    }

    void Aggro(Unit* pWho)
    {
        m_creature->MonsterYell(INF_CORR_SAY_AGGRO, LANG_UNIVERSAL, NULL);
        Events.ScheduleEventInRange(EVENT_BLIGHT, 25*IN_MILLISECONDS, 30*IN_MILLISECONDS,20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_VOID_STRIKE, 5*IN_MILLISECONDS, 10*IN_MILLISECONDS, 7*IN_MILLISECONDS, 12*IN_MILLISECONDS);
    }

    void Reset()
    {
        m_bYelledReturn = false;
        Events.Reset();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->MonsterYell(INF_CORR_SAY_DEATH, LANG_UNIVERSAL, NULL);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INFINITE_CORRUPTER, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance->GetData(TYPE_INFINITE_CORRUPTER) == FAIL)
        {
            if (!m_bYelledReturn)
            {
                m_bYelledReturn = true;
                m_creature->MonsterYell(INF_CORR_SAY_DEPARTURE, LANG_UNIVERSAL, NULL);
            }
            m_creature->RemoveFromWorld();
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_pInstance->GetData(TYPE_INFINITE_CORRUPTER) == FAIL)
        {
            if (m_creature->getVictim())
                DoStartNoMovement(m_creature->getVictim());
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->MoveTargetedHome();
        }
        if (m_pInstance->GetData(TYPE_EPOCH_EVENT) != DONE) // prevent exploiting to get here
            EnterEvadeMode();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BLIGHT:
                    m_creature->CastSpell(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0), SPELL_CORRUPTING_BLIGHT, true);
                    break;
                case EVENT_VOID_STRIKE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_VOID_STRIKE, false);
                    break;
            }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_infinite_corruptor(Creature* pCreature)
{
    return new boss_infinite_corruptorAI(pCreature);
}

void AddSC_boss_infinite_corruptor()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_infinite_corruptor";
    pNewScript->GetAI = &GetAI_boss_infinite_corruptor;
    pNewScript->RegisterSelf();
}
