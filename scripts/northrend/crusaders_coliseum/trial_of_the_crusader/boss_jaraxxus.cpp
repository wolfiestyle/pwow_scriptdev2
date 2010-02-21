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
SDName: Boss Jaraxxus
SD%Complete: 0
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_jaraxxus
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    SPELL_BERSERK               = 64328,
    SPELL_FEL_FIREBALL_N10      = 66532,
    SPELL_FEL_FIREBALL_N25      = 66963,
    SPELL_FEL_FIREBALL_H10      = 66964,
    SPELL_FEL_FIREBALL_H25      = 66965,
    SPELL_FEL_LIGHTNING_N10     = 66528,
    SPELL_FEL_LIGHTNING_N25     = 67029,
    SPELL_FEL_LIGHTNING_H10     = 67030,
    SPELL_FEL_LIGHTNING_H25     = 67031,
    SPELL_INCINERATE_FLESH_N10  = 66237,
    SPELL_INCINERATE_FLESH_N25  = 67049,
    SPELL_INCINERATE_FLESH_H10  = 67050,
    SPELL_INCINERATE_FLESH_H25  = 67051,
    SPELL_LEGION_FLAME_N10      = 66197,
    SPELL_LEGION_FLAME_N25      = 68123,
    SPELL_LEGION_FLAME_H10      = 68124,
    SPELL_LEGION_FLAME_H25      = 68125,
    SPELL_INFERNAL_ERUPTION_N10 = 66258,
    SPELL_INFERNAL_ERUPTION_N25 = 67901,
    SPELL_INFERNAL_ERUPTION_H10 = 67902,
    SPELL_INFERNAL_ERUPTION_H25 = 67903,
    SPELL_NETHER_PORTAL_N10     = 66269,
    SPELL_NETHER_PORTAL_N25     = 67898,
    SPELL_NETHER_PORTAL_H10     = 67899,
    SPELL_NETHER_PORTAL_H25     = 67900,
    SPELL_NETHER_POWER          = 67009,
};

struct MANGOS_DLL_DECL boss_toc_jaraxxusAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_jaraxxusAI(Creature* pCreature):
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

void AddSC_boss_jaraxxus()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_jaraxxus);
}
