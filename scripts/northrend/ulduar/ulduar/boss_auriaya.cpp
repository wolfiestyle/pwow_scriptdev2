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
SDName: Boss Auriaya
SD%Complete: 50%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1603079,
    SAY_SLAY_1                          = -1603080,
    SAY_SLAY_2                          = -1603081,
    SAY_BERSERK                         = -1603082,
    SAY_DEATH                           = -1603083,
    EMOTE_SCREECH                       = -1603084,
    EMOTE_DEFENDER                      = -1603085,
};
*/

enum Spells
{
    // Auriaya
    SPELL_BERSERK           = 47008,
    SPELL_SONIC_SCREECH     = 64422,
    SPELL_SONIC_SCREECH_H   = 64688,
    SPELL_SENTINEL_BLAST    = 64389,
    SPELL_SENTINEL_BLAST_H  = 64678,
    SPELL_TERRIFY_SCREECH   = 64386,
    SPELL_GUARDIAN_SWARM    = 64396,
    SPELL_SUMMON_DEFENDER   = 64447,

    // Sanctum Sentry
    SPELL_SAVAGE_POUNCE     = 64666,
    SPELL_SAVAGE_POUNCE_H   = 64374,
    SPELL_STR_OF_THE_PACK   = 64369,

    // Feral Defender (summoned by spell 64449)
    SPELL_FERAL_ESSENCE     = 64455,
    SPELL_FERAL_RUSH        = 64496,
    SPELL_FERAL_RUSH_H      = 64674,
    SPELL_FERAL_POUNCE      = 64478,
    SPELL_FERAL_POUNCE_H    = 64669,
    SPELL_SEEPING_ESSENCE   = 64457,
    // Seeping Essence Stalker (summoned by spell 64457)
    SPELL_SEEPING_DAMAGE    = 64458,
    SPELL_SEEPING_DAMAGE_H  = 64676
};

enum Events
{
    // Auriaya
    EVENT_BERSERK = 1,
    EVENT_SONIC_SCREECH,
    EVENT_SENTINEL_BLAST,
    EVENT_TERRIFY_SCREECH,
    EVENT_GUARDIAN_SWARM,
    EVENT_SUMMON_DEFENDER,
    // Sanctum Sentry
    EVENT_SAVAGE_POUNCE,
    // Feral Defender
    EVENT_FERAL_RUSH,
    EVENT_FERAL_POUNCE
};

enum Adds
{
    NPC_FERAL_DEFENDER      = 34035,
    NPC_SWARMING_GUARDIAN   = 34034
};

enum Says
{
    SAY_AGGRO           = -1300080,
    SAY_KILLED_PLAYER1  = -1300081,
    SAY_KILLED_PLAYER2  = -1300082,
    SAY_BERSERK         = -1300083,
    SAY_DEATH           = -1300084
};

#define BERSERK_TIMER           10*MINUTE*IN_MILLISECONDS
#define SONIC_SCREECH_TIMER     urand(30, 35)*IN_MILLISECONDS
#define SENTINEL_BLAST_TIMER    urand(15, 20)*IN_MILLISECONDS
#define TERRIFY_SCREECH_TIMER   urand(30, 40)*IN_MILLISECONDS
#define GUARDIAN_SWARM_TIMER    urand(25, 30)*IN_MILLISECONDS
#define SUMMON_DEFENDER_TIMER   60*IN_MILLISECONDS
#define SAVAGE_POUNCE_TIMER     urand(15, 20)*IN_MILLISECONDS
#define FERAL_RUSH_TIMER        urand(20, 25)*IN_MILLISECONDS
#define FERAL_POUNCE_TIMER      urand(15, 20)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_auriayaAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    boss_auriayaAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_AURIAYA)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        UnSummonAdds();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        UnSummonAdds();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        events.RescheduleEvent(EVENT_SONIC_SCREECH, SONIC_SCREECH_TIMER);
        events.RescheduleEvent(EVENT_SENTINEL_BLAST, SENTINEL_BLAST_TIMER);
        events.RescheduleEvent(EVENT_TERRIFY_SCREECH, TERRIFY_SCREECH_TIMER);
        events.RescheduleEvent(EVENT_GUARDIAN_SWARM, GUARDIAN_SWARM_TIMER);
        events.RescheduleEvent(EVENT_SUMMON_DEFENDER, SUMMON_DEFENDER_TIMER);
        for (uint32 i = 0; i < HEROIC(2, 4); i++)
            if (Creature *Sentry = GET_CREATURE(DATA_SANCTUM_SENTRY+i))
                if (Sentry->isAlive())
                    Sentry->AddThreat(pWho);
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon)
            pSummon->SetInCombatWithZone();
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
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_SONIC_SCREECH:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_SONIC_SCREECH, SPELL_SONIC_SCREECH_H));
                    events.ScheduleEvent(EVENT_SONIC_SCREECH, SONIC_SCREECH_TIMER);
                    break;
                case EVENT_SENTINEL_BLAST:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_SENTINEL_BLAST, SPELL_SENTINEL_BLAST_H));
                    events.ScheduleEvent(EVENT_SENTINEL_BLAST, SENTINEL_BLAST_TIMER);
                    break;
                case EVENT_TERRIFY_SCREECH:
                    DoCast(m_creature->getVictim(), SPELL_TERRIFY_SCREECH);
                    events.ScheduleEvent(EVENT_TERRIFY_SCREECH, TERRIFY_SCREECH_TIMER);
                    break;
                case EVENT_GUARDIAN_SWARM:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        UnSummonGuardianSwarm();
                        DoCast(target, SPELL_GUARDIAN_SWARM);
                    }
                    events.ScheduleEvent(EVENT_GUARDIAN_SWARM, GUARDIAN_SWARM_TIMER);
                    break;
                case EVENT_SUMMON_DEFENDER:
                    DoCast(m_creature, SPELL_SUMMON_DEFENDER, true);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void UnSummonGuardianSwarm()
    {
        std::list<Creature*> SwarmingGuardian;
        GetCreatureListWithEntryInGrid(SwarmingGuardian, m_creature, NPC_SWARMING_GUARDIAN, 100.0f);
        for (std::list<Creature*>::const_iterator i = SwarmingGuardian.begin(); i != SwarmingGuardian.end(); ++i)
            (*i)->ForcedDespawn();
    }

    void UnSummonAdds()
    {
        if (Creature *FeralDefender = GetClosestCreatureWithEntry(m_creature, NPC_FERAL_DEFENDER, 100.0f))
            FeralDefender->ForcedDespawn();

        UnSummonGuardianSwarm();
    }
};

struct MANGOS_DLL_DECL boss_auriaya_sentryAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    //float m_fFollowAngle;

    boss_auriaya_sentryAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        //m_fFollowAngle = M_PI/2.0f + M_PI*rand_norm();
        events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_SAVAGE_POUNCE, SAVAGE_POUNCE_TIMER);
        DoCast(m_creature, SPELL_STR_OF_THE_PACK, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            /*
            if (Creature *Auriaya = GET_CREATURE(TYPE_AURIAYA))
                if (m_creature->isAlive() && Auriaya->isAlive())
                    m_creature->GetMotionMaster()->MoveFollow(Auriaya, 2.0f, m_fFollowAngle);
            */
            return;
        }

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SAVAGE_POUNCE:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_SAVAGE_POUNCE, SPELL_SAVAGE_POUNCE_H));
                    events.ScheduleEvent(EVENT_SAVAGE_POUNCE, SAVAGE_POUNCE_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_auriaya_defenderAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    boss_auriaya_defenderAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        events.RescheduleEvent(EVENT_FERAL_RUSH, FERAL_RUSH_TIMER);
        events.RescheduleEvent(EVENT_FERAL_POUNCE, FERAL_POUNCE_TIMER);
        DoCast(m_creature, SPELL_FERAL_ESSENCE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_FERAL_RUSH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, HEROIC(SPELL_FERAL_RUSH, SPELL_FERAL_RUSH_H));
                    events.ScheduleEvent(EVENT_FERAL_RUSH, FERAL_RUSH_TIMER);
                    break;
                case EVENT_FERAL_POUNCE:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_FERAL_POUNCE, SPELL_FERAL_POUNCE_H));
                    events.ScheduleEvent(EVENT_FERAL_POUNCE, FERAL_POUNCE_TIMER);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriayaAI(pCreature);
}

CreatureAI* GetAI_boss_auriaya_sentry(Creature* pCreature)
{
    return new boss_auriaya_sentryAI(pCreature);
}

CreatureAI* GetAI_boss_auriaya_defender(Creature* pCreature)
{
    return new boss_auriaya_defenderAI(pCreature);
}

void AddSC_boss_auriaya()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_auriaya";
    NewScript->GetAI = &GetAI_boss_auriaya;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_auriaya_sentry";
    NewScript->GetAI = &GetAI_boss_auriaya_sentry;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_auriaya_defender";
    NewScript->GetAI = &GetAI_boss_auriaya_defender;
    NewScript->RegisterSelf();
}
