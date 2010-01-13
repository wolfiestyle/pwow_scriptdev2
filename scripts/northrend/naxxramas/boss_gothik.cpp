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
SDName: Boss_Gothik
SD%Complete: 0
SDComment: Placeholder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SAY_SPEECH                = -1533040,
    SAY_KILL                  = -1533041,
    SAY_DEATH                 = -1533042,
    SAY_TELEPORT              = -1533043,

    //Gothik
    SPELL_HARVESTSOUL         = 28679,
    SPELL_SHADOWBOLT          = 29317,
    H_SPELL_SHADOWBOLT        = 56405,

    //Unrelenting Trainee
    SPELL_EAGLECLAW           = 30285,
    SPELL_KNOCKDOWN_PASSIVE   = 6961,

    //Unrelenting Deathknight
    SPELL_CHARGE              = 22120,
    SPELL_SHADOW_MARK         = 27825,

    //Unrelenting Rider
    SPELL_UNHOLY_AURA         = 55606,
    H_SPELL_UNHOLY_AURA       = 55608,
    SPELL_SHADOWBOLT_VOLLEY   = 27831,                      //Search thru targets and find those who have the SHADOW_MARK to cast this on
    H_SPELL_SHADOWBOLT_VOLLEY = 55638,

    //Spectral Trainee
    SPELL_ARCANE_EXPLOSION    = 27989,

    //Spectral Deathknight
    SPELL_WHIRLWIND           = 28334,
    SPELL_SUNDER_ARMOR        = 25051,                      //cannot find sunder that reduces armor by 2950
    SPELL_CLEAVE              = 20677,
    SPELL_MANA_BURN           = 17631,

    //Spectral Rider
    SPELL_LIFEDRAIN           = 24300,
    //USES SAME UNHOLY AURA AS UNRELENTING RIDER

    //Spectral Horse
    SPELL_STOMP               = 27993
};

//Summon Positions
#define UNRELENTING_X               2691.670654 + rand()%41-20
#define UNRELENTING_Y               -3392.531006 + rand()%41-20
#define UNRELENTING_Z               267.683990
#define UNRELENTING_O               0

#define SPECTRAL_X                  2691.226318 + rand()%41-20
#define SPECTRAL_Y                  -3333.039551 + rand()%41-20
#define SPECTRAL_Z                  267.683990
#define SPECTRAL_O                  0

//Teleport Positions
#define TELE_1X                      2691.670654
#define TELE_1Y                      -3392.531006
#define TELE_1Z                      267.683990
#define TELE_1O                      0

#define TELE_2X                      2691.226318
#define TELE_2Y                      -3333.039551
#define TELE_2Z                      267.683990
#define TELE_2O                      0

//Gothik
struct MANGOS_DLL_DECL boss_gothikAI : public ScriptedAI
{
    boss_gothikAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 HarvestSoul_Timer;                               //Timer for Harvest Soul spell
    uint32 Shadowbolt_Timer;                                //Timer for Shadowbolt spell
    uint32 UnrelentingTrainee_Timer;                        //Timer for summoning an Unrelenting Trainee
    uint32 UnrelentingDeathKnight_Timer;                    //Timer for summoning an Unrelenting Death Knight
    uint32 UnrelentingRider_Timer;                          //Timer for summoning an Unrelenting Rider
    uint32 UnrelentingTrainee_Counter;                      //Counter for summoning an Unrelenting Trainee
    uint32 UnrelentingDeathKnight_Counter;                  //Counter for summoning an Unrelenting Death Knight
    uint32 UnrelentingRider_Counter;                        //Counter for summoning an Unrelenting Rider
    uint32 Phase_Timer;                                     //Timer for Phase 2
    uint8 phase;                                            //The current battle phase we are in
    uint32 Side_Timer;                                      //Timer to teleport to the other side
    uint8 side;                                             //The current side we are in
    bool speech;                                            //Check for SAY_SPEECH

    void Reset()
    {
        HarvestSoul_Timer = 15000;                          //15 seconds
        Shadowbolt_Timer = 1000 + rand()%500;               //1-1.5 seconds
        UnrelentingTrainee_Timer = 0;                       //0 seconds
        UnrelentingDeathKnight_Timer = 50000;               //50 seconds
        UnrelentingRider_Timer = 110000;                    //110 seconds
        UnrelentingTrainee_Counter = 0;
        UnrelentingDeathKnight_Counter = 0;
        UnrelentingRider_Counter = 0;
        Phase_Timer = 274000;                               //4.34 minutes = 274 seconds
        phase = 1;                                          //We start with phase 1
        Side_Timer = 10000;                                 //10 seconds
        if (m_pInstance && side != 0)
            if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE)))
                pGate->SetGoState(GO_STATE_ACTIVE);
        side = 1;                                           //We start from the living side
        speech = true;

        //Boss unattackable in phase 1
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOTHIK, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (speech && who->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(who, 30.0f) && m_creature->IsHostileTo(who))
        {
            //Say some stuff
            DoScriptText(SAY_SPEECH, m_creature);
            speech = false;

            m_creature->TauntApply(who); //Apply taunt to put boss in combat

            if (m_pInstance)
                if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE)))
                    pGate->SetGoState(GO_STATE_READY);
        }
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOTHIK, IN_PROGRESS);
    }

    void KilledUnit()
    {
        //Say some stuff
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        //Say some stuff
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GOTHIK, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Phase 1
        if (phase == 1)
        {
            //Phase Timer
            if (Phase_Timer < diff)
            {
                //Say some stuff
                DoScriptText(SAY_TELEPORT, m_creature);

                //Descend from balcony
                m_creature->NearTeleportTo(TELE_1X,TELE_1Y, TELE_1Z, TELE_1O);

                //Make boss attackable
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                //Enable movement
                DoStartMovement(m_creature->getVictim());

                phase = 2;

                //End phase 1
                return;
            }else Phase_Timer -= diff;

            //Disable movement
            DoStartNoMovement(m_creature->getVictim());

            //Summon Unrelenting Trainee
            if (UnrelentingTrainee_Counter < (m_bIsRegularMode ? 24 : 36))
            {
                if (UnrelentingTrainee_Timer < diff)
                {
                    for(uint8 i = 0; i < (m_bIsRegularMode ? 2 : 3); i++)    //We summon 2 or 3 every time
                        (m_creature->SummonCreature(16124,UNRELENTING_X,UNRELENTING_Y,UNRELENTING_Z,UNRELENTING_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();

                    UnrelentingTrainee_Timer = 20000;    //20 seconds
                    UnrelentingTrainee_Counter++;
                }else UnrelentingTrainee_Timer -= diff;
            }

            //Summon Unrelenting Death Knight
            if (UnrelentingDeathKnight_Counter < (m_bIsRegularMode ? 7 : 14))
            {
                if (UnrelentingDeathKnight_Timer < diff)
                {
                    for(uint8 i = 0; i < (m_bIsRegularMode ? 1 : 2); i++)    //We summon 1 or 2 every time
                        (m_creature->SummonCreature(16125,UNRELENTING_X,UNRELENTING_Y,UNRELENTING_Z,UNRELENTING_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();

                    UnrelentingDeathKnight_Timer = 25000;    //25 seconds
                    UnrelentingDeathKnight_Counter++;
                }else UnrelentingDeathKnight_Timer -= diff;
            }

            //Summon Unrelenting Rider
            if (UnrelentingRider_Counter < 4)
            {
                if (UnrelentingRider_Timer < diff)
                {
                    (m_creature->SummonCreature(16126,UNRELENTING_X,UNRELENTING_Y,UNRELENTING_Z,UNRELENTING_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();

                    UnrelentingRider_Timer = 30000;    //30 seconds
                    UnrelentingRider_Counter++;
                }else UnrelentingRider_Timer -= diff;
            }
        }

        //Phase 2
        if (phase == 2)
        {
            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 30)
            {
                if (Side_Timer < diff)
                {
                    if (side == 1)
                    {
                        m_creature->NearTeleportTo(TELE_2X,TELE_2Y, TELE_2Z, TELE_2O);
                        side = 2;
                    }
                    else if (side == 2)
                    {
                        m_creature->NearTeleportTo(TELE_1X,TELE_1Y, TELE_1Z, TELE_1O);
                        side = 1;
                    }
                    Side_Timer = 10000;
                }else Side_Timer -= diff;
            }
            else if (side != 0)
            {
                if (m_pInstance)
                    if (GameObject* pGate = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_MILI_GOTH_COMBAT_GATE)))
                        pGate->SetGoState(GO_STATE_ACTIVE);
                side = 0;
            }

            //Spell Harvest Soul timer
            if (HarvestSoul_Timer < diff)
            {
                //Cast spell one on our current target.
                DoCast(m_creature->getVictim(), SPELL_HARVESTSOUL);

                HarvestSoul_Timer = 15000;
            }else HarvestSoul_Timer -= diff;
            
            //Uses Shadow Bolt instead of melee
            if (Shadowbolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOWBOLT : H_SPELL_SHADOWBOLT);

                Shadowbolt_Timer = 1000 + rand()%500;
            }else Shadowbolt_Timer -= diff;
        }      
    }
};

//Unrelenting Trainee
struct MANGOS_DLL_DECL mob_unrelentingtraineeAI : public ScriptedAI
{
    mob_unrelentingtraineeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void JustDied(Unit* Killer)
    {
        //Summon Spectral Trainee
        (m_creature->SummonCreature(16127,SPECTRAL_X,SPECTRAL_Y,SPECTRAL_Z,SPECTRAL_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

//Unrelenting Death Knight
struct MANGOS_DLL_DECL mob_unrelentingdeathknightAI : public ScriptedAI
{
    mob_unrelentingdeathknightAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 ShadowMark_Timer;         //Timer for Shadow Mark spell
    uint32 Intercept_Timer;          //Timer for Intercept spell

    void Reset()
    {
        ShadowMark_Timer = 15000;    //15 seconds
        Intercept_Timer = 0;         //instant
    }

    void JustDied(Unit* Killer)
    {
        //Summon Spectral Death Knight
        (m_creature->SummonCreature(16148,SPECTRAL_X,SPECTRAL_Y,SPECTRAL_Z,SPECTRAL_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Shadow Mark Timer
        if (ShadowMark_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_SHADOW_MARK);

            ShadowMark_Timer = 15000;
        }else ShadowMark_Timer -= diff;

        //Intercept Timer
        if (Intercept_Timer < diff)
        {
            if (!m_creature->IsWithinDist(m_creature->getVictim(),ATTACK_DISTANCE))
            {
                //Cast spell one on our current target.
                DoCast(m_creature->getVictim(),SPELL_CHARGE);
            }

            Intercept_Timer = 2000;
        }else Intercept_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Unrelenting Rider
struct MANGOS_DLL_DECL mob_unrelentingriderAI : public ScriptedAI
{
    mob_unrelentingriderAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 ShadowboltVolley_Timer;         //Timer for Shadowbolt Volley

    void Reset()
    {
        ShadowboltVolley_Timer = 15000;    //15 seconds
    }

    void JustDied(Unit* Killer)
    {
        //Summon Spectral Rider
        (m_creature->SummonCreature(16150,SPECTRAL_X,SPECTRAL_Y,SPECTRAL_Z,SPECTRAL_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();

        //Summon Spectral Horse
        (m_creature->SummonCreature(16149,SPECTRAL_X,SPECTRAL_Y,SPECTRAL_Z,SPECTRAL_O,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,5000))->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Shadowbolt Volley Timer
        if (ShadowboltVolley_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_SHADOWBOLT_VOLLEY);

            ShadowboltVolley_Timer = 15000;
        }else ShadowboltVolley_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Spectral Trainee
struct MANGOS_DLL_DECL mob_spectraltraineeAI : public ScriptedAI
{
    mob_spectraltraineeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 ArcaneExplosion_Timer;         //Timer for Arcane Explosion

    void Reset()
    {
        ArcaneExplosion_Timer = 15000;    //15 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Arcane Explosion Timer
        if (ArcaneExplosion_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_ARCANE_EXPLOSION);

            ArcaneExplosion_Timer = 15000;
        }else ArcaneExplosion_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Spectral Death Knight
struct MANGOS_DLL_DECL mob_spectraldeathknightAI : public ScriptedAI
{
    mob_spectraldeathknightAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Whirlwind_Timer;         //Timer for Whirlwind

    void Reset()
    {
        Whirlwind_Timer = 15000;    //15 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Whirlwind Timer
        if (Whirlwind_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_WHIRLWIND);

            Whirlwind_Timer = 15000;
        }else Whirlwind_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Spectral Rider
struct MANGOS_DLL_DECL mob_spectralriderAI : public ScriptedAI
{
    mob_spectralriderAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 DrainLife_Timer;         //Timer for Drain Life

    void Reset()
    {
        DrainLife_Timer = 15000;    //15 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Drain Life Timer
        if (DrainLife_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_LIFEDRAIN);

            DrainLife_Timer = 15000;
        }else DrainLife_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Spectral Horse
struct MANGOS_DLL_DECL mob_spectralhorseAI : public ScriptedAI
{
    mob_spectralhorseAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 Stomp_Timer;         //Timer for Stomp

    void Reset()
    {
        Stomp_Timer = 15000;    //15 seconds
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Stomp Timer
        if (Stomp_Timer < diff)
        {
            //Cast spell one on our current target.
            DoCast(m_creature->getVictim(),SPELL_STOMP);

            Stomp_Timer = 15000;
        }else Stomp_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gothik(Creature* pCreature)
{
    return new boss_gothikAI(pCreature);
}

CreatureAI* GetAI_mob_unrelentingtrainee(Creature* pCreature)
{
    return new mob_unrelentingtraineeAI(pCreature);
}

CreatureAI* GetAI_mob_unrelentingdeathknight(Creature* pCreature)
{
    return new mob_unrelentingdeathknightAI(pCreature);
}

CreatureAI* GetAI_mob_unrelentingrider(Creature* pCreature)
{
    return new mob_unrelentingriderAI(pCreature);
}

CreatureAI* GetAI_mob_spectraltrainee(Creature* pCreature)
{
    return new mob_spectraltraineeAI(pCreature);
}

CreatureAI* GetAI_mob_spectraldeathknight(Creature* pCreature)
{
    return new mob_spectraldeathknightAI(pCreature);
}

CreatureAI* GetAI_mob_spectralrider(Creature* pCreature)
{
    return new mob_spectralriderAI(pCreature);
}

CreatureAI* GetAI_mob_spectralhorse(Creature* pCreature)
{
    return new mob_spectralhorseAI(pCreature);
}

void AddSC_boss_gothik()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gothik";
    newscript->GetAI = &GetAI_boss_gothik;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_unrelentingtrainee";
    newscript->GetAI = &GetAI_mob_unrelentingtrainee;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_unrelentingdeathknight";
    newscript->GetAI = &GetAI_mob_unrelentingdeathknight;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_unrelentingrider";
    newscript->GetAI = &GetAI_mob_unrelentingrider;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spectraltrainee";
    newscript->GetAI = &GetAI_mob_spectraltrainee;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spectraldeathknight";
    newscript->GetAI = &GetAI_mob_spectraldeathknight;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spectralrider";
    newscript->GetAI = &GetAI_mob_spectralrider;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spectralhorse";
    newscript->GetAI = &GetAI_mob_spectralhorse;
    newscript->RegisterSelf();
}
