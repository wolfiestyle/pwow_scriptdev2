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
SDName: Boss Flame Leviathan
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_AGGRO                               = -1603159,
    SAY_SLAY                                = -1603160,
    SAY_DEATH                               = -1603161,

    SAY_CHANGE_1                            = -1603162,
    SAY_CHANGE_2                            = -1603163,
    SAY_CHANGE_3                            = -1603164,
    SAY_PLAYER_RIDE                         = -1603165,
    SAY_OVERLOAD_1                          = -1603166,
    SAY_OVERLOAD_2                          = -1603167,
    SAY_OVERLOAD_3                          = -1603168,

    SAY_HARD_MODE                           = -1603169,

    SAY_TOWER_FROST                         = -1603170,
    SAY_TOWER_FIRE                          = -1603171,
    SAY_TOWER_ENERGY                        = -1603172,
    SAY_TOWER_NATURE                        = -1603173,
    SAY_TOWER_DOWN                          = -1603174,

    EMOTE_PURSUE                            = -1603175,
};
*/

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

#define MISSILE_BARRAGE_TIMER   urand(10, 15)*IN_MILLISECONDS
#define FLAME_VENTS_TIMER       60*IN_MILLISECONDS
#define BATTERING_RAM_TIMER     urand(25, 30)*IN_MILLISECONDS
#define GATHERING_SPEED_TIMER   urand(30, 40)*IN_MILLISECONDS
#define TARGET_TIMER            30*IN_MILLISECONDS

static const float Turret_Pos[4][2] = {
    {-10, -16},
    {-10,  16},
    { 10, -16},
    { 10,  16}
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
    bool SystemsShutdown;

    typedef std::list<uint64> GuidList;
    GuidList m_Turrets;

    void Reset() 
    {
        MissileBarrage_Timer = MISSILE_BARRAGE_TIMER;
        FlameVents_Timer = FLAME_VENTS_TIMER;
        BatteringRam_Timer = BATTERING_RAM_TIMER;
        GatheringSpeed_Timer = GATHERING_SPEED_TIMER;
        Target_Timer = TARGET_TIMER;
        SystemsShutdown = true;

        UnSummonTurrets();

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon && pSummon->GetEntry() == NPC_DEFENSE_TURRET)
        {
            m_Turrets.push_back(pSummon->GetGUID());
            pSummon->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (pSummon && pSummon->GetEntry() == NPC_DEFENSE_TURRET)
            for (GuidList::iterator i = m_Turrets.begin(); i != m_Turrets.end(); ++i)
                if ((*i) == pSummon->GetGUID())
                {
                    m_Turrets.erase(i);
                    break;
                }
    }

    void SummonedCreatureDespawn(Creature *pSummon)
    {
        SummonedCreatureJustDied(pSummon);
    }

    void Aggro(Unit* pWho)
    {
        // Summon turrets
        for (size_t i = 0; i < HEROIC(2, 4); i++)
            Creature *Turret = DoSpawnCreature(NPC_DEFENSE_TURRET,
                    Turret_Pos[i][0], Turret_Pos[i][1], 0, m_creature->GetOrientation(),
                    TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
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

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        //Systems Shutdown
        if (SystemsShutdown && m_Turrets.empty())
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
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
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
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target,SPELL_MISSILE_BARRAGE);

            MissileBarrage_Timer = MISSILE_BARRAGE_TIMER;
        }else MissileBarrage_Timer -= diff;

        DoMeleeAttackIfReady();
    }

    void UnSummonTurrets()
    {
        for (GuidList::const_iterator i = m_Turrets.begin(); i != m_Turrets.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        m_Turrets.clear();
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
