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
SDName: Boss Flame Leviathan
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    // Flame Leviathan
    SPELL_MISSILE_BARRAGE       = 62400,
    SPELL_FLAME_VENTS           = 62396,
    SPELL_BATTERING_RAM         = 62376,
    SPELL_GATHERING_SPEED       = 62375,
    SPELL_SYSTEMS_SHUTDOWN      = 62475,

    // Defense Turret
    SPELL_SEARING_FLAME         = 62402
};

enum Adds
{
    NPC_DEFENSE_TURRET          = 33142
};

enum Says
{
    SAY_AGGRO               = -1300200,
    SAY_KILLED_PLAYER1      = -1300201,
    SAY_NO_TOWERS           = -1300202,
    SAY_AT_LEAST_1_TOWER    = -1300203,
    SAY_THORIMS_TOWER       = -1300204,
    SAY_MIMIRONS_TOWER      = -1300205,
    SAY_HODIRS_TOWER        = -1300206,
    SAY_FREYAS_TOWER        = -1300207,
    SAY_CHANGE_TARGET1      = -1300208,
    SAY_CHANGE_TARGET2      = -1300209,
    SAY_CHANGE_TARGET3      = -1300210,
    SAY_PLAYER_ON_TOP       = -1300211,
    SAY_OVERLOAD_CIRCUIT1   = -1300212,
    SAY_OVERLOAD_CIRCUIT2   = -1300213,
    SAY_OVERLOAD_CIRCUIT3   = -1300214,
    SAY_DEATH               = -1300215
};

#define MISSILE_BARRAGE_TIMER   urand(10, 15)*IN_MILISECONDS
#define FLAME_VENTS_TIMER       60*IN_MILISECONDS
#define BATTERING_RAM_TIMER     urand(25, 30)*IN_MILISECONDS
#define GATHERING_SPEED_TIMER   urand(30, 40)*IN_MILISECONDS
#define TARGET_TIMER            30*IN_MILISECONDS

static const float Turret_Pos[4][2] = {
    {218, -17},
    {218, -49},
    {238, -17},
    {238, -49}
};

struct MANGOS_DLL_DECL boss_flame_leviathanAI : public ScriptedAI
{
    boss_flame_leviathanAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_LEVIATHAN)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 const m_uiBossEncounterId;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 MissileBarrage_Timer;
    uint32 FlameVents_Timer;
    uint32 BatteringRam_Timer;
    uint32 GatheringSpeed_Timer;
    uint32 Target_Timer;
    std::vector<uint64> Turret_GUID;
    std::bitset<4> Turret_Alive;
    bool SystemsShutdown;

    void Reset() 
    {
        MissileBarrage_Timer = MISSILE_BARRAGE_TIMER;
        FlameVents_Timer = FLAME_VENTS_TIMER;
        BatteringRam_Timer = BATTERING_RAM_TIMER;
        GatheringSpeed_Timer = GATHERING_SPEED_TIMER;
        Target_Timer = TARGET_TIMER;

        Turret_GUID.assign(4, 0);
        Turret_Alive.reset();
        SystemsShutdown = true;

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        // Summon turrets
        for (size_t i = 0; i < HEROIC(2, 4); i++)
        {
            Creature *Turret = m_creature->SummonCreature(NPC_DEFENSE_TURRET,
                    Turret_Pos[i][0], Turret_Pos[i][1],
                    m_creature->GetPositionZ(), m_creature->GetOrientation(),
                    TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILISECONDS);
            Turret_GUID[i] = Turret->GetGUID();
            Turret_Alive[i] = true;
            Turret->AddThreat(pWho);
        }
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(SAY_KILLED_PLAYER1, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Handle Turrets
        for (size_t i = 0; i < HEROIC(2, 4); i++)
        {
            if (Creature *Turret = m_creature->GetMap()->GetCreature(Turret_GUID[i]))
                if (Turret->isAlive())
                {
                    //Turret->GetMotionMaster()->MovePoint(0, Turret_Pos[i][0], Turret_Pos[i][1], m_creature->GetPositionZ());
                    continue;
                }
            Turret_Alive[i] = false;
        }

        //Systems Shutdown
        if (SystemsShutdown && Turret_Alive.none())
        {
            DoScriptText(SAY_NO_TOWERS, m_creature);
            DoCast(m_creature, SPELL_SYSTEMS_SHUTDOWN, true);
            SystemsShutdown = false;
        }

        //Change target timer
        if (Target_Timer < diff)
        {
            switch(urand(0,2))
            {
                case 0: DoScriptText(SAY_CHANGE_TARGET1, m_creature); break;
                case 1: DoScriptText(SAY_CHANGE_TARGET2, m_creature); break;
                case 2: DoScriptText(SAY_CHANGE_TARGET3, m_creature); break;
            }
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                m_creature->TauntApply(target);

            Target_Timer = TARGET_TIMER;
        }else Target_Timer -= diff;

        //Gathering Speed timer
        if (GatheringSpeed_Timer < diff)
        {
            DoCast(m_creature,SPELL_GATHERING_SPEED);

            GatheringSpeed_Timer = GATHERING_SPEED_TIMER;
        }else GatheringSpeed_Timer -= diff;

        //Battering Ram timer
        if (BatteringRam_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_BATTERING_RAM);

            BatteringRam_Timer = BATTERING_RAM_TIMER;
        }else BatteringRam_Timer -= diff;

        //Flame Vents timer
        if (FlameVents_Timer < diff)
        {
            DoCast(m_creature,SPELL_FLAME_VENTS);

            FlameVents_Timer = FLAME_VENTS_TIMER;
        }else FlameVents_Timer -= diff;

        //Missile Barrage timer
        if (MissileBarrage_Timer < diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target,SPELL_MISSILE_BARRAGE);

            MissileBarrage_Timer = MISSILE_BARRAGE_TIMER;
        }else MissileBarrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL leviathan_turretAI: public Scripted_NoMovementAI
{
    uint32 SearingFlame_Timer;

    leviathan_turretAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        SearingFlame_Timer = 2000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SearingFlame_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SEARING_FLAME);
            SearingFlame_Timer = 2000;
        }
        else
            SearingFlame_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_flame_leviathan(Creature* pCreature)
{
    return new boss_flame_leviathanAI(pCreature);
}

CreatureAI* GetAI_leviathan_turret(Creature* pCreature)
{
    return new leviathan_turretAI(pCreature);
}

void AddSC_boss_flame_leviathan()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_flame_leviathan";
    newscript->GetAI = &GetAI_boss_flame_leviathan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "leviathan_turret";
    newscript->GetAI = &GetAI_leviathan_turret;
    newscript->RegisterSelf();
}
