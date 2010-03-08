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
SDName: Boss Gormok
SD%Complete: 10
SDComment: vehicle support is needed to properly implement the 'throw snobold' event
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_gormok
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    // Gormok
    SPELL_IMPALE_N10            = 66331,    // "Requires Polearms"
    SPELL_IMPALE_N25            = 67477,
    SPELL_IMPALE_H10            = 67478,
    SPELL_IMPALE_H25            = 67479,
    SPELL_STAGGERING_STOMP_N10  = 66330,
    SPELL_STAGGERING_STOMP_N25  = 67647,
    SPELL_STAGGERING_STOMP_H10  = 67648,
    SPELL_STAGGERING_STOMP_H25  = 67649,
    // Snobold Vassal
    SPELL_SNOBOLLED             = 66406,    // dummy effect, target vehicle
    SPELL_RISING_ANGER          = 66636,    // target Gormok
    SPELL_BATTER                = 66408,
    SPELL_FIRE_BOMB             = 66313,
    SPELL_HEAD_CRACK            = 66407,
};

enum AddIds
{
    NPC_SNOBOLD_VASSAL          = 34800,
};

enum Events
{
    // Gormok
    EVENT_IMPALE = 1,
    EVENT_STAGGERING_STOMP,
    EVENT_THROW_SNOBOLD,
    // Snobold Vassal
    EVENT_BATTER,
    EVENT_FIRE_BOMB,
    EVENT_HEAD_CRACK,
};

#define TIMER_IMPALE            10*IN_MILISECONDS
#define TIMER_STAGGERING_STOMP  urand(20, 25)*IN_MILISECONDS
#define TIMER_THROW_SNOBOLD     urand(20, 45)*IN_MILISECONDS

#define MAX_SNOBOLD             5

struct MANGOS_DLL_DECL boss_toc_gormokAI: public boss_trial_of_the_crusaderAI
{
    uint32 m_uiSnoboldCount;

    typedef std::list<uint64> GuidList;
    GuidList m_Snobolds;

    boss_toc_gormokAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        m_uiSnoboldCount(0)
    {
    }

    void Reset()
    {
        DespawnSummons();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        Events.RescheduleEvent(EVENT_IMPALE, TIMER_IMPALE);
        Events.RescheduleEvent(EVENT_STAGGERING_STOMP, TIMER_STAGGERING_STOMP);
        Events.RescheduleEvent(EVENT_THROW_SNOBOLD, TIMER_THROW_SNOBOLD);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon && pSummon->GetEntry() == NPC_SNOBOLD_VASSAL)
        {
            m_Snobolds.push_back(pSummon->GetGUID());
            pSummon->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_IMPALE:
                    DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_IMPALE));
                    Events.ScheduleEvent(EVENT_IMPALE, TIMER_IMPALE);
                    break;
                case EVENT_STAGGERING_STOMP:
                    DoCast(m_creature, DIFFICULTY(SPELL_STAGGERING_STOMP));
                    Events.ScheduleEvent(EVENT_STAGGERING_STOMP, TIMER_STAGGERING_STOMP);
                    break;
                case EVENT_THROW_SNOBOLD:
                    DoSpawnCreature(NPC_SNOBOLD_VASSAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                    if (++m_uiSnoboldCount < MAX_SNOBOLD)
                        Events.ScheduleEvent(EVENT_THROW_SNOBOLD, TIMER_THROW_SNOBOLD);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void DespawnSummons()
    {
        for (GuidList::const_iterator i = m_Snobolds.begin(); i != m_Snobolds.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                pSummon->ForcedDespawn();
        m_Snobolds.clear();
        m_uiSnoboldCount = 0;
    }
};

void AddSC_boss_gormok()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_gormok);
}
