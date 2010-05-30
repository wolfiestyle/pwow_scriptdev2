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
SDName: Boss_KelThuzud
SD%Complete: 0
SDComment: VERIFY SCRIPT
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    //when shappiron dies. dialog between kel and lich king (in this order)
    SAY_SAPP_DIALOG1          = -1533084,
    SAY_SAPP_DIALOG2_LICH     = -1533085,
    SAY_SAPP_DIALOG3          = -1533086,
    SAY_SAPP_DIALOG4_LICH     = -1533087,
    SAY_SAPP_DIALOG5          = -1533088,

    //when cat dies
    SAY_CAT_DIED              = -1533089,

    //when each of the 4 wing bosses dies
    SAY_TAUNT1                = -1533090,
    SAY_TAUNT2                = -1533091,
    SAY_TAUNT3                = -1533092,
    SAY_TAUNT4                = -1533093,

    SAY_SUMMON_MINIONS        = -1533105,                   //start of phase 1

    SAY_AGGRO1                = -1533094,                   //start of phase 2
    SAY_AGGRO2                = -1533095,
    SAY_AGGRO3                = -1533096,

    SAY_SLAY1                 = -1533097,
    SAY_SLAY2                 = -1533098,

    SAY_DEATH                 = -1533099,

    SAY_CHAIN1                = -1533100,
    SAY_CHAIN2                = -1533101,
    SAY_FROST_BLAST           = -1533102,

    SAY_REQUEST_AID           = -1533103,                   //start of phase 3
    SAY_ANSWER_REQUEST        = -1533104,                   //lich king answer

    SAY_SPECIAL1_MANA_DET     = -1533106,
    SAY_SPECIAL3_MANA_DET     = -1533107,
    SAY_SPECIAL2_DISPELL      = -1533108,

    //spells to be casted
    SPELL_FROST_BOLT          = 28478,
    SPELL_FROST_BOLT_H        = 55802,
    SPELL_FROST_BOLT_NOVA     = 28479,
    SPELL_FROST_BOLT_NOVA_H   = 55807,

    SPELL_CHAINS_OF_KELTHUZAD = 28410,                      //casted spell should be 28408. Also as of 303, heroic only
    SPELL_MANA_DETONATION     = 27819,
    SPELL_SHADOW_FISURE       = 27810,
    SPELL_FROST_BLAST         = 27808
};

//Positions
#define ADDX_LEFT_FAR               3766.662842 + rand()%11 - 5
#define ADDY_LEFT_FAR               -5070.999512 + rand()%11 - 5
#define ADDZ_LEFT_FAR               143.223862
#define ADDO_LEFT_FAR               3.757832

#define ADDX_LEFT_MIDDLE            3728.849609 + rand()%11 - 5
#define ADDY_LEFT_MIDDLE            -5041.993164 + rand()%11 - 5
#define ADDZ_LEFT_MIDDLE            143.425568
#define ADDO_LEFT_MIDDLE            4.582493

#define ADDX_LEFT_NEAR              3680.705566 + rand()%11 - 5
#define ADDY_LEFT_NEAR              -5056.435547 + rand()%11 - 5
#define ADDZ_LEFT_NEAR              143.205765
#define ADDO_LEFT_NEAR              5.261880

#define ADDX_RIGHT_FAR              3750.646484 + rand()%11 - 5
#define ADDY_RIGHT_FAR              -5156.183594 + rand()%11 - 5
#define ADDZ_RIGHT_FAR              143.181213
#define ADDO_RIGHT_FAR              2.190950

#define ADDX_RIGHT_MIDDLE           3703.780273 + rand()%11 - 5
#define ADDY_RIGHT_MIDDLE           -5171.265625 + rand()%11 - 5
#define ADDZ_RIGHT_MIDDLE           143.416779
#define ADDO_RIGHT_MIDDLE           1.397684

#define ADDX_RIGHT_NEAR             3665.404053 + rand()%11 - 5
#define ADDY_RIGHT_NEAR             -5142.086426 + rand()%11 - 5
#define ADDZ_RIGHT_NEAR             143.195999
#define ADDO_RIGHT_NEAR             0.600505

#define GUARDIAN_X                  3715.033691 + rand()%21 - 10
#define GUARDIAN_Y                  -5106.866211 + rand()%21 - 10
#define GUARDIAN_Z                  141.289963
#define GUARDIAN_O                  6.196979

//Amount of adds
#define NUM_GUARDIANS		4
#define NUM_SOLDIERS		72
#define NUM_ABOMINATIONS	12
#define NUM_WEAVERS			12

struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    typedef std::vector<uint64> u64vec;

    boss_kelthuzadAI(Creature* pCreature) : ScriptedAI(pCreature),
        GuardiansOfIcecrown(NUM_GUARDIANS,0),
        SoldierOfTheFrozenWastes(NUM_SOLDIERS,0),
        UnstoppableAbomination(NUM_ABOMINATIONS,0),
        SoulWeaver(NUM_WEAVERS,0)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 Phase;
    uint32 Phase_Timer;
    uint32 FrostBolt_Timer;
    uint32 FrostBoltNova_Timer;
    uint32 ManaDetonation_Timer;
    uint32 FrostBlast_Timer;
    uint32 ShadowFisure_Timer;
	uint32 Chains_Timer;
    uint32 Guardians_Counter;
    uint32 Guardians_Timer;
    uint32 Soldiers_Timer;
    uint32 Soldiers_Counter;
    uint32 Abominations_Timer;
    uint32 Abominations_Counter;
    uint32 Weavers_Timer;
    uint32 Weavers_Counter;
    u64vec GuardiansOfIcecrown;
    u64vec SoldierOfTheFrozenWastes;
    u64vec UnstoppableAbomination;
    u64vec SoulWeaver;
    bool started;

    void DespawnCreature(uint64 guid)
    {
        if (Creature* pSummon = (Creature*)Unit::GetUnit(*m_creature, guid))
            if (pSummon->isAlive())
                pSummon->ForcedDespawn();
    }

    void DespawnMobs()
    {
        if (Phase > 0)
        {
            for (uint8 i=0; i<NUM_SOLDIERS; i++)
                DespawnCreature(SoldierOfTheFrozenWastes[i]);
            for (uint8 i=0; i<NUM_ABOMINATIONS; i++)
                DespawnCreature(UnstoppableAbomination[i]);
            for (uint8 i=0; i<NUM_WEAVERS; i++)
                DespawnCreature(SoulWeaver[i]);
        }
    }

    void Reset()
    {
        Phase_Timer = 228000;
        FrostBolt_Timer = (rand()%10)*1000;           //0-9 seconds
        FrostBoltNova_Timer = (20+rand()%10)*1000;    //20-29 seconds
        ManaDetonation_Timer = 30000;                 //30 seconds
        FrostBlast_Timer = 45000;                     //34 seconds
        ShadowFisure_Timer = (30+rand()%30)*1000;     //30-60 seconds
		Chains_Timer = 90000;                         //90 seconds
        Guardians_Counter = 0;
        Guardians_Timer = 0;
        Soldiers_Timer = 0;
        Soldiers_Counter = 0;
        Abominations_Timer = 0;
        Abominations_Counter = 0;
        Weavers_Timer = 12000;
        Weavers_Counter = 0;
        started = false;

        DespawnMobs();
        Phase = 0;

        //Clear GUIDs
        std::fill(GuardiansOfIcecrown.begin(), GuardiansOfIcecrown.end(), 0);
        std::fill(SoldierOfTheFrozenWastes.begin(), SoldierOfTheFrozenWastes.end(), 0);
        std::fill(UnstoppableAbomination.begin(), UnstoppableAbomination.end(), 0);
        std::fill(SoulWeaver.begin(), SoulWeaver.end(), 0);

        //Boss unattackable in phase 1
        if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELTHUZAD, NOT_STARTED);
    }

    void KilledUnit()
    {
        switch(urand(0,1))
        {
            case 0: DoScriptText(SAY_SLAY1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        for (uint32 i=0; i<Guardians_Counter; i++)
            DespawnCreature(GuardiansOfIcecrown[i]);

        DespawnMobs();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELTHUZAD, DONE);
    }

    void Aggro(Unit* who)
    {
        if (Phase == 2 || Phase == 3)
        {
            switch(urand(0,2))
            {
                case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
                case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
                case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
            }
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KELTHUZAD, IN_PROGRESS);
    }

    /*void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SAPPHIRON, FAIL);
    }*/

    void MoveInLineOfSight(Unit *who)
    {
        if (!started && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 40.0f) && m_creature->IsHostileTo(who))
        {
            m_creature->TauntApply(who); //Apply taunt to put boss in combat
            started = true;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Pulling KT to the corridor
        if (!m_creature->IsWithinDist2d(3749.68,-5114.06,120.0f))
            m_creature->AI()->EnterEvadeMode();

        //Preparation phase
        if (Phase == 0)
        {
            //Summon at Left Near
            for (uint8 i=0; i<12; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=0; i<2; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=0; i<2; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Summon at Left Middle
            for (uint8 i=12; i<24; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=2; i<4; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=2; i<4; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Summon at Left Far
            for (uint8 i=24; i<36; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=4; i<6; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=4; i<6; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Summon at Right Near
            for (uint8 i=36; i<48; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=6; i<8; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=6; i<8; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Summon at Right Middle
            for (uint8 i=48; i<60; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=8; i<10; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=8; i<10; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Summon at Right Far
            for (uint8 i=60; i<72; i++)
            {
                SoldierOfTheFrozenWastes[i] = m_creature->SummonCreature(16427,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=10; i<12; i++)
            {
                UnstoppableAbomination[i] = m_creature->SummonCreature(16428,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }
            for (uint8 i=10; i<12; i++)
            {
                SoulWeaver[i] = m_creature->SummonCreature(16429,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_CORPSE_DESPAWN,0)->GetGUID();
            }

            //Shuffle Arrays
            std::random_shuffle(SoldierOfTheFrozenWastes.begin(), SoldierOfTheFrozenWastes.end());
            std::random_shuffle(UnstoppableAbomination.begin(), UnstoppableAbomination.end());
            std::random_shuffle(SoulWeaver.begin(), SoulWeaver.end());

            DoScriptText(SAY_SUMMON_MINIONS, m_creature);

            Phase = 1;
        }

        //Phase 1
        if (Phase == 1)
        {
            //Phase Timer
            if (Phase_Timer < diff)
            {
                //Make boss attackable
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Enable movement
                DoStartMovement(m_creature->getVictim());

                Phase = 2;
                return;

            }else Phase_Timer -= diff;

            //Disable movement
            DoStartNoMovement(m_creature->getVictim());

            //Soldiers of the Frozen Wastes Timer
            if (Soldiers_Counter < NUM_SOLDIERS)
            {
                if (Soldiers_Timer < diff)
                {
                    if (Creature* pSummon = (Creature*)Unit::GetUnit(*m_creature, SoldierOfTheFrozenWastes[Soldiers_Counter]))
                        if (pSummon->isAlive())
                            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                pSummon->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

                    Soldiers_Counter++;

                    Soldiers_Timer = 3000;
                }else Soldiers_Timer -= diff;
            }

            //Unstoppable Abomination Timer
            if (Abominations_Counter < NUM_ABOMINATIONS)
            {
                if (Abominations_Timer < diff)
                {
                    if (Creature* pSummon = (Creature*)Unit::GetUnit(*m_creature, UnstoppableAbomination[Abominations_Counter]))
                        if (pSummon->isAlive())
                            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                pSummon->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

                    Abominations_Counter++;

                    Abominations_Timer = 24000;
                }else Abominations_Timer -= diff;
            }

            //Soul Weaver Timer
            if (Weavers_Counter < NUM_WEAVERS)
            {
                if (Weavers_Timer < diff)
                {
                    if (Creature* pSummon = (Creature*)Unit::GetUnit(*m_creature, SoulWeaver[Weavers_Counter]))
                        if (pSummon->isAlive())
                            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                pSummon->GetMotionMaster()->MovePoint(0, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());

                    Weavers_Counter++;

                    Weavers_Timer = 24000;
                }else Weavers_Timer -= diff;
            }
        }

        //Phase 2
        if (Phase == 2)
        {
            //When HP reaches 45% we move on to phase 3
            if (m_creature->GetHealthPercent() <= 45.0f)
            {
                DoScriptText(SAY_REQUEST_AID, m_creature);
                DoScriptText(SAY_ANSWER_REQUEST, m_creature);

                Phase = 3;

                //Terminate phase 2
                return;
            }
        }

        //Phase 3
        if (Phase == 3)
        {
			if (Guardians_Counter < (m_bIsRegularMode ? 2 : 4))
            {
                //Guardians Timer Timer
                if (Guardians_Timer < diff)
                {
                    GuardiansOfIcecrown[Guardians_Counter] = m_creature->SummonCreature(16441,GUARDIAN_X,GUARDIAN_Y,GUARDIAN_Z,GUARDIAN_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000)->GetGUID();
                    if (Creature* pSummon = (Creature*)Unit::GetUnit(*m_creature, GuardiansOfIcecrown[Guardians_Counter]))
                        pSummon->TauntApply(m_creature->getVictim());
                    Guardians_Counter++;

                    Guardians_Timer = 5000;
                }else Guardians_Timer -= diff;
            }
        }

        //Phase 2 and 3
        if (Phase == 2 || Phase == 3)
        {
            //Chains of Kel Thuzad Timer
			/*
			if (!m_bIsRegularMode)
			{
				if (Chains_Timer < diff)
				{
				    DoCast(m_creature,SPELL_CHAINS_OF_KELTHUZAD);

					switch(rand()%2)
					{
						case 0: DoScriptText(SAY_CHAIN1, m_creature); break;
						case 1: DoScriptText(SAY_CHAIN2, m_creature); break;
					}

				    Chains_Timer = 90000;
				}else Chains_Timer -= diff;
			}
			*/

            //Shadow Fissure Timer
            if (ShadowFisure_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target,SPELL_SHADOW_FISURE);

                ShadowFisure_Timer = (30+rand()%30)*1000;
            }else ShadowFisure_Timer -= diff;

            //Frost Blast Timer
            if (FrostBlast_Timer < diff)
            {
                DoCast(m_creature->getVictim(),SPELL_FROST_BLAST);

                DoScriptText(SAY_FROST_BLAST, m_creature);

                FrostBlast_Timer = 45000;
            }else FrostBlast_Timer -= diff;

            //Mana Detonation Timer
            if (ManaDetonation_Timer < diff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(target,SPELL_MANA_DETONATION);

                switch(rand()%2)
                {
                    case 0: DoScriptText(SAY_SPECIAL1_MANA_DET, m_creature); break;
                    case 1: DoScriptText(SAY_SPECIAL3_MANA_DET, m_creature); break;
                }

                ManaDetonation_Timer = 30000;
            }else ManaDetonation_Timer -= diff;

            //Frost Bolt Nova Timer
            if (FrostBoltNova_Timer < diff)
            {
				DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_BOLT_NOVA : SPELL_FROST_BOLT_NOVA_H);

                FrostBoltNova_Timer = (20+rand()%10)*1000;
            }else FrostBoltNova_Timer -= diff;

            //Frost Bolt Timer
            if (FrostBolt_Timer < diff)
            {
				DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FROST_BOLT : SPELL_FROST_BOLT_H);

                FrostBolt_Timer = (rand()%15)*1000;
            }else FrostBolt_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature* pCreature)
{
    return new boss_kelthuzadAI(pCreature);
}

void AddSC_boss_kelthuzad()
{
    Script* NewScript;
    NewScript = new Script;
    NewScript->Name = "boss_kelthuzad";
    NewScript->GetAI = &GetAI_boss_kelthuzadAI;
    NewScript->RegisterSelf();
}
