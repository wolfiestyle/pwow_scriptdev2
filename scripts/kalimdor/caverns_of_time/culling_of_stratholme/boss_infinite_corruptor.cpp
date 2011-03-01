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

struct MANGOS_DLL_DECL boss_infinite_corruptorAI : public ScriptedAI
{
    boss_infinite_corruptorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_INFINITE_CORRUPTER, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance->GetData(TYPE_INFINITE_CORRUPTER) == FAIL)
            m_creature->RemoveFromWorld();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_pInstance->GetData(TYPE_INFINITE_CORRUPTER) == FAIL)
        {
            if (m_creature->getVictim())
                DoStartNoMovement(m_creature->getVictim());
            m_creature->GetMotionMaster()->MoveTargetedHome();
        }
        if (m_pInstance->GetData(TYPE_EPOCH_EVENT) != DONE) // prevent exploiting to get here
            EnterEvadeMode();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

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
