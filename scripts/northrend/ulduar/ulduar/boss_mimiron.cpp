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
SDName: Boss Mimiron
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Says
{
    SAY_AGGRO                   = -1300085,
    SAY_HARD_MODE               = -1300086,
    SAY_BERSERK                 = -1300087,
    SAY_LEV_ACTIVATED           = -1300088,
    SAY_LEV_KILL_PLAYER1        = -1300089,
    SAY_LEV_KILL_PLAYER2        = -1300090,
    SAY_LEV_DEATH               = -1300091,
    SAY_VX_ACTIVATED            = -1300092,
    SAY_VX_KILLED_PLAYER1       = -1300093,
    SAY_VX_KILLED_PLAYER2       = -1300094,
    SAY_VX_DEATH                = -1300095,
    SAY_AERIAL_ACTIVATED        = -1300096,
    SAY_AERIAL_KILLED_PLAYER1   = -1300097,
    SAY_AERIAL_KILLED_PLAYER2   = -1300098,
    SAY_AERIAL_DEATH            = -1300099,
    SAY_V07_ACTIVATED           = -1300100,
    SAY_V07_KILLED_PLAYER1      = -1300101,
    SAY_V07_KILLED_PLAYER2      = -1300102,
    SAY_DEFEATED                = -1300103,
    SAY_YOGG_HELP               = -1300104
};

struct MANGOS_DLL_DECL boss_mimironAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    boss_mimironAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_MIMIRON)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEFEATED, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
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

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mimiron(Creature* pCreature)
{
    return new boss_mimironAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_mimiron";
    NewScript->GetAI = &GetAI_boss_mimiron;
    NewScript->RegisterSelf();
}
