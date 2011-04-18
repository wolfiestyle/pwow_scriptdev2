/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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

enum Spells
{
    SPELL_ARCANE_BREATH         = 56272,
    SPELL_ARCANE_BREATH_H       = 60072,
    SPELL_VORTEX                = 56105,
    SPELL_ARCANE_PULSE          = 57432,
    SPELL_ARCANE_STORM          = 61693,
    SPELL_ARCANE_STORM_H        = 61694,
    SPELL_STATIC_FIELD          = 57430,
    SPELL_SURGE_OF_POWER        = 56505,
    SPELL_SURGE_OF_POWER_H      = 57407,
    //SPELL_DEEP_BREATH           = 0,
    //SPELL_DEEP_BREATH_H         = 0,
    //SPELL_ENRAGE                = 0,
};

enum Npcs
{
    NPC_POWER_SPARK             = 30084,
    NPC_NEXUS_LORD              = 30245,
    NPC_SCION_OF_ETERNITY       = 30249,
};

#define SUMMON_X                (754.055786f + (rand()%61 - 30))
#define SUMMON_Y                (1301.669922f + (rand()%61 - 30))
#define SUMMON_Z                (266.170898f + (rand()%61 - 30))

struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPhase;

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

    void Reset() 
    {
        m_uiPhase = 1;
        Enrage_Timer = 10*MINUTE*IN_MILLISECONDS;
        ArcaneBreath_Timer = 40*IN_MILLISECONDS;
        ArcaneStorm_Timer = 15*IN_MILLISECONDS;
        Vortex_Timer = 90*IN_MILLISECONDS;
        PowerSpark_Timer = 30*IN_MILLISECONDS;
        DeepBreath_Timer = 40*IN_MILLISECONDS;
        StaticField_Timer = 15*IN_MILLISECONDS;
        ArcanePulse_Timer = 30*IN_MILLISECONDS;
        SurgeOfPower_Timer = 40*IN_MILLISECONDS;

        std::fill_n(NexusLord_GUID, 2, 0);
        std::fill_n(ScionOfEternity_GUID, 2, 0);

        //Remove flags
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            if (uint64 AlexstraszaGiftGUID = m_pInstance->GetData64(DATA_MALYGOS_LOOT))
                m_pInstance->DoRespawnGameObject(AlexstraszaGiftGUID, 7*DAY);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Phase 1
        if (m_uiPhase == 1)
        {
            //Change phase
            if (m_creature->GetHealthPercent() <= 50.0f)
            {
                //Set Motion
                //m_creature->GetMotionMaster()->Clear(false);
                //m_creature->GetMotionMaster()->MoveIdle();
                //m_creature->SetHover(true);

                //Make unattackable
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Summon creatures
                for (uint32 i = 0; i < 2; ++i)
                {
                    Creature* pSummon = m_creature->SummonCreature(NPC_NEXUS_LORD,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    if (!pSummon)
                        continue;
                    NexusLord_GUID[i] = pSummon->GetGUID();
                    pSummon->SetInCombatWithZone();
                }
                for (uint32 i = 0; i < 2; ++i)
                {
                    Creature* pSummon = m_creature->SummonCreature(NPC_SCION_OF_ETERNITY,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000);
                    if (!pSummon)
                        continue;
                    ScionOfEternity_GUID[i] = pSummon->GetGUID();
                    pSummon->SetInCombatWithZone();
                }

                //Set to phase 2 and terminate phase 1
                m_uiPhase = 2;
                return;
            }

            //Vortex timer
            if (Vortex_Timer < diff)
            {
                DoCast(m_creature, SPELL_VORTEX);

                Vortex_Timer = 90*IN_MILLISECONDS;
            }
            else
                Vortex_Timer -= diff;

            //Arcane Breath timer
            if (ArcaneBreath_Timer < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H);
    
                ArcaneBreath_Timer = 40*IN_MILLISECONDS;
            }
            else
                ArcaneBreath_Timer -= diff;

            //Power Spark timer
            if (PowerSpark_Timer < diff)
            {
                Creature* pSummon = m_creature->SummonCreature(NPC_POWER_SPARK,SUMMON_X,SUMMON_Y,SUMMON_Z,0,TEMPSUMMON_CORPSE_DESPAWN,0);
                if (pSummon)
                {
                    DoStartNoMovement(m_creature);
                    pSummon->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
                }

                PowerSpark_Timer = 30*IN_MILLISECONDS;
            }
            else
                PowerSpark_Timer -= diff;

            //Arcane Storm timer
            if (ArcaneStorm_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
    
                ArcaneStorm_Timer = 15*IN_MILLISECONDS;
            }
            else
                ArcaneStorm_Timer -= diff;
        }

        //Phase 2
        if (m_uiPhase == 2)
        {
            //Change phase
            bool Phase2_Summons = false;
            for (uint32 i = 0; i < 2; ++i)
            {
                if (Creature* pSummon = m_creature->GetMap()->GetCreature(NexusLord_GUID[i]))
                    if (pSummon->isAlive())
                    {
                        Phase2_Summons = true;
                        break;
                    }
                if (Creature* pSummon = m_creature->GetMap()->GetCreature(ScionOfEternity_GUID[i]))
                    if (pSummon->isAlive())
                    {
                        Phase2_Summons = true;
                        break;
                    }
            }
            if (!Phase2_Summons)
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_uiPhase = 3;
                return;
            }

            //Deep Breath timer
            /*
            if (DeepBreath_Timer < diff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_DEEP_BREATH : SPELL_DEEP_BREATH_H);
    
                DeepBreath_Timer = 40*IN_MILLISECONDS;
            }
            else
                DeepBreath_Timer -= diff;
            */

            //Arcane Storm timer
            if (ArcaneStorm_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
    
                ArcaneStorm_Timer = 15*IN_MILLISECONDS;
            }
            else
                ArcaneStorm_Timer -= diff;
        }

        //Phase 3
        if (m_uiPhase == 3)
        {
            //Surge of Power timer
            if (SurgeOfPower_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, m_bIsRegularMode ? SPELL_SURGE_OF_POWER : SPELL_SURGE_OF_POWER_H);
    
                SurgeOfPower_Timer = 40*IN_MILLISECONDS;
            }
            else
                SurgeOfPower_Timer -= diff;

            //Arcane Pulse timer
            if (ArcanePulse_Timer < diff)
            {
                DoCast(m_creature, SPELL_ARCANE_PULSE);
    
                ArcanePulse_Timer = 30*IN_MILLISECONDS;
            }
            else
                ArcanePulse_Timer -= diff;

            //Static Field timer
            if (StaticField_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    DoCast(target, SPELL_STATIC_FIELD);
    
                StaticField_Timer = 15*IN_MILLISECONDS;
            }
            else
                StaticField_Timer -= diff;
        }

        //Enrage timer
        /*
        if (Enrage_Timer < diff)
        {
            DoCast(m_creature, SPELL_ENRAGE);
    
            Enrage_Timer = 10*MINUTE*IN_MILLISECONDS;
        }
        else
            Enrage_Timer -= diff;
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
    if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pGO->GetInstanceData()))
        if (uint64 MalygosGUID = m_pInstance->GetData64(TYPE_MALYGOS))
            if (Creature* Malygos = pGO->GetMap()->GetCreature(MalygosGUID))
            {
                Malygos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6 | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                Malygos->SetInCombatWithZone();
            }

    return true;
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
    newscript->pGOUse = &GOHello_go_TheFocusingIris;
    newscript->RegisterSelf();
}
