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
SDName: Boss_Heigan
SD%Complete: 70%
SDComment: Verify
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_AGGRO1      = -1533109,
    SAY_AGGRO2      = -1533110,
    SAY_AGGRO3      = -1533111,
    SAY_SLAY        = -1533112,
    SAY_TAUNT1      = -1533113,
    SAY_TAUNT2      = -1533114,
    SAY_TAUNT3      = -1533115,
    SAY_TAUNT4      = -1533116,
    SAY_TAUNT5      = -1533117,
    SAY_DEATH       = -1533118,

    //Spell used by floor peices to cause damage to players
    SPELL_ERUPTION  = 29371,

    //Spells by boss
    SPELL_DISRUPTION = 29310,
    SPELL_FEVER      = 29998,
    SPELL_FEVER_H    = 55011,
    SPELL_PLAGUE     = 30122
};

//Teleport to Platform Positions
#define TELE_X         2793.860107
#define TELE_Y         -3707.379883
#define TELE_Z         276.627014
#define TELE_O         0.593412


struct MANGOS_DLL_DECL boss_heiganAI : public ScriptedAI
{
    boss_heiganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Disruption_Timer;
    uint32 Fever_Timer;
    uint32 Eruption1_Timer;
    uint32 Eruption2_Timer;
    uint32 Say_Timer;
    uint32 Phase1to2_Timer;
    uint32 Phase2to1_Timer;
    uint32 phase;

    void Reset()
    {
        Disruption_Timer = 15000;           //15 seconds
        Fever_Timer = 40000;                //40 seconds
        Eruption1_Timer = 8000;             //8 seconds
        Eruption2_Timer = 2500;             //2.5 seconds
        Say_Timer = (rand()%30+15)*1000;    //15-45 seconds
        Phase1to2_Timer = 90000;            //90 seconds
        Phase2to1_Timer = 45000;            //45 seconds
        phase = 1;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        //Say some stuff
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, IN_PROGRESS);
    }

    void KilledUnit()
    {
        //Say some stuff
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        //Say some stuff
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_HEIGAN, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Say some stuff
        if (Say_Timer < diff)
        {
            switch(rand()%4)
            {
                case 0: DoScriptText(SAY_TAUNT1, m_creature); break;
                case 1: DoScriptText(SAY_TAUNT2, m_creature); break;
                case 2: DoScriptText(SAY_TAUNT3, m_creature); break;
                case 3: DoScriptText(SAY_TAUNT5, m_creature); break;
            }

            Say_Timer = (rand()%30+15)*1000;
        }else Say_Timer -= diff;

        //Phase 1
        if (phase == 1)
        {
            //Phase 1 to 2 timer
			/*
            if (Phase1to2_Timer < diff)
            {
                //Teleport to platform
                m_creature->NearTeleportTo(TELE_X,TELE_Y, TELE_Z, TELE_O);

                //Say some stuff
                DoScriptText(SAY_TAUNT4, m_creature);

                //We switch to phase 2
                phase = 2;

                //Reset timer
                Phase1to2_Timer = 90000;

                //Terminate phase 1
                return;
            }else Phase1to2_Timer -= diff;
			*/

            //Spell Disruption timer
            if (Disruption_Timer < diff)
            {
                DoCast(m_creature,SPELL_DISRUPTION);

                Disruption_Timer = 15000;
            }else Disruption_Timer -= diff;

            //Decrepit Fever timer
            if (Fever_Timer < diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target,m_bIsRegularMode ? SPELL_FEVER : SPELL_FEVER_H);

                Fever_Timer = 40000;
            }else Fever_Timer -= diff;

            //Eruption timer
            /*
            if (Eruption1_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_ERUPTION);

                Eruption1_Timer = 8000;
            }else Eruption1_Timer -= diff;
            */

            DoMeleeAttackIfReady();
        }

        //Phase 2
        if (phase == 2)
        {
            //Phase 2 to 1 timer
            if (Phase2to1_Timer < diff)
            {
                //Enable movement
                DoStartMovement(m_creature->getVictim());

                //We switch to phase 1
                phase = 1;

                //Reset timer
                Phase2to1_Timer = 45000;

                //Terminate phase 2
                return;
            }else Phase2to1_Timer -= diff;

            //Disable movement
            DoStartNoMovement(m_creature->getVictim());

            //Eruption timer
            /*
            if (Eruption2_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_ERUPTION);

                Eruption2_Timer = 2500;
            }else Eruption2_Timer -= diff;
            */

            //Heigan channels this spell throughout phase 2
            DoCast(m_creature,SPELL_PLAGUE);
        } 
    }
};

CreatureAI* GetAI_boss_heigan(Creature* pCreature)
{
    return new boss_heiganAI(pCreature);
}

void AddSC_boss_heigan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_heigan";
    newscript->GetAI = &GetAI_boss_heigan;
    newscript->RegisterSelf();
}
