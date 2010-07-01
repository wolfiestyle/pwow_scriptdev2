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
SDName: boss_deathbringer_saurfang
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                   = 26662,
    SPELL_MARK_FALLEN_CHAMPION      = 72293,
    SPELL_MARK_FALLEN_CHAMPION_HEAL = 72260,
    SPELL_MARK_FALLEN_CHAMPION_SELF = 72256,
    SPELL_BLOOD_LINK                = 72178,
    SPELL_BLOOD_LINK_ENERGIZE       = 72195,
    SPELL_FRENZY                    = 72737,
    SPELL_BOILING_BLOOD             = 72385,
    SPELL_BLOOD_NOVA                = 72380,
    //SPELL_BLOOD_NOVA_VISUAL       = 73058,
    SPELL_RUNE_OF_BLOOD             = 72410,
    SPELL_RUNE_OF_BLOOD_AURA        = 72408, // required, for some reason, to make rune of blood work.

    SPELL_ADD_BLOOD_LINK            = 72176,
    SPELL_RESISTANT_SKIN            = 72723,
    SPELL_SCENT_OF_BLOOD            = 72769,
};

enum Npcs
{
    NPC_BLOOD_BEAST                 = 38508,
};

enum Says
{
    SAY_DEATHFANG_ALLIANCE_INTRO1   = -1300430,
    SAY_DEATHFANG_ALLIANCE_INTRO2   = -1300431,
    SAY_MURADIN_ALLIANCE_INTRO3     = -1300432,
    SAY_MURADIN_ALLIANCE_INTRO4     = -1300433,
    SAY_DEATHFANG_ALLIANCE_INTRO5   = -1300434,

    SAY_SAURFANG_HORDE_INTRO1       = -1300435,
    SAY_DEATHFANG_HORDE_INTRO2      = -1300436,
    SAY_SAURFANG_HORDE_INTRO3       = -1300437,
    SAY_DEATHFANG_HORDE_INTRO4      = -1300438,
    SAY_SAURFANG_HORDE_INTRO5       = -1300439,
    SAY_SAURFANG_HORDE_INTRO6       = -1300440,
    SAY_SAURFANG_HORDE_INTRO7       = -1300441,
    EMOTE_SAURFANG_HORDE_INTRO8     = -1300442,
    SAY_DEATHFANG_HORDE_INTRO9      = -1300443,

    SAY_AGGRO                       = -1300444,
    SAY_MARK_OF_THE_FALLEN_CHAMPION = -1300445,
    SAY_SUMMON_ADDS                 = -1300446,
    SAY_KILLED_PLAYER1              = -1300447,
    SAY_KILLED_PLAYER2              = -1300448,
    SAY_BERSERK                     = -1300449,
    SAY_DEATH                       = -1300450,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_FRENZY,
    EVENT_BOILING_BLOOD,
    EVENT_RUNE_OF_BLOOD,
    EVENT_SUMMON_ADDS,
    EVENT_BLOOD_NOVA,
    EVENT_BUFF_ADDS,
};

#define TIMER_BERSERK           8*MINUTE*IN_MILLISECONDS
#define TIMER_BOILING_BLOOD     15*IN_MILLISECONDS
#define TIMER_RUNE_OF_BLOOD     25*IN_MILLISECONDS
#define TIMER_SUMMON_ADDS       35*IN_MILLISECONDS
#define TIMER_BLOOD_NOVA        22*IN_MILLISECONDS
#define TIMER_BUFF_ADDS         5*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_deathbringer_saurfangAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    bool IsSoftEnraged;

    boss_deathbringer_saurfangAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        IsSoftEnraged(false)
    {
        m_creature->setPowerType(POWER_ENERGY);
        m_creature->SetMaxPower(POWER_ENERGY, 100);
    }

    void Reset()
    {
        IsSoftEnraged = false;
        SummonMgr.UnsummonAll();
        m_creature->SetPower(POWER_ENERGY, 0);
        RemoveAuras();
        boss_icecrown_citadelAI::Reset();
    }

    void RemoveAuras()
    {
        Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Unit *pPlayer = itr->getSource();
            if (!pPlayer)
                continue;
            pPlayer->RemoveAurasDueToSpell(SPELL_MARK_FALLEN_CHAMPION);
        }
    }

    void Aggro(Unit* pWho)
    {
        RESCHEDULE_EVENT(BERSERK);
        RESCHEDULE_EVENT(SUMMON_ADDS);
        RESCHEDULE_EVENT(BOILING_BLOOD);
        RESCHEDULE_EVENT(RUNE_OF_BLOOD);
        RESCHEDULE_EVENT(BLOOD_NOVA);
        DoScriptText(SAY_AGGRO, m_creature);
        DoCast(m_creature, SPELL_BLOOD_LINK, true);
        DoCast(m_creature, SPELL_RUNE_OF_BLOOD_AURA, true);
        DoCast(m_creature, SPELL_MARK_FALLEN_CHAMPION_SELF, true);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
        if (pWho && pWho->HasAura(SPELL_MARK_FALLEN_CHAMPION))
            DoCast(m_creature, SPELL_MARK_FALLEN_CHAMPION_HEAL, true);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        DoScriptText(SAY_DEATH, m_creature);
        RemoveAuras();
        m_BossEncounter = DONE;
    }

    void SpellHitTarget(Unit *pVictim, const SpellEntry *pSpell)
    {
        if (pSpell->SpellDifficultyId == 2171) // (mark of the fallen champion damage)
            DoCast(m_creature, SPELL_BLOOD_LINK_ENERGIZE, true);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (!pSummon || pSummon->GetEntry() != NPC_BLOOD_BEAST)
            return;
        pSummon->SetOwnerGUID(m_creature->GetGUID());
        pSummon->CastSpell(pSummon, SPELL_RESISTANT_SKIN, true);
        pSummon->CastSpell(pSummon, SPELL_ADD_BLOOD_LINK, true);
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetPower(POWER_ENERGY) == 100)
        {
            Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 2);
            if (!pTarget)
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_MARK_FALLEN_CHAMPION);
            m_creature->SetPower(POWER_ENERGY, 0);
        }

        if (!IsSoftEnraged && m_creature->GetHealthPercent() < 30.0f)
        {
            DoCast(m_creature, SPELL_FRENZY);
            IsSoftEnraged = true;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_SUMMON_ADDS:
                {
                    DoScriptText(SAY_SUMMON_ADDS, m_creature);
                    static const float Positions[5][2] = {{0.0f, -10.0f}, {0.0f, 10.0f}, {10.0f, 0.0f}, {-5.0f, 5.0f}, {-5.0f, -5.0f}};
                    for (int i = 0; i < 2; i++)
                        SummonMgr.SummonCreatureAt(m_creature, NPC_BLOOD_BEAST, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000, Positions[i][0], Positions[i][1]);

                    if (!m_bIs10Man) //pentagon pattern in 25 man
                        for(int i = 2; i < 5; i++)
                            SummonMgr.SummonCreatureAt(m_creature, NPC_BLOOD_BEAST, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000, Positions[i][0], Positions[i][1]);

                    if (m_bIsHeroic)
                        RESCHEDULE_EVENT(BUFF_ADDS);
                    RESCHEDULE_EVENT(SUMMON_ADDS);
                    break;
                }
                case EVENT_BOILING_BLOOD:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_BOILING_BLOOD);
                    RESCHEDULE_EVENT(BOILING_BLOOD);
                    break;
                case EVENT_RUNE_OF_BLOOD:
                    DoCast(m_creature->getVictim(), SPELL_RUNE_OF_BLOOD);
                    RESCHEDULE_EVENT(RUNE_OF_BLOOD);
                    break;
                case EVENT_BLOOD_NOVA:
                {
                    Unit *pTarget = GetPlayerAtMinimumRange(10.0f);
                    if (!pTarget)
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    if (pTarget)
                        DoCast(pTarget, SPELL_BLOOD_NOVA); //targets ranged targets. Problem: the visual for this spell does not trigger the actual spell.
                    RESCHEDULE_EVENT(BLOOD_NOVA);
                    break;
                }
                case EVENT_BUFF_ADDS:
                {
                    std::list<Creature*> Adds;
                    SummonMgr.GetAllSummonsWithId(Adds, NPC_BLOOD_BEAST);
                    for (std::list<Creature*>::iterator i = Adds.begin(); i!= Adds.end(); ++i)
                        (*i)->CastSpell(*i, SPELL_SCENT_OF_BLOOD, false);
                }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_deathbringer_saurfang()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_deathbringer_saurfang);
}
