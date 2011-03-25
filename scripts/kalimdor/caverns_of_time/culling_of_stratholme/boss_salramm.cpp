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
SDName: Boss Salramm the Fleshcrafter
SD%Complete: 0
SDComment: placeholder
SDCategory: Caverns of Time, Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

enum Says
{
    SAY_SALRAMM_SPAWN               = ENTRY - 110,
    SAY_SALRAMM_AGGRO               = ENTRY - 111,
    SAY_SALRAMM_SLAY_01             = ENTRY - 112,
    SAY_SALRAMM_SLAY_02             = ENTRY - 113,
    SAY_SALRAMM_SLAY_03             = ENTRY - 114,
    SAY_SALRAMM_SUMMON_GHOULS_01    = ENTRY - 115,
    SAY_SALRAMM_SUMMON_GHOULS_02    = ENTRY - 116,
    SAY_SALRAMM_GHOUL_EXPLODE_01    = ENTRY - 117,
    SAY_SALRAMM_GHOUL_EXPLODE_02    = ENTRY - 118,
    SAY_SALRAMM_FLESH_01            = ENTRY - 119,
    SAY_SALRAMM_FLESH_02            = ENTRY - 120,
    SAY_SALRAMM_FLESH_03            = ENTRY - 121,
    SAY_SALRAMM_DEATH               = ENTRY - 122,
};

enum Spells
{
    SPELL_SUMMON_GHOULS     = 52451,
    
    SPELL_GHOUL_EXPLODE     = 52480,
    SPELL_GHOUL_EXPLODE_H   = 58825,

    SPELL_SHADOW_BOLT       = 57725,
    SPELL_SHADOW_BOLT_H     = 58827,

    SPELL_CURSE_OF_FLESH    = 58845,

    SPELL_STEAL_FLESH       = 52708,
};

enum Events
{
    EVENT_SUMMON_GHOULS = 1,
    EVENT_GHOUL_EXPLODE,
    EVENT_SHADOW_BOLT,
    EVENT_CURSE_OF_FLESH,
    EVENT_STEAL_FLESH,
};

#define CURSE_TIMER 12*IN_MILLISECONDS
#define GHOUL_TIMER 20*IN_MILLISECONDS
#define EXPLODE_TIMER 5*IN_MILLISECONDS
#define SHADOW_BOLT_TIMER 7*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_salrammAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;
    SummonManager SummonMgr;

    bool m_bIsRegularMode;

    boss_salrammAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_creature->MonsterYellToZone(SAY_SALRAMM_SPAWN, 0, NULL);
        Reset();
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWHo)
    {
        Events.ScheduleEvent(EVENT_CURSE_OF_FLESH, CURSE_TIMER, CURSE_TIMER);
        Events.ScheduleEvent(EVENT_SUMMON_GHOULS, GHOUL_TIMER, GHOUL_TIMER);
        Events.ScheduleEvent(EVENT_SHADOW_BOLT, SHADOW_BOLT_TIMER, SHADOW_BOLT_TIMER);
        Events.ScheduleEventInRange(EVENT_STEAL_FLESH, 10*IN_MILLISECONDS, 20*IN_MILLISECONDS, 10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        DoScriptText(SAY_SALRAMM_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_SALRAMM_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SALRAMM_EVENT, DONE);
    }

    void JustSummoned(Creature* pSumm)
    {
        if (pSumm)
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
    }
    void SummonedCreatureJustDied(Creature* pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SALRAMM_EVENT, FAIL);
    }

    void KilledUnit(Unit* pKilled)
    {
        if (pKilled->GetTypeId() == TYPEID_PLAYER)
        {
            switch(urand(0, 2))
            {
                case 0:
                    DoScriptText(SAY_SALRAMM_SLAY_01, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_SALRAMM_SLAY_02, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_SALRAMM_SLAY_03, m_creature);
                    break;
            }
        }
    }
    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SUMMON_GHOULS:
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_GHOULS, true);
                    DoScriptText(urand(0, 1)? SAY_SALRAMM_SUMMON_GHOULS_01 : SAY_SALRAMM_SUMMON_GHOULS_02, m_creature);
                    Events.ScheduleEvent(EVENT_GHOUL_EXPLODE, EXPLODE_TIMER);
                    break;
                case EVENT_GHOUL_EXPLODE:
                    DoScriptText( urand(0,1)? SAY_SALRAMM_GHOUL_EXPLODE_01 : SAY_SALRAMM_GHOUL_EXPLODE_02, m_creature);
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_GHOUL_EXPLODE : SPELL_GHOUL_EXPLODE_H , true);
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_GHOUL_EXPLODE : SPELL_GHOUL_EXPLODE_H , true);
                    break;
                case EVENT_SHADOW_BOLT:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H, false);
                    break;
                case EVENT_CURSE_OF_FLESH:
                    m_creature->CastSpell(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0), SPELL_CURSE_OF_FLESH, false);
                    break;
                case EVENT_STEAL_FLESH:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_STEAL_FLESH, false);
                    switch(urand(0, 2))
                    {
                        case 0:
                            DoScriptText(SAY_SALRAMM_SLAY_01, m_creature);
                            break;
                        case 1:
                            DoScriptText(SAY_SALRAMM_SLAY_02, m_creature);
                            break;
                        case 2:
                            DoScriptText(SAY_SALRAMM_SLAY_03, m_creature);
                            break;
                    }
                    break;
            }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_salramm(Creature* pCreature)
{
    return new boss_salrammAI(pCreature);
}

void AddSC_boss_salramm()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_salramm";
    pNewScript->GetAI = &GetAI_boss_salramm;
    pNewScript->RegisterSelf();
}
