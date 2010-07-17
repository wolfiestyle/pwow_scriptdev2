/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss Assembly of Iron
SD%Complete: 75%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    // Any boss
    SPELL_SUPERCHARGE          = 61920,
    SPELL_BERSERK              = 47008,   // Hard enrage, don't know the correct ID.

    // Steelbreaker
    SPELL_HIGH_VOLTAGE         = 61890,
    SPELL_HIGH_VOLTAGE_H       = 63498,
    SPELL_FUSION_PUNCH         = 61903,
    SPELL_FUSION_PUNCH_H       = 63493,
    SPELL_STATIC_DISRUPTION    = 44008,
    SPELL_STATIC_DISRUPTION_H  = 63494,
    SPELL_OVERWHELMING_POWER   = 64637,
    SPELL_OVERWHELMING_POWER_H = 61888,
    SPELL_ELECTRICAL_CHARGE    = 61902,

    // Runemaster Molgeim
    SPELL_SHIELD_OF_RUNES      = 62274,
    SPELL_SHIELD_OF_RUNES_H    = 63489,
    SPELL_RUNE_OF_POWER        = 64320,
    SPELL_RUNE_OF_DEATH        = 62269,
    SPELL_RUNE_OF_DEATH_H      = 63490,
    SPELL_RUNE_OF_SUMMONING    = 62273,
    SPELL_LIGHTNING_BLAST      = 62054,
    SPELL_LIGHTNING_BLAST_H    = 63491,
    NPC_LIGHTNING_ELEMENTAL    = 32958,

    // Stormcaller Brundir
    SPELL_CHAIN_LIGHTNING      = 61879,
    SPELL_CHAIN_LIGHTNING_H    = 63479,
    SPELL_OVERLOAD             = 61869,
    SPELL_OVERLOAD_H           = 63481,
    SPELL_LIGHTNING_WHIRL      = 61915,
    SPELL_LIGHTNING_WHIRL_H    = 63483,
    SPELL_LIGHTNING_TENDRILS   = 61887,
    SPELL_LIGHTNING_TENDRILS_H = 63486,
    SPELL_STORMSHIELD          = 64187
};

enum Events
{
    EVENT_BERSERK,
    // Steelbreaker
    EVENT_FUSION_PUNCH,
    EVENT_STATIC_DISRUPTION,
    EVENT_OVERWHELMING_POWER,
    // Molgeim
    EVENT_RUNE_OF_POWER,
    EVENT_SHIELD_OF_RUNES,
    EVENT_RUNE_OF_DEATH,
    EVENT_RUNE_OF_SUMMONING,
    EVENT_LIGHTNING_BLAST,
    // Brundir
    EVENT_CHAIN_LIGHTNING,
    EVENT_OVERLOAD,
    EVENT_LIGHTNING_WHIRL,
    EVENT_LIGHTNING_TENDRILS,
    EVENT_STORMSHIELD,
    // Rune of Summoning
    EVENT_SUMMON_ELEMENTAL
};

enum Says
{
    // Steelbreaker
    SAY_STEEL_AGGRO             = -1300048,
    SAY_STEEL_SPECIAL           = -1300049,
    SAY_STEEL_KILLED_PLAYER1    = -1300050,
    SAY_STEEL_KILLED_PLAYER2    = -1300051,
    SAY_STEEL_BERSERK           = -1300052,
    SAY_STEEL_DEATH1            = -1300053,
    SAY_STEEL_DEATH2            = -1300054,

    // Runemaster Molgeim
    SAY_RUNE_AGGRO              = -1300055,
    SAY_RUNE_RUNE_OF_DEATH      = -1300056,
    SAY_RUNE_SUMMON_ADDS        = -1300057,
    SAY_RUNE_KILLED_PLAYER1     = -1300058,
    SAY_RUNE_KILLED_PLAYER2     = -1300059,
    SAY_RUNE_BERSERK            = -1300060,
    SAY_RUNE_DEATH1             = -1300061,
    SAY_RUNE_DEATH2             = -1300062,

    // Stormcaller Brundir
    SAY_STORM_AGGRO             = -1300063,
    SAY_STORM_SPECIAL           = -1300064,
    SAY_STORM_FLIGHT            = -1300065,
    SAY_STORM_KILLED_PLAYER1    = -1300066,
    SAY_STORM_KILLED_PLAYER2    = -1300067,
    SAY_STORM_BERSERK           = -1300068,
    SAY_STORM_DEATH1            = -1300069,
    SAY_STORM_DEATH2            = -1300070
};

#define BERSERK_TIMER   15*MINUTE*IN_MILLISECONDS

static bool IsEncounterComplete(ScriptedInstance *pInstance, Creature *pCreature)
{
   if (!pInstance || !pCreature)
        return false;

    for (uint32 i = 0; i < 3; i++)
    {
        uint64 guid = pInstance->GetData64(DATA_STEELBREAKER+i);
        if (!guid)
            return false;

        if (Creature *boss = pCreature->GetMap()->GetCreature(guid))
        {
            if (boss->isAlive())
                return false;
        }
        else
            return false;
    }

    return true;
}

struct MANGOS_DLL_DECL boss_steelbreakerAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    uint32 phase;

    boss_steelbreakerAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_ASSEMBLY)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        phase = 0;
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0,1) ? SAY_STEEL_DEATH1 : SAY_STEEL_DEATH2, m_creature);
        if (IsEncounterComplete(m_pInstance, m_creature))
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* Brundir = GET_CREATURE(DATA_BRUNDIR))
            if(Brundir->isAlive())
                Brundir->AddThreat(pWho);
        if (Creature* Molgeim = GET_CREATURE(DATA_MOLGEIM))
            if(Molgeim->isAlive())
                Molgeim->AddThreat(pWho);
        DoCast(m_creature, HEROIC(SPELL_HIGH_VOLTAGE, SPELL_HIGH_VOLTAGE_H), true);
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        UpdatePhase();
        DoScriptText(SAY_STEEL_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdatePhase()
    {
        phase++;
        events.SetPhase(phase);
        events.RescheduleEvent(EVENT_FUSION_PUNCH, 15*IN_MILLISECONDS);
        if (phase >= 2)
            events.RescheduleEvent(EVENT_STATIC_DISRUPTION, 30*IN_MILLISECONDS);
        if (phase >= 3)
            events.RescheduleEvent(EVENT_OVERWHELMING_POWER, urand(0, 5)*IN_MILLISECONDS);
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage >= m_creature->GetHealth())
        {
            if (Creature* Brundir = GET_CREATURE(DATA_BRUNDIR))
                if(Brundir->isAlive())
                {
                    Brundir->SetHealth(Brundir->GetMaxHealth());
                    Brundir->CastSpell(Brundir, SPELL_SUPERCHARGE, true);
                }

            if (Creature* Molgeim = GET_CREATURE(DATA_MOLGEIM))
                if(Molgeim->isAlive())
                {
                    Molgeim->SetHealth(Molgeim->GetMaxHealth());
                    Molgeim->CastSpell(Molgeim, SPELL_SUPERCHARGE, true);
                }

            //m_creature->CastSpell(m_creature, SPELL_SUPERCHARGE, true);
        }
    }

    void SpellHit(Unit *from, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SUPERCHARGE)
            UpdatePhase();
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_STEEL_KILLED_PLAYER1 : SAY_STEEL_KILLED_PLAYER2, m_creature);
        if(phase == 3)
            DoCast(m_creature, SPELL_ELECTRICAL_CHARGE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_STEEL_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_FUSION_PUNCH:
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_FUSION_PUNCH, SPELL_FUSION_PUNCH_H));
                    events.ScheduleEvent(EVENT_FUSION_PUNCH, urand(13, 22)*IN_MILLISECONDS);
                    break;
                case EVENT_STATIC_DISRUPTION:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(Target, HEROIC(SPELL_STATIC_DISRUPTION, SPELL_STATIC_DISRUPTION_H));
                    events.ScheduleEvent(EVENT_STATIC_DISRUPTION, urand(20, 40)*IN_MILLISECONDS);
                    break;
                case EVENT_OVERWHELMING_POWER:
                    DoScriptText(SAY_STEEL_SPECIAL, m_creature);
                    DoCast(m_creature->getVictim(), HEROIC(SPELL_OVERWHELMING_POWER, SPELL_OVERWHELMING_POWER_H));
                    events.ScheduleEvent(EVENT_OVERWHELMING_POWER, HEROIC(60, 35)*IN_MILLISECONDS);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_runemaster_molgeimAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    uint32 phase;

    boss_runemaster_molgeimAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_ASSEMBLY)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        phase = 0;
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0,1) ? SAY_RUNE_DEATH1 : SAY_RUNE_DEATH2, m_creature);
        if (IsEncounterComplete(m_pInstance, m_creature))
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* Steelbreaker = GET_CREATURE(DATA_STEELBREAKER))
            if (Steelbreaker->isAlive())
                Steelbreaker->AddThreat(pWho);
        if (Creature* Brundir = GET_CREATURE(DATA_BRUNDIR))
            if(Brundir->isAlive())
                Brundir->AddThreat(pWho);
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        UpdatePhase();
        DoScriptText(SAY_RUNE_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdatePhase()
    {
        phase++;
        events.SetPhase(phase);
        events.RescheduleEvent(EVENT_SHIELD_OF_RUNES, 27*IN_MILLISECONDS);
        events.RescheduleEvent(EVENT_RUNE_OF_POWER, 60*IN_MILLISECONDS);
        if (phase >= 2)
            events.RescheduleEvent(EVENT_RUNE_OF_DEATH, 30*IN_MILLISECONDS);
        if (phase >= 3)
            events.RescheduleEvent(EVENT_RUNE_OF_SUMMONING, urand(20, 30)*IN_MILLISECONDS);
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage >= m_creature->GetHealth())
        {
            if (Creature* Steelbreaker = GET_CREATURE(DATA_STEELBREAKER))
                if (Steelbreaker->isAlive())
                {
                   Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
                   Steelbreaker->CastSpell(Steelbreaker, SPELL_SUPERCHARGE, true);
                }

            if (Creature* Brundir = GET_CREATURE(DATA_BRUNDIR))
                if (Brundir->isAlive())
                {
                    Brundir->SetHealth(Brundir->GetMaxHealth());
                    Brundir->CastSpell(Brundir, SPELL_SUPERCHARGE, true);
                }

            //m_creature->CastSpell(m_creature, SPELL_SUPERCHARGE, true);
        }
    }

    void SpellHit(Unit *from, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SUPERCHARGE)
            UpdatePhase();
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_RUNE_KILLED_PLAYER1 : SAY_RUNE_KILLED_PLAYER2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_RUNE_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_RUNE_OF_POWER: // Improve target selection; random alive friendly
                {
                    Unit *Target = DoSelectLowestHpFriendly(60);
                    if (!Target || (Target && !Target->isAlive()))
                        Target = m_creature;
                    DoCast(Target, SPELL_RUNE_OF_POWER);
                    events.ScheduleEvent(EVENT_RUNE_OF_POWER, 60*IN_MILLISECONDS);
                    break;
                }
                case EVENT_SHIELD_OF_RUNES:
                    DoCast(m_creature, HEROIC(SPELL_SHIELD_OF_RUNES, SPELL_SHIELD_OF_RUNES_H));
                    events.ScheduleEvent(EVENT_SHIELD_OF_RUNES, urand(27, 34)*IN_MILLISECONDS);
                    break;
                case EVENT_RUNE_OF_DEATH:
                    DoScriptText(SAY_RUNE_RUNE_OF_DEATH, m_creature);
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(Target, HEROIC(SPELL_RUNE_OF_DEATH, SPELL_RUNE_OF_DEATH_H));
                    events.ScheduleEvent(EVENT_RUNE_OF_DEATH, urand(30, 40)*IN_MILLISECONDS);
                    break;
                case EVENT_RUNE_OF_SUMMONING:
                    DoScriptText(SAY_RUNE_SUMMON_ADDS, m_creature);
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(Target, SPELL_RUNE_OF_SUMMONING);
                    events.ScheduleEvent(EVENT_RUNE_OF_SUMMONING, urand(20, 30)*IN_MILLISECONDS);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_stormcaller_brundirAI: public ScriptedAI
{
    uint32 const m_uiBossEncounterId;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager events;
    uint32 phase;

    boss_stormcaller_brundirAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_ASSEMBLY)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        events.Reset();
        phase = 0;
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0,1) ? SAY_STORM_DEATH1 : SAY_STORM_DEATH2, m_creature);
        if (IsEncounterComplete(m_pInstance, m_creature))
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* Steelbreaker = GET_CREATURE(DATA_STEELBREAKER))
            if (Steelbreaker->isAlive())
                Steelbreaker->AddThreat(pWho);
        if (Creature* Molgeim = GET_CREATURE(DATA_MOLGEIM))
            if(Molgeim->isAlive())
                Molgeim->AddThreat(pWho);
        events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);
        UpdatePhase();
        DoScriptText(SAY_STORM_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdatePhase()
    {
        phase++;
        events.SetPhase(phase);
        events.RescheduleEvent(EVENT_CHAIN_LIGHTNING, urand(9, 17)*IN_MILLISECONDS);
        events.RescheduleEvent(EVENT_OVERLOAD, urand(60, 125)*IN_MILLISECONDS);
        if (phase >= 2)
            events.RescheduleEvent(EVENT_LIGHTNING_WHIRL, urand(20, 40)*IN_MILLISECONDS);
        if (phase >= 3)
        {
            DoCast(m_creature, SPELL_STORMSHIELD);
            events.RescheduleEvent(EVENT_LIGHTNING_TENDRILS, urand(40, 80)*IN_MILLISECONDS);
        }
    }

    void DamageTaken(Unit* pKiller, uint32 &damage)
    {
        if (damage >= m_creature->GetHealth())
        {
            if (Creature* Steelbreaker = GET_CREATURE(DATA_STEELBREAKER))
                if (Steelbreaker->isAlive())
                {
                    Steelbreaker->SetHealth(Steelbreaker->GetMaxHealth());
                    Steelbreaker->CastSpell(Steelbreaker, SPELL_SUPERCHARGE, true);
                }

            if (Creature* Molgeim = GET_CREATURE(DATA_MOLGEIM))
                if (Molgeim->isAlive())
                {
                    Molgeim->SetHealth(Molgeim->GetMaxHealth());
                    Molgeim->CastSpell(Molgeim, SPELL_SUPERCHARGE, true);
                }

            //m_creature->CastSpell(m_creature, SPELL_SUPERCHARGE, true);
        }
    }

    void SpellHit(Unit *from, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SUPERCHARGE)
            UpdatePhase();
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_STORM_KILLED_PLAYER1 : SAY_STORM_KILLED_PLAYER2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        while(uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_STORM_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(Target, HEROIC(SPELL_CHAIN_LIGHTNING, SPELL_CHAIN_LIGHTNING_H));
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(9, 17)*IN_MILLISECONDS);
                    break;
                case EVENT_OVERLOAD:
                    DoScriptText(SAY_STORM_SPECIAL, m_creature);
                    DoCast(m_creature, HEROIC(SPELL_OVERLOAD, SPELL_OVERLOAD_H));
                    events.ScheduleEvent(EVENT_OVERLOAD, urand(60, 125)*IN_MILLISECONDS);
                    break;
                case EVENT_LIGHTNING_WHIRL:
                    DoCast(m_creature, HEROIC(SPELL_LIGHTNING_WHIRL, SPELL_LIGHTNING_WHIRL_H));
                    events.ScheduleEvent(EVENT_LIGHTNING_WHIRL, urand(20, 40)*IN_MILLISECONDS);
                    break;
                case EVENT_LIGHTNING_TENDRILS:
                    DoScriptText(SAY_STORM_FLIGHT, m_creature);
                    DoCast(m_creature, HEROIC(SPELL_LIGHTNING_TENDRILS, SPELL_LIGHTNING_TENDRILS_H));
                    events.DelayEvents(15000, 5000);
                    DoResetThreat();
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_lightning_elementalAI : public ScriptedAI
{
    bool m_bIsRegularMode;

    mob_lightning_elementalAI(Creature *pCreature):
        ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        Unit *Target = NULL;
        float min_dist = 100.0f;
        Map::PlayerList const& plist = m_creature->GetMap()->GetPlayers();
        if (plist.isEmpty())
            return;
        for (Map::PlayerList::const_iterator i = plist.begin(); i != plist.end(); ++i)
            if (i->getSource()->isAlive())
            {
                float dist = m_creature->GetDistance(i->getSource());
                if (dist < min_dist)
                {
                    min_dist = dist;
                    Target = i->getSource();
                }
            }
        if (Target)
        {
            m_creature->AddThreat(Target, 5000000.0f);
            AttackStart(Target);
        }
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if (spell->Id == HEROIC(SPELL_LIGHTNING_BLAST, SPELL_LIGHTNING_BLAST_H))
        {
            m_creature->SetHealth(0);
            m_creature->setDeathState(JUST_DIED);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Unit *Target = m_creature->getVictim();
        if (m_creature->IsWithinDistInMap(Target, 5.0f))
            DoCast(Target, HEROIC(SPELL_LIGHTNING_BLAST, SPELL_LIGHTNING_BLAST_H));
    }
};

struct MANGOS_DLL_DECL mob_rune_of_summoningAI : public ScriptedAI
{
    uint32 n_summons;
    EventManager events;

    mob_rune_of_summoningAI(Creature *pCreature):
        ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        n_summons = 1; /*m_creature->GetMap()->GetPlayers().getSize();*/
        events.Reset();
        events.RescheduleEvent(EVENT_SUMMON_ELEMENTAL, 1000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->isAlive())
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SUMMON_ELEMENTAL:
                    if (n_summons > 0)
                    {
                        m_creature->SummonCreature(NPC_LIGHTNING_ELEMENTAL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
                        if (--n_summons)
                            events.ScheduleEvent(EVENT_SUMMON_ELEMENTAL, 1000);
                        else
                        {
                            m_creature->SetHealth(0);
                            m_creature->setDeathState(JUST_DIED);
                        }
                    }
                    break;
                default:
                    break;
            }
    }
};

CreatureAI* GetAI_boss_steelbreaker(Creature* pCreature)
{
    return new boss_steelbreakerAI(pCreature);
}

CreatureAI* GetAI_boss_runemaster_molgeim(Creature* pCreature)
{
    return new boss_runemaster_molgeimAI(pCreature);
}

CreatureAI* GetAI_boss_stormcaller_brundir(Creature* pCreature)
{
    return new boss_stormcaller_brundirAI(pCreature);
}

CreatureAI* GetAI_mob_lightning_elemental(Creature* pCreature)
{
    return new mob_lightning_elementalAI(pCreature);
}

CreatureAI* GetAI_mob_rune_of_summoning(Creature* pCreature)
{
    return new mob_rune_of_summoningAI(pCreature);
}

void AddSC_boss_assembly_of_iron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_steelbreaker";
    newscript->GetAI = &GetAI_boss_steelbreaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_runemaster_molgeim";
    newscript->GetAI = &GetAI_boss_runemaster_molgeim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_stormcaller_brundir";
    newscript->GetAI = &GetAI_boss_stormcaller_brundir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_lightning_elemental";
    newscript->GetAI = &GetAI_mob_lightning_elemental;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rune_of_summoning";
    newscript->GetAI = &GetAI_mob_rune_of_summoning;
    newscript->RegisterSelf();
}
