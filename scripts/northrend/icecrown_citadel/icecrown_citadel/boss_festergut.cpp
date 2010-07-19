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
SDName: boss_festergut
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                   = 47008,
    SPELL_GAS_SPORE                 = 71221,
    SPELL_PUNGENT_BLIGHT            = 69195,
    SPELL_GASEOUS_BLIGHT_LEVEL0     = 69157, //strongest
    SPELL_GASEOUS_BLIGHT_LEVEL1     = 69162,
    SPELL_GASEOUS_BLIGHT_LEVEL2     = 69164, //weakest
    SPELL_GASEOUS_BLIGHT_VISUAL0    = 69126,
    SPELL_GASEOUS_BLIGHT_VISUAL1    = 69152,
    SPELL_GASEOUS_BLIGHT_VISUAL2    = 69154,
    SPELL_INHALE_BLIGHT             = 69165,
    SPELL_VILE_GAS                  = 69240,
    SPELL_GASTRIC_BLOAT             = 72219,
    SPELL_GASTRIC_EXPLOSION         = 72227,
    SPELL_INNOCULATED               = 69291,
    SPELL_INHALED_BLIGHT            = 69166,

    SPELL_MALLEABLE_GOO             = 70852,
};

static const uint32 BlightSpells[3][2] = 
{
    {SPELL_GASEOUS_BLIGHT_LEVEL0, SPELL_GASEOUS_BLIGHT_VISUAL0},
    {SPELL_GASEOUS_BLIGHT_LEVEL1, SPELL_GASEOUS_BLIGHT_VISUAL1},
    {SPELL_GASEOUS_BLIGHT_LEVEL2, SPELL_GASEOUS_BLIGHT_VISUAL2}
};

enum NPCS
{
    NPC_VILE_GAS_STALKER    = 38548, // dummy npc - used for blight target (might not be correct one)
};

enum Says
{
    SAY_STINKY_DEATH        = -1300472,
    SAY_AGGRO               = -1300473,
    SAY_PUTRICIDE_GAS_CLOUD = -1300474,
    SAY_GAS_SPORE           = -1300475,
    SAY_PUNGENT_BLIGHT1     = -1300476,
    SAY_PUNGENT_BLIGHT2     = -1300477,
    SAY_KILLED_PLAYER1      = -1300478,
    SAY_KILLED_PLAYER2      = -1300479,
    SAY_BERSERK             = -1300480,
    SAY_DEATH1              = -1300481,
    SAY_PUTRICIDE_DEATH2    = -1300482,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_GAS_SPORE,
    EVENT_INHALE_BLIGHT,
    EVENT_VILE_GAS,
    EVENT_GASTRIC_BLOAT,
    EVENT_MALLEABLE_GOO,
    EVENT_START_GAS_CLOUD,
};

#define TIMER_BERSERK           5*MINUTE*IN_MILLISECONDS
#define TIMER_GAS_SPORE         35*IN_MILLISECONDS
#define TIMER_VILE_GAS          20*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_INHALE_BLIGHT     30*IN_MILLISECONDS, 35*IN_MILLISECONDS
#define TIMER_GASTRIC_BLOAT     8*IN_MILLISECONDS, 12*IN_MILLISECONDS
#define TIMER_MALLEABLE_GOO     17*IN_MILLISECONDS, 23*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_festergutAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    uint32 CurrBlightStrength;

    boss_festergutAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        CurrBlightStrength(0)
    {
    }

    void Reset()
    {
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            Putricide->MonsterMoveWithSpeed(4356.7f, 3265.5f, 384.4f);
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        Creature *GasTarget = SummonMgr.SummonCreatureAt(m_creature, NPC_VILE_GAS_STALKER);
        if (GasTarget)
        {
            GasTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            GasTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        CurrBlightStrength = 0;
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            Putricide->MonsterMoveWithSpeed(4300.9f, 3192.2f, 389.4f);
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(GAS_SPORE);
        SCHEDULE_EVENT_R(INHALE_BLIGHT);
        SCHEDULE_EVENT_R(VILE_GAS);
        SCHEDULE_EVENT_R(GASTRIC_BLOAT);
        if (m_bIsHeroic)
            SCHEDULE_EVENT_R(MALLEABLE_GOO);
        Events.ScheduleEvent(EVENT_START_GAS_CLOUD, 5000);
        DoScriptText(SAY_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_DEATH1, m_creature);
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
        {
            DoScriptText(SAY_PUTRICIDE_DEATH2, Putricide);
            Putricide->MonsterMoveWithSpeed(4356.7f, 3265.5f, 384.4f);
        }
    }

    void RemoveInnoculate()
    {
        Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Unit *pPlayer = itr->getSource();
            if (!pPlayer)
                continue;
            pPlayer->RemoveAurasByDifficulty(SPELL_INNOCULATED);
        }
    }

    void SpellHitTarget(Unit *pTarget, const SpellEntry *pSpell)
    {
        if (pSpell->Id == SPELL_INHALE_BLIGHT)
        {
            Creature *BlightTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_VILE_GAS_STALKER);
            if (BlightTarget && CurrBlightStrength <= 2)
            {
                BlightTarget->RemoveAurasDueToSpell(BlightSpells[CurrBlightStrength][0]);
                BlightTarget->RemoveAurasDueToSpell(BlightSpells[CurrBlightStrength][1]);
                if (CurrBlightStrength < 2)
                {
                    BlightTarget->CastSpell(m_creature, BlightSpells[CurrBlightStrength+1][0], true);
                    BlightTarget->CastSpell(BlightTarget, BlightSpells[CurrBlightStrength+1][1], true);
                }
                CurrBlightStrength++;
                return;
            }
        }
        else if (pSpell->SpellDifficultyId == 1988)  // Pungent Blight
        {
            Creature *BlightTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_VILE_GAS_STALKER);
            if (BlightTarget && CurrBlightStrength == 3)
            {
                BlightTarget->CastSpell(m_creature, BlightSpells[0][0], true);
                BlightTarget->CastSpell(BlightTarget, BlightSpells[0][1], true);
                m_creature->RemoveAurasByDifficulty(SPELL_INHALED_BLIGHT);
                CurrBlightStrength = 0;
                RemoveInnoculate();
                DoScriptText(SAY_PUNGENT_BLIGHT2, m_creature);
            }
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BERSERK, m_creature);
                    break;
                case EVENT_GAS_SPORE:
                {
                    Unit *Target = NULL;
                    Unit *LastTarget = NULL;
                    for (int i = 0; i < (m_bIs10Man ? 2 : 3); i++)
                    {
                        do // prevent selection of same target for gas spore
                        {
                            Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                            if (m_creature->getAttackers().size() <= (m_bIs10Man ? 2 : 3))
                                break;
                        }
                        while (Target == LastTarget);
                        if (Target)
                            DoCast(Target, SPELL_GAS_SPORE, true);
                        LastTarget = Target;
                    }
                    DoScriptText(SAY_GAS_SPORE, m_creature);
                    break;
                }
                case EVENT_INHALE_BLIGHT:
                    if (CurrBlightStrength >= 3)
                    {
                        DoCast(m_creature, SPELL_PUNGENT_BLIGHT);
                        DoScriptText(SAY_PUNGENT_BLIGHT1, m_creature);
                    }
                    else
                        DoCast(m_creature, SPELL_INHALE_BLIGHT);
                    break;
                case EVENT_VILE_GAS:
                {
                    Unit *Target = GetPlayerAtMinimumRange(10);
                    if (!Target)
                        Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    if (Target)
                        DoCast(Target, SPELL_VILE_GAS);
                    break;
                }
                case EVENT_GASTRIC_BLOAT:
                {
                    Aura *PlayerAura;
                    if (PlayerAura = m_creature->getVictim()->GetAuraByDifficulty(SPELL_GASTRIC_BLOAT, EFFECT_INDEX_1))
                        if (PlayerAura->GetStackAmount() >= 9)
                            m_creature->getVictim()->CastSpell(m_creature->getVictim(), SPELL_GASTRIC_EXPLOSION, true);
                    DoCast(m_creature->getVictim(), SPELL_GASTRIC_BLOAT);
                    break;
                }
                case EVENT_MALLEABLE_GOO:
                    if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
                        if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            Putricide->CastSpell(target, SPELL_MALLEABLE_GOO, false);
                    break;
                case EVENT_START_GAS_CLOUD:
                    if (Creature *GasTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_VILE_GAS_STALKER))
                    {
                        GasTarget->CastSpell(m_creature, SPELL_GASEOUS_BLIGHT_LEVEL0, true);
                        GasTarget->CastSpell(GasTarget, SPELL_GASEOUS_BLIGHT_VISUAL0, true);
                    }
                    if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
                        DoScriptText(SAY_PUTRICIDE_GAS_CLOUD, Putricide);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_festergut()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_festergut);
}
