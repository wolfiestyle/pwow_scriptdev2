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
SD%Complete: 75%
SDComment: Attacks players after "dead", causes some random teleport (add spell?)
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

    SPELL_INTRO_CHOKE               = 72305, // NOT actual spell. I cannot find the real one though.
    SPELL_ACHIEVEMENT               = 72928, // Saurfang kill credit
};

enum Npcs
{
    // intro NPCs
    NPC_HIGH_OVERLORD_SAURFANG      = 37187,
    NPC_KORKRON_REAVER              = 37920,
    NPC_MURADIN_BRONZEBEARD         = 37200,
    NPC_SKYBREAKER_MARINE           = 37830,
    NPC_VARIAN_WYRM                 = 37879,
    NPC_JAINA_PROUDMOORE            = 37188,

    // adds
    NPC_BLOOD_BEAST                 = 38508,

    NPC_INTRO_OUTRO_CONTROLLER      = 37948,    // name is 'deathwhisper controller'. Not used in Lady Deathwhisper's script, so it's reused here.
};

enum Says
{
    // intro
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

    // Deathbringer Saurfang
    SAY_AGGRO                       = -1300444,
    SAY_MARK_OF_THE_FALLEN_CHAMPION = -1300445,
    SAY_SUMMON_ADDS                 = -1300446,
    SAY_KILLED_PLAYER1              = -1300447,
    SAY_KILLED_PLAYER2              = -1300448,
    SAY_BERSERK                     = -1300449,
    SAY_DEATH                       = -1300450,

    // outro
    EMOTE_MURADIN_ALLIANCE_OUTRO1   = -1300451,
    SAY_MURADIN_ALLIANCE_OUTRO2     = -1300452,
    SAY_MURADIN_ALLIANCE_OUTRO3     = -1300453,
    SAY_MURADIN_ALLIANCE_OUTRO4     = -1300454,
    SAY_MURADIN_ALLIANCE_OUTRO5     = -1300455,
    SAY_SAURFANG_ALLIANCE_OUTRO6    = -1300456,
    SAY_MURADIN_ALLIANCE_OUTRO7     = -1300457,
    SAY_VARIAN_ALLIANCE_OUTRO8      = -1300458,
    SAY_MURADIN_ALLIANCE_OUTRO9     = -1300459,
    SAY_SAURFANG_ALLIANCE_OUTRO10   = -1300460,
    SAY_SAURFANG_ALLIANCE_OUTRO11   = -1300461,
    SAY_VARIAN_ALLIANCE_OUTRO12     = -1300462,
    EMOTE_JANIA_ALLIANCE_OUTRO13    = -1300463,
    SAY_VARIAN_ALLIANCE_OUTRO14     = -1300464,
    SAY_JANIA_ALLIANCE_OUTRO15      = -1300465,
    SAY_VARIAN_ALLIANCE_OUTRO16     = -1300466,
    SAY_MURADIN_ALLIANCE_OUTRO17    = -1300467,

    EMOTE_SAURFANG_HORDE_OUTRO1     = -1300468,
    EMOTE_SAURFANG_HORDE_OUTRO2     = -1300469,
    SAY_SAURFANG_HORDE_OUTRO3       = -1300470,
    SAY_SAURFANG_HORDE_OUTRO4       = -1300471,
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

enum Messages
{
    MESSAGE_START_INTRO = 1,
    MESSAGE_START_OUTRO,
    MESSAGE_END_OUTRO,
    MESSAGE_END_INTRO,
};

static const float IntroSummonPosition[2] = {-563.5f, 2211.6f};

#define FLOOR_HEIGHT            539.3f

#define TIMER_BERSERK           8*MINUTE*IN_MILLISECONDS
#define TIMER_BOILING_BLOOD     15*IN_MILLISECONDS
#define TIMER_RUNE_OF_BLOOD     25*IN_MILLISECONDS
#define TIMER_SUMMON_ADDS       35*IN_MILLISECONDS
#define TIMER_BLOOD_NOVA        22*IN_MILLISECONDS
#define TIMER_BUFF_ADDS         5*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_deathbringer_saurfangAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    bool IsSoftEnraged :1;
    bool HasDoneIntro :1;
    bool IsDoingIntro :1;
    bool IsHorde :1;

    boss_deathbringer_saurfangAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        IsSoftEnraged(false),
        HasDoneIntro(false),
        IsDoingIntro(false),
        IsHorde(true)
    {
        m_creature->setPowerType(POWER_ENERGY);
        m_creature->SetMaxPower(POWER_ENERGY, 100);
    }

    void Reset()
    {
        if (m_BossEncounter == DONE)
            return;
        IsSoftEnraged = false;
        SummonMgr.UnsummonAllWithId(NPC_BLOOD_BEAST);
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

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && !HasDoneIntro)
        {
            if (Creature *Controller = SummonMgr.SummonCreatureAt(m_creature, NPC_INTRO_OUTRO_CONTROLLER))
            {
                Controller->SetOwnerGUID(m_creature->GetGUID());
                Controller->SetVisibility(VISIBILITY_OFF);
            }
            BroadcastScriptMessageToEntry(m_creature, NPC_INTRO_OUTRO_CONTROLLER, 5.0f, MESSAGE_START_INTRO);
            HasDoneIntro = true;
            IsDoingIntro = true;
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        else if (!IsDoingIntro)
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (IsDoingIntro)
            return;
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(SUMMON_ADDS);
        SCHEDULE_EVENT(BOILING_BLOOD);
        SCHEDULE_EVENT(RUNE_OF_BLOOD);
        SCHEDULE_EVENT(BLOOD_NOVA);
        DoScriptText(SAY_AGGRO, m_creature);
        DoCast(m_creature, SPELL_BLOOD_LINK, true);
        DoCast(m_creature, SPELL_RUNE_OF_BLOOD_AURA, true);
        DoCast(m_creature, SPELL_MARK_FALLEN_CHAMPION_SELF, true);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        DoScriptText(urand(0, 1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
        if (pWho && pWho->HasAura(SPELL_MARK_FALLEN_CHAMPION))
            DoCast(m_creature, SPELL_MARK_FALLEN_CHAMPION_HEAL, true);
    }

    void JustDied(Unit* pKiller)
    {
        Events.Reset();
        SummonMgr.UnsummonAllWithId(NPC_BLOOD_BEAST);
        DoScriptText(SAY_DEATH, m_creature);
        RemoveAuras();
        m_BossEncounter = DONE;
        DoCast(m_creature, SPELL_ACHIEVEMENT, true);
        // start outro
        BroadcastScriptMessageToEntry(m_creature, NPC_INTRO_OUTRO_CONTROLLER, 200.0f, MESSAGE_START_OUTRO);
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

    void ScriptMessage(WorldObject *pSender, uint32 data1, uint32 data2)
    {
        Creature *pCreatureSender = dynamic_cast<Creature*>(pSender);
        if (pCreatureSender && pCreatureSender->GetEntry() == NPC_INTRO_OUTRO_CONTROLLER)
        {
            switch (data1)
            {
                case MESSAGE_END_INTRO:
                    IsDoingIntro = false;
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    break;
                case MESSAGE_END_OUTRO:
                    SummonMgr.UnsummonCreature(pCreatureSender);
                    break;
            }
            return;
        }
        ScriptEventInterface::ScriptMessage(pSender, data1, data2);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (IsDoingIntro || !m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        if (m_creature->GetPower(POWER_ENERGY) == 100)
        {
            Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 2);
            if (!pTarget)
                pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_MARK_FALLEN_CHAMPION);
            DoScriptText(SAY_MARK_OF_THE_FALLEN_CHAMPION, m_creature);
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
                        for (int i = 2; i < 5; i++)
                            SummonMgr.SummonCreatureAt(m_creature, NPC_BLOOD_BEAST, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000, Positions[i][0], Positions[i][1]);

                    if (m_bIsHeroic)
                        Events.ScheduleEvent(EVENT_BUFF_ADDS, TIMER_BUFF_ADDS);
                    break;
                }
                case EVENT_BOILING_BLOOD:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_BOILING_BLOOD);
                    break;
                case EVENT_RUNE_OF_BLOOD:
                    DoCast(m_creature->getVictim(), SPELL_RUNE_OF_BLOOD);
                    break;
                case EVENT_BLOOD_NOVA:
                {
                    Unit *pTarget = GetPlayerAtMinimumRange(10.0f);
                    if (!pTarget)
                        pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0);
                    if (pTarget)
                        DoCast(pTarget, SPELL_BLOOD_NOVA); //targets ranged targets. Problem: the visual for this spell does not trigger the actual spell.
                    break;
                }
                case EVENT_BUFF_ADDS:
                {
                    std::list<Creature*> Adds;
                    SummonMgr.GetAllSummonsWithId(Adds, NPC_BLOOD_BEAST);
                    for (std::list<Creature*>::const_iterator i = Adds.begin(); i!= Adds.end(); ++i)
                        (*i)->CastSpell(*i, SPELL_SCENT_OF_BLOOD, false);
                    break;
                }
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_saurfang_intro_outro_controllerAI: public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance *m_pInstance;
    SummonManager SummonMgr;
    uint32 TalkTimer;
    uint32 TalkPhase;
    bool IsHorde;

    mob_saurfang_intro_outro_controllerAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        SummonMgr(pCreature),
        TalkTimer(0),
        TalkPhase(0),
        IsHorde(false)
    {
    }

    void Reset() {}

    void DoIntroCharge(Creature *Leader, uint32 FollowerId)   
    {
        Leader->MonsterMove(m_creature->GetPositionX()-10, Leader->GetPositionY(), FLOOR_HEIGHT+5, 1000);
        std::list<Creature*> Followers;
        SummonMgr.GetAllSummonsWithId(Followers, FollowerId);
        for (std::list<Creature*>::const_iterator i = Followers.begin(); i != Followers.end(); ++i)
            (*i)->MonsterMove(m_creature->GetPositionX()-10, (*i)->GetPositionY(), FLOOR_HEIGHT+5, 1000);
    }

    
    void ScriptMessage(WorldObject *pSender, uint32 data1, uint32 data2)
    {
        Unit *BossSaurfang = m_creature->GetOwner();
        if (BossSaurfang && pSender == BossSaurfang)
            switch (data1)
            {
                case MESSAGE_START_INTRO:
                {
                    IsHorde = IS_HORDE;
                    TalkPhase = 1;
                    uint32 LeaderId;
                    uint32 FollowerId;
                    if (IsHorde)
                    {
                        LeaderId = NPC_HIGH_OVERLORD_SAURFANG;
                        FollowerId = NPC_KORKRON_REAVER;
                        TalkTimer = 6*IN_MILLISECONDS;
                    }
                    else
                    {
                        LeaderId = NPC_MURADIN_BRONZEBEARD;
                        FollowerId = NPC_SKYBREAKER_MARINE;
                        TalkTimer = 18*IN_MILLISECONDS;
                    }
                    Creature *Leader = SummonMgr.SummonCreature(LeaderId, IntroSummonPosition[0], IntroSummonPosition[1], FLOOR_HEIGHT);
                    Unit *SaurfangBoss = m_creature->GetOwner();
                    if (Leader && SaurfangBoss)
                    {
                        Leader->StopAttackFaction(BossSaurfang->getFaction());
                        Leader->MonsterMove(-544.1f, 2211.2f, 539.2f, 2000);
                        if (IsHorde)
                            DoScriptText(SAY_SAURFANG_HORDE_INTRO1, Leader);
                        else
                            DoScriptText(SAY_DEATHFANG_ALLIANCE_INTRO1, BossSaurfang);
                        Leader->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    SummonMgr.SummonCreatures(FollowerId, IntroSummonPosition[0], IntroSummonPosition[1], FLOOR_HEIGHT, 4);
                    std::vector<Creature*> Followers;
                    Followers.reserve(4);
                    SummonMgr.GetAllSummonsWithId(Followers, FollowerId);
                    for (size_t i = 0; i < Followers.size(); i++)
                    {
                        Followers[i]->StopAttackFaction(BossSaurfang->getFaction());
                        Followers[i]->MonsterMove(-537.0f, 2202 + 5*i, FLOOR_HEIGHT, 2000);
                        Followers[i]->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
                    break;
                }
                case MESSAGE_START_OUTRO:
                    TalkPhase = 9;
                    break;
                default:
                    break;
            }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (TalkPhase)
        {
            if (TalkTimer < uiDiff)
            {
                Creature *HighSaurfang = SummonMgr.GetFirstFoundSummonWithId(NPC_HIGH_OVERLORD_SAURFANG);
                Unit *BossSaurfang = m_creature->GetOwner();
                TalkPhase++;
                if (IsHorde)
                {
                    switch (TalkPhase)
                    {
                        // Intro
                        case 2:
                            if (BossSaurfang)
                                DoScriptText(SAY_DEATHFANG_HORDE_INTRO2, BossSaurfang);
                            TalkTimer = 14*IN_MILLISECONDS;
                            break;
                        case 3:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_INTRO3, HighSaurfang);
                            TalkTimer = 7*IN_MILLISECONDS;
                            break;
                        case 4:
                            if (BossSaurfang)
                                DoScriptText(SAY_DEATHFANG_HORDE_INTRO4, BossSaurfang);
                            TalkTimer = 10*IN_MILLISECONDS;
                            break;
                        case 5:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_INTRO5, HighSaurfang);
                            TalkTimer = 15*IN_MILLISECONDS;
                            break;
                        case 6:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_INTRO6, HighSaurfang);
                            TalkTimer = 14*IN_MILLISECONDS;
                            break;
                        case 7:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_INTRO7, HighSaurfang);
                            TalkTimer = 4*IN_MILLISECONDS;
                            break;
                        case 8:
                            if (HighSaurfang)
                            {
                                DoScriptText(EMOTE_SAURFANG_HORDE_INTRO8, HighSaurfang);
                                DoIntroCharge(HighSaurfang, NPC_KORKRON_REAVER);
                            }
                            TalkTimer = 1000;
                            break;
                        case 9:
                            if (HighSaurfang)
                            {
                                HighSaurfang->CastSpell(HighSaurfang, SPELL_INTRO_CHOKE, true);
                                HighSaurfang->StopMoving();
                                std::list<Creature*> Korkron; 
                                SummonMgr.GetAllSummonsWithId(Korkron, NPC_KORKRON_REAVER);
                                for (std::list<Creature*>::const_iterator i = Korkron.begin(); i!= Korkron.end(); ++i)
                                {
                                    (*i)->CastSpell(*i, SPELL_INTRO_CHOKE, true);
                                    (*i)->StopMoving();
                                }
                            }
                            if (BossSaurfang)
                            {
                                BossSaurfang->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                DoScriptText(SAY_DEATHFANG_HORDE_INTRO9, BossSaurfang);
                            }
                            TalkPhase = 0;
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 5.0f, MESSAGE_END_INTRO);
                            break;
                        // Outro
                        case 10:
                            if (HighSaurfang)
                            {
                                HighSaurfang->RemoveAurasDueToSpell(SPELL_INTRO_CHOKE);
                                HighSaurfang->MonsterMove(HighSaurfang->GetPositionX(), HighSaurfang->GetPositionY(), FLOOR_HEIGHT, 100);
                                std::list<Creature*> Korkron; 
                                SummonMgr.GetAllSummonsWithId(Korkron, NPC_KORKRON_REAVER);
                                for (std::list<Creature*>::const_iterator i = Korkron.begin(); i != Korkron.end(); ++i)
                                {
                                    (*i)->RemoveAurasDueToSpell(SPELL_INTRO_CHOKE);
                                    (*i)->MonsterMove((*i)->GetPositionX(), (*i)->GetPositionY(), FLOOR_HEIGHT, 100);
                                }
                                DoScriptText(EMOTE_SAURFANG_HORDE_OUTRO1, HighSaurfang);
                            }
                            TalkTimer = 2*IN_MILLISECONDS;
                            break;
                        case 11:
                            if (HighSaurfang)
                            {
                                if (BossSaurfang)
                                    HighSaurfang->MonsterMoveWithSpeed(BossSaurfang->GetPositionX(), BossSaurfang->GetPositionY(), BossSaurfang->GetPositionZ());
                                DoScriptText(EMOTE_SAURFANG_HORDE_OUTRO2, HighSaurfang);
                            }
                            TalkTimer = 6*IN_MILLISECONDS;
                            break;
                        case 12:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_OUTRO3, HighSaurfang);
                            TalkTimer = 10*IN_MILLISECONDS;
                            break;
                        case 13:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_HORDE_OUTRO4, HighSaurfang);
                            TalkTimer = 7*IN_MILLISECONDS;
                            break;
                        case 14:
                            SummonMgr.UnsummonAll();
                            TalkPhase = 0;
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 200.0f, MESSAGE_END_OUTRO);
                            break;
                        default:
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 5.0f, MESSAGE_END_INTRO);
                            TalkPhase = 0;
                            break;
                    }
                }
                else // Alliance
                {
                    Creature *Muradin = SummonMgr.GetFirstFoundSummonWithId(NPC_MURADIN_BRONZEBEARD);
                    switch (TalkPhase)
                    {
                        // Intro
                        case 2:
                            if (BossSaurfang)
                                DoScriptText(SAY_DEATHFANG_ALLIANCE_INTRO2, BossSaurfang);
                            TalkTimer = 10*IN_MILLISECONDS;
                            break;
                        case 3:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_INTRO3, Muradin);
                            TalkTimer = 5*IN_MILLISECONDS;
                            break;
                        case 4:
                            if (Muradin)
                            {
                                DoScriptText(SAY_MURADIN_ALLIANCE_INTRO4, Muradin);
                                DoIntroCharge(Muradin, NPC_SKYBREAKER_MARINE);
                            }
                            TalkTimer = 1000;
                            break;
                        case 5:
                            if (Muradin)
                            {
                                Muradin->CastSpell(Muradin, SPELL_INTRO_CHOKE, true);
                                Muradin->StopMoving();
                                std::list<Creature*> Marines;
                                SummonMgr.GetAllSummonsWithId(Marines, NPC_SKYBREAKER_MARINE);
                                for (std::list<Creature*>::const_iterator i = Marines.begin(); i != Marines.end(); ++i)
                                {
                                    (*i)->CastSpell(*i, SPELL_INTRO_CHOKE, true);
                                    (*i)->StopMoving();
                                }
                            }
                            TalkTimer = 1*IN_MILLISECONDS;
                            break;
                        case 6:
                            if (BossSaurfang)
                            {
                                DoScriptText(SAY_DEATHFANG_ALLIANCE_INTRO5, BossSaurfang);
                                BossSaurfang->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            }
                            TalkPhase = 0;
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 5.0f, MESSAGE_END_INTRO);
                            break;
                        // Outro
                        case 10:
                            if (Muradin)
                            {
                                Muradin->RemoveAurasDueToSpell(SPELL_INTRO_CHOKE);
                                Muradin->MonsterMove(Muradin->GetPositionX(), Muradin->GetPositionY(), FLOOR_HEIGHT, 100);
                                std::list<Creature*> Marines; 
                                SummonMgr.GetAllSummonsWithId(Marines, NPC_SKYBREAKER_MARINE);
                                for (std::list<Creature*>::const_iterator i = Marines.begin(); i != Marines.end(); ++i)
                                {
                                    (*i)->RemoveAurasDueToSpell(SPELL_INTRO_CHOKE);
                                    (*i)->MonsterMove((*i)->GetPositionX(), (*i)->GetPositionY(), FLOOR_HEIGHT, 100);
                                }
                                DoScriptText(EMOTE_MURADIN_ALLIANCE_OUTRO1, Muradin);
                            }
                            TalkTimer = 1*IN_MILLISECONDS;
                            break;
                        case 11:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO2, Muradin);
                            TalkTimer = 6*IN_MILLISECONDS;
                            break;
                        case 12:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO3, Muradin);
                            TalkTimer = 3*IN_MILLISECONDS;
                            break;
                        case 13:
                            if (Muradin)
                            {
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO4, Muradin);
                                Muradin->MonsterMove(-510.3f, 2231.7f, 539.2f, 3000);
                                std::vector<Creature*> Skybreakers;
                                Skybreakers.reserve(4);
                                SummonMgr.GetAllSummonsWithId(Skybreakers, NPC_SKYBREAKER_MARINE);
                                for (size_t i = 0; i < Skybreakers.size(); i++)
                                    Skybreakers[i]->MonsterMove(-519.0f + 5*i, 2227.0f, FLOOR_HEIGHT, 3000);
                            }
                            TalkTimer = 5*IN_MILLISECONDS;
                            if (Creature *Saurfang = SummonMgr.SummonCreature(NPC_HIGH_OVERLORD_SAURFANG, -518.0f, 2247.0f, FLOOR_HEIGHT, 5.33f))
                                Saurfang->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            break;
                        case 14:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO5, Muradin);
                            if (HighSaurfang)
                                HighSaurfang->MonsterMoveWithSpeed(-511.5f, 2236.2f, FLOOR_HEIGHT);
                            TalkTimer = 6*IN_MILLISECONDS;
                            break;
                        case 15:
                            if (HighSaurfang)
                                DoScriptText(SAY_SAURFANG_ALLIANCE_OUTRO6, HighSaurfang);
                            TalkTimer = 7.5*IN_MILLISECONDS;
                            break;
                        case 16:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO7, Muradin);
                            TalkTimer = 7*IN_MILLISECONDS;
                            break;
                        case 17:
                        {
                            SummonMgr.SummonCreature(NPC_JAINA_PROUDMOORE, -521.0f, 2220.4f, FLOOR_HEIGHT, 0.789f);
                            Creature *Varian = SummonMgr.SummonCreature(NPC_VARIAN_WYRM, -524.0f, 2221.4f, FLOOR_HEIGHT, 0.789f);
                            if (Varian)
                                DoScriptText(SAY_VARIAN_ALLIANCE_OUTRO8, Varian);
                            TalkTimer = 5*IN_MILLISECONDS;
                            break;
                        }
                        case 18:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO9, Muradin);
                            TalkTimer = 2*IN_MILLISECONDS;
                            break;
                        case 19:
                            if (HighSaurfang && BossSaurfang)
                                HighSaurfang->MonsterMove(BossSaurfang->GetPositionX(), BossSaurfang->GetPositionY(), BossSaurfang->GetPositionZ(), 8*IN_MILLISECONDS);
                            TalkTimer = 8*IN_MILLISECONDS;
                            break;
                        case 20:
                            if (HighSaurfang) // needs to pick up deathbringer saurfang, not sure how to do so.
                                DoScriptText(SAY_SAURFANG_ALLIANCE_OUTRO10, HighSaurfang);
                            TalkTimer = 5*IN_MILLISECONDS;
                            break;
                        case 21:
                            if (HighSaurfang)
                                HighSaurfang->MonsterMove(-518.0f, 2247.0f, FLOOR_HEIGHT, 10*IN_MILLISECONDS);
                            TalkTimer = 10*IN_MILLISECONDS;
                            break;
                        case 22:
                            if (HighSaurfang)
                            {
                                HighSaurfang->SetOrientation(4.73f);
                                DoScriptText(SAY_SAURFANG_ALLIANCE_OUTRO11, HighSaurfang);
                            }
                            TalkTimer = 8*IN_MILLISECONDS;
                            break;
                        case 23:
                        {
                            Creature *Varian = SummonMgr.GetFirstFoundSummonWithId(NPC_VARIAN_WYRM);
                            if (Varian)
                                DoScriptText(SAY_VARIAN_ALLIANCE_OUTRO12, Varian);
                            TalkTimer = 17*IN_MILLISECONDS;
                            break;
                        }
                        case 24:
                        {
                            SummonMgr.UnsummonAllWithId(NPC_HIGH_OVERLORD_SAURFANG);
                            Creature *Jaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE);
                            if (Jaina)
                                DoScriptText(EMOTE_JANIA_ALLIANCE_OUTRO13, Jaina);
                            TalkTimer = 4*IN_MILLISECONDS;
                            break;
                        }
                        case 25:
                        {
                            Creature *Varian = SummonMgr.GetFirstFoundSummonWithId(NPC_VARIAN_WYRM);
                            if (Varian)
                                DoScriptText(SAY_VARIAN_ALLIANCE_OUTRO14, Varian);
                            TalkTimer = 3*IN_MILLISECONDS;
                            break;
                        }
                        case 26:
                        {
                            Creature *Jaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE);
                            if (Jaina)
                                DoScriptText(SAY_JANIA_ALLIANCE_OUTRO15, Jaina);
                            TalkTimer = 7*IN_MILLISECONDS;
                            break;
                        }
                        case 27:
                        {
                            Creature *Varian = SummonMgr.GetFirstFoundSummonWithId(NPC_VARIAN_WYRM);
                            if (Varian)
                                DoScriptText(SAY_VARIAN_ALLIANCE_OUTRO16, Varian);
                            TalkTimer = 10*IN_MILLISECONDS;
                            break;
                        }
                        case 28:
                            if (Muradin)
                                DoScriptText(SAY_MURADIN_ALLIANCE_OUTRO17, Muradin);
                            TalkTimer = 3*IN_MILLISECONDS;
                            break;
                        case 29:
                            SummonMgr.UnsummonAll();
                            TalkPhase = 0;
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 200.0f, MESSAGE_END_OUTRO);
                            break;
                        default:
                            BroadcastScriptMessageToEntry(m_creature, NPC_SAURFANG, 5.0f, MESSAGE_END_INTRO);
                            TalkPhase = 0;
                            break;
                    }
                }
            }
            else
                TalkTimer -= uiDiff;
        }
    }
};
void AddSC_boss_deathbringer_saurfang()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_deathbringer_saurfang);
    REGISTER_SCRIPT(mob_saurfang_intro_outro_controller);
}
