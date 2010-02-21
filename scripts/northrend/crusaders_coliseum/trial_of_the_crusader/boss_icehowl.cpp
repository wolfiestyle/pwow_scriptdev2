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
SDName: Boss Icehowl
SD%Complete: 0
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_icehowl
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    SPELL_BERKSERK              = 26662,
    SPELL_FEROCIOUS_BUTT_N10    = 66770,
    SPELL_FEROCIOUS_BUTT_N25    = 67654,
    SPELL_FEROCIOUS_BUTT_H10    = 67655,
    SPELL_FEROCIOUS_BUTT_H25    = 67656,
    SPELL_ARCTIC_BREATH_N10     = 66689,
    SPELL_ARCTIC_BREATH_N25     = 67650,
    SPELL_ARCTIC_BREATH_H10     = 67651,
    SPELL_ARCTIC_BREATH_H25     = 67652,
    SPELL_WHIRL_N10             = 67345,
    SPELL_WHIRL_N25             = 67663,
    SPELL_WHIRL_H10             = 67664,
    SPELL_WHIRL_H25             = 67665,
    SPELL_MASSIVE_CRASH_N10     = 66683,
    SPELL_MASSIVE_CRASH_N25     = 67660,
    SPELL_MASSIVE_CRASH_H10     = 67661,
    SPELL_MASSIVE_CRASH_H25     = 67662,
    SPELL_FROTHING_RAGE_N10     = 66759,
    SPELL_FROTHING_RAGE_N25     = 67657,
    SPELL_FROTHING_RAGE_H10     = 67658,
    SPELL_FROTHING_RAGE_H25     = 67659,
    SPELL_TRAMPLE               = 66734,    // just the dmg part
    SPELL_STAGGERED_DAZE        = 66758,    // part of the Trample effect
};

struct MANGOS_DLL_DECL boss_toc_icehowlAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_icehowlAI(Creature* pCreature):
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

void AddSC_boss_icehowl()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_icehowl);
}
