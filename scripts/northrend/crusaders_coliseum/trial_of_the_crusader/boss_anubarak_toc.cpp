/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss Anub'arak
SD%Complete: 0
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_anubarak
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

//evidently, azjol-nerub was merely a set-back

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_FREEZING_SLASH        = 66012,
    SPELL_PENETRATING_COLD_N10  = 66013,
    SPELL_PENETRATING_COLD_N25  = 67700,
    SPELL_PENETRATING_COLD_H10  = 68509,
    SPELL_PENETRATING_COLD_H25  = 68510,
    SPELL_LEECHING_SWARM_N10    = 66118,
    SPELL_LEECHING_SWARM_N25    = 67630,
    SPELL_LEECHING_SWARM_H10    = 68646,
    SPELL_LEECHING_SWARM_H25    = 68647,
    SPELL_PURSUING_SPIKES       = 65922,  // only part of the effect, probably used by dummy creature
};

enum AddIds
{
    NPC_FROST_SPHERE            = 34606,
    NPC_NERUBIAN_BURROWER       = 34607,
    NPC_SWARM_SCARAB            = 34605,
};

enum Says
{
    SAY_AGGRO                   = -1300355,
    SAY_SUBMERGE                = -1300356,
    SAY_LEECHING_SWARM          = -1300357,
    SAY_KILLED_PLAYER1          = -1300358,
    SAY_KILLED_PLAYER2          = -1300359,
    SAY_DEATH                   = -1300360,
    SAY_BERSERK                 = -1300361,
};

enum Events
{
    EVENT_BERSERK = 1,
};

#define TIMER_BERSERK           10*MINUTE*IN_MILISECONDS

struct MANGOS_DLL_DECL boss_toc_anubarakAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_anubarakAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        Events.RescheduleEvent(EVENT_BERSERK, TIMER_BERSERK);
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

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        //only non-summoned boss, so to streamline things its easier to do this
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
            if(barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

void AddSC_boss_anubarak_toc()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_anubarak);
}
