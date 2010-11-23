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
SDName: boss_zarithian
SD%Complete:
SDComment:placeholder
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum Says
{
    ZARITHRIAN_AGGRO01      = -1620019,
    ZARITHRIAN_SLAY01       = -1620020,
    ZARITHRIAN_SLAY02       = -1620021,
    ZARITHRIAN_DEATH01      = -1620022,
    ZARITHRIAN_SPECIAL01    = -1620023,
};

enum Events
{
    EVENT_SUMMON_ADDS = 1,
    EVENT_ROAR,
    EVENT_CLEAVE_ARMOR,

    EVENT_GOUT,
    EVENT_NOVA,
};

enum Spells
{
    // zarithrian
    SPELL_CLEAVE_ARMOR          = 74367,
    SPELL_INTIMIDATING_ROAR     = 74384,
    SPELL_SUMMON_FLAMECALLER    = 74398,

    // flamecaller
    SPELL_BLAST_NOVA    = 74392,
    SPELL_LAVA_GOUT     = 74395,
};

enum Adds
{
    NPC_FLAMECALLER = 39814,
};

#define TIMER_CLEAVE_ARMOR  15*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define TIMER_ROAR          33*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_ADDS          15500, 45500

#define TIMER_GOUT          3*IN_MILLISECONDS, 5*IN_MILLISECONDS
#define TIMER_NOVA          5*IN_MILLISECONDS, 15*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_flamecaller_rsAI: public ScriptedAI
{
    EventManager Events;
    mob_flamecaller_rsAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
    }

    void Reset()
    {
        Events.Reset();
    };

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_GOUT, TIMER_GOUT, TIMER_GOUT);
        Events.ScheduleEventInRange(EVENT_NOVA, TIMER_NOVA, TIMER_NOVA);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_GOUT:
                    DoCast(m_creature->getVictim(), SPELL_LAVA_GOUT);
                    break;
                case EVENT_NOVA:
                    DoCast(m_creature, SPELL_BLAST_NOVA);
                    break;
                default:
                    break;
            }
    }
};

struct MANGOS_DLL_DECL boss_zarithrianAI: public boss_ruby_sanctumAI
{
    SummonManager SummonMgr;
    boss_zarithrianAI(Creature* pCreature):
        boss_ruby_sanctumAI(pCreature),
        SummonMgr(m_creature)
    {
    }

    void Reset()
    {
        if (GameObject* pFlameWall = GET_GAMEOBJECT(DATA_FLAME_WALLS))
            pFlameWall->SetGoState(GO_STATE_ACTIVE);
        SummonMgr.UnsummonAll();
        boss_ruby_sanctumAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        if (GameObject* pFlameWall = GET_GAMEOBJECT(DATA_FLAME_WALLS))
            pFlameWall->SetGoState(GO_STATE_READY);
        DoScriptText(ZARITHRIAN_AGGRO01, m_creature);
        Events.ScheduleEventInRange(EVENT_ROAR, 15000, 15500, TIMER_ROAR);
        Events.ScheduleEvent(EVENT_SUMMON_ADDS, TIMER_ADDS);
        Events.ScheduleEventInRange(EVENT_CLEAVE_ARMOR, TIMER_CLEAVE_ARMOR, TIMER_CLEAVE_ARMOR);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        if (roll_chance_i(50))
            DoScriptText(ZARITHRIAN_SLAY01, m_creature);
        else
            DoScriptText(ZARITHRIAN_SLAY02, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        DoScriptText(ZARITHRIAN_DEATH01, m_creature);
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
                case EVENT_CLEAVE_ARMOR:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE_ARMOR);
                    break;
                case EVENT_ROAR:
                    Events.DelayEventsWithId(EVENT_CLEAVE_ARMOR, 2*IN_MILLISECONDS);
                    DoCast(m_creature, SPELL_INTIMIDATING_ROAR);
                    break;
                case EVENT_SUMMON_ADDS:
                    DoScriptText(ZARITHRIAN_SPECIAL01, m_creature);
                    m_creature->CastSpell(3024.79f, 477.42f, 90.23f, SPELL_SUMMON_FLAMECALLER, true);
                    m_creature->CastSpell(3018.698f, 575.33f, 88.61f, SPELL_SUMMON_FLAMECALLER, true);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustSummoned(Creature* pSumm)
    {
        if (pSumm && pSumm->GetEntry() == NPC_FLAMECALLER)
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
        if (pSumm)
            pSumm->SetInCombatWithZone();
    };
};

void AddSC_boss_zarithrian()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_zarithrian);
    REGISTER_SCRIPT(mob_flamecaller_rs);
};
