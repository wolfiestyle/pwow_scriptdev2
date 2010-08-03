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
SDName: boss_the_lich_king
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

struct MANGOS_DLL_DECL boss_the_lich_kingAI: public boss_icecrown_citadelAI
{
    boss_the_lich_kingAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature)
    {
    }

    /*
    void Reset()
    {
        boss_icecrown_citadelAI::Reset();
    }
    */

    void Aggro(Unit* pWho)
    {
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
    }

    void JustDied(Unit* pKiller)
    {
        m_BossEncounter = DONE;
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_the_lich_king()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_the_lich_king);
}
