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
SDName: Boss_Noth
SD%Complete: 40
SDComment: Missing Balcony stage
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_AGGRO1                          = -1533075,
    SAY_AGGRO2                          = -1533076,
    SAY_AGGRO3                          = -1533077,
    SAY_SUMMON                          = -1533078,
    SAY_SLAY1                           = -1533079,
    SAY_SLAY2                           = -1533080,
    SAY_DEATH                           = -1533081,

    SPELL_BLINK                         = 29211,            //29208, 29209 and 29210 too
    SPELL_CRIPPLE                       = 29212,
    SPELL_CRIPPLE_H                     = 54814,
    SPELL_CURSE_PLAGUEBRINGER           = 29213,
    SPELL_CURSE_PLAGUEBRINGER_H         = 54835,
    BERSERK                             = 64238,

    SPELL_SUMMON_CHAMPION_AND_CONSTRUCT = 29240,
    SPELL_SUMMON_GUARDIAN_AND_CONSTRUCT = 29269,

    NPC_PLAGUED_WARRIOR                 = 16984,
    NPC_PLAGUED_CHAMPION                = 16983,
    NPC_PLAGUED_GUARDIAN                = 16981
};

uint32 m_auiSpellSummonPlaguedWarrior[]=
{
    29247, 29248, 29249
};

uint32 m_auiSpellSummonPlaguedChampion[]=
{
    29217, 29224, 29225, 29227, 29238, 29255, 29257, 29258, 29262, 29267
};

uint32 m_auiSpellSummonPlaguedGuardian[]=
{
    29226, 29239, 29256, 29268
};

// Teleport position of Noth on his balcony
#define TELE_X 2631.370
#define TELE_Y -3529.680
#define TELE_Z 274.040
#define TELE_O 6.277

//Summon positions (Phase 1)
#define ADD_X 2684.804 + rand()%21-10
#define ADD_Y -3502.517 + rand()%21-10
#define ADD_Z 261.313

struct MANGOS_DLL_DECL boss_nothAI : public ScriptedAI
{
    boss_nothAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Phase_Timer;      //Timer for Phase 1 and 2
    uint8 Phase_Counter;     //Counter for Phase 2
    uint8 Phase;             //Current Phase
    bool Enraged;            //Check if boss is enraged

    uint32 Curse_Timer;      //Timer for Curse of the Plaguebringer
    uint32 Blink_Timer;      //Timer for Blink
    uint32 Cripple_Timer;    //Timer for Cripple
    uint32 Summon_Timer;     //Timer for summons

    uint32 Wave_Timer;       //Timer for waves
    uint8 Wave_Counter;      //Counter for waves

    //coords before port so we can get back
    float dX, dY, dZ, dO;

    void Reset()
    {
        Phase = 1;
        Phase_Counter = 0;
        Phase_Timer = 110000;    //110 seconds
        Curse_Timer = 45000;     //45 seconds
        Blink_Timer = 20500;     //20.5 seconds, 0.5 seconds after Cripple
        Cripple_Timer = 20000;   //20 seconds
        Summon_Timer = 30000;    //30 seconds
        Wave_Timer = 0;          //instant
        Wave_Counter = 0;
        Enraged = false;

        //Remove flags
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOTH, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOTH, IN_PROGRESS);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AddThreat(pTarget);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1)?SAY_SLAY1:SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOTH, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NOTH, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Phase 1
        if (Phase == 1)
        {
            //Phase Timer
            if (Phase_Timer < uiDiff && !Enraged)
            {
                //Make unattackable
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Stop attack and remove auras
                m_creature->AttackStop();
                m_creature->RemoveAllAuras();

                //Get current position
                dX = m_creature->GetPositionX();
                dY = m_creature->GetPositionY();
                dZ = m_creature->GetPositionZ();
                dO = m_creature->GetOrientation();

                //Teleport to balcony
                m_creature->NearTeleportTo(TELE_X,TELE_Y, TELE_Z, TELE_O);

                //We switch to phase 2
                Phase = 2;

                //Reset timer
                Phase_Timer = 70000;

                //Increase Phase Counter
                Phase_Counter++;

                //Terminate phase 1
                return;
            }else Phase_Timer -= uiDiff;

            //Curse_Timer
            if (Curse_Timer < uiDiff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
					DoCast(target, m_bIsRegularMode ? SPELL_CURSE_PLAGUEBRINGER : SPELL_CURSE_PLAGUEBRINGER_H);
                Curse_Timer = 45000;
            }
            else Curse_Timer -= uiDiff;

            //Cripple Timer
            if (Cripple_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_CRIPPLE : SPELL_CRIPPLE_H);
                Cripple_Timer = 20000;
				Blink_Timer = 500;   //Just in case
            }
            else Cripple_Timer -= uiDiff;

            //Blink Timer
            if (Blink_Timer < uiDiff)
            {
                DoCast(m_creature, SPELL_BLINK);
                Blink_Timer = 20500;
            }
            else Blink_Timer -= uiDiff;

            //Summon_Timer
            if (Summon_Timer < uiDiff)
            {
                DoScriptText(SAY_SUMMON, m_creature);

				for(uint8 i = 0; i < (m_bIsRegularMode ? 2 : 3); i++)
                    m_creature->SummonCreature(NPC_PLAGUED_WARRIOR,ADD_X,ADD_Y,ADD_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);

                Summon_Timer = 30000;
            }
            else Summon_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        //Phase 2
        if (Phase == 2)
        {
            //Phase Timer
            if (Phase_Timer < uiDiff)
            {
                //Teleport to previous position
                m_creature->NearTeleportTo(dX,dY,dZ,dO);

                //Make attackable
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Enable movement
                DoStartMovement(m_creature->getVictim());

                //We switch to phase 1
                Phase = 1;

                //Reset timer
                Phase_Timer = 110000;

                //Reset wave timer and counter
                Wave_Timer = 0;
                Wave_Counter = 0;

                //Check for berserk
                if (Phase_Counter >= 3)
                {
                    DoCast(m_creature, BERSERK);
                    Enraged = true;
                }

                //Terminate phase 2
                return;
            }else Phase_Timer -= uiDiff;

            //Disable movement
            DoStartNoMovement(m_creature->getVictim());

            if(Wave_Timer < uiDiff && Wave_Counter < 2)
            {
				for(uint8 i = 0; i < ((3 - Phase_Counter) * (m_bIsRegularMode ? 1 : 2)); i++)
                    m_creature->SummonCreature(NPC_PLAGUED_CHAMPION,ADD_X,ADD_Y,ADD_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);

                for(uint8 i = 0; i < ((Phase_Counter - 1) * (m_bIsRegularMode ? 1 : 2)); i++)
                    m_creature->SummonCreature(NPC_PLAGUED_GUARDIAN,ADD_X,ADD_Y,ADD_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                
                Wave_Timer = 35000;
                Wave_Counter++;
            }
            else Wave_Timer -= uiDiff;
        }
    }
};

struct MANGOS_DLL_DECL noth_championAI : public ScriptedAI
{
    noth_championAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 MortalStrike_Timer;                            
    uint32 Shadow_Timer;                               
    
    void Reset()
    {
        MortalStrike_Timer = 15000;                          
        Shadow_Timer = 25000;               
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (MortalStrike_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),32736);
            MortalStrike_Timer = 15000;
        }else MortalStrike_Timer -= uiDiff;

        if (Shadow_Timer < uiDiff)
        {
           
            DoCast(m_creature->getVictim(),30138);

            Shadow_Timer = 25000;
        }else Shadow_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL noth_warrAI : public ScriptedAI
{
    noth_warrAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 cleave_timer;                                                          

    void Reset()
    {
        cleave_timer = 13000;                                     
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (cleave_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),15496);
            cleave_timer = 15000;
        }else cleave_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL noth_guardAI : public ScriptedAI
{
    noth_guardAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 arcane_timer;                                                          

    void Reset()
    {
        arcane_timer = 9000;                                     
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (arcane_timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),15496);
            arcane_timer = 15000;
        }else arcane_timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_noth_champion(Creature* pCreature)
{
    return new noth_championAI(pCreature);
}

CreatureAI* GetAI_noth_warr(Creature* pCreature)
{
    return new noth_warrAI(pCreature);
}

CreatureAI* GetAI_noth_guard(Creature* pCreature)
{
    return new noth_guardAI(pCreature);
}

CreatureAI* GetAI_boss_noth(Creature* pCreature)
{
    return new boss_nothAI(pCreature);
}

void AddSC_boss_noth()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_noth";
    NewScript->GetAI = &GetAI_boss_noth;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "noth_warr";
    NewScript->GetAI = &GetAI_noth_warr;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "noth_champion";
    NewScript->GetAI = &GetAI_noth_champion;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "noth_guard";
    NewScript->GetAI = &GetAI_noth_guard;
    NewScript->RegisterSelf();
}
