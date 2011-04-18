/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_sindragosa
SD%Complete: 90%
SDComment: Line of sight does not check NPCs, so, abilities that require the Frost Tomb as a LOS breaker will not be used [Frost Bomb and Mystic Buffet].
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                   = 26662,
    SPELL_FROST_AURA                = 70084,
    SPELL_CLEAVE                    = 19983,
    SPELL_TAIL_SMASH                = 71077,
    SPELL_FROST_BREATH              = 69649,
    SPELL_PERMEATING_CHILL          = 70109,
    SPELL_ICY_GRIP                  = 70117,
    SPELL_BLISTERING_COLD           = 70123,
    SPELL_UNCHAINED_MAGIC           = 69762,
    SPELL_FROST_BOMB                = 71053,
    SPELL_MYSTIC_BUFFET             = 70128,
    SPELL_ASPHYXIATION              = 71665,
    SPELL_FROST_BOMB_TARGET_VISUAL  = 70022,
    SPELL_FROST_BEACON              = 70126,
    SPELL_ICE_TOMB_VISUAL           = 69675,
    SPELL_ICE_TOMB_AURA             = 70157,
    SPELL_ICE_TOMB_AURA2            = 69700,
};

enum Npcs
{
    NPC_RIMEFANG                    = 37533,
    NPC_SPINESTALKER                = 37534,
    NPC_ICE_TOMB                    = 36980,
    NPC_FROST_BOMB_TARGET           = 37186,
};

enum Says
{
    SAY_AGGRO                       = -1300530,
    SAY_UNCHAINED_MAGIC             = -1300531,
    SAY_BLISTERING_COLD             = -1300532,
    SAY_RESPITE_FOR_TORMENTED_SOUL  = -1300533,
    SAY_AIR_PHASE                   = -1300534,
    SAY_PHASE_LAND_TWO              = -1300535,
    SAY_KILLED_PLAYER1              = -1300536,
    SAY_KILLED_PLAYER2              = -1300537,
    SAY_BERSERK                     = -1300538,
    SAY_DEATH                       = -1300539,
    SAY_BLISTERING_COLD_EMOTE       = -1300540,
    SAY_ICE_TOMB_EMOTE              = -1300541,
};

enum Events
{
    EVENT_BERSERK = 100,
    EVENT_CLEAVE,
    EVENT_TAIL_SMASH,
    EVENT_FROST_BREATH,
    EVENT_ICY_GRIP,
    EVENT_UNCHAINED_MAGIC,
    EVENT_FROST_BOMB,
    EVENT_FROST_BOMB_EXPLODE,
    EVENT_FLY,
    EVENT_LAND,
    EVENT_ICE_TOMB,

    // Messages
    MESSAGE_OTHER_GUARD_DEAD,
};

enum Phases
{
    PHASE_LAND_ONE = 1, // 100% - 85%
    PHASE_AIR,
    PHASE_LAND_TWO,     // 85% - 35%
    PHASE_LAND_THREE,   // 35% - 0%

    PMASK_AIR               = bit_mask<PHASE_AIR>::value,
    PMASK_LAND_TWO          = bit_mask<PHASE_LAND_TWO>::value,
    PMASK_LAND_THREE        = bit_mask<PHASE_LAND_THREE>::value,
    PMASK_LAND              = bit_mask<PHASE_LAND_ONE, PHASE_LAND_TWO, PHASE_LAND_THREE>::value,
};

#define FLOOR_HEIGHT                203.4f
#define CENTER_LINE_Y               2484.5f
#define CENTER_X                    4408.0f
#define SUMMON_X                    4455.5f

#define TIMER_BERSERK               10*MINUTE*IN_MILLISECONDS
#define TIMER_CLEAVE                10*IN_MILLISECONDS
#define TIMER_TAIL_SMASH            30*IN_MILLISECONDS
#define TIMER_FROST_BREATH          22*IN_MILLISECONDS
#define TIMER_ICY_GRIP              69*IN_MILLISECONDS
#define TIMER_UNCHAINED_MAGIC       32*IN_MILLISECONDS
#define TIMER_FLY                   110*IN_MILLISECONDS
#define TIMER_LAND                  45*IN_MILLISECONDS
#define TIMER_FROST_BOMB            7*IN_MILLISECONDS
#define TIMER_FROST_BOMB_EXPLODE    5*IN_MILLISECONDS
#define TIMER_ICE_TOMB              18*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_sindragosaAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;

    boss_sindragosaAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature)
    {
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        RemoveEncounterAuras(SPELL_ICE_TOMB_AURA, SPELL_ICE_TOMB_AURA2, SPELL_ASPHYXIATION);
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        if (InstanceProgressionCheck())
            return;
        m_BossEncounter = IN_PROGRESS;
        SetCombatMovement(true);
        DoCast(m_creature, SPELL_FROST_AURA, true);
        DoCast(m_creature, SPELL_PERMEATING_CHILL, true);
        Events.SetPhase(PHASE_LAND_ONE);
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(CLEAVE, 0, 0, PMASK_LAND);
        SCHEDULE_EVENT(TAIL_SMASH, 0, 0, PMASK_LAND);
        SCHEDULE_EVENT(FROST_BREATH, 0, 0, PMASK_LAND);
        SCHEDULE_EVENT(ICY_GRIP, 0, 0, PMASK_LAND);
        SCHEDULE_EVENT(UNCHAINED_MAGIC, 0, 0, PMASK_LAND);
        //SCHEDULE_EVENT(FROST_BOMB, 0, 0, PMASK_AIR);
        SCHEDULE_EVENT(ICE_TOMB, 0, 0, PMASK_LAND_THREE);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void SpellHitTarget(Unit *pWho, const SpellEntry *pSpell)
    {
        if (pWho && pSpell->Id == SPELL_ICE_TOMB_AURA)
            if (Creature *IceTomb = SummonMgr.SummonCreatureAt(pWho, NPC_ICE_TOMB, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000))
            {
                IceTomb->setFaction(m_creature->getFaction());
                IceTomb->SetCreatorGuid(pWho->GetObjectGuid());
            }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm && pSumm->GetEntry() == NPC_ICE_TOMB)
        {
            if (Unit *IcedTarget = m_creature->GetMap()->GetUnit(pSumm->GetCreatorGuid()))
            {
                IcedTarget->RemoveAurasDueToSpell(SPELL_ICE_TOMB_AURA);
                IcedTarget->RemoveAurasDueToSpell(SPELL_ICE_TOMB_AURA2);
                IcedTarget->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);
            }
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
        }
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        m_BossEncounter = DONE;
        DoScriptText(SAY_DEATH, m_creature);
        SummonMgr.UnsummonAll();
        RemoveEncounterAuras(SPELL_ICE_TOMB_AURA, SPELL_ICE_TOMB_AURA2, SPELL_ASPHYXIATION);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        if (Events.GetPhase() == PHASE_LAND_ONE && m_creature->GetHealthPercent() < 85.0f)
        {
            Events.ScheduleEvent(EVENT_FLY, 0, TIMER_FLY, 0, 0, PMASK_LAND_TWO);
            Events.SetPhase(PHASE_LAND_TWO);
        }
        else if (Events.GetPhase() != PHASE_LAND_THREE && m_creature->GetHealthPercent() < 35.0f)
        {
            Events.ScheduleEvent(EVENT_LAND, 0);
            Events.SetPhase(PHASE_LAND_THREE);
            //DoCast(m_creature, SPELL_MYSTIC_BUFFET);
            DoScriptText(SAY_PHASE_LAND_TWO, m_creature);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BERSERK, m_creature);
                    break;
                case EVENT_CLEAVE:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                    break;
                case EVENT_TAIL_SMASH:
                    DoCast(m_creature, SPELL_TAIL_SMASH);
                    break;
                case EVENT_FROST_BREATH:
                    DoCast(m_creature->getVictim(), SPELL_FROST_BREATH);
                    break;
                case EVENT_ICY_GRIP:
                    DoCast(m_creature->getVictim(), SPELL_ICY_GRIP, true);
                    DoCast(m_creature->getVictim(), SPELL_BLISTERING_COLD);
                    DoScriptText(SAY_BLISTERING_COLD, m_creature);
                    DoScriptText(SAY_BLISTERING_COLD_EMOTE, m_creature);
                    break;
                case EVENT_UNCHAINED_MAGIC:
                {
                    for (uint32 i = 2; i; i--)
                    {
                        bool IsProperTarget = false;
                        Unit *Target = NULL;
                        for (uint32 j = m_bIs10Man ? 20 : 35; j ; j--) // for loop instead of do/while loop just in case the raid is crazy and brought no mana users.
                        {
                            Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1);
                            if (!Target || Target->GetTypeId() != TYPEID_PLAYER)
                                continue;
                            if (Target->getPowerType() == POWER_MANA && Target->getClass() != CLASS_HUNTER)
                            {
                                IsProperTarget = true;
                                break;
                            }
                        }
                        if (IsProperTarget && Target)
                            DoCast(Target, SPELL_UNCHAINED_MAGIC);
                    }
                    DoScriptText(SAY_UNCHAINED_MAGIC, m_creature);
                    break;
                }
                case EVENT_FROST_BOMB:
                    float x, y;
                    GetRandomPointInCircle(x, y, 40.0f, CENTER_X, CENTER_LINE_Y);
                    if (Creature *FrostBombTarget = SummonMgr.SummonCreature(NPC_FROST_BOMB_TARGET, x, y, FLOOR_HEIGHT, 0.0f, TEMPSUMMON_TIMED_DESPAWN, TIMER_FROST_BOMB_EXPLODE+1000))
                        FrostBombTarget->CastSpell(FrostBombTarget, SPELL_FROST_BOMB_TARGET_VISUAL, true);
                    Events.ScheduleEvent(EVENT_FROST_BOMB_EXPLODE, TIMER_FROST_BOMB_EXPLODE);
                    break;
                case EVENT_FROST_BOMB_EXPLODE:
                    if (Creature *FrostBombTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_FROST_BOMB_TARGET))
                    {
                        FrostBombTarget->CastSpell(FrostBombTarget, SPELL_FROST_BOMB, true);
                        SummonMgr.RemoveSummonFromList(FrostBombTarget->GetObjectGuid());
                    }
                    break;
                case EVENT_FLY:
                    SetCombatMovement(false);
                    DoStartNoMovement(m_creature->getVictim());
                    m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_HEIGHT + 35.0f);
                    Events.ScheduleEvent(EVENT_LAND, TIMER_LAND);
                    Events.ScheduleEvent(EVENT_ICE_TOMB, 0);
                    Events.SetPhase(PHASE_AIR);
                    DoScriptText(SAY_AIR_PHASE, m_creature);
                    break;
                case EVENT_LAND:
                {
                    SetCombatMovement(true);
                    DoStartMovement(m_creature->getVictim());
                    if (Events.GetPhase() == PHASE_AIR)
                        Events.SetPhase(PHASE_LAND_TWO);
                    std::list<Creature*> FrostTombs;
                    SummonMgr.GetAllSummonsWithId(FrostTombs, NPC_ICE_TOMB);
                    for (std::list<Creature*>::iterator i = FrostTombs.begin(); i != FrostTombs.end(); ++i)
                        if (Unit *IcedTarget = (*i)->GetOwner())
                            IcedTarget->CastSpell(IcedTarget, SPELL_ASPHYXIATION, true);
                    break;
                }
                case EVENT_ICE_TOMB:
                {
                    if (Events.GetPhase() == PHASE_AIR)
                        for (int i = m_bIs10Man ? 2 : (m_bIsHeroic ? 6 : 5); i ; i--)
                        {
                            Unit *Target = NULL;
                            do // prevent selection of same target for ice tomb
                            {
                                Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                                if (!Target || m_creature->getAttackers().size() <= (m_bIs10Man ? 2 : 5))
                                    break;
                            }
                            while (Target->HasAura(SPELL_FROST_BEACON));
                            if (Target)
                            {
                                DoCast(Target, SPELL_FROST_BEACON, true);
                                DoCast(Target, SPELL_ICE_TOMB_VISUAL, true);
                            }
                        }
                    else
                        if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        {
                            DoCast(Target, SPELL_FROST_BEACON, true);
                            DoCast(Target, SPELL_ICE_TOMB_VISUAL, true);
                            DoScriptText(SAY_ICE_TOMB_EMOTE, m_creature, Target);
                        }
                    break;
                }
                default:
                    break;
            }

        if (Events.GetPhase() != PHASE_AIR)
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_sindragosa_guardAI: public ScriptedAI, ScriptMessageInterface
{
    mob_sindragosa_guardAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
    }

    void Reset() {}

    void JustDied(Unit *pKiller)
    {
        if (m_creature->GetEntry() == NPC_RIMEFANG)
            BroadcastScriptMessageToEntry(m_creature, NPC_SPINESTALKER, 500.0f, MESSAGE_OTHER_GUARD_DEAD);
        else if (m_creature->GetEntry() == NPC_SPINESTALKER)
            BroadcastScriptMessage(m_creature, NPC_RIMEFANG, MESSAGE_OTHER_GUARD_DEAD);
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_OTHER_GUARD_DEAD && m_creature->GetHealth() == 0)
            if (Creature *Sindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, SUMMON_X, CENTER_LINE_Y, FLOOR_HEIGHT, M_PI_F, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS))
                Sindragosa->SetInCombatWithZone();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_sindragosa()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_sindragosa);
    REGISTER_SCRIPT(mob_sindragosa_guard);
}
