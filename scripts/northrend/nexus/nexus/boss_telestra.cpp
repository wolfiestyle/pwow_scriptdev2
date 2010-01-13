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
SDName: Boss_Telestra
SD%Complete: 10%
SDComment:
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"

enum
{
    //Grand Magus Telestra
    SAY_AGGRO               = -1576000,
    SAY_SPLIT_1             = -1576001,
    SAY_SPLIT_2             = -1576002,
    SAY_MERGE               = -1576003,
    SAY_KILL                = -1576004,
    SAY_DEATH               = -1576005,

    SPELL_FIREBOMB          = 47773,
    SPELL_FIREBOMB_H        = 56934,
    SPELL_ICE_NOVA          = 47772,
    SPELL_ICE_NOVA_H        = 56935,
    SPELL_GRAVITY_WELL      = 47756,
    SPELL_SUMMON_CLONES     = 47710,

    SPELL_ARCANE_VISUAL     = 47704,
    SPELL_FIRE_VISUAL       = 47705,
    SPELL_FROST_VISUAL      = 47706,

    SPELL_SUMMON_FIRE       = 47707,
    SPELL_SUMMON_ARCANE     = 47708,
    SPELL_SUMMON_FROST      = 47709,

    SPELL_FIRE_DIES         = 47711,
    SPELL_ARCANE_DIES       = 47713,
    SPELL_FROST_DIES        = 47712,

    SPELL_SPAWN_BACK_IN     = 47714,

    NPC_TELEST_FIRE         = 26928,
    NPC_TELEST_ARCANE       = 26929,
    NPC_TELEST_FROST        = 26930,

    //Fire Clone
    SPELL_SCORCH            = 47723,
    SPELL_SCORCH_H          = 56938,
    SPELL_FIREBLAST         = 47721,
    SPELL_FIREBLAST_H       = 56939,

    //Arcane Clone
    SPELL_CRITTER           = 47731,
    SPELL_TIMESTOP          = 47736,

    //Frost Clone
    SPELL_BLIZZARD          = 47727,
    SPELL_BLIZZARD_H        = 56936,
    SPELL_ICEBARB           = 47729,
    SPELL_ICEBARB_H         = 56937
};

/*######
## boss_telestra
######*/

struct MANGOS_DLL_DECL boss_telestraAI : public ScriptedAI
{
    boss_telestraAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 phase;
    uint32 IceNova_Timer;
    uint32 GravityWell_Timer;
    uint32 Firebomb_Timer;

    uint64 FireCloneGUID;
    uint64 ArcaneCloneGUID;
    uint64 FrostCloneGUID;

    bool FireClone_Dead;
    bool ArcaneClone_Dead;
    bool FrostClone_Dead;

    void Reset()
    {
        phase = 1;
        IceNova_Timer = 15000;                //15 seconds
        GravityWell_Timer = 40000;            //40 seconds
        Firebomb_Timer = 1500 + rand()%500;   //1.5-2 seconds

        FireClone_Dead = false;
        ArcaneClone_Dead = false;
        FrostClone_Dead = false;

        //Make visible
        if (m_creature->GetVisibility() == VISIBILITY_OFF)
            m_creature->SetVisibility(VISIBILITY_ON);

        //Remove flags
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (urand(0, 1))
            DoScriptText(SAY_KILL, m_creature);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->TauntApply(m_creature->getVictim());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Phase 1
        if (phase == 1)
        {
            //Summon clones at 50% HP
            if (m_creature->GetHealth() <= (int)(m_creature->GetMaxHealth() / 2))
            {
                //Say some stuff
                 switch(rand()%2)
                {
                    case 0: DoScriptText(SAY_SPLIT_1, m_creature); break;
                    case 1: DoScriptText(SAY_SPLIT_2, m_creature); break;
                }

                //Summon clones
                FireCloneGUID = (m_creature->SummonCreature(NPC_TELEST_FIRE,m_creature->GetPositionX()+rand()%6-2.5,m_creature->GetPositionY()+rand()%6-2.5,m_creature->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000))->GetGUID();
                ArcaneCloneGUID = (m_creature->SummonCreature(NPC_TELEST_ARCANE,m_creature->GetPositionX()+rand()%6-2.5,m_creature->GetPositionY()+rand()%6-2.5,m_creature->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000))->GetGUID();
                FrostCloneGUID = (m_creature->SummonCreature(NPC_TELEST_FROST,m_creature->GetPositionX()+rand()%6-2.5,m_creature->GetPositionY()+rand()%6-2.5,m_creature->GetPositionZ(),0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000))->GetGUID();

                //Make invisible
                m_creature->SetVisibility(VISIBILITY_OFF);

                //Set flags
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                //Remove auras
                m_creature->RemoveAllAuras();

                //Change to Phase 2
                phase = 2;

                //Terminate Phase 1
                return;
            }
        }

        //Phase 1 and 3
        if (phase == 1 || phase == 3)
        {
            //Spell Gravity Well timer
            if (GravityWell_Timer < uiDiff)
            {
                //Cast spell one on our current target.
                DoCast(m_creature,SPELL_GRAVITY_WELL);

                GravityWell_Timer = 40000;
            }else GravityWell_Timer -= uiDiff;

            //Spell Ice Nova timer
            if (IceNova_Timer < uiDiff)
            {
                //Cast spell one on our current target.
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ICE_NOVA : SPELL_ICE_NOVA_H);

                IceNova_Timer = 15000;
            }else IceNova_Timer -= uiDiff;

            //Uses Firebomb as a main attack
            if (Firebomb_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FIREBOMB : SPELL_FIREBOMB_H);

                Firebomb_Timer = 1500 + rand()%500;
            }else Firebomb_Timer -= uiDiff;
        }

        //Phase 2
        if (phase == 2)
        {
            if (!FireClone_Dead)
            {
                Creature* FireClone = (Creature*)Unit::GetUnit(*m_creature, FireCloneGUID);
                if (FireClone->isDead() || !FireClone)
                    FireClone_Dead = true;
            }
            if (!ArcaneClone_Dead)
            {
                Creature* ArcaneClone = (Creature*)Unit::GetUnit(*m_creature, ArcaneCloneGUID);
                if (ArcaneClone->isDead() || !ArcaneClone)
                    ArcaneClone_Dead = true;
            }
            if (!FrostClone_Dead)
            {
                Creature* FrostClone = (Creature*)Unit::GetUnit(*m_creature, FrostCloneGUID);
                if (FrostClone->isDead() || !FrostClone)
                    FrostClone_Dead = true;
            }

            if (FireClone_Dead && ArcaneClone_Dead && FrostClone_Dead)
            {
                //Set health
                m_creature->SetHealth((int) (m_creature->GetMaxHealth() * 0.25));

                //Remove flags
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

                //Make visible
                m_creature->SetVisibility(VISIBILITY_ON);

                //Change to Phase 3
                phase = 3;

                //Terminate Phase 2
                return;
            }
        }
    }
};

CreatureAI* GetAI_boss_telestra(Creature* pCreature)
{
    return new boss_telestraAI(pCreature);
}

/*######
## Fire Clone
######*/

struct MANGOS_DLL_DECL boss_telestra_fireAI : public ScriptedAI
{
    boss_telestra_fireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 FireBlast_Timer;
    uint32 Scorch_Timer;

    void Reset() 
    {
        FireBlast_Timer = 15000;            //15 seconds
        Scorch_Timer = 1000 + rand()%500;   //1-1.5 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Fire Blast timer
        if (FireBlast_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FIREBLAST  : SPELL_FIREBLAST_H);

            FireBlast_Timer = 15000;
        }else FireBlast_Timer -= diff;

        //Uses Scorch as a main attack
        if (Scorch_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SCORCH : SPELL_SCORCH_H);

            Scorch_Timer = 1000 + rand()%500;
        }else Scorch_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_telestra_fire(Creature* pCreature)
{
    return new boss_telestra_fireAI(pCreature);
}

/*######
## Arcane Clone
######*/

struct MANGOS_DLL_DECL boss_telestra_arcaneAI : public ScriptedAI
{
    boss_telestra_arcaneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 TimeStop_Timer;
    uint32 Critter_Timer;

    void Reset() 
    {
        TimeStop_Timer = 15000;              //15 seconds
        Critter_Timer = 1500 + rand()%500;   //1.5-2 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Time Stop timer
        if (TimeStop_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature,SPELL_TIMESTOP);

            TimeStop_Timer = 15000;
        }else TimeStop_Timer -= diff;

 
        //Uses Critter as a main attack
        if (Critter_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_CRITTER);

            Critter_Timer = 1500 + rand()%500;
        }else Critter_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_telestra_arcane(Creature* pCreature)
{
    return new boss_telestra_arcaneAI(pCreature);
}

/*######
## Frost Clone
######*/

struct MANGOS_DLL_DECL boss_telestra_frostAI : public ScriptedAI
{
    boss_telestra_frostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 IceBarb_Timer;
    uint32 Blizzard_Timer;

    void Reset() 
    {
        IceBarb_Timer = 15000;                //15 seconds
        Blizzard_Timer = 6000 + rand()%500;   //6-6.5 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Ice Barb timer
        if (IceBarb_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ICEBARB : SPELL_ICEBARB_H);

            IceBarb_Timer = 15000;
        }else IceBarb_Timer -= diff;

         //Uses Blizzard as a main attack
        if (Blizzard_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BLIZZARD : SPELL_BLIZZARD_H);

            Blizzard_Timer = 6000 + rand()%500;
        }else Blizzard_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_telestra_frost(Creature* pCreature)
{
    return new boss_telestra_frostAI(pCreature);
}

void AddSC_boss_telestra()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_telestra";
    newscript->GetAI = &GetAI_boss_telestra;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_telestra_fire";
    newscript->GetAI = &GetAI_boss_telestra_fire;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_telestra_arcane";
    newscript->GetAI = &GetAI_boss_telestra_arcane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_telestra_frost";
    newscript->GetAI = &GetAI_boss_telestra_frost;
    newscript->RegisterSelf();
}
