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
SDName: boss_festergut
SD%Complete: 90%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                   = 47008,
    SPELL_GAS_SPORE                 = 69278,
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
    SPELL_DECIMATE                  = 71123,
    SPELL_MORTAL_WOUND              = 71127,
    SPELL_PLAGUE_STENCH             = 71805,
};

static const uint32 BlightSpells[3][2] = 
{
    {SPELL_GASEOUS_BLIGHT_LEVEL0, SPELL_GASEOUS_BLIGHT_VISUAL0},
    {SPELL_GASEOUS_BLIGHT_LEVEL1, SPELL_GASEOUS_BLIGHT_VISUAL1},
    {SPELL_GASEOUS_BLIGHT_LEVEL2, SPELL_GASEOUS_BLIGHT_VISUAL2}
};

enum NPCS
{
    NPC_VILE_GAS_STALKER    = 38548, // dummy npc - used for blight target (correct entry seems to be 36659)
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

    EVENT_DECIMATE,
    EVENT_MORTAL_WOUND,
};

#define TIMER_BERSERK           5*MINUTE*IN_MILLISECONDS
#define TIMER_GAS_SPORE         35*IN_MILLISECONDS
#define TIMER_VILE_GAS          20*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_INHALE_BLIGHT     30*IN_MILLISECONDS, 35*IN_MILLISECONDS
#define TIMER_GASTRIC_BLOAT     10*IN_MILLISECONDS, 14*IN_MILLISECONDS
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
        CurrBlightStrength = 0;
        SummonMgr.UnsummonAll();
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            SendScriptMessageTo(Putricide, m_creature, MESSAGE_PUTRICIDE, 0);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_FESTERGUT_DOOR))
            Door->SetGoState(GO_STATE_ACTIVE);
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        if (InstanceProgressionCheck())
            return;
        if (Creature *GasTarget = SummonMgr.SummonCreatureAt(m_creature, NPC_VILE_GAS_STALKER))
            GasTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        CurrBlightStrength = 0;
        if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
            SendScriptMessageTo(Putricide, m_creature, MESSAGE_PUTRICIDE, 1);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_FESTERGUT_DOOR))
            Door->SetGoState(GO_STATE_READY);
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(GAS_SPORE);
        SCHEDULE_EVENT_R(INHALE_BLIGHT);
        SCHEDULE_EVENT_R(VILE_GAS);
        SCHEDULE_EVENT_R(GASTRIC_BLOAT);
        if (m_bIsHeroic)
            SCHEDULE_EVENT_R(MALLEABLE_GOO);
        Events.ScheduleEvent(EVENT_START_GAS_CLOUD, 5*IN_MILLISECONDS);
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
            SendScriptMessageTo(Putricide, m_creature, MESSAGE_PUTRICIDE, 0);
        }
    }

    void SpellHit(Unit* pDoneBy, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_INHALE_BLIGHT)
        {
            Creature *BlightTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_VILE_GAS_STALKER);
            if (BlightTarget && CurrBlightStrength <= 2)
            {
                BlightTarget->RemoveAurasDueToSpell(BlightSpells[CurrBlightStrength][0]);
                BlightTarget->RemoveAurasDueToSpell(BlightSpells[CurrBlightStrength][1]);
                CurrBlightStrength++;
                if (CurrBlightStrength <= 2)
                {
                    BlightTarget->CastSpell(m_creature, BlightSpells[CurrBlightStrength][0], true);
                    BlightTarget->CastSpell(BlightTarget, BlightSpells[CurrBlightStrength][1], true);
                }
                return;
            }
        }
        else if (pSpell->SpellDifficultyId == 1988)  // Pungent Blight
        {
            Creature *BlightTarget = SummonMgr.GetFirstFoundSummonWithId(NPC_VILE_GAS_STALKER);
            if (BlightTarget && CurrBlightStrength >= 3)
            {
                BlightTarget->CastSpell(m_creature, BlightSpells[0][0], true);
                BlightTarget->CastSpell(BlightTarget, BlightSpells[0][1], true);
                m_creature->RemoveAurasByDifficulty(SPELL_INHALED_BLIGHT);
                CurrBlightStrength = 0;
                DoScriptText(SAY_PUNGENT_BLIGHT2, m_creature);
            }
        }
    }

    void SpellHitTarget(Unit* pVictim, SpellEntry const* pSpell)
    {
        // Pungent Blight - remove Inoculate
        if (pVictim && pSpell->SpellDifficultyId == 1988)
            pVictim->RemoveAurasByDifficulty(SPELL_INNOCULATED);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        if (m_creature->getVictim()->GetEntry() == NPC_PUTRICIDE)
        {
            EnterEvadeMode();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BERSERK, m_creature);
                    break;
                case EVENT_GAS_SPORE:
                    DoCast(m_creature, SPELL_GAS_SPORE, true);
                    DoScriptText(SAY_GAS_SPORE, m_creature);
                    break;
                case EVENT_INHALE_BLIGHT:
                    if (CurrBlightStrength >= 3)
                    {
                        DoCast(m_creature, SPELL_PUNGENT_BLIGHT);
                        DoScriptText(SAY_PUNGENT_BLIGHT1, m_creature);
                        Events.SetCooldown(3000);
                        Events.DelayEventsWithId(EVENT_VILE_GAS, 10*IN_MILLISECONDS);
                    }
                    else
                    {
                        DoCast(m_creature, SPELL_INHALE_BLIGHT);
                        Events.SetCooldown(3500);
                    }
                    break;
                case EVENT_VILE_GAS:
                {
                    Map* pMap = m_creature->GetMap();

                    Unit* Target = NULL;
                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();
                        std::vector<Unit*> ranged_players;

                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            Unit* pPlayer = i->getSource();
                            if (pPlayer->GetTypeId() != TYPEID_PLAYER)
                                continue;

                            if (pPlayer->GetDistance2d(m_creature) > 10.0f)
                                ranged_players.push_back(pPlayer);
                        }
                        // check amount of players on melee / range
                        // if there is enough ranged players, the vile gas will be casted 
                        // on range only, otherwise it will hit randomly any member (including melee)
                        if (ranged_players.size() < (m_bIs10Man ? 3 : 8))
                            Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                        else
                        {
                            std::random_shuffle(ranged_players.begin(), ranged_players.end());
                            Target = ranged_players[0];
                        }
                    }
                    if (!Target)
                        Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    if (Target)
                        DoCast(Target, SPELL_VILE_GAS);
                    break;
                }
                case EVENT_GASTRIC_BLOAT:
                {
                    Unit *target = m_creature->getVictim();
                    DoCast(target, SPELL_GASTRIC_BLOAT);
                    if (Aura *PlayerAura = target->GetAuraByDifficulty(SPELL_GASTRIC_BLOAT, EFFECT_INDEX_1))
                        if (PlayerAura->GetStackAmount() >= 10)
                            target->CastSpell(target, SPELL_GASTRIC_EXPLOSION, true);
                    break;
                }
                case EVENT_MALLEABLE_GOO:
                    if (Creature *Putricide = GET_CREATURE(TYPE_PUTRICIDE))
                        if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
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

struct MANGOS_DLL_DECL mob_stinky_ICCAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    EventManager Events;

    mob_stinky_ICCAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(m_creature->GetInstanceData()))
    {
        Reset();
    }
    
    void Reset()
    {
        if (!m_creature->HasAura(SPELL_PLAGUE_STENCH))
            DoCast(m_creature, SPELL_PLAGUE_STENCH, true);

        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_DECIMATE,     10*IN_MILLISECONDS, 20*IN_MILLISECONDS, 20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_MORTAL_WOUND, 10*IN_MILLISECONDS, 15*IN_MILLISECONDS, 20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_DECIMATE:
                    DoCast(m_creature->getVictim(), SPELL_DECIMATE, false);
                    break;
                case EVENT_MORTAL_WOUND:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_WOUND);
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pFestergut = GET_CREATURE(TYPE_FESTERGUT))
        {
            m_creature->PlayDirectSound(16907); // if festergut is too far away, we play the sound
            DoScriptText(SAY_STINKY_DEATH, pFestergut);
        }
    }

};

void AddSC_boss_festergut()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_festergut);
    REGISTER_SCRIPT(mob_stinky_ICC);
}
