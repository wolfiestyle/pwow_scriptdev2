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
SDName: Boss Valkyr Twins
SD%Complete: 0
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_fjola_lightbane
boss_toc_eydis_darkbane
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL boss_toc_eydis_darkbaneAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_eydis_darkbaneAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_fjola_lightbaneAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_fjola_lightbaneAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_twin_valkyr()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_fjola_lightbane);
    REGISTER_SCRIPT(boss_toc_eydis_darkbane);
}
