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
SDName: Boss Drakos the Interrogator
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"

#define SPELL_THUNDERING_STOMP              50774
#define SPELL_THUNDERING_STOMP_H            59370
#define SPELL_MAGIC_PULL                    51336

struct MANGOS_DLL_DECL boss_drakosAI : public ScriptedAI
{
    boss_drakosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 ThunderingStomp_Timer;
    uint32 MagicPull_Timer;

    void Reset() 
    {
        ThunderingStomp_Timer = 15000;
        MagicPull_Timer = 30000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Magic Pull timer
        if (MagicPull_Timer < diff)
        {
            DoCast(m_creature,SPELL_MAGIC_PULL);

            MagicPull_Timer = 30000;
        }else MagicPull_Timer -= diff;

        //Thundering Stomp timer
        if (ThunderingStomp_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_THUNDERING_STOMP : SPELL_THUNDERING_STOMP_H);

            ThunderingStomp_Timer = 15000;
        }else ThunderingStomp_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakos(Creature* pCreature)
{
    return new boss_drakosAI(pCreature);
}

void AddSC_boss_drakos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_drakos";
    newscript->GetAI = &GetAI_boss_drakos;
    newscript->RegisterSelf();
}
