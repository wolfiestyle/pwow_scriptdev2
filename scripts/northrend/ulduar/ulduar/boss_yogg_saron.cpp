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
SDName: Boss Yogg-Saron
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_SARA_INTRO_1                            = -1603197,
    SAY_SARA_INTRO_2                            = -1603198,
    SAY_SARA_AGGRO                              = -1603199,
    SAY_SARA_HELP_1                             = -1603200,
    SAY_SARA_HELP_2                             = -1603201,
    SAY_SARA_SLAY_1                             = -1603202,
    SAY_SARA_SLAY_2                             = -1603203,
    SAY_WIPE_PHASE_1                            = -1603204,

    SAY_PHASE_2_INTRO                           = -1603205,
    SAY_SARA_PHASE_2_INTRO_A                    = -1603206,
    SAY_SARA_PHASE_2_INTRO_B                    = -1603207,

    SAY_MADNESS                                 = -1603209,
    SAY_PHASE_3                                 = -1603210,
    SAY_SLAY_1                                  = -1603211,
    SAY_SLAY_2                                  = -1603212,
    SAY_DEATH                                   = -1603213,
    SAY_TO_INSANE_1                             = -1603214,
    SAY_TO_INSANE_2                             = -1603215,

    SAY_LICH_KING_1                             = -1603216,
    SAY_CHAMPION_1                              = -1603217,
    SAY_CHAMPION_2                              = -1603218,
    SAY_LICH_KING_2                             = -1603219,
    SAY_YOGG_V3_1                               = -1603220,
    SAY_YOGG_V3_2                               = -1603221,

    SAY_NELTHARION_1                            = -1603222,
    SAY_YSERA                                   = -1603223,
    SAY_NELTHARION_2                            = -1603224,
    SAY_MALYGOS                                 = -1603225,
    SAY_YOGG_V2                                 = -1603226,

    SAY_GARONA_1                                = -1603227,
    SAY_GARONA_2                                = -1603228,
    SAY_YOGG_V1_1                               = -1603229,
    SAY_YOGG_V1_2                               = -1603230,
    SAY_GARONA_3                                = -1603231,
    SAY_GARONA_4                                = -1603232,
    SAY_YOGG_V1_3                               = -1603233,

    EMOTE_VISION_BLAST                          = -1603234,
    EMOTE_SHATTER_BLAST                         = -1603235,
};
*/

enum Spells
{
    // Yogg-Saron
    SPELL_EXTINGUISH_LIFE       = 64166,  // oneshots raid, berserk
    SPELL_CRUSHER_TENTACLE      = 64139,
    SPELL_CORRUPTOR_TENTACLE    = 64143,
    SPELL_CONSTRICTOR_TENTACLE  = 64133,
    SPELL_LUNATIC_GAZE          = 64163,
    SPELL_EMPOWERING_SHADOWS    = 64468,
    SPELL_EMPOWERING_SHADOWS_H  = 64486,
    SPELL_DEAFENING_ROAR        = 64189,  // only in 25 man mode
    // Tentacles
    SPELL_ERUPT                 = 64144,
    SPELL_FOCUSED_ANGER         = 57688,
    SPELL_DIMINISH_POWER        = 64145,
    SPELL_SQUEEZE               = 64125,  // doesnt seem to be the correct one
    SPELL_SQUEEZE_H             = 64126,
    SPELL_APATHY                = 64156,
    SPELL_BLACK_PLAGUE          = 64153,
    SPELL_CURSE_OF_DOOM         = 64157,
    SPELL_DRAINING_POISON       = 64152
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_SUMMON_CRUSHER,
    EVENT_SUMMON_CORRUPTOR,
    EVENT_SUMMON_CONSTRICTOR,
    EVENT_LUNATIC_GAZE,
    EVENT_EMPOWERING_SHADOWS,
    EVENT_DEAFENING_ROAR
};

enum AddIds
{
    NPC_CRUSHER_TENTACLE        = 33966,
    NPC_CORRUPTOR_TENTACLE      = 33985,
    NPC_CONSTRICTOR_TENTACLE    = 33983
};

enum Says
{
    SAY_PHASE1_1        = -1300161,
    SAY_PHASE1_2        = -1300162,
    SAY_PHASE1_3        = -1300163,
    SAY_KILLED_PLAYER1  = -1300164,
    SAY_KILLED_PLAYER2  = -1300165,
    SAY_WIPE_PHASE1     = -1300166,

    SAY_START_PHASE2_1  = -1300167,
    SAY_START_PHASE2_2  = -1300168,
    SAY_START_PHASE2_3  = -1300169,
    SAY_START_PHASE2_4  = -1300170,
    SAY_START_PHASE2_5  = -1300171,
    SAY_PHASE2_1        = -1300172,
    SAY_PHASE2_2        = -1300173,
    SAY_WHISP_INSANE1   = -1300174,
    SAY_WHISP_INSANE2   = -1300175,

    SAY_START_VISION    = -1300176,
    //VISIONS:
    //SAY_V1_YOGG1 = vision 1, yogg-saron first say (ordered chronologically)
    //vision 1 (lich king + immolated champion)
    SAY_V1_LICH1        = -1300177,
    SAY_V1_CHAMP1       = -1300178,
    SAY_V1_CHAMP2       = -1300179,
    SAY_V1_LICH2        = -1300180,
    SAY_V1_YOGG1        = -1300181,
    SAY_V1_YOGG2        = -1300182,

    //vision 2 (demon soul forging)
    SAY_V2_NELTH1       = -1300183,
    SAY_V2_YSERA1       = -1300184,
    SAY_V2_NELTH2       = -1300185,
    SAY_V2_MALY1        = -1300186,
    SAY_V2_YOGG1        = -1300187,

    //vision 3 (assassination of king llane)
    // (HALF = halforcen)
    SAY_V3_HALF1        = -1300188,
    SAY_V3_HALF2        = -1300189,
    SAY_V3_HALF3        = -1300190,
    SAY_V3_YOGG1        = -1300191,
    SAY_V3_YOGG2        = -1300192,
    SAY_V3_LLANE1       = -1300193,
    SAY_V3_HALF4        = -1300194,
    SAY_V3_YOGG3        = -1300195,

    SAY_START_PHASE3    = -1300196,
    SAY_LUNATIC_GAZE    = -1300197,
    SAY_DEAFENING_ROAR  = -1300198,
    SAY_DEATH           = -1300199
};

#define BERSERK_TIMER               15*MINUTE*IN_MILLISECONDS
#define SUMMON_CRUSHER_TIMER        urand(20, 35)*IN_MILLISECONDS
#define SUMMON_CORRUPTOR_TIMER      urand(15, 20)*IN_MILLISECONDS
#define SUMMON_CONSTRICTOR_TIMER    urand(10, 15)*IN_MILLISECONDS
#define LUNATIC_GAZE_TIMER          urand(30, 45)*IN_MILLISECONDS
#define EMPOWERING_SHADOWS_TIMER    urand(30, 40)*IN_MILLISECONDS
#define DEAFENING_ROAR_TIMER        urand(50, 60)*IN_MILLISECONDS

#define MAX_TENTACLES               5
#define SUMMON_RADIUS               38.0f

static const float RoomCenter[3] = {1981.5f, -28.5f, 325.0f};

static const uint32 CorruptorSpells[4] = {
    SPELL_APATHY,
    SPELL_BLACK_PLAGUE,
    SPELL_CURSE_OF_DOOM,
    SPELL_DRAINING_POISON
};

struct MANGOS_DLL_DECL boss_yoggsaronAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    typedef std::list<uint64> GuidList;
    GuidList m_SummonList;

    boss_yoggsaronAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_YOGGSARON)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        UnSummonTentacles();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        UnSummonTentacles();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon)
        {
            m_SummonList.push_back(pSummon->GetGUID());
            pSummon->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (pSummon)
            m_SummonList.remove(pSummon->GetGUID());
    }

    void SummonedCreatureDespawn(Creature *pSummon)
    {
        SummonedCreatureJustDied(pSummon);
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_CRUSHER, SUMMON_CRUSHER_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_CORRUPTOR, SUMMON_CORRUPTOR_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_CONSTRICTOR, SUMMON_CONSTRICTOR_TIMER);
        events.RescheduleEvent(EVENT_LUNATIC_GAZE, LUNATIC_GAZE_TIMER);
        events.RescheduleEvent(EVENT_EMPOWERING_SHADOWS, EMPOWERING_SHADOWS_TIMER);
        if (!m_bIsRegularMode)
            events.RescheduleEvent(EVENT_DEAFENING_ROAR, DEAFENING_ROAR_TIMER);
        DoScriptText(SAY_START_PHASE3, m_creature);  // just to make him say something
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void GetRandomPointInCircle(float max_rad, float &x, float &y, float &z)
    {
        float ang = 2*M_PI * rand_norm();
        float rad = max_rad * sqrt(rand_norm());
        x = rad * cos(ang) + RoomCenter[0];
        y = rad * sin(ang) + RoomCenter[1];
        z = RoomCenter[2];
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_EXTINGUISH_LIFE);
                    events.ScheduleEvent(EVENT_BERSERK, 10*IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_CRUSHER:
                    if (m_SummonList.size() < MAX_TENTACLES)
                    {
                        float x, y, z;
                        GetRandomPointInCircle(SUMMON_RADIUS, x, y, z);
                        m_creature->SummonCreature(NPC_CRUSHER_TENTACLE, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    }
                    events.ScheduleEvent(EVENT_SUMMON_CRUSHER, SUMMON_CRUSHER_TIMER);
                    break;
                case EVENT_SUMMON_CORRUPTOR:
                    if (m_SummonList.size() < MAX_TENTACLES)
                    {
                        float x, y, z;
                        GetRandomPointInCircle(SUMMON_RADIUS, x, y, z);
                        m_creature->SummonCreature(NPC_CORRUPTOR_TENTACLE, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    }
                    events.ScheduleEvent(EVENT_SUMMON_CORRUPTOR, SUMMON_CORRUPTOR_TIMER);
                    break;
                case EVENT_SUMMON_CONSTRICTOR:
                    if (m_SummonList.size() < MAX_TENTACLES)
                    {
                        float x, y, z;
                        GetRandomPointInCircle(SUMMON_RADIUS, x, y, z);
                        m_creature->SummonCreature(NPC_CONSTRICTOR_TENTACLE, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    }
                    events.ScheduleEvent(EVENT_SUMMON_CONSTRICTOR, SUMMON_CONSTRICTOR_TIMER);
                    break;
                case EVENT_LUNATIC_GAZE:
                    DoScriptText(SAY_LUNATIC_GAZE, m_creature);
                    DoCast(m_creature, SPELL_LUNATIC_GAZE);
                    events.ScheduleEvent(EVENT_LUNATIC_GAZE, LUNATIC_GAZE_TIMER);
                    break;
                case EVENT_EMPOWERING_SHADOWS:
                    DoCast(m_creature, HEROIC(SPELL_EMPOWERING_SHADOWS, SPELL_EMPOWERING_SHADOWS_H));
                    events.ScheduleEvent(EVENT_EMPOWERING_SHADOWS, EMPOWERING_SHADOWS_TIMER);
                    break;
                case EVENT_DEAFENING_ROAR:
                    DoScriptText(SAY_DEAFENING_ROAR, m_creature);
                    DoCast(m_creature, SPELL_DEAFENING_ROAR);
                    events.ScheduleEvent(EVENT_DEAFENING_ROAR, DEAFENING_ROAR_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void UnSummonTentacles()
    {
        for (GuidList::const_iterator i = m_SummonList.begin(); i != m_SummonList.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        m_SummonList.clear();
    }
};

// base class for all other tentacles
struct MANGOS_DLL_DECL npc_yogg_tentacleAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    npc_yogg_tentacleAI(Creature *pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }
};

struct MANGOS_DLL_DECL npc_crusher_tentacleAI: public npc_yogg_tentacleAI
{
    bool casting;

    npc_crusher_tentacleAI(Creature *pCreature):
        npc_yogg_tentacleAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        casting = false;
        DoCast(m_creature, SPELL_FOCUSED_ANGER, true);
        DoCast(m_creature, SPELL_ERUPT, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!casting)
        {
            DoCast(m_creature, SPELL_DIMINISH_POWER);
            casting = true;
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_corruptor_tentacleAI: public npc_yogg_tentacleAI
{
    uint32 m_uiCastTimer;

    npc_corruptor_tentacleAI(Creature *pCreature):
        npc_yogg_tentacleAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_uiCastTimer = 5*IN_MILLISECONDS;
        DoCast(m_creature, SPELL_ERUPT, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCastTimer < uiDiff)
        {
            if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target, CorruptorSpells[urand(0, 3)]);
            m_uiCastTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiCastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_constrictor_tentacleAI: public npc_yogg_tentacleAI
{
    Unit *target;

    npc_constrictor_tentacleAI(Creature *pCreature):
        npc_yogg_tentacleAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        target = NULL;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(target && uiDamage > m_creature->GetHealth())
            target->RemoveAurasDueToSpell(HEROIC(SPELL_SQUEEZE, SPELL_SQUEEZE_H));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!target)
        {
            target = m_creature->getVictim();
            m_creature->AddThreat(target, 5000000.0f);
            target->CastSpell(target, HEROIC(SPELL_SQUEEZE, SPELL_SQUEEZE_H), true);
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_yoggsaron(Creature* pCreature)
{
    return new boss_yoggsaronAI(pCreature);
}

CreatureAI* GetAI_npc_crusher_tentacle(Creature* pCreature)
{
    return new npc_crusher_tentacleAI(pCreature);
}

CreatureAI* GetAI_npc_corruptor_tentacle(Creature* pCreature)
{
    return new npc_corruptor_tentacleAI(pCreature);
}

CreatureAI* GetAI_npc_constrictor_tentacle(Creature* pCreature)
{
    return new npc_constrictor_tentacleAI(pCreature);
}

void AddSC_boss_yogg_saron()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_yoggsaron";
    NewScript->GetAI = &GetAI_boss_yoggsaron;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_crusher_tentacle";
    NewScript->GetAI = &GetAI_npc_crusher_tentacle;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_corruptor_tentacle";
    NewScript->GetAI = &GetAI_npc_corruptor_tentacle;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_constrictor_tentacle";
    NewScript->GetAI = &GetAI_npc_constrictor_tentacle;
    NewScript->RegisterSelf();
}
