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
SDName: Boss_Sapphiron
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    EMOTE_BREATH       = -1533082,
    EMOTE_ENRAGE       = -1533083,

    SPELL_ICEBOLT      = 28522,
    SPELL_ICEBOLT_H    = 28526,
    SPELL_FROST_BREATH = 29318,
    SPELL_FROST_BREATH_H = 28524,
    SPELL_FROST_AURA   = 28531,
    SPELL_FROST_AURA_H = 55799,
    SPELL_LIFE_DRAIN   = 28542,
    SPELL_LIFE_DRAIN_H = 55665,
    SPELL_BLIZZARD     = 28547,
    SPELL_BLIZZARD_H   = 55699,
    SPELL_BESERK       = 26662,
    SPELL_CLEAVE       = 19983,
    SPELL_TAIL_SWEEP   = 55697,
    SPELL_TAIL_SWEEP_H = 55696
};

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Icebolt_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    bool FrostBreath_Check;
    uint32 FrostAura_Timer;
    uint32 LifeDrain_Timer;
    uint32 Blizzard_Timer;
    uint32 Cleave_Timer;
    uint32 TailSweep_Timer;
    uint32 Fly_Timer;
    uint32 Beserk_Timer;
    uint32 phase;
    uint32 Phase1to2_Timer;
    uint32 Phase2to1_Timer;

    void Reset()
    {
        Icebolt_Count = 0;
        Icebolt_Timer = 4000;                     //4 seconds
        FrostBreath_Timer = 15000;                //15 seconds, 7 seconds after seconds Ice Bolt
        FrostBreath_Check = true;
        FrostAura_Timer = 2000;                   //2 seconds
        LifeDrain_Timer = 24000;                  //24 seconds
        Blizzard_Timer = 20000;                   //20 seconds
        Cleave_Timer = 15000 + rand()%30000;      //15-45 seconds
        TailSweep_Timer = 15000 + rand()%30000;   //15-45 seconds
        Beserk_Timer = 900000;                    //900 seconds or 15 minutes
        phase = 1;
        Phase1to2_Timer = 45000;                  //45 seconds
        Phase2to1_Timer = 17000;                  //17 seconds, 2 seconds after Frost Breath

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, FAIL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (phase == 1)
        {
			/*
            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 10)
            {
                if (Phase1to2_Timer < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->SetHover(true);

                    Phase1to2_Timer = 45000;
                    phase = 2;

                    //Terminate phase 1
                    return;
                }else Phase1to2_Timer -= uiDiff;
            }
			*/

            if (Cleave_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(),SPELL_CLEAVE);
                Cleave_Timer = 15000 + rand()%30000;
            }else Cleave_Timer -= uiDiff;

            if (TailSweep_Timer < uiDiff)
            {
				DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_TAIL_SWEEP : SPELL_TAIL_SWEEP_H);
                TailSweep_Timer = 15000 + rand()%30000;
            }else TailSweep_Timer -= uiDiff;

            if (LifeDrain_Timer < uiDiff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, m_bIsRegularMode ? SPELL_LIFE_DRAIN : SPELL_LIFE_DRAIN_H);

                LifeDrain_Timer = 24000;
            }else LifeDrain_Timer -= uiDiff;

            if (Blizzard_Timer < uiDiff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, m_bIsRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H);

                Blizzard_Timer = 20000;
            }else Blizzard_Timer -= uiDiff;

            if (FrostAura_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_AURA : SPELL_FROST_AURA_H);
                FrostAura_Timer = 5000;
            }else FrostAura_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        //Phase 2
        if (phase == 2)
        {
            if (Phase2to1_Timer < uiDiff)
            {
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                m_creature->SetHover(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                Icebolt_Count = 0;
                FrostBreath_Check = true;
                Phase2to1_Timer = 17000;
                phase = 1;

                 //Terminate phase 2
                 return;
             }else Phase2to1_Timer -= uiDiff;

            if (Icebolt_Count < 2)
            {
                if (Icebolt_Timer < uiDiff)
                {
                    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
						DoCast(target, m_bIsRegularMode ? SPELL_ICEBOLT : SPELL_ICEBOLT_H);

                    Icebolt_Count++;
                    Icebolt_Timer = 4000;
                }else Icebolt_Timer -= uiDiff;
            }

            if (FrostBreath_Check)
            {
                if (FrostBreath_Timer < uiDiff)
                {
                    DoScriptText(EMOTE_BREATH, m_creature);
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_BREATH : SPELL_FROST_BREATH_H);

                    FrostBreath_Check = false;
                    FrostBreath_Timer = 15000;
                }else FrostBreath_Timer -= uiDiff;
            }
        }

        if (Beserk_Timer < uiDiff)
        {
            DoScriptText(EMOTE_ENRAGE, m_creature);
            DoCast(m_creature,SPELL_BESERK);
            Beserk_Timer = 300000;
        }else Beserk_Timer -= uiDiff;
    }   
};

CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI(pCreature);
}

void AddSC_boss_sapphiron()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_sapphiron";
    NewScript->GetAI = &GetAI_boss_sapphiron;
    NewScript->RegisterSelf();
}
