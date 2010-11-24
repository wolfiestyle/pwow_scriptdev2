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
SDName: boss_saviana
SD%Complete:
SDComment: placeholder
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum Says
{
    SAVIANA_AGGRO01     = -1620030,
    SAVIANA_SLAY01      = -1620031,
    SAVIANA_SLAY02      = -1620032,
    SAVIANA_DEATH01     = -1620033,
    SAVIANA_SPECIAL01   = -1620034,

};

enum Events
{
    EVENT_FLY_COMBO = 1,
    EVENT_FLY_CAST,
    EVENT_FLY_DOWN,
    EVENT_FIRE_BREATH,
    EVENT_ENRAGE,
};

enum Spells
{
    SPELL_ENRAGE        = 78722,
    SPELL_CONFLAGRATION = 74454,
    SPELL_FIRE_BREATH   = 74403,
    SPELL_FLAME_BEACON  = 74453,
    SPELL_CONFLAG_REAL  = 74456,
};

#define TIMER_FIRE_BREATH   25*IN_MILLISECONDS
#define TIMER_FLY_COMBO     50* IN_MILLISECONDS
#define TIMER_ENRAGE        30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_savianaAI: public boss_ruby_sanctumAI
{
    boss_savianaAI(Creature* pCreature):
        boss_ruby_sanctumAI(pCreature)
    {
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
    }

    /*
    void Reset()
    {
        boss_ruby_sanctumAI::Reset();
    }
    */

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAVIANA_AGGRO01, m_creature);
        Events.ScheduleEvent(EVENT_ENRAGE, TIMER_ENRAGE, TIMER_ENRAGE);
        Events.ScheduleEvent(EVENT_FLY_COMBO, 27*IN_MILLISECONDS, TIMER_FLY_COMBO);
        Events.ScheduleEvent(EVENT_FIRE_BREATH, 12*IN_MILLISECONDS, TIMER_FIRE_BREATH);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        if (roll_chance_i(50))
            DoScriptText(SAVIANA_SLAY01, m_creature);
        else
            DoScriptText(SAVIANA_SLAY02, m_creature);
    }

    void SpellHitTarget(Unit *pWho, const SpellEntry *pSpell)
    {
        if (pWho && pSpell->Id == SPELL_CONFLAGRATION)
            m_creature->CastSpell(pWho, SPELL_CONFLAG_REAL, true);
    }
    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAVIANA_DEATH01, m_creature);
        m_BossEncounter = DONE;
        boss_ruby_sanctumAI::JustDied(pKiller);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FLY_COMBO:
                    Events.DelayEventsWithId(EVENT_ENRAGE, 12*IN_MILLISECONDS);
                    Events.DelayEventsWithId(EVENT_FIRE_BREATH, 12*IN_MILLISECONDS);
                    DoStartNoMovement(m_creature->getVictim());
                    m_creature->GetMotionMaster()->MovePoint(0, 3162.167f, 691.915f, 106.734f);
                    Events.ScheduleEvent(EVENT_FLY_CAST, 5*IN_MILLISECONDS);
                    break;
                case EVENT_FLY_CAST:
                    for (uint32 i = (m_bIs10Man ? 3 : 6); i ; i--)
                    {
                        Unit *Target = NULL;
                        do // prevent selection of same target for Flame Beacon
                        {
                            Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                            if (!Target || m_creature->getAttackers().size() < (m_bIs10Man ? 4 : 7))
                                break;
                        }
                        while (Target->HasAura(SPELL_FLAME_BEACON));
                        if (Target)
                        {
                            DoCast(Target, SPELL_FLAME_BEACON, true);
                            DoCast(Target, SPELL_CONFLAGRATION, true);
                        }
                    }
                    DoScriptText(SAVIANA_SPECIAL01, m_creature);
                    Events.ScheduleEvent(EVENT_FLY_DOWN, 5*IN_MILLISECONDS);
                    break;
                case EVENT_FLY_DOWN:
                    DoStartMovement(m_creature->getVictim());
                    break;
                case EVENT_FIRE_BREATH:
                    DoCast(m_creature->getVictim(), SPELL_FIRE_BREATH, false);
                    break;
                case EVENT_ENRAGE:
                    DoCast(m_creature, SPELL_ENRAGE, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_saviana()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_saviana);
};
