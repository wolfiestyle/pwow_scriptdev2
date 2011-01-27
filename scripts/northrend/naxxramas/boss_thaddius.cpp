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
SDName: Boss_Thaddius
SD%Complete: 90%
SDComment: PLEASE VERIFY
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    // Stalagg
    SAY_STAL_AGGRO                = -1533023,
    SAY_STAL_SLAY                 = -1533024,
    SAY_STAL_DEATH                = -1533025,

    SPELL_POWERSURGE              = 28134,
    H_SPELL_POWERSURGE            = 54529,

    //Feugen
    SAY_FEUG_AGGRO                = -1533026,
    SAY_FEUG_SLAY                 = -1533027,
    SAY_FEUG_DEATH                = -1533028,

    SPELL_STATICFIELD             = 28135,
    H_SPELL_STATICFIELD           = 54528,

    //both
    SPELL_MAGNETICPULL            = 28338,

    //Thaddus
    SAY_GREET                     = -1533029,
    SAY_AGGRO1                    = -1533030,
    SAY_AGGRO2                    = -1533031,
    SAY_AGGRO3                    = -1533032,
    SAY_SLAY                      = -1533033,
    SAY_ELECT                     = -1533034,
    SAY_DEATH                     = -1533035,
    SAY_SCREAM1                   = -1533036,
    SAY_SCREAM2                   = -1533037,
    SAY_SCREAM3                   = -1533038,
    SAY_SCREAM4                   = -1533039,

    SPELL_BALL_LIGHTNING          = 28299,

    SPELL_CHARGE_POSITIVE_DMGBUFF = 29659,
    SPELL_CHARGE_POSITIVE_NEARDMG = 28059,

    SPELL_CHARGE_NEGATIVE_DMGBUFF = 29660,
    SPELL_CHARGE_NEGATIVE_NEARDMG = 28084,

    SPELL_POLARITY_SHIFT          = 28089,           //The above are a result of this

    SPELL_CHAIN_LIGHTNING         = 28167,
    H_SPELL_CHAIN_LIGHTNING       = 54531,

    SPELL_BESERK                  = 26662,

    //generic
    C_TESLA_COIL                  = 16218                   //the coils (emotes "Tesla Coil overloads!")
};

struct MANGOS_DLL_DECL boss_thaddiusAI : public ScriptedAI
{
    boss_thaddiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 ChainLightning_Timer;                            //Timer for Chain Lightning
    uint32 PolarityShift_Timer;                             //Timer for Polarity Shift
    uint32 BallLightning_Timer;                             //Timer for Ball Lightning
    uint32 BerserkerEnrage_Timer;                           //Timer for Berserker Enrage
    uint32 Screams_Timer;                                   //Timer for Screams
    bool greet;                                             //Check for SAY_GREET

    void Reset()
    {
        ChainLightning_Timer = 15000;                       //15 seconds
        PolarityShift_Timer = 30000;                        //30 seconds
        BallLightning_Timer = 5000;                         //5 seconds
        BerserkerEnrage_Timer = 360000;                     //6 minutes
        Screams_Timer = (rand()%30+15)*1000;                //15-44 seconds
        greet = true;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        //Return since boss is locked
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            return;

        //Say some stuff
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_THADDIUS, IN_PROGRESS);
    }

    void MoveInLineOfSight(Unit *who)
    {
        //Return since boss is locked
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            return;

        if (greet && m_creature->IsWithinDistInMap(who, 60.0f) && who->GetTypeId() == TYPEID_PLAYER)
        {
            //Say some stuff
            DoScriptText(SAY_GREET, m_creature);
            greet = false;
        }

        ScriptedAI::MoveInLineOfSight(who);
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
            m_pInstance->SetData(TYPE_THADDIUS, DONE);
    }


    void UpdateAI(const uint32 diff)
    {
        //Return since boss is locked
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
            return;

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Chain Lightning timer
        if (ChainLightning_Timer < diff)
        {
            //Cast spell on our current target.
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
				DoCast(target, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : H_SPELL_CHAIN_LIGHTNING);

            ChainLightning_Timer = 15000;
        }
        else 
            ChainLightning_Timer -= diff;

        //Spell Polarity Shift timer
        if (PolarityShift_Timer < diff)
        {
            //Cast spell on our current target.
            DoCast(m_creature->getVictim(),SPELL_POLARITY_SHIFT);

            PolarityShift_Timer = 30000;
        }
        else
            PolarityShift_Timer -= diff;

        //Spell Ball Lightning timer
        if (BallLightning_Timer < diff)
        {
            if (!m_creature->IsWithinDist(m_creature->getVictim(),ATTACK_DISTANCE))
            {
                //Cast spell on our current target.
                DoCast(m_creature->getVictim(),SPELL_BALL_LIGHTNING);
            }

            BallLightning_Timer = 5000;
        }
        else
            BallLightning_Timer -= diff;

        //Spell Berserker Enrage timer
        if (BerserkerEnrage_Timer < diff)
        {
            //Cast spell on self.
            DoCast(m_creature,SPELL_BESERK);

            //Cast our beserk spell agian in 12 seconds if we didn't kill everyone
            BerserkerEnrage_Timer = 12000;
        }
        else
            BerserkerEnrage_Timer -= diff;

        //Screams timer
        if (Screams_Timer < diff)
        {
            //Say some stuff
            switch(rand()%4)
            {
                case 0: DoScriptText(SAY_SCREAM1,m_creature); break;
                case 1: DoScriptText(SAY_SCREAM2,m_creature); break;
                case 2: DoScriptText(SAY_SCREAM3,m_creature); break;
                case 3: DoScriptText(SAY_SCREAM4,m_creature); break;
            }

            Screams_Timer = (rand()%30+15)*1000;
        }
        else
            Screams_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_feugenAI : public ScriptedAI
{
    boss_feugenAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 StaticField_Timer;                               //Timer for Static Field
    uint32 MagneticPull_Timer;                              //Timer for Magnetic Pull
    uint32 Resurrect_Timer;                                 //Timer for Resurrection

    uint64 StalaggGUID;
    uint64 ThaddiusGUID;

    void Reset()
    {
        StaticField_Timer = 3000;                           //3 seconds
        MagneticPull_Timer = 30000;                         //30 seconds
        Resurrect_Timer = 5000;                             //5 seconds

        StalaggGUID = 0;
        ThaddiusGUID = 0;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_FEUG_AGGRO, m_creature);
    }

    void KilledUnit()
    {
        //Say some stuff
        DoScriptText(SAY_FEUG_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        //Say some stuff
        DoScriptText(SAY_FEUG_DEATH, m_creature);

        //Unlock Thaddius
        if (StalaggGUID && ThaddiusGUID)
        {
            Creature* Stalagg = m_creature->GetMap()->GetCreature(StalaggGUID);
            Creature* Thaddius = m_creature->GetMap()->GetCreature(ThaddiusGUID);
            if(Stalagg && !Stalagg->isAlive() && Thaddius)
            {
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance)
        {
            if (!StalaggGUID)
                StalaggGUID = m_pInstance->GetData64(NPC_STALAGG);
            if (!ThaddiusGUID)
                ThaddiusGUID = m_pInstance->GetData64(NPC_THADDIUS);
        }

        //Respawn Stalagg after 5 seconds
        if (StalaggGUID)
        {
            Creature* Stalagg = m_creature->GetMap()->GetCreature(StalaggGUID);
            if (Stalagg && !Stalagg->isAlive())
            {
                if (Resurrect_Timer < diff)
                {
                    Stalagg->Respawn();
                    Stalagg->AI()->EnterEvadeMode();

                    Resurrect_Timer = 5000;
                }else Resurrect_Timer -= diff;
            }
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Static Field timer
        if (StaticField_Timer < diff)
        {
            //Cast spell on our current target.
			DoCast(m_creature, m_bIsRegularMode ? SPELL_STATICFIELD : H_SPELL_STATICFIELD);

            StaticField_Timer = 3000;
        }else StaticField_Timer -= diff;

        //Spell Magnetic Pull timer
		/*
        if (MagneticPull_Timer < diff)
        {
            //Cast spell on our current target.
            DoCast(m_creature->getVictim(),SPELL_MAGNETICPULL);

            MagneticPull_Timer = 30000;
        }else MagneticPull_Timer -= diff;
		*/

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_stalaggAI : public ScriptedAI
{
    boss_stalaggAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 PowerSurge_Timer;                                //Timer for Static Field
    uint32 MagneticPull_Timer;                              //Timer for Magnetic Pull
    uint32 Resurrect_Timer;                                 //Timer for Resurrection

    uint64 FeugenGUID;
    uint64 ThaddiusGUID;

    void Reset()
    {
        PowerSurge_Timer = 15000;                           //15 seconds
        MagneticPull_Timer = 30000;                         //30 seconds
        Resurrect_Timer = 5000;                             //5 seconds

        FeugenGUID = 0;
        ThaddiusGUID = 0;
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_STAL_AGGRO, m_creature);
    }

    void KilledUnit()
    {
        //Say some stuff
        DoScriptText(SAY_STAL_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        //Say some stuff
        DoScriptText(SAY_STAL_DEATH, m_creature);

        //Unlock Thaddius
        if (FeugenGUID && ThaddiusGUID)
        {
            Creature* Feugen = m_creature->GetMap()->GetCreature(FeugenGUID);
            Creature* Thaddius = m_creature->GetMap()->GetCreature(ThaddiusGUID);
            if(Feugen && !Feugen->isAlive() && Thaddius)
            {
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                Thaddius->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_6);
            }
        }
    }

    //*** HANDLED FUNCTION ***
    //Update AI is called Every single map update (roughly once every 100ms if a player is within the grid)
    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance)
        {
            if (!FeugenGUID)
                FeugenGUID = m_pInstance->GetData64(NPC_FEUGEN);
            if (!ThaddiusGUID)
                ThaddiusGUID = m_pInstance->GetData64(NPC_THADDIUS);
        }

        //Respawn Feugen after 5 seconds
        if (FeugenGUID)
        {
            Creature* Feugen = m_creature->GetMap()->GetCreature(FeugenGUID);
            if (Feugen && !Feugen->isAlive())
            {
                if (Resurrect_Timer < diff)
                {
                    Feugen->Respawn();
                    Feugen->AI()->EnterEvadeMode();

                    Resurrect_Timer = 5000;
                }else Resurrect_Timer -= diff;
            }
        }

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Spell Power Surge timer
        if (PowerSurge_Timer < diff)
        {
            //Cast spell on self.
			DoCast(m_creature, m_bIsRegularMode ? SPELL_POWERSURGE : H_SPELL_POWERSURGE);

            PowerSurge_Timer = 15000;
        }else PowerSurge_Timer -= diff;

        //Spell Magnetic Pull timer
		/*
        if (MagneticPull_Timer < diff)
        {
            //Cast spell on our current target.
            DoCast(m_creature->getVictim(),SPELL_MAGNETICPULL);

            MagneticPull_Timer = 30000;
        }else MagneticPull_Timer -= diff;
		*/

        DoMeleeAttackIfReady();
    }
};

//This is the GetAI method used by all scripts that involve AI
//It is called every time a new creature using this script is created
CreatureAI* GetAI_boss_thaddius(Creature* pCreature)
{
    return new boss_thaddiusAI(pCreature);
}

CreatureAI* GetAI_boss_feugen(Creature* pCreature)
{
    return new boss_feugenAI(pCreature);
}

CreatureAI* GetAI_boss_stalagg(Creature* pCreature)
{
    return new boss_stalaggAI(pCreature);
}

void AddSC_boss_thaddius()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_thaddius";
    newscript->GetAI = &GetAI_boss_thaddius;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_feugen";
    newscript->GetAI = &GetAI_boss_feugen;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_stalagg";
    newscript->GetAI = &GetAI_boss_stalagg;
    newscript->RegisterSelf();
}
