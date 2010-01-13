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
SDName: Boss Varos Cloudstrider
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"

#define SPELL_ENERGIZE_CORES              50785
#define SPELL_ENERGIZE_CORES_H            50785
#define SPELL_CALL_CAPTAIN                51002
#define SPELL_CALL_CAPTAIN_H              51002
#define SPELL_AMPLIFY_MAGIC               51054
#define SPELL_AMPLIFY_MAGIC_H             59371

struct MANGOS_DLL_DECL boss_varosAI : public ScriptedAI
{
    boss_varosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 EnergizeCores_Timer;
    uint32 CallCaptain_Timer;
    uint32 AmplifyMagic_Timer;

    void Reset() 
    {
        EnergizeCores_Timer = 15000;
        CallCaptain_Timer = 30000;
        AmplifyMagic_Timer = 40000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Amplify Magic timer
        if (AmplifyMagic_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_AMPLIFY_MAGIC : SPELL_AMPLIFY_MAGIC_H);

            AmplifyMagic_Timer = 40000;
        }else AmplifyMagic_Timer -= diff;

        //Call Azure Ring Captain timer
        if (CallCaptain_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CALL_CAPTAIN : SPELL_CALL_CAPTAIN_H);

            CallCaptain_Timer = 30000;
        }else CallCaptain_Timer -= diff;

        //Energize Cores timer
        if (EnergizeCores_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ENERGIZE_CORES : SPELL_ENERGIZE_CORES_H);

            EnergizeCores_Timer = 15000;
        }else EnergizeCores_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_varos(Creature* pCreature)
{
    return new boss_varosAI(pCreature);
}

void AddSC_boss_varos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_varos";
    newscript->GetAI = &GetAI_boss_varos;
    newscript->RegisterSelf();
}
