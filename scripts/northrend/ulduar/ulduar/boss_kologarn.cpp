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
SDName: Boss Kologarn
SD%Complete: 50%
SDComment: There is a bunch of spells and summons related to Focused Eyebeam, its not just a single spell event
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1603126,
    SAY_SHOCKWAVE                       = -1603127,
    SAY_GRAB                            = -1603128,
    SAY_ARM_LOST_LEFT                   = -1603129,
    SAY_ARM_LOST_RIGHT                  = -1603130,
    SAY_SLAY_1                          = -1603131,
    SAY_SLAY_2                          = -1603132,
    SAY_BERSERK                         = -1603133,
    SAY_DEATH                           = -1603134,

    EMOTE_ARM_RIGHT                     = -1603135,
    EMOTE_ARM_LEFT                      = -1603136,
    EMOTE_STONE_GRIP                    = -1603137,
};
*/

enum Spells
{
    // Kologarn
    SPELL_FOCUSED_EYEBEAM   = 63347,  // not the correct ones
    SPELL_FOCUSED_EYEBEAM_H = 63977,
    SPELL_TWO_ARM_SMASH     = 63356,
    SPELL_TWO_ARM_SMASH_H   = 64003,
    SPELL_ONE_ARM_SMASH     = 63573,
    SPELL_ONE_ARM_SMASH_H   = 64006,
    SPELL_STONE_SHOUT       = 63716,
    SPELL_STONE_SHOUT_H     = 64005,
    SPELL_PETRIFY_BREATH    = 62030,
    SPELL_PETRIFY_BREATH_H  = 63980,

    // Arms
    SPELL_STONE_GRIP        = 62166,  // spell requires vehicle support to work properly
    SPELL_STONE_GRIP_H      = 63981,
    SPELL_STONE_GRIP_DOT    = 64290,  // used to dispel effect
    SPELL_STONE_GRIP_DOT_H  = 64292,
    SPELL_ARM_SWEEP         = 63766,
    SPELL_ARM_SWEEP_H       = 63983,
    SPELL_ARM_DEAD_DAMAGE   = 63629,
    SPELL_ARM_DEAD_DAMAGE_H = 63979
};

enum Events
{
    EVENT_EYEBEAM = 1,
    EVENT_SMASH,
    EVENT_BREATH,
    EVENT_GRIP,
    EVENT_SWEEP
};

enum Says
{
    SAY_AGGRO           = -1300071,
    SAY_SHOCKWAVE       = -1300072,
    SAY_SQUEEZE         = -1300073,
    SAY_LEFT_ARM_DEAD   = -1300074,
    SAY_RIGHT_ARM_DEAD  = -1300075,
    SAY_KILLED_PLAYER1  = -1300076,
    SAY_KILLED_PLAYER2  = -1300077,
    SAY_BERSERK         = -1300078,
    SAY_DEATH           = -1300079
};

#define KOLOGARN_ATTACK_DISTANCE    15.0f

struct MANGOS_DLL_DECL boss_kologarnAI: public Scripted_NoMovementAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;

    boss_kologarnAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_uiBossEncounterId(TYPE_KOLOGARN)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);  // to disable rotation (hacky)
        if (Creature *LeftArm = GET_CREATURE(DATA_LEFT_ARM))
            if (LeftArm->isDead())
                LeftArm->Respawn();
        if (Creature *RightArm = GET_CREATURE(DATA_RIGHT_ARM))
            if (RightArm->isDead())
                RightArm->Respawn();
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature *LeftArm = GET_CREATURE(DATA_LEFT_ARM))
            if (LeftArm->isAlive())
            {
                LeftArm->SetHealth(0);
                LeftArm->SetDeathState(JUST_DIED);
            }
        if (Creature *RightArm = GET_CREATURE(DATA_RIGHT_ARM))
            if (RightArm->isAlive())
            {
                RightArm->SetHealth(0);
                RightArm->SetDeathState(JUST_DIED);
            }
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        //events.RescheduleEvent(EVENT_EYEBEAM, 15000);  // need to find correct way of implementing this
        events.RescheduleEvent(EVENT_SMASH, 5000);
        events.RescheduleEvent(EVENT_BREATH, 10000);
        if (Creature *LeftArm = GET_CREATURE(DATA_LEFT_ARM))
            if (LeftArm->isAlive())
                LeftArm->AddThreat(pWho);
        if (Creature *RightArm = GET_CREATURE(DATA_RIGHT_ARM))
            if (RightArm->isAlive())
                RightArm->AddThreat(pWho);
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

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        Creature *LeftArm = GET_CREATURE(DATA_LEFT_ARM);
        Creature *RightArm = GET_CREATURE(DATA_RIGHT_ARM);

        bool left = LeftArm && LeftArm->isAlive();
        bool right = RightArm && RightArm->isAlive();

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                /*case EVENT_EYEBEAM:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, HEROIC(SPELL_FOCUSED_EYEBEAM, SPELL_FOCUSED_EYEBEAM_H), true);
                    events.ScheduleEvent(EVENT_EYEBEAM, 15000);
                    m_uiGcd = 1500;
                    break;*/
                case EVENT_SMASH:
                    if (left && right)
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_TWO_ARM_SMASH, SPELL_TWO_ARM_SMASH_H), true);
                    else if (left || right)
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_ONE_ARM_SMASH, SPELL_ONE_ARM_SMASH_H), true);
                    else
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_STONE_SHOUT, SPELL_STONE_SHOUT_H), true);
                    events.ScheduleEvent(EVENT_SMASH, 15000);
                    break;
                case EVENT_BREATH:
                {
                    bool player_in_melee_range = false;
                    ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
                    for (ThreatList::const_iterator hr = tlist.begin(); hr != tlist.end(); hr++)
                    {
                        Unit *hostil = m_creature->GetMap()->GetUnit((*hr)->getUnitGuid());
                        if (hostil && hostil->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(hostil, KOLOGARN_ATTACK_DISTANCE))
                        {
                            player_in_melee_range = true;
                            break;
                        }
                    }
                    if (!player_in_melee_range)
                        DoCast(m_creature->getVictim(), HEROIC(SPELL_PETRIFY_BREATH, SPELL_PETRIFY_BREATH_H), true);
                    events.ScheduleEvent(EVENT_BREATH, 10000);
                    break;
                }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_kologarn_armAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    uint32 m_uiUngripDamage;

    boss_kologarn_armAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        m_uiUngripDamage = 0;
        events.Reset();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);  // to disable rotation (hacky)
    }

    void JustDied(Unit* pKiller)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_LEFT_ARM:
                Say(SAY_LEFT_ARM_DEAD);
                break;
            case NPC_RIGHT_ARM:
                Say(SAY_RIGHT_ARM_DEAD);
                break;
            default:
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_LEFT_ARM:
                events.RescheduleEvent(EVENT_SWEEP, 20000);
                break;
            case NPC_RIGHT_ARM:
                events.RescheduleEvent(EVENT_GRIP, 15000);
                break;
            default:
                break;
        }
    }

    void RemoveStoneGrip()
    {
        ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator hr = tlist.begin(); hr != tlist.end(); hr++)
        {
            Unit *hostil = m_creature->GetMap()->GetUnit((*hr)->getUnitGuid());
            if (hostil && hostil->isAlive())
                hostil->RemoveAurasDueToSpell(HEROIC(SPELL_STONE_GRIP_DOT, SPELL_STONE_GRIP_DOT_H));
        }
        events.ScheduleEvent(EVENT_GRIP, 15000);
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage >= m_creature->GetHealth())
        {
            if (Creature *Kologarn = GET_CREATURE(TYPE_KOLOGARN))
                DoCast(Kologarn, HEROIC(SPELL_ARM_DEAD_DAMAGE, SPELL_ARM_DEAD_DAMAGE_H), true);
            if (m_creature->GetEntry() == NPC_RIGHT_ARM)
                RemoveStoneGrip();
            return;
        }
        if (m_uiUngripDamage > 0)
        {
            if (damage >= m_uiUngripDamage)
            {
                RemoveStoneGrip();
                m_uiUngripDamage = 0;
            }
            else
                m_uiUngripDamage -= damage;
        }
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        Say(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SWEEP:
                    Say(SAY_SHOCKWAVE);
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_ARM_SWEEP, SPELL_ARM_SWEEP_H), true);
                    events.ScheduleEvent(EVENT_SWEEP, 20000);
                    break;
                case EVENT_GRIP:
                    Say(SAY_SQUEEZE);
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_STONE_GRIP, SPELL_STONE_GRIP_H), true);
                    m_uiUngripDamage = HEROIC(100000, 480000);
                    //events.ScheduleEvent(EVENT_GRIP, 15000);
                    break;
                default:
                    break;
            }

        //DoMeleeAttackIfReady();
    }

    // make arms say from body
    void Say(int32 text_id)
    {
        if (Creature *Kologarn = GET_CREATURE(TYPE_KOLOGARN))
            DoScriptText(text_id, Kologarn);
    }
};

CreatureAI* GetAI_boss_kologarn(Creature* pCreature)
{
    return new boss_kologarnAI(pCreature);
}

CreatureAI* GetAI_boss_kologarn_arm(Creature* pCreature)
{
    return new boss_kologarn_armAI(pCreature);
}

void AddSC_boss_kologarn()
{
    Script *NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_kologarn";
    NewScript->GetAI = &GetAI_boss_kologarn;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_kologarn_arm";
    NewScript->GetAI = &GetAI_boss_kologarn_arm;
    NewScript->RegisterSelf();
}
