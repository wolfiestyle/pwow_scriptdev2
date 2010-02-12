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
SDName: Boss Mimiron
SD%Complete: I'd hazard it at around 80%
SDComment: I can't get P3Wx2 laser barrage to work (weirdness in point movement, graphics), voltron movement BAD(vehicle support!), hard mode completely not done
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "TemporarySummon.h"
#include "NullCreatureAI.h"

enum Says
{
    SAY_AGGRO                   = -1300085,
    SAY_HARD_MODE               = -1300086,
    SAY_BERSERK                 = -1300087,
    SAY_LEV_ACTIVATED           = -1300088,
    SAY_LEV_KILL_PLAYER1        = -1300089,
    SAY_LEV_KILL_PLAYER2        = -1300090,
    SAY_LEV_DEATH               = -1300091,
    SAY_VX_ACTIVATED            = -1300092,
    SAY_VX_KILLED_PLAYER1       = -1300093,
    SAY_VX_KILLED_PLAYER2       = -1300094,
    SAY_VX_DEATH                = -1300095,
    SAY_AERIAL_ACTIVATED        = -1300096,
    SAY_AERIAL_KILLED_PLAYER1   = -1300097,
    SAY_AERIAL_KILLED_PLAYER2   = -1300098,
    SAY_AERIAL_DEATH            = -1300099,
    SAY_V07_ACTIVATED           = -1300100,
    SAY_V07_KILLED_PLAYER1      = -1300101,
    SAY_V07_KILLED_PLAYER2      = -1300102,
    SAY_DEFEATED                = -1300103,
    SAY_YOGG_HELP               = -1300104
};

enum NPCS
{
    // Leviathan
    NPC_PROXIMITY_MINE              = 34362,

    // VX001
    NPC_ROCKET_STRIKE_TARGET        = 34047,
    NPC_LASER_BARRAGE_TARGET        = 34211,

    // Aerial Command Unit
    NPC_MAGNETIC_CORE               = 34068
};

enum Spells
{
    SPELL_BERSERK                   = 47008, //I'm REALLY not sure about the real spell

    // Leviathan
    SPELL_PROXIMITY_MINES           = 63027,
    SPElL_PROXIMITY_MINE_EXPLODE_N  = 66351,
    SPELL_PROXIMITY_MINE_EXPLODE_H  = 63009,
    SPELL_NAHPALM_SHELLS_N          = 63666,
    SPELL_NAHPALM_SHELLS_H          = 65026,
    SPELL_PLASMA_BLAST_N            = 62997,
    SPELL_PLASMA_BLAST_H            = 64529,
    SPELL_SHOCK_BLAST               = 63631,

    // VX-001
    SPELL_HEAT_WAVE                 = 64534,
    SPELL_RAPID_BURST_N             = 64019,
    SPELL_RAPID_BURST_H             = 64531,
    SPELL_ROCKET_STRIKE             = 64064,
    SPELL_ROCKET_STRIKE_TARGET      = 63036,
    SPELL_SPINNING_UP               = 63414,
    SPELL_LASER_VISUAL              = 63300,
    SPELL_HAND_PULSE_N              = 64348,
    SPELL_HAND_PULSE_H              = 64537,

    // Aerial command Unit
    SPELL_PLASMA_BALL_N             = 63689,
    SPELL_PLASMA_BALL_H             = 64535,
    // Adds for ACU
    SPELL_BOMB_SUMMON               = 63811,
    SPELL_BOMB_EXPLODE              = 63801,
    SPELL_ASSAULT_SUMMON            = 64427,
    SPELL_JUNK_SUMMON               = 63819,
    SPELL_MAGNETIC_FIELD            = 64668,
    SPELL_MAGNETIC_PULL             = 31703, //"This spell is meant for general use on creature scripts." yay.
    SPELL_MAGNETIC_CORE_EFFECT      = 64436,

    // all adds
    SPELL_SELF_REPAIR               = 64383
};

enum AddIds
{
    NPC_BOMB_BOT                    = 33836,
    NPC_ASSAULT_BOT                 = 34057,
    NPC_JUNK_BOT                    = 33855
};

enum Phases
{
    PHASE_START = 1,
    PHASE_LEV,
    PHASE_VX,
    PHASE_AERIAL,
    PHASE_VOLTRON,
    PHASE_END
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_START_NEXT_PHASE,

    // Leviathan
    EVENT_PROXIMITY_MINES,
    EVENT_NAHPALM_SHELLS,
    EVENT_PLASMA_BLAST,
    EVENT_SHOCK_BLAST,

    // VX-001
    EVENT_RAPID_BURST_BEGIN,
    EVENT_RAPID_BURST,
    EVENT_ROCKET_STRIKE,
    EVENT_ROCKET_STRIKE2,
    EVENT_LASER_BARRAGE,

    // Aerial command unit
    EVENT_PLASMA_BALL,
    EVENT_BOMB_BOT,
    EVENT_TRASH_BOT,
    EVENT_ASSAULT_BOT,
    EVENT_MAGNETIC_FIELD
};

#define TIMER_BERSERK           15*MINUTE*IN_MILISECONDS
#define TIMER_START_FIRST_PHASE 20*IN_MILISECONDS
#define TIMER_START_NEXT_PHASE  30*IN_MILISECONDS
// Leviathan
#define TIMER_PROXIMITY_MINES   urand(28,30)*IN_MILISECONDS
#define TIMER_NAHPALM_SHELLS    urand(7,10)*IN_MILISECONDS
#define TIMER_PLASMA_BLAST      urand(28,32)*IN_MILISECONDS
#define TIMER_SHOCK_BLAST       urand(7,13)*IN_MILISECONDS
// VX-001
#define TIMER_RAPID_BURST_BEGIN 4*IN_MILISECONDS
#define TIMER_RAPID_BURST       0.5*IN_MILISECONDS
#define TIMER_ROCKET_STRIKE     urand(20,25)*IN_MILISECONDS
#define TIMER_LASER_BARRAGE     30*IN_MILISECONDS
// Aerial Command Unit
#define TIMER_PLASMA_BALL       urand(3,4)*IN_MILISECONDS
#define TIMER_BOMB_BOT          urand(10,15)*IN_MILISECONDS //not quite sure about these summon timers
#define TIMER_ASSAULT_BOT       urand(25,35)*IN_MILISECONDS //<-
#define TIMER_TRASH_BOT         urand(10,15)*IN_MILISECONDS //<-
#define TIMER_MAGNETIC_FIELD    urand(15,20)*IN_MILISECONDS //not sure about this one either

#define GET_CREATURE_FROM_GUID(guid)    (m_creature->GetMap()->GetCreature(guid))
//only for use in voltron peices
template <typename T>
T* GetAIFromGUID(uint64 guid, Creature const* source)
{
    if (Creature *pCreature = source->GetMap()->GetCreature(guid))
        return dynamic_cast<T*>(pCreature->AI());
    else
        return NULL;
}
#define MIMIRON_AI      GetAIFromGUID<boss_mimironAI>(MimironGUID, m_creature)
#define GET_MIMIRON_AI \
    boss_mimironAI *mimironAI = MIMIRON_AI; \
    if (!mimironAI) return;
#define IF_MIMIRON_AI \
    boss_mimironAI *mimironAI = MIMIRON_AI; \
    if (mimironAI)

struct mob_leviathan_mk_II_AI;
struct mob_VX001_AI;
struct mob_aerial_command_unitAI;
struct boss_mimironAI;
struct mob_proximity_mineAI;

struct MANGOS_DLL_DECL boss_mimironAI: public Scripted_NoMovementAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    bool HasFinished;
    EventMap Events;
    uint64 LeviathanGUID;
    uint64 VX001_GUID;
    uint64 AerialCommandGUID;
    uint32 CurrPhase;

    boss_mimironAI(Creature *pCreature);

    void Reset();
    void JustDied(Unit *pKiller);
    void Aggro(Unit *pWho);
    void UpdateAI(const uint32 uiDiff);
    void RegisterPhaseFinished(uint32 phase);
    void KilledUnit(Unit *victim);
    void RegisterVoltronPieceDied(uint32 id);
};

//VoltronPieceAI- basic start point for all the voltron pieces
struct MANGOS_DLL_DECL VoltronPieceAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventMap Events;
    uint64 MimironGUID;
    bool IsInUse;
    bool IsVoltron;
    bool IsDeadAsVoltron;

    VoltronPieceAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if (Creature *Mimiron = GET_CREATURE(TYPE_MIMIRON))
            MimironGUID = Mimiron->GetGUID();
        Reset();    //note: will call subclass function first
    }

    void Reset()
    {
        IsInUse = false;
        IsVoltron = false;
        IsDeadAsVoltron = false;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SELF_REPAIR)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            IsInUse = true;
        }
    }

    void DamageTaken(Unit *pDoneBy, uint32& uiDamage)
    {
        if(uiDamage > m_creature->GetHealth())
        {
            m_creature->RemoveAllAuras();
            m_creature->SetHealth(1);
            m_creature->RemoveAllAttackers();
            IsInUse = false;
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            uiDamage = 0;
            if(IsVoltron)
            {
                if(!IsDeadAsVoltron)
                {
                    IsDeadAsVoltron = true;
                    JustDied(pDoneBy);
                }
            }
            else
                JustDied(pDoneBy);
        }
    }

    void JustDied(Unit *killer)
    {
        GET_MIMIRON_AI;
        if (!IsVoltron)
            mimironAI->RegisterPhaseFinished(mimironAI->CurrPhase);
        else // if(IsVoltron)
            mimironAI->RegisterVoltronPieceDied(m_creature->GetEntry());
    }

    void KilledUnit(Unit *who)
    {
        GET_MIMIRON_AI;
        if(who && who->GetTypeId() == TYPEID_PLAYER)
            mimironAI->KilledUnit(who);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        GET_MIMIRON_AI;
        if(IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            if (mimironAI)
                mimironAI->EnterEvadeMode();
        }

        //needs vehicle support for proper animations + movements
        //for now, hacky fix
        if (IsVoltron && IsInUse && m_creature->GetEntry() != NPC_LEVIATHAN_MKII)
            if (Creature *Leviathan = GET_CREATURE_FROM_GUID(mimironAI->LeviathanGUID))
            {
                float x, y, z, o;
                Leviathan->GetPosition(x, y, z);
                o = Leviathan->GetOrientation();
                z += m_creature->GetEntry() == NPC_VX001 ? 6 : 11;
                m_creature->NearTeleportTo(x, y, z, o);
            }
    }

    virtual void StartVoltron()
    {
        IF_MIMIRON_AI
            m_creature->Attack(mimironAI->m_creature->getVictim(), true);
        Events.Reset();
        IsInUse = true;
        IsVoltron = true;
        SetCombatMovement(false);
        m_creature->setDeathState(JUST_ALIVED);
        m_creature->SetHealth(m_creature->GetMaxHealth()/2);
        Aggro(m_creature->getVictim());
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        if (m_creature->GetEntry() != NPC_LEVIATHAN_MKII)
            if (Creature *Leviathan = GET_CREATURE_FROM_GUID(mimironAI->LeviathanGUID))
            {
                float x, y, z, o;
                Leviathan->GetPosition(x, y, z);
                o = Leviathan->GetOrientation();
                z += m_creature->GetEntry() == NPC_VX001 ? 6 : 11;
                m_creature->NearTeleportTo(x, y, z, o);
            }
    }
};

struct MANGOS_DLL_DECL mob_leviathan_mk_II_AI : public VoltronPieceAI
{
    std::list<uint64> listMinesGUID;

    mob_leviathan_mk_II_AI(Creature *pCreature) : VoltronPieceAI(pCreature) 
    {
    }

    void Reset()
    {
        VoltronPieceAI::Reset();
        SetCombatMovement(true);
        Events.Reset();
    }

    void StartVoltron()
    {
        VoltronPieceAI::StartVoltron();
        SetCombatMovement(true);
    }

    void Aggro(Unit *who)
    {
        if(!IsInUse)
        {
            EnterEvadeMode();
            return;
        }
        Events.Reset();
        if(!IsVoltron)
            Events.ScheduleEvent(EVENT_NAHPALM_SHELLS, TIMER_NAHPALM_SHELLS);
        Events.ScheduleEvent(EVENT_PLASMA_BLAST, TIMER_PLASMA_BLAST);
        Events.ScheduleEvent(EVENT_PROXIMITY_MINES, 0);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell);

    void GetRandomPointInPerimeter(float radius, float &x, float &y)
    {
        float ang = 2*M_PI * rand_norm();
        x = radius * cos(ang);
        y = radius * sin(ang);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        VoltronPieceAI::UpdateAI(uiDiff);

        if(!IsInUse)
        {
            if(!listMinesGUID.empty())
            {
                for(std::list<uint64>::iterator i = listMinesGUID.begin(); i != listMinesGUID.end(); ++i)
                {
                    if (Creature *pCreature = GET_CREATURE_FROM_GUID(*i))
                        pCreature->ForcedDespawn();
                }
                listMinesGUID.clear();
            }
            Events.Reset();
            DoStartNoMovement(m_creature->getVictim());
            return;
        }

        Events.Update(uiDiff);
        while (uint32 eventId = Events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_PROXIMITY_MINES:
                DoCast(m_creature, SPELL_PROXIMITY_MINES);
                Events.RescheduleEvent(EVENT_PROXIMITY_MINES, TIMER_PROXIMITY_MINES);
                break;

            case EVENT_NAHPALM_SHELLS:
                if (Player *victim = GetPlayerAtMinimumRange(15))   // ranged player
                    DoCast(victim, HEROIC(SPELL_NAHPALM_SHELLS_N, SPELL_NAHPALM_SHELLS_H));
                else if (Unit *target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, HEROIC(SPELL_NAHPALM_SHELLS_N, SPELL_NAHPALM_SHELLS_H));
                Events.RescheduleEvent(EVENT_NAHPALM_SHELLS, TIMER_NAHPALM_SHELLS);
                break;

            case EVENT_PLASMA_BLAST:
                if(!IsVoltron)
                    if (Unit *target = m_creature->getVictim())
                        DoCast(target, HEROIC(SPELL_PLASMA_BLAST_N,SPELL_PLASMA_BLAST_H));
                Events.RescheduleEvent(EVENT_SHOCK_BLAST, TIMER_SHOCK_BLAST);
                Events.RescheduleEvent(EVENT_PLASMA_BLAST, TIMER_PLASMA_BLAST);
                break;

            case EVENT_SHOCK_BLAST:
                //timed urand(7,13) secs after plasma blast
                DoCast(m_creature->getVictim(), SPELL_SHOCK_BLAST);
                break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void JustSummoned(Creature *summ);

    void DeSummonMine(uint64 mineGUID)
    {
        listMinesGUID.remove(mineGUID);
        if (Creature *pSummon = GET_CREATURE_FROM_GUID(mineGUID))
            pSummon->ForcedDespawn();
    }
};

struct MANGOS_DLL_DECL mob_proximity_mineAI : public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    mob_leviathan_mk_II_AI *levAI;
    uint64 LeviathanGUID;   // set from mob_leviathan_mk_II_AI::SpellHit
    uint32 ExplodeTimer;

    mob_proximity_mineAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature) 
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        ExplodeTimer = 35*IN_MILISECONDS;
    }

    void Explode()
    {
        DoCast(m_creature, HEROIC(SPElL_PROXIMITY_MINE_EXPLODE_N, SPELL_PROXIMITY_MINE_EXPLODE_H), true);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == SPElL_PROXIMITY_MINE_EXPLODE_N || spell->Id == SPELL_PROXIMITY_MINE_EXPLODE_H)
            JustDied(caster);
    }

    void MoveInLineOfSight(Unit *who)
    {
        if(who && who->GetTypeId() == TYPEID_PLAYER && who->IsWithinDistInMap(m_creature, 2.0f))
            Explode();
    }

    void JustDied(Unit *killer)
    {
        if (Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID))
        {
            if (mob_leviathan_mk_II_AI *leviathanAI = dynamic_cast<mob_leviathan_mk_II_AI*>(Leviathan->AI()))
                leviathanAI->DeSummonMine(m_creature->GetGUID());
        }
        else
            m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 diff)
    {
        if(ExplodeTimer < diff)
            Explode();
        else
            ExplodeTimer -= diff;
    }
};

struct MANGOS_DLL_DECL mob_VX001_AI : public VoltronPieceAI
{
    Creature *LaserTar;
    float LaserStartAngle;

    mob_VX001_AI(Creature *pCreature): 
        VoltronPieceAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        VoltronPieceAI::Reset();
        DoCast(m_creature, SPELL_HEAT_WAVE, true);
        SetCombatMovement(false);
        LaserTar = NULL;
        LaserStartAngle = 0;
    }

    void AttackStart(Unit *pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if(!IsVoltron)
                DoStartNoMovement(pWho);
        }
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        if(IsVoltron)
            m_creature->RemoveAurasDueToSpell(SPELL_HEAT_WAVE);
        Events.ScheduleEvent(EVENT_RAPID_BURST_BEGIN, TIMER_RAPID_BURST_BEGIN);
        Events.ScheduleEvent(EVENT_ROCKET_STRIKE, TIMER_ROCKET_STRIKE);
        Events.ScheduleEvent(EVENT_LASER_BARRAGE, TIMER_LASER_BARRAGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || !IsInUse)
            return;

        VoltronPieceAI::UpdateAI(uiDiff);

        Events.Update(uiDiff);
        if(LaserTar)
            m_creature->SetFacingToObject(LaserTar);

        while(uint32 EventId = Events.ExecuteEvent())
        {
            switch (EventId)
            {
            case EVENT_RAPID_BURST_BEGIN:
                Events.RescheduleEvent(EVENT_RAPID_BURST_BEGIN, TIMER_RAPID_BURST_BEGIN);
                Events.ScheduleEvent(EVENT_RAPID_BURST, TIMER_RAPID_BURST);
                Events.ScheduleEvent(EVENT_RAPID_BURST, 2*TIMER_RAPID_BURST);
                Events.ScheduleEvent(EVENT_RAPID_BURST, 3*TIMER_RAPID_BURST);
                Events.ScheduleEvent(EVENT_RAPID_BURST, 4*TIMER_RAPID_BURST);
                Events.ScheduleEvent(EVENT_RAPID_BURST, 5*TIMER_RAPID_BURST);
                //fall through
            case EVENT_RAPID_BURST:
                if(!LaserTar)
                    if (Unit *Target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        if(IsVoltron)
                            DoCast(Target, HEROIC(SPELL_HAND_PULSE_N, SPELL_HAND_PULSE_H));
                        else
                            DoCast(Target, HEROIC(SPELL_RAPID_BURST_N, SPELL_RAPID_BURST_H));
                    }
                break;

            case EVENT_ROCKET_STRIKE:
                if(IsVoltron)
                    Events.ScheduleEvent(EVENT_ROCKET_STRIKE2, 1);
                Events.RescheduleEvent(EVENT_ROCKET_STRIKE, TIMER_ROCKET_STRIKE);
            case EVENT_ROCKET_STRIKE2:
                if(!LaserTar)
                    if (Unit *Target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        float x, y, z;
                        Target->GetPosition(x, y, z);
                        m_creature->CastSpell(x, y, z, SPELL_ROCKET_STRIKE_TARGET, false);
                        Creature *RocketTar = m_creature->SummonCreature(NPC_ROCKET_STRIKE_TARGET, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 60*IN_MILISECONDS);
                        if(RocketTar)
                        {
                            RocketTar->SetMonsterMoveFlags(MONSTER_MOVE_NONE);
                            RocketTar->CastSpell(RocketTar, SPELL_ROCKET_STRIKE, false);
                        }
                    }
                break;

            case EVENT_LASER_BARRAGE:
                /*if(MimironAI->m_creature && !LaserTar)
                {
                    LaserTar = m_creature->SummonCreature(NPC_LASER_BARRAGE_TARGET,30*cos(LaserStartAngle)+m_creature->GetPositionX(),30*sin(LaserStartAngle)+m_creature->GetPositionY(),MimironAI->m_creature->GetPositionZ(),0,TEMPSUMMON_CORPSE_DESPAWN,0);
                }
                if(LaserTar)
                {
                    DoCast(LaserTar,SPELL_SPINNING_UP);
                }
                Events.RescheduleEvent(EVENT_LASER_BARRAGE,TIMER_LASER_BARRAGE);*/
                break;
            }
        }
    }
};

struct MANGOS_DLL_DECL mob_burst_targetAI : public NullCreatureAI
{
    mob_VX001_AI *VX001_AI;
    uint64 VX001_GUID;
    uint32 LaserStartAngle;
    mob_burst_targetAI(Creature *pCreature) : NullCreatureAI(pCreature) 
    {
        /*
        //36.5257 is dist btw two points on perimeter of circle with radius 30, separated by 5pi/12 radians (75 degrees), 5 secs is time to be traversed in
        m_creature->SetSpeed(MOVE_WALK,36.5257/5,true);
        GET_CREATURE_FROM_GUID(VX001_GUID) = NULL;
        GET_CREATURE_FROM_GUID(VX001_GUID) = GetClosestCreatureWithEntry(m_creature,NPC_VX001,50);
        VX001_AI = NULL;
        if(GET_CREATURE_FROM_GUID(VX001_GUID))
        {
            VX001_AI = dynamic_cast<mob_VX001_AI*>(GET_CREATURE_FROM_GUID(VX001_GUID)->AI());
            LaserStartAngle = GET_CREATURE_FROM_GUID(VX001_GUID)->GetAngle(m_creature);
            m_creature->GetMotionMaster()->MovePoint(1,GET_CREATURE_FROM_GUID(VX001_GUID)->GetPositionX()+30*cos(LaserStartAngle+5*M_PI/12),
                GET_CREATURE_FROM_GUID(VX001_GUID)->GetPositionY()+30*sin(LaserStartAngle+5*M_PI/12),m_creature->GetPositionZ());
        }
        */
        Reset();
    }

    void MovementInform(uint32 type, uint32 data)
    {/*
        if(type != POINT_MOTION_TYPE)
            return;
        switch(data)
        {
        case 1:
            m_creature->GetMotionMaster()->MovePoint(2,GET_CREATURE_FROM_GUID(VX001_GUID)->GetPositionX()+30*cos(LaserStartAngle+5*M_PI/6),
                GET_CREATURE_FROM_GUID(VX001_GUID)->GetPositionY()+30*sin(LaserStartAngle+5*M_PI/6),m_creature->GetPositionZ());
            break;
        case 2:
            if(VX001_AI)
                VX001_AI->LaserTar = NULL;
            m_creature->ForcedDespawn();
            break;
        }*/
    }
    
    void Reset()
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
    }

};

struct MANGOS_DLL_DECL mob_aerial_command_unitAI : public VoltronPieceAI
{
    bool IsOnGround;

    mob_aerial_command_unitAI(Creature *pCreature) : VoltronPieceAI(pCreature) 
    {
        Reset();
    }

    void Reset()
    {
        VoltronPieceAI::Reset();
        IsOnGround = false;
        m_creature->SetMonsterMoveFlags(MONSTER_MOVE_FLY);
        SetCombatMovement(true);
    }

    void StartVoltron()
    {
        VoltronPieceAI::StartVoltron();
        IsOnGround = false;
        if (Creature *MagneticCore = GetClosestCreatureWithEntry(m_creature, NPC_MAGNETIC_CORE, 30.0f))
            MagneticCore->ForcedDespawn();
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        if(!IsVoltron)
        {
            Events.ScheduleEvent(EVENT_BOMB_BOT, TIMER_BOMB_BOT);
            Events.ScheduleEvent(EVENT_ASSAULT_BOT, TIMER_ASSAULT_BOT);
            Events.ScheduleEvent(EVENT_TRASH_BOT, TIMER_TRASH_BOT);
        }
        Events.ScheduleEvent(EVENT_PLASMA_BALL, TIMER_PLASMA_BALL);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        VoltronPieceAI::UpdateAI(uiDiff);

        if (!IsInUse)
        {
            DoStartNoMovement(m_creature->getVictim());
            return;
        }

        Events.Update(uiDiff);

        while(uint32 EventId = Events.ExecuteEvent())
        {
            switch (EventId)
            {
            case EVENT_PLASMA_BALL:
                if(!IsOnGround)
                    if (Unit *target = SelectUnit(SELECT_TARGET_TOPAGGRO, 0))
                        DoCast(target, HEROIC(SPELL_PLASMA_BALL_N, SPELL_PLASMA_BALL_H));
                Events.RescheduleEvent(EVENT_PLASMA_BALL, TIMER_PLASMA_BALL);
                break;
            case EVENT_BOMB_BOT:
                if(!IsOnGround)
                    DoCast(m_creature, SPELL_BOMB_SUMMON);
                Events.RescheduleEvent(EVENT_BOMB_BOT, TIMER_BOMB_BOT);
                break;
            case EVENT_ASSAULT_BOT:
                if(!IsOnGround)
                {
                    WorldLocation SummonLoc = GetRandomSummonArea();
                    m_creature->CastSpell(SummonLoc.coord_x, SummonLoc.coord_y, SummonLoc.coord_z, SPELL_ASSAULT_SUMMON, false);
                }
                Events.RescheduleEvent(EVENT_ASSAULT_BOT, TIMER_ASSAULT_BOT);
                break;
            case EVENT_TRASH_BOT:
                if(!IsOnGround)
                {
                    WorldLocation SummonLoc = GetRandomSummonArea();
                    m_creature->CastSpell(SummonLoc.coord_x, SummonLoc.coord_y, SummonLoc.coord_z, SPELL_JUNK_SUMMON, false);
                }
                Events.RescheduleEvent(EVENT_TRASH_BOT, TIMER_TRASH_BOT);
                break;
            }
        }

        if(!IsVoltron)
        {
            Creature *MagneticCore = GetClosestCreatureWithEntry(m_creature, NPC_MAGNETIC_CORE, 100.0f);
            if(MagneticCore)
            {
                if(!IsOnGround)
                {
                    MagneticCore->CastSpell(m_creature, SPELL_MAGNETIC_PULL, false);
                    m_creature->CastSpell(m_creature, SPELL_MAGNETIC_CORE_EFFECT, false);
                    DoStartNoMovement(m_creature->getVictim());
                    IsOnGround = true;
                }
                else
                {
                    DoStartNoMovement(m_creature->getVictim());
                    if(!m_creature->HasAura(SPELL_MAGNETIC_CORE_EFFECT))
                    {
                        DoStartMovement(m_creature->getVictim());
                        IsOnGround = false;
                        MagneticCore->ForcedDespawn();
                    }
                }
            }
        }
    }//yet again the end brackets!

    void JustSummoned(Creature *summ)
    {
        if(summ)
            summ->SetInCombatWithZone();
    }

    WorldLocation GetRandomSummonArea()
    {
        // summ locs at:
        // 16,27,39 yrds away from mim (diff of 11)
        // at pi/3, pi, 4pi/3 (diff of +/- 2pi/3 from pi)
        float radius = 16 + 11*urand(0,2);
        float ang = M_PI + float(irand(-1,1))*(2*M_PI/3);

        WorldLocation returnLoc;
        if (Creature *Mimiron = GET_CREATURE_FROM_GUID(MimironGUID))
        {
            returnLoc.coord_x = Mimiron->GetPositionX() + radius * cos(ang);
            returnLoc.coord_y = Mimiron->GetPositionY() + radius * sin(ang);
            returnLoc.coord_z = m_creature->GetPositionZ();
        }
        return returnLoc;
    }
};

struct MANGOS_DLL_DECL mob_bomb_botAI : public ScriptedAI
{
    mob_bomb_botAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_creature->getVictim()->IsWithinDistInMap(m_creature, 3.0f))
        {
            DoCast(m_creature->getVictim(), SPELL_BOMB_EXPLODE, true);
            m_creature->ForcedDespawn(1000);
        }
    }
};

struct MANGOS_DLL_DECL mob_assault_botAI : public ScriptedAI
{
    EventMap Events;

    mob_assault_botAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit *who)
    {
        Events.ScheduleEvent(EVENT_MAGNETIC_FIELD, TIMER_MAGNETIC_FIELD);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        uint32 EventId = Events.ExecuteEvent();
        if (EventId == EVENT_MAGNETIC_FIELD)
        {
            DoCast(m_creature->getVictim(), SPELL_MAGNETIC_FIELD);
            Events.RescheduleEvent(EVENT_MAGNETIC_FIELD, TIMER_MAGNETIC_FIELD);
        }

        DoMeleeAttackIfReady();
    }
};

//MIMIRON FUNCTIONS
//(to avoid nasty type casting errors)
boss_mimironAI::boss_mimironAI(Creature *pCreature):
    Scripted_NoMovementAI(pCreature),
    m_uiBossEncounterId(TYPE_MIMIRON)
{
    m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
    m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    LeviathanGUID = VX001_GUID = AerialCommandGUID = 0;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    HasFinished = false;
    Reset();
}

void boss_mimironAI::Reset()
{
    Events.Reset();
    if(Creature *VX001 = GET_CREATURE_FROM_GUID(VX001_GUID))
    {
        static_cast<TemporarySummon*>(VX001)->UnSummon();
        VX001_GUID = 0;
    }
    if(Creature *AerialCommand = GET_CREATURE_FROM_GUID(AerialCommandGUID))
    {
        static_cast<TemporarySummon*>(AerialCommand)->UnSummon();
        AerialCommandGUID = 0;
    }
    if(Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID))
    {
        static_cast<TemporarySummon*>(Leviathan)->UnSummon();
        LeviathanGUID = 0;
    }
    if (m_pInstance && !HasFinished)
        m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);

    CurrPhase = PHASE_START;
}

void boss_mimironAI::JustDied(Unit *pKiller)
{
    //instance update in RegisterPhaseFinished()
}

void boss_mimironAI::Aggro(Unit *pWho)
{
    if(!HasFinished)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        CurrPhase = PHASE_START;
        Events.Reset();
        Events.ScheduleEvent(EVENT_START_NEXT_PHASE, TIMER_START_FIRST_PHASE);
        Events.ScheduleEvent(EVENT_BERSERK, TIMER_BERSERK);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }
}

void boss_mimironAI::UpdateAI(const uint32 uiDiff)
{
    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        return;

    Events.Update(uiDiff);

    while (uint32 eventId = Events.ExecuteEvent())
    {
        switch (eventId)
        {
        case EVENT_BERSERK:
            DoScriptText(SAY_BERSERK, m_creature);
            if(Creature *AerialCommand = GET_CREATURE_FROM_GUID(AerialCommandGUID))
            {
                AerialCommand->InterruptNonMeleeSpells(false);
                AerialCommand->CastSpell(AerialCommand, SPELL_BERSERK, false);
            }
            if(Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID))
            {
                Leviathan->InterruptNonMeleeSpells(false);
                Leviathan->CastSpell(Leviathan, SPELL_BERSERK, false);
            }
            if(Creature *VX001 = GET_CREATURE_FROM_GUID(VX001_GUID))
            {
                VX001->InterruptNonMeleeSpells(false);
                VX001->CastSpell(VX001, SPELL_BERSERK, false);
            }
            break;
        case EVENT_START_NEXT_PHASE:
            CurrPhase++;
            switch(CurrPhase)
            {
            case PHASE_LEV:
                DoScriptText(SAY_LEV_ACTIVATED, m_creature);
                if(Creature *Leviathan = DoSpawnCreature(NPC_LEVIATHAN_MKII,0,0,0,0,TEMPSUMMON_MANUAL_DESPAWN,0))
                {
                    LeviathanGUID = Leviathan->GetGUID();
                    if (mob_leviathan_mk_II_AI *LevAI = dynamic_cast<mob_leviathan_mk_II_AI*>(Leviathan->AI()))
                        LevAI->IsInUse = true;
                }
                break;
            case PHASE_VX:
                DoScriptText(SAY_VX_ACTIVATED, m_creature);
                if(Creature *VX001 = DoSpawnCreature(NPC_VX001,0,0,0,0,TEMPSUMMON_MANUAL_DESPAWN,0))
                {
                    VX001_GUID = VX001->GetGUID();
                    if (mob_VX001_AI *VX_AI = dynamic_cast<mob_VX001_AI*>(VX001->AI()))
                        VX_AI->IsInUse = true;
                }
                break;
            case PHASE_AERIAL:
                DoScriptText(SAY_AERIAL_ACTIVATED, m_creature);
                if(Creature *AerialCommand = DoSpawnCreature(NPC_AERIAL_COMMAND_UNIT,0,0,10,0,TEMPSUMMON_MANUAL_DESPAWN,0))
                {
                    AerialCommandGUID = AerialCommand->GetGUID();
                    if (mob_aerial_command_unitAI *ACU_AI = dynamic_cast<mob_aerial_command_unitAI*>(AerialCommand->AI()))
                        ACU_AI->IsInUse = true;
                }
                break;
            case PHASE_VOLTRON:
                DoScriptText(SAY_V07_ACTIVATED, m_creature);
                if(Creature *VX001 = GET_CREATURE_FROM_GUID(VX001_GUID))
                {
                    VX001->RemoveAurasDueToSpell(SPELL_HEAT_WAVE);
                    if (VoltronPieceAI *voltronAI = dynamic_cast<VoltronPieceAI*>(VX001->AI()))
                        voltronAI->StartVoltron();
                }
                if(Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID))
                    if (VoltronPieceAI *voltronAI = dynamic_cast<VoltronPieceAI*>(Leviathan->AI()))
                        voltronAI->StartVoltron();
                if(Creature *AerialCommand = GET_CREATURE_FROM_GUID(AerialCommandGUID))
                    if (VoltronPieceAI *voltronAI = dynamic_cast<VoltronPieceAI*>(AerialCommand->AI()))
                        voltronAI->StartVoltron();
                break;
            case PHASE_END:
                break;
            }
        }
    }
}

void boss_mimironAI::RegisterPhaseFinished(uint32 phase)
{
    switch (phase)
    {
    case PHASE_LEV:
        DoScriptText(SAY_LEV_DEATH, m_creature);
        break;
    case PHASE_VX:
        DoScriptText(SAY_VX_DEATH, m_creature);
        break;
    case PHASE_AERIAL:
        {
            DoScriptText(SAY_AERIAL_DEATH, m_creature);
            // remove all summons
            std::list<Creature*> Summons;
            GetCreatureListWithEntryInGrid(Summons, m_creature, NPC_BOMB_BOT, 100.0f);
            GetCreatureListWithEntryInGrid(Summons, m_creature, NPC_ASSAULT_BOT, 100.0f);
            GetCreatureListWithEntryInGrid(Summons, m_creature, NPC_JUNK_BOT, 100.0f);
            for (std::list<Creature*>::const_iterator i = Summons.begin(); i != Summons.end(); ++i)
                (*i)->ForcedDespawn();
            break;
        }
    case PHASE_VOLTRON:
        if (Creature *VX001 = GET_CREATURE_FROM_GUID(VX001_GUID))
            VX001->InterruptNonMeleeSpells(true);

        if (Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID))
            Leviathan->InterruptNonMeleeSpells(true);

        if (Creature *AerialCommand = GET_CREATURE_FROM_GUID(AerialCommandGUID))
            AerialCommand->InterruptNonMeleeSpells(true);

        DoScriptText(SAY_DEFEATED, m_creature);
        Events.Reset();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
        HasFinished = true;
        m_creature->setFaction(35);
        EnterEvadeMode();
        return;
    }
    Events.ScheduleEvent(EVENT_START_NEXT_PHASE, TIMER_START_NEXT_PHASE);
}

void boss_mimironAI::KilledUnit(Unit *victim)
{
    switch(CurrPhase)
    {
    case PHASE_LEV:
        DoScriptText(urand(0,1) ? SAY_LEV_KILL_PLAYER1 : SAY_LEV_KILL_PLAYER2, m_creature);
        break;

    case PHASE_VX:
        DoScriptText(urand(0,1) ? SAY_VX_KILLED_PLAYER1 : SAY_VX_KILLED_PLAYER2, m_creature);
        break;

    case PHASE_AERIAL:
        DoScriptText(urand(0,1) ? SAY_AERIAL_KILLED_PLAYER1 : SAY_AERIAL_KILLED_PLAYER2, m_creature);
        break;

    case PHASE_VOLTRON:
        DoScriptText(urand(0,1) ? SAY_V07_KILLED_PLAYER1 : SAY_V07_KILLED_PLAYER1, m_creature);
        break;
    }
}

void boss_mimironAI::RegisterVoltronPieceDied(uint32 npcId)
{
    Creature *AerialCommand = GET_CREATURE_FROM_GUID(AerialCommandGUID);
    Creature *Leviathan = GET_CREATURE_FROM_GUID(LeviathanGUID);
    Creature *VX001 = GET_CREATURE_FROM_GUID(VX001_GUID);

    if(AerialCommand && Leviathan && VX001)
    {
        VoltronPieceAI *acuAI = dynamic_cast<VoltronPieceAI*>(AerialCommand->AI());
        VoltronPieceAI *levAI = dynamic_cast<VoltronPieceAI*>(Leviathan->AI());
        VoltronPieceAI *vxAI  = dynamic_cast<VoltronPieceAI*>(VX001->AI());
        if (acuAI && levAI && vxAI && acuAI->IsDeadAsVoltron && levAI->IsDeadAsVoltron && vxAI->IsDeadAsVoltron)
        {
            RegisterPhaseFinished(PHASE_VOLTRON);
            return;
        }
    }
    switch (npcId)
    {
    case NPC_VX001:
        if(VX001)
            VX001->CastSpell(VX001, SPELL_SELF_REPAIR, true);
        break;
    case NPC_AERIAL_COMMAND_UNIT:
        if(AerialCommand)
            AerialCommand->CastSpell(AerialCommand, SPELL_SELF_REPAIR, true);
        break;
    case NPC_LEVIATHAN:
        if(Leviathan)
            Leviathan->CastSpell(Leviathan, SPELL_SELF_REPAIR, true);
        break;
    }
}

// spell hit for leviathan mark II AI,(again, to avoid nasty type casting errors)
void mob_leviathan_mk_II_AI::JustSummoned(Creature *summ)
{
    if(summ && summ->GetEntry() == NPC_PROXIMITY_MINE)
    {
        listMinesGUID.push_back(summ->GetGUID());
        if(mob_proximity_mineAI *mineAI = dynamic_cast<mob_proximity_mineAI*>(summ->AI()))
            mineAI->LeviathanGUID = m_creature->GetGUID();
    }
}

void mob_leviathan_mk_II_AI::SpellHit(Unit *caster, const SpellEntry *spell)
{
    int NumOfMines = urand(8,10);
    switch (spell->Id)
    {
    case SPELL_PROXIMITY_MINES:
        for(int i = 0 ; i < NumOfMines ; i++)
        {
            float x, y;
            GetRandomPointInPerimeter(15, x, y);
            DoSpawnCreature(NPC_PROXIMITY_MINE,x,y,0,0,TEMPSUMMON_CORPSE_DESPAWN,0);
        }
        break;
    case SPELL_SELF_REPAIR:
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        IsInUse = true;
        break;
    }
}

CreatureAI *GetAI_boss_mimiron(Creature *pCreature)
{
    return new boss_mimironAI(pCreature);
}

CreatureAI *GetAI_mob_leviathan_mk_II_AI(Creature *pCreature)
{
    return new mob_leviathan_mk_II_AI(pCreature);
}

CreatureAI *GetAI_mob_proximity_mineAI(Creature *pCreature)
{
    return new mob_proximity_mineAI(pCreature);
}

CreatureAI *GetAI_mob_VX001_AI(Creature *pCreature)
{
    return new mob_VX001_AI(pCreature);
}

CreatureAI *GetAI_mob_burst_targetAI(Creature *pCreature)
{
    return new mob_burst_targetAI(pCreature);
}

CreatureAI *GetAI_mob_aerial_command_unitAI(Creature *pCreature)
{
    return new mob_aerial_command_unitAI(pCreature);
}

CreatureAI *GetAI_mob_bomb_botAI(Creature *pCreature)
{
    return new mob_bomb_botAI(pCreature);
}

CreatureAI *GetAI_mob_assault_botAI(Creature *pCreature)
{
    return new mob_assault_botAI(pCreature);
}

void AddSC_boss_mimiron()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_mimiron";
    NewScript->GetAI = &GetAI_boss_mimiron;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_leviathan_mk_II_AI";
    NewScript->GetAI = &GetAI_mob_leviathan_mk_II_AI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_proximity_mineAI";
    NewScript->GetAI = &GetAI_mob_proximity_mineAI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_VX001_AI";
    NewScript->GetAI = &GetAI_mob_VX001_AI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_burst_targetAI";
    NewScript->GetAI = &GetAI_mob_burst_targetAI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_aerial_command_unitAI";
    NewScript->GetAI = &GetAI_mob_aerial_command_unitAI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_bomb_botAI";
    NewScript->GetAI = &GetAI_mob_bomb_botAI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_assault_botAI";
    NewScript->GetAI = &GetAI_mob_assault_botAI;
    NewScript->RegisterSelf();
}
