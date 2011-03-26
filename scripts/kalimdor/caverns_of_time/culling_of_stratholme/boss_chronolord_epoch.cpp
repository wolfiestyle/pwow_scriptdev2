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
SDName: Boss Chronolord Epoch
SD%Complete: 0
SDComment: placeholder
SDCategory: Caverns of Time, Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

enum Says
{
    SAY_EPOCH_TIMEWARP_01   = ENTRY  - 148,
    SAY_EPOCH_TIMEWARP_02   = ENTRY  - 149,
    SAY_EPOCH_TIMEWARP_03   = ENTRY  - 150,
    SAY_EPOCH_SLAY_01       = ENTRY  - 151,
    SAY_EPOCH_SLAY_02       = ENTRY  - 152,
    SAY_EPOCH_SLAY_03       = ENTRY  - 153,
    SAY_EPOCH_DEATH         = ENTRY  - 154,
};

enum Spells
{
    SPELL_CURSE_OF_EXERTION = 52772,
    SPELL_TIME_STOP         = 58848,
    SPELL_TIME_WARP         = 52766,

    SPELL_WOUNDING_STRIKE_H = 58830,
    SPELL_WOUNDING_STRIKE   = 52771,
};

enum Events
{
    EVENT_CURSE_OF_EXERTION = 1,
    EVENT_TIME_STOP,
    EVENT_TIME_WARP,
    EVENT_WOUNDING_STRIKE,
};

#define TIMER_CURSE 10*IN_MILLISECONDS
#define TIMER_WARP  13*IN_MILLISECONDS
#define TIMER_WOUNDING_STRIKE  5*IN_MILLISECONDS
#define TIMER_TIME_STOP 20*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_chronolord_epochAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;
    bool m_bIsRegularMode;

    boss_chronolord_epochAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEvent(EVENT_CURSE_OF_EXERTION, TIMER_CURSE, TIMER_CURSE);
        Events.ScheduleEvent(EVENT_TIME_WARP, TIMER_WARP, TIMER_WARP);
        Events.ScheduleEvent(EVENT_WOUNDING_STRIKE, TIMER_WOUNDING_STRIKE, TIMER_WOUNDING_STRIKE);
        if (!m_bIsRegularMode)
            Events.ScheduleEvent(EVENT_TIME_STOP, TIMER_TIME_STOP, TIMER_TIME_STOP);
    }

    void Reset()
    {
        Events.Reset();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EPOCH_EVENT, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EPOCH_EVENT, FAIL);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
            case EVENT_CURSE_OF_EXERTION:
                m_creature->CastSpell(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0), SPELL_CURSE_OF_EXERTION, true);
                break;
            case EVENT_TIME_WARP: // AoE
                m_creature->CastSpell(m_creature, SPELL_TIME_WARP, true);
                break;
            case EVENT_WOUNDING_STRIKE:
                m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_WOUNDING_STRIKE : SPELL_WOUNDING_STRIKE_H, true);
                break;
            case EVENT_TIME_STOP: //AoE
                m_creature->CastSpell(m_creature, SPELL_TIME_STOP, true);
                break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_chronolord_epoch(Creature* pCreature)
{
    return new boss_chronolord_epochAI(pCreature);
}

void AddSC_boss_chronolord_epoch()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_chronolord_epoch";
    pNewScript->GetAI = &GetAI_boss_chronolord_epoch;
    pNewScript->RegisterSelf();
}
