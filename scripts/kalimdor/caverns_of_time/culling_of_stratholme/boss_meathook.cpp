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
SDName: Boss Meathook
SD%Complete: 0
SDComment: placeholder
SDCategory: Caverns of Time, Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

enum Says
{
    SAY_MEATHOOK_SPAWN      = ENTRY - 100,
    SAY_MEATHOOK_AGGRO      = ENTRY - 101,
    SAY_MEATHOOK_SLAY_01    = ENTRY - 102,
    SAY_MEATHOOK_SLAY_02    = ENTRY - 104,
    SAY_MEATHOOK_SLAY_03    = ENTRY - 105,
    SAY_MEATHOOK_DEATH      = ENTRY - 106,
};

enum Spells
{
    SPELL_CONSTRICTING_CHAINS   = 52696,
    SPELL_CONSTRICTING_CHAINS_H = 58823,

    SPELL_DISEASE_EXPULSION     = 52666,
    SPELL_DISEASE_EXPULSION_H   = 58824,
    
    SPELL_FRENZY                = 58841,
};

enum Events
{
    EVENT_CHAINS = 1,
    EVENT_FRENZY,
    EVENT_DISEASE,
};

#define TIMER_CHAINS    15*IN_MILLISECONDS
#define TIMER_FRENZY    20*IN_MILLISECONDS, 35*IN_MILLISECONDS
#define TIMER_DISEASE   2*IN_MILLISECONDS, 3*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_meathookAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;

    bool m_bIsRegularMode;

    boss_meathookAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->MonsterYellToZone(SAY_MEATHOOK_SPAWN, 0, NULL);
        Reset();
    }


    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_MEATHOOK_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_CHAINS, TIMER_CHAINS, TIMER_CHAINS);
        Events.ScheduleEventInRange(EVENT_FRENZY, TIMER_FRENZY, TIMER_FRENZY);
        Events.ScheduleEventInRange(EVENT_DISEASE, TIMER_DISEASE, TIMER_DISEASE);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            switch(urand(0,2))
            {
            case 0:
                DoScriptText(SAY_MEATHOOK_SLAY_01, m_creature);    
                break;
            case 1:
                DoScriptText(SAY_MEATHOOK_SLAY_02, m_creature);    
                break;
            case 2:
                DoScriptText(SAY_MEATHOOK_SLAY_03, m_creature);    
                break;
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_MEATHOOK_DEATH, m_creature);    

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEATHOOK_EVENT, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MEATHOOK_EVENT, FAIL);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CHAINS:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(m_bIsRegularMode ? ATTACKING_TARGET_RANDOM : ATTACKING_TARGET_BOTTOMAGGRO,m_bIsRegularMode ? 1 : 0))
                        if (pTarget != m_creature->getVictim())
                            m_creature->CastSpell(pTarget, m_bIsRegularMode ? SPELL_CONSTRICTING_CHAINS : SPELL_CONSTRICTING_CHAINS_H, false);
                    break;
                case EVENT_FRENZY:
                    m_creature->MonsterTextEmote("Meathook goes into a frenzy!", NULL);
                    DoCast(m_creature, SPELL_FRENZY, true);
                    break;
                case EVENT_DISEASE:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_DISEASE_EXPULSION : SPELL_DISEASE_EXPULSION_H, false);
                    break;
            }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_meathook(Creature* pCreature)
{
    return new boss_meathookAI(pCreature);
}

void AddSC_boss_meathook()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_meathook";
    pNewScript->GetAI = &GetAI_boss_meathook;
    pNewScript->RegisterSelf();
}
