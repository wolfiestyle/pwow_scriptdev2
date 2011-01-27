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
SDName: Boss_Malygos
SD%Complete: 0
SDComment:
SDCategory: Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "def_eye_of_eternity.h"

#define SPELL_ARCANE_BREATH           56272
#define SPELL_ARCANE_BREATH_H         60072
#define SPELL_VORTEX                  56105
#define SPELL_ARCANE_PULSE            57432
#define SPELL_ARCANE_STORM            61693
#define SPELL_ARCANE_STORM_H          61694
#define SPELL_STATIC_FIELD            57430
#define SPELL_SURGE_OF_POWER          56505
#define SPELL_SURGE_OF_POWER_H        57407
#define SPELL_DEEP_BREATH             0
#define SPELL_DEEP_BREATH_H           0
#define SPELL_ENRAGE                  0

#define NPC_POWER_SPARK               30084
#define NPC_NEXUS_LORD                30245
#define NPC_SCION_OF_ETERNITY         30249

#define SUMMON_X                      754.055786 + rand()%61 - 30
#define SUMMON_Y                      1301.669922 + rand()%61 - 30
#define SUMMON_Z                      266.170898 + rand()%61 - 30


struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 phase;
    uint32 Enrage_Timer;
    uint32 ArcaneBreath_Timer;
    uint32 ArcaneStorm_Timer;
    uint32 Vortex_Timer;
    uint32 PowerSpark_Timer;
    uint32 DeepBreath_Timer;
    uint32 StaticField_Timer;
    uint32 ArcanePulse_Timer;
    uint32 SurgeOfPower_Timer;
    uint64 NexusLord_GUID[2];
    uint64 ScionOfEternity_GUID[2];
    uint64 AlexstraszaGiftGUID;
    bool Phase2_Summons;

    void Reset() 
    {
        phase = 1;
        Enrage_Timer = 600000;        //10 minutes
        ArcaneBreath_Timer = 40000;   //40 seconds
        ArcaneStorm_Timer = 15000;    //15 seconds
        Vortex_Timer = 90000;         //90 seconds
        PowerSpark_Timer = 30000;     //30 seconds
        DeepBreath_Timer = 40000;     //40 seconds
        StaticField_Timer = 15000;    //15 seconds
        ArcanePulse_Timer = 30000;    //30 seconds
        SurgeOfPower_Timer = 40000;   //40 seconds
        AlexstraszaGiftGUID = 0;
        for (uint8 i=0;i<2;i++)
            NexusLord_GUID[i] = 0;
        for (uint8 i=0;i<2;i++)
            ScionOfEternity_GUID[i] = 0;

        //Remove flags
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
        {
            AlexstraszaGiftGUID = m_pInstance->GetData64(DATA_ALEXSTRASZAGIFT);
            if (AlexstraszaGiftGUID)
                m_pInstance->DoRespawnGameObject(AlexstraszaGiftGUID);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Phase 1
        if (phase == 1)
        {
            //Change phase
            if ((int) (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() +0.5) <= 50)
            {
                //Set Motion
                //m_creature->GetMotionMaster()->Clear(false);
                //m_creature->GetMotionMaster()->MoveIdle();
                //m_creature->SetHover(true);

                //Make unattackable
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Summon creatures
                for (uint8 i=0;i<2;i++)
                {
                    Creature* pSummon = m_creature->SummonCreature(NPC_NEXUS_LORD,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    NexusLord_GUID[i] = pSummon->GetGUID();
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        pSummon->TauntApply(target);
                }
                for (uint8 i=0;i<2;i++)
                {
                    Creature* pSummon = m_creature->SummonCreature(NPC_SCION_OF_ETERNITY,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    ScionOfEternity_GUID[i] = pSummon->GetGUID();
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        pSummon->TauntApply(target);
                }

                //Set to phase 2 and terminate phase 1
                phase = 2;
                return;
            }

            //Vortex timer
            if (Vortex_Timer < diff)
            {
                DoCast(m_creature, SPELL_VORTEX);

                Vortex_Timer = 90000;
            }else Vortex_Timer -= diff;

            //Arcane Breath timer
            if (ArcaneBreath_Timer < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H);
    
                ArcaneBreath_Timer = 40000;
            }else ArcaneBreath_Timer -= diff;

            //Power Spark timer
            if (PowerSpark_Timer < diff)
            {
                Creature* pSummon = m_creature->SummonCreature(NPC_POWER_SPARK,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                pSummon->GetMotionMaster()->Clear(false);
                pSummon->GetMotionMaster()->MoveFollow(m_creature,0,0);

                PowerSpark_Timer = 30000;
            }else PowerSpark_Timer -= diff;

            //Arcane Storm timer
            if (ArcaneStorm_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
    
                ArcaneStorm_Timer = 15000;
            }else ArcaneStorm_Timer -= diff;
        }

        //Phase 2
        if (phase == 2)
        {
            //Change phase
            {
                Phase2_Summons = false;
                for (uint8 i=0;i<2;i++)
                {
                    if (Creature* pSummon = m_creature->GetMap()->GetCreature(NexusLord_GUID[i]))
                        if (pSummon->isAlive())
                            Phase2_Summons = true;
                }
                for (uint8 i=0;i<2;i++)
                {
                    if (Creature* pSummon = m_creature->GetMap()->GetCreature(ScionOfEternity_GUID[i]))
                        if (pSummon->isAlive())
                            Phase2_Summons = true;
                }
                if (!Phase2_Summons)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    phase = 3;
                    return;
                }
            }

            //Deep Breath timer
            /*
            if (DeepBreath_Timer < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_DEEP_BREATH : SPELL_DEEP_BREATH_H);
    
                DeepBreath_Timer = 40000;
            }else DeepBreath_Timer -= diff;
            */

            //Arcane Storm timer
            if (ArcaneStorm_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
    
                ArcaneStorm_Timer = 15000;
            }else ArcaneStorm_Timer -= diff;
        }

        //Phase 3
        if (phase == 3)
        {
            //Surge of Power timer
            if (SurgeOfPower_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_SURGE_OF_POWER : SPELL_SURGE_OF_POWER_H);
    
                SurgeOfPower_Timer = 40000;
            }else SurgeOfPower_Timer -= diff;

            //Arcane Pulse timer
            if (ArcanePulse_Timer < diff)
            {
                DoCast(m_creature, SPELL_ARCANE_PULSE);
    
                ArcanePulse_Timer = 30000;
            }else ArcanePulse_Timer -= diff;

            //Static Field timer
            if (StaticField_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_STATIC_FIELD);
    
                StaticField_Timer = 15000;
            }else StaticField_Timer -= diff;
        }

        //Enrage timer
        /*
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
    
            Enrage_Timer = 600000;
        }else Enrage_Timer -= diff;
        */

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}

bool GOHello_go_TheFocusingIris(Player* pPlayer, GameObject* pGO)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        uint64 MalygosGUID = m_pInstance->GetData64(DATA_MALYGOS);
        if (MalygosGUID)
        {
			if (Creature* Malygos = pGO->GetMap()->GetCreature(MalygosGUID))
			{
				Malygos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6);
				Malygos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
				Malygos->TauntApply(pPlayer);
			}
        }
    }
    return false;
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_the_focusing_iris";
    newscript->pGOHello = &GOHello_go_TheFocusingIris;
    newscript->RegisterSelf();
}
