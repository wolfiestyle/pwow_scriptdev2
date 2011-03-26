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
SDName: Boss Mal'ganis
SD%Complete: 0
SDComment: placeholder
SDCategory: Caverns of Time, Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

enum Says
{    
    SAY_MALGANIS_SLEEP_01   = ENTRY - 165,
    SAY_MALGANIS_SLEEP_02   = ENTRY - 166,
    SAY_MALGANIS_SLAY_01    = ENTRY - 167,
    SAY_MALGANIS_SLAY_02    = ENTRY - 168,
    SAY_MALGANIS_SLAY_03    = ENTRY - 169,
    SAY_MALGANIS_OUTRO      = ENTRY - 170,
};

enum Spells
{
    SPELL_CARRION_SWARM     = 52720,
    SPELL_CARRION_SWARM_H   = 58852,

    SPELL_MIND_BLAST        = 52722,
    SPELL_MIND_BLAST_H      = 58850,

    SPELL_SLEEP             = 52721,
    SPELL_SLEEP_H           = 58849,

    SPELL_VAMPIRIC_TOUCH    = 52723,
};

enum Events
{
    EVENT_CARRION_SWARM = 1,
    EVENT_MIND_BLAST,
    EVENT_SLEEP,
    EVENT_TOUCH,
};

#define TIMER_SWARM 21*IN_MILLISECONDS, 26*IN_MILLISECONDS
#define TIMER_SLEEP 18*IN_MILLISECONDS, 24*IN_MILLISECONDS
#define TIMER_BLAST 13*IN_MILLISECONDS, 17*IN_MILLISECONDS
#define TIMER_TOUCH 30*IN_MILLISECONDS, 45*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_malganisAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;
    bool m_bIsRegularMode;
    bool m_bEncounterDone;
    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;

    boss_malganisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        m_uiTalkPhase = 0;
        m_uiTalkTimer = 0;
        m_bEncounterDone = false;
        Events.Reset();
    }
/*
    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALGANIS_EVENT, DONE);
    }
*/
    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,2))
        {
            case 0:
                DoScriptText(SAY_MALGANIS_SLAY_01, m_creature);
                break;
            case 1:
                DoScriptText(SAY_MALGANIS_SLAY_01, m_creature);
                break;
            case 2:
                DoScriptText(SAY_MALGANIS_SLAY_01, m_creature);
                break;
        }
    }
    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_CARRION_SWARM, 4*IN_MILLISECONDS, 8*IN_MILLISECONDS, TIMER_SWARM);
        Events.ScheduleEventInRange(EVENT_MIND_BLAST,TIMER_SWARM, TIMER_BLAST);
        Events.ScheduleEventInRange(EVENT_SLEEP, 7*IN_MILLISECONDS, 12*IN_MILLISECONDS, TIMER_SLEEP);
        Events.ScheduleEventInRange(EVENT_TOUCH, 14*IN_MILLISECONDS, 25*IN_MILLISECONDS, TIMER_TOUCH);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALGANIS_EVENT, FAIL);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bEncounterDone && m_uiTalkPhase)
            UpdateOutro(uiDiff);

        if (m_bEncounterDone) // dont do further updates on AI if encounter is considered complete
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CARRION_SWARM:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CARRION_SWARM : SPELL_CARRION_SWARM_H, true);
                    break;
                case EVENT_MIND_BLAST:
                    if (Unit* pTar = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        m_creature->CastSpell(pTar, m_bIsRegularMode ? SPELL_MIND_BLAST : SPELL_MIND_BLAST_H, false);
                    }
                    break;
                case EVENT_SLEEP:
                    if (Unit* pTar = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        m_creature->CastSpell(pTar, m_bIsRegularMode ? SPELL_SLEEP : SPELL_SLEEP_H, false);
                        DoScriptText(urand(0,1) ? SAY_MALGANIS_SLEEP_01 : SAY_MALGANIS_SLEEP_02, m_creature);
                    }
                    break;
                case EVENT_TOUCH:
                    m_creature->CastSpell(m_creature, SPELL_VAMPIRIC_TOUCH, true);
                    break;
            }
        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_bEncounterDone)
        {
            if (m_creature->GetHealth() < uiDamage)
            {
                m_bEncounterDone = true;
                SetCombatMovement(false);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveAllAttackers();
                m_creature->RemoveAllAuras();
                m_creature->ClearInCombat();
                m_creature->getThreatManager().clearReferences();
                m_uiTalkTimer = 0*IN_MILLISECONDS;
                uiDamage = 0;
                m_creature->SetHealth(1);
                m_uiTalkPhase = 1;
            }
        }
    }

    void UpdateOutro(uint32 const uiDiff)
    {
        if (m_uiTalkTimer <= uiDiff)
        {
            switch(m_uiTalkPhase)
            {
                case 1:
                    m_creature->MonsterMoveWithSpeed(2291.93f, 1509.89f, 130.92f, 3*IN_MILLISECONDS);
                    m_uiTalkTimer = 4*IN_MILLISECONDS;
                    break;
                case 2:
                    if (m_pInstance)
                        if (Creature* pArthas = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_ARTHAS)))
                            m_creature->SetFacingToObject(pArthas);
                    
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 3:
                    DoScriptText(SAY_MALGANIS_OUTRO, m_creature);
                    m_uiTalkTimer = 18*IN_MILLISECONDS;
                    break;
                case 4:
                    if (m_pInstance)
                        if (Creature* pArthas = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_ARTHAS)))
                            SendScriptMessageTo(pArthas, m_creature, 2, 0);
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 5:
                    DespawnCreature(m_creature);
                    break;
                default:
                    break;
            }
            if ((m_uiTalkPhase+1) <= 5)
                m_uiTalkPhase++;
        }
        else
            m_uiTalkTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_malganis(Creature* pCreature)
{
    return new boss_malganisAI(pCreature);
}


void AddSC_boss_malganis()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_malganis";
    pNewScript->GetAI = &GetAI_boss_malganis;
    pNewScript->RegisterSelf();
}
