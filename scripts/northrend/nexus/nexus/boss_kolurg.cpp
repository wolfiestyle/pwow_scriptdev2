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
SDName: Boss Commander Kolurg
SD%Complete: 0
SDComment:
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"

#define SPELL_CHARGE              60067
#define SPELL_SHOUT               19134
#define SPELL_WHIRLWIND           38619

struct MANGOS_DLL_DECL boss_kolurgAI : public ScriptedAI
{
    boss_kolurgAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Charge_Timer;
    uint32 Shout_Timer;
    uint32 Whirlwind_Timer;

    void Reset() 
    {
        Charge_Timer = 15000;
        Shout_Timer = 30000;
        Whirlwind_Timer = 40000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Whirlwind timer
        if (Whirlwind_Timer < diff)
        {
            DoCast(m_creature,SPELL_WHIRLWIND);

            Whirlwind_Timer = 40000;
        }else Whirlwind_Timer -= diff;

        //Shout timer
        if (Shout_Timer < diff)
        {
            DoCast(m_creature,SPELL_SHOUT);

            Shout_Timer = 30000;
        }else Shout_Timer -= diff;

        //Charge timer
        if (Charge_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target,SPELL_CHARGE);

            Charge_Timer = 15000;
        }else Charge_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kolurg(Creature* pCreature)
{
    return new boss_kolurgAI(pCreature);
}

void AddSC_boss_kolurg()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_kolurg";
    newscript->GetAI = &GetAI_boss_kolurg;
    newscript->RegisterSelf();
}
