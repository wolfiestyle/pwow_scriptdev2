/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_the_lich_king
SD%Complete: 80%
SDComment: Various platfrom transformations not working,
           Valkyr Shadowguards not working,
           Frostmourne room looks terrible,
           Ice Spheres don't have a 'beam' to the targeted player,
           players can release after 10% outro,
           creature templates missing for 39190 (Wicked Spirits) - replaced with Vile Spirits.
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                       = 47008,
    SPELL_PLAGUE_AVOIDANCE              = 72846,    // Prevents Lich King from being targeted by necrotic plague
    // Phase 1
    SPELL_SUMMON_SHAMBLING_HORROR       = 70372,
    SPELL_SUMMON_DRUDGE_GHOULS          = 70358,
    SPELL_INFEST                        = 70541,
    SPELL_NECROTIC_PLAGUE               = 70338,
    SPELL_SHADOW_TRAP                   = 73539,
    // Phase 2
    SPELL_SOUL_REAPER                   = 69409,
    SPELL_DEFILE_SUMMON                 = 72762,
    SPELL_DEFILE_AURA                   = 72743,
    SPELL_DEFILE_GROW                   = 74164,
    // Phase 3
    SPELL_HARVEST_SOULS                 = 73654,
    SPELL_HARVEST_SOUL                  = 68980,
    SPELL_HARVEST_SOUL_TELEPORT         = 72546,
    SPELL_HARVESTED_SOUL                = 72679,
    SPELL_HARVEST_SOUL_DUMMY            = 76379,
    SPELL_KILL_FROSTMOURNE_PLAYER       = 72627,
    SPELL_VILE_SPIRITS                  = 70498,
    SPELL_SUMMON_VALKYR                 = 69037,
    // Phase transitions
    SPELL_REMORSELESS_WINTER            = 68981,
    SPELL_SUMMON_ICE_SPHERE             = 69104,
    SPELL_PAIN_AND_SUFFERING            = 72133,
    SPELL_RAGING_SPIRIT                 = 69201,
    SPELL_QUAKE                         = 72262,    // visual
    // Intro
    SPELL_ICE_LOCK                      = 71614,
    // Outro
    SPELL_FURY_OF_FROSTMOURNE           = 72350,
    SPELL_FURY_OF_FROSTMOURNE_EFF       = 72351,    // prevent from "releasing" (needs core implementation)
    SPELL_RAISE_DEAD                    = 71769,
    SPELL_FROSTMOURNE_BROKEN_AURA       = 72405,
    SPELL_FROSTMOURNE_BROKEN_AURA2      = 72398,
    SPELL_BREAK_FROSTMOURNE             = 73017,
    SPELL_BREAK_FROSTMOURNE2            = 72726,
    SPELL_BREAK_ICE_LOCK                = 71797,
    SPELL_MASS_RESSURECTION             = 72429,
    SPELL_MASS_RESSURECTION_EFFECT      = 72423,

    // terenas menethil
    SPELL_LIGHTS_FAVOR                  = 69382,
    SPELL_RESTORE_SOUL                  = 73650,
    // soul warden
    SPELL_SOUL_RIP                      = 69397,
    SPELL_DARK_HUNGER_AURA              = 69383,
    SPELL_DARK_HUNGER_HEAL              = 69384,
    // valkyr
    SPELL_WINGS_OF_THE_DAMNED           = 74352,
    // Ice sphere
    SPELL_ICE_PULSE                     = 69099,    // not working - cannot move while casting spell
    SPELL_ICE_BURST                     = 69108,
    // Shambling horror
    SPELL_SHOCKWAVE                     = 72149,
    SPELL_ENRAGE                        = 72143,
    SPELL_FRENZY                        = 28747,
    SPELL_EMERGE_VISUAL                 = 20568,    // "Ragnaros Emerge" looks good, but is not the one im really looking for...
    // Raging Spirit
    SPELL_SOUL_SHRIEK                   = 69242,
    // Vile Spirit
    SPELL_SPIRIT_BURST                  = 70503,
    // Shadow Trap
    SPELL_SHADOW_TRAP_PRE_AURA          = 73530,
    SPELL_SHADOW_TRAP_AURA              = 73525,
};

enum Npcs
{
    NPC_DEFILE                          = 38757,
    NPC_SHADOW_TRAP                     = 39137,
    NPC_RAGING_SPIRIT                   = 36701,
    NPC_VALKYR_SHADOWGUARD              = 36609,
    NPC_TERENAS_MENETHIL_FROSTMOURNE    = 36823,
    NPC_TERENAS_MENETHIL_OUTRO          = 38579,
    NPC_SPIRIT_WARDEN                   = 36824,
    NPC_VILE_SPIRIT                     = 37799,
    NPC_WICKED_SPIRIT                   = 39190,
    NPC_TIRION_FORDRING                 = 38995,
    NPC_FROSTMOURNE                     = 38584,
    NPC_ICE_SPHERE                      = 36633,
    NPC_SHAMBLING_HORROR                = 37698,
    NPC_DRUDGE_GHOUL                    = 37695,
};

enum Says
{
    SAY_LICHKING_INTRO1                 = -1300545,
    SAY_TIRION_INTRO2                   = -1300546,
    SAY_LICHKING_INTRO3                 = -1300547,
    SAY_TIRION_INTRO4                   = -1300548,
    SAY_LICHKING_INTRO5                 = -1300549,
    SAY_AGGRO                           = -1300550,
    SAY_REMORSELESS_WINTER_START        = -1300551,
    SAY_REMORSELESS_WINTER_END          = -1300552,
    SAY_SUMMON_VALKYR                   = -1300553,
    SAY_HARVEST_SOUL                    = -1300554,
    SAY_TERENAS_FROSTMOURNE_INTRO1      = -1300555,
    SAY_TERENAS_FROSTMOURNE_INTRO2      = -1300556,
    SAY_TERENAS_FROSTMOURNE_INTRO3      = -1300557,
    SAY_PLAYER_ESCAPE_FROSTMOURNE       = -1300558,
    SAY_PLAYER_KILLED_IN_FROSTMOURNE    = -1300559,
    SAY_SPECIAL                         = -1300560,
    SAY_SPECIAL2                        = -1300561,
    SAY_KILLED_PLAYER1                  = -1300562,
    SAY_KILLED_PLAYER2                  = -1300563,
    SAY_BERSERK                         = -1300564,
    SAY_LICHKING_OUTRO1                 = -1300565,
    SAY_LICHKING_OUTRO2                 = -1300566,
    SAY_LICHKING_OUTRO3                 = -1300567,
    SAY_LICHKING_OUTRO4                 = -1300568,
    SAY_LICHKING_OUTRO5                 = -1300569,
    SAY_TIRION_OUTRO6                   = -1300570,
    SAY_LICHKING_OUTRO7                 = -1300571,
    SAY_TIRION_OUTRO8                   = -1300572,
    SAY_TERENAS_OUTRO9                  = -1300573,
    SAY_TERENAS_OUTRO10                 = -1300574,
    SAY_TIRION_OUTRO11                  = -1300575,
    SAY_LICHKING_OUTRO12                = -1300576,
    SAY_LICHKING_OUTRO13                = -1300577,
    SAY_DEFILE                          = -1300578,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_SUMMON_DRUDGE_GHOULS,
    EVENT_SUMMON_SHAMBLING_HORROR,
    EVENT_INFEST,
    EVENT_SHADOW_TRAP,
    EVENT_NECROTIC_PLAGUE,
    EVENT_SOUL_REAPER,
    EVENT_SUMMON_VALKYR,
    EVENT_DEFILE,
    EVENT_HARVEST_SOUL,
    EVENT_HARVEST_SOUL_HEROIC_TELE,
    EVENT_VILE_SPIRITS,
    EVENT_REMORSELESS_WINTER,
    EVENT_PAIN_AND_SUFFERING,
    EVENT_RAGING_SPIRIT,
    EVENT_ICE_SPHERE,
    EVENT_QUAKE,
    EVENT_DROP_EDGES,
    EVENT_REBUILD_PLATFORM,
    // Shambling Horror
    EVENT_ENRAGE,
    EVENT_SHOCKWAVE,
    // Raging Spirit
    EVENT_SOUL_SHRIEK,
    // Terenas
    EVENT_TELEPORT_OUT,
    EVENT_SUMMON_FROSTMOURNE_SPIRITS,
    EVENT_TALK,
};

enum Cooldowns
{
    COOLDOWN_TRANSITION_PHASE = 1,
};

enum Phases
{
    PHASE_ONE = 1,
    PHASE_TRANSITION_ONE,
    PHASE_TWO,
    PHASE_TRANSITION_TWO,
    PHASE_THREE,
    PHASE_FROSTMOURNE,
    PHASE_OUTRO,

    PMASK_PHASE_ONE             = bit_mask<PHASE_ONE>::value,
    PMASK_PHASE_ONE_AND_TWO     = bit_mask<PHASE_ONE, PHASE_TWO>::value,
    PMASK_PHASE_TWO             = bit_mask<PHASE_TWO>::value,
    PMASK_PHASE_TWO_AND_THREE   = bit_mask<PHASE_TWO, PHASE_THREE>::value,
    PMASK_PHASE_THREE           = bit_mask<PHASE_THREE>::value,
    PMASK_PHASE_FROSTMOURNE     = bit_mask<PHASE_FROSTMOURNE>::value,
    PMASK_TRANSITION_PHASE      = bit_mask<PHASE_TRANSITION_ONE, PHASE_TRANSITION_TWO>::value,
};

static const float CenterPosition[3]        = {503.7f, -2124.5f, 1043.8f};
static const float TerenasSummonPosition[3] = {510.7f, -2505.0f, 1060.8f};

#define OUTRO_CINEMATIC                     16

#define TIMER_BERSERK                       15*MINUTE*IN_MILLISECONDS
#define TIMER_SUMMON_DRUDGE_GHOULS          20*IN_MILLISECONDS
#define TIMER_SUMMON_SHAMBLING_HORROR       MINUTE*IN_MILLISECONDS
#define TIMER_INFEST                        22*IN_MILLISECONDS
#define TIMER_SHADOW_TRAP                   16*IN_MILLISECONDS
#define TIMER_NECROTIC_PLAGUE               30*IN_MILLISECONDS
#define TIMER_SOUL_REAPER                   30*IN_MILLISECONDS
#define TIMER_SUMMON_VALKYR                 45*IN_MILLISECONDS
#define TIMER_DEFILE                        32*IN_MILLISECONDS
#define TIMER_HARVEST_SOUL                  75*IN_MILLISECONDS
#define TIMER_HARVEST_SOUL_HEROIC_TELE      6*IN_MILLISECONDS
#define TIMER_VILE_SPIRITS                  30*IN_MILLISECONDS
#define TIMER_REMORSELESS_WINTER            2*IN_MILLISECONDS
#define TIMER_PAIN_AND_SUFFERING            1.5*IN_MILLISECONDS
#define TIMER_RAGING_SPIRIT                 17*IN_MILLISECONDS
#define TIMER_ICE_SPHERE                    7*IN_MILLISECONDS
#define TIMER_QUAKE                         MINUTE*IN_MILLISECONDS
// Shambling Horror
#define TIMER_ENRAGE                        25*IN_MILLISECONDS
#define TIMER_SHOCKWAVE                     20*IN_MILLISECONDS
// Raging Sprit
#define TIMER_SOUL_SHRIEK                   15*IN_MILLISECONDS
// Soul Warden
#define TIMER_SOUL_RIP                      10*IN_MILLISECONDS
// Terenas
#define TIMER_TELEPORT_OUT                  40*IN_MILLISECONDS
#define TIMER_SUMMON_FROSTMOURNE_SPIRITS    20*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_the_lich_kingAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    uint32 NumberOfHarvestsToApply;             // number of SPELL_HARVESTED_SOULs to cast on self upon player exit on heroic
    uint32 TalkTimer;
    uint32 TalkPhase;
    bool HasDoneIntro;

    typedef std::list<ObjectGuid> GuidList;
    GuidList PlayersInFrostmourne;
    GuidList PlayersToInstakill;                // weird things happed when a player dies in a loading screen - delay it until they have arrived (server crash!)

    boss_the_lich_kingAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        NumberOfHarvestsToApply(0),
        TalkTimer(0),
        TalkPhase(0),
        HasDoneIntro(false)
    {
        pCreature->SetPhaseMask(0xFFFF, true); // all phases
    }

    void Reset()
    {
        NumberOfHarvestsToApply = 0;
        TalkTimer = 0;
        TalkPhase = 0;
        HasDoneIntro = false;
        SummonMgr.UnsummonAll();
        RemoveAllFrostmournePlayers(m_creature->isAlive());
        if (GameObject *Platform = GET_GAMEOBJECT(DATA_LICHKING_PLATFORM))
            Platform->Rebuild(m_creature);
        if (GameObject *Precipice = GET_GAMEOBJECT(DATA_LICHKING_PRECIPICE))
            Precipice->Rebuild(m_creature);
        for (uint32 i = 0; i < 4; i++)
            if (GameObject *Spike = GET_GAMEOBJECT(DATA_LICHKING_SPIKE1 + i))
                Spike->SetGoState(GO_STATE_READY);
        if (Creature *Fordring = GetClosestCreatureWithEntry(m_creature, NPC_TIRION_FORDRING, 200.0f))
        {
            Fordring->RemoveAllAuras();
            Fordring->GetMotionMaster()->MoveTargetedHome();
            Fordring->SetPhaseMask(65535, true);
        }
        //m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        boss_icecrown_citadelAI::Reset();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!HasDoneIntro && !TalkPhase && pWho && pWho->GetTypeId() == TYPEID_PLAYER &&
            pWho->IsWithinDist(m_creature, 50.0f) && pWho->isTargetableForAttack() &&
            icc::MeetsRequirementsForBoss(m_pInstance, TYPE_LICH_KING))
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoScriptText(SAY_LICHKING_INTRO1, m_creature);
            m_creature->SetSheath(SHEATH_STATE_MELEE);
            m_creature->SetStandState(EMOTE_STATE_NONE);
            m_creature->MonsterMoveWithSpeed(460.9f, -2124.4f, 1040.9f);
            if (Creature *Fordring = GetClosestCreatureWithEntry(m_creature, NPC_TIRION_FORDRING, 200.0f))
            {
                Fordring->MonsterMoveWithSpeed(490.7f, -2124.4f, 1040.9f);
                Fordring->SetPhaseMask(0xFFFF, true);
            }
            TalkTimer = 14*IN_MILLISECONDS;
            TalkPhase = 1;
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (InstanceProgressionCheck())
            return;
        if (m_creature->GetHealth() != m_creature->GetMaxHealth())  // Aggro() is sometimes called after players leave frostmourne on heroic
            return;

        SetCombatMovement(true);
        DoStartMovement(pWho);
        PlayersToInstakill.clear();
        PlayersInFrostmourne.clear();
        Events.SetPhase(PHASE_ONE);
        m_BossEncounter = IN_PROGRESS;
        DoCast(m_creature, SPELL_PLAGUE_AVOIDANCE, true);
        SCHEDULE_EVENT(BERSERK);
        Events.ScheduleEvent(EVENT_SUMMON_DRUDGE_GHOULS, 10*IN_MILLISECONDS, TIMER_SUMMON_DRUDGE_GHOULS, 0, 0, PMASK_PHASE_ONE);
        Events.RescheduleEvent(EVENT_SUMMON_SHAMBLING_HORROR, 15*IN_MILLISECONDS, TIMER_SUMMON_SHAMBLING_HORROR, 0, 0, PMASK_PHASE_ONE);
        SCHEDULE_EVENT(INFEST, 0, 0, PMASK_PHASE_ONE_AND_TWO);
        if (m_bIsHeroic)
            SCHEDULE_EVENT(SHADOW_TRAP, 0, 0, PMASK_PHASE_ONE);
        SCHEDULE_EVENT(NECROTIC_PLAGUE, 0, 0, PMASK_PHASE_ONE);
        SCHEDULE_EVENT(SOUL_REAPER, 0, 0, PMASK_PHASE_TWO_AND_THREE);
        SCHEDULE_EVENT(SUMMON_VALKYR, 0, 0, PMASK_PHASE_TWO);
        SCHEDULE_EVENT(DEFILE, 0, 0, PMASK_PHASE_TWO_AND_THREE);
        SCHEDULE_EVENT(HARVEST_SOUL, 0, 0, PMASK_PHASE_THREE);
        Events.ScheduleEvent(EVENT_VILE_SPIRITS, TIMER_VILE_SPIRITS/2, TIMER_VILE_SPIRITS, 0, 0, PMASK_PHASE_THREE);
        SCHEDULE_EVENT(PAIN_AND_SUFFERING, 0, COOLDOWN_TRANSITION_PHASE, PMASK_TRANSITION_PHASE);
        SCHEDULE_EVENT(RAGING_SPIRIT, 0, COOLDOWN_TRANSITION_PHASE, PMASK_TRANSITION_PHASE);
        SCHEDULE_EVENT(ICE_SPHERE, 0, COOLDOWN_TRANSITION_PHASE, PMASK_TRANSITION_PHASE);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && Events.GetPhase() != PHASE_OUTRO && roll_chance_i(70))
            DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm)
        {
            switch (pSumm->GetEntry())
            {
                case NPC_DEFILE:
                    pSumm->CastSpell(pSumm, SPELL_DEFILE_AURA, true);
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    pSumm->ForcedDespawn(MINUTE*IN_MILLISECONDS);
                    break;
                case NPC_FROSTMOURNE:
                    pSumm->CastSpell(pSumm, SPELL_FROSTMOURNE_BROKEN_AURA, true);
                    pSumm->CastSpell(pSumm, SPELL_FROSTMOURNE_BROKEN_AURA2, true);
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    break;
                case NPC_VALKYR_SHADOWGUARD:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                       SendScriptMessageTo(pSumm, Target);
                    pSumm->SetInCombatWithZone();
                    break;
                case NPC_TERENAS_MENETHIL_FROSTMOURNE:  // Friendly summons
                case NPC_TERENAS_MENETHIL_OUTRO:
                    break;
                case NPC_SHAMBLING_HORROR:
                case NPC_DRUDGE_GHOUL:
                    pSumm->CastSpell(pSumm, SPELL_EMERGE_VISUAL, false);
                    //no break
                default:
                    pSumm->SetInCombatWithZone();
                    break;
            }
            if (pSumm->GetEntry() != NPC_TERENAS_MENETHIL_FROSTMOURNE// NPCs that are summoned by SummonMgr
                && pSumm->GetEntry() != NPC_SHAMBLING_HORROR
                && pSumm->GetEntry() != NPC_SPIRIT_WARDEN
                && pSumm->GetEntry() != NPC_TERENAS_MENETHIL_OUTRO)
                SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
        }
    }

    void JustDied(Unit* pKiller)
    {
        PlayersToInstakill.clear();
        RemoveAllFrostmournePlayers(false);
        SummonMgr.UnsummonAll();
        Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Player *pPlayer = itr->getSource();
            if (!pPlayer)
                continue;
            pPlayer->SendMovieStart(OUTRO_CINEMATIC);
        }
        m_BossEncounter = DONE;
    }

    void StartTransitionPhase()
    {
        DoStartNoMovement(m_creature->getVictim());
        m_creature->MonsterMove(CenterPosition[0], CenterPosition[1], CenterPosition[2], TIMER_REMORSELESS_WINTER);
        if (Events.GetPhase() != PHASE_TRANSITION_ONE)
            if (GameObject* Platform = GET_GAMEOBJECT(DATA_LICHKING_PLATFORM))
                Platform->DamageTaken(m_creature, 100000); //Rebuild platform
        if (GameObject *Wind = GET_GAMEOBJECT(DATA_LICHKING_FROSTY_WIND))
            Wind->SetGoState(GO_STATE_ACTIVE);
        Events.ScheduleEvent(EVENT_REMORSELESS_WINTER, TIMER_REMORSELESS_WINTER);
        Events.ScheduleEvent(EVENT_QUAKE, TIMER_QUAKE + 4*IN_MILLISECONDS);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveAllAuras(); //any kind of damage will remove remorseless winter (unknown why)
        Events.SetCooldown(5*IN_MILLISECONDS, COOLDOWN_TRANSITION_PHASE);
    }

    void RemoveAllFrostmournePlayers(bool Kill, bool Attack = false)
    {
        for (GuidList::const_iterator i = PlayersInFrostmourne.begin(); i!= PlayersInFrostmourne.end(); ++i)
        {
            if (Unit *UnitInFrostmourne = m_creature->GetMap()->GetUnit(*i))
            {
                DoTeleportPlayer(UnitInFrostmourne, 530.0f, -2124.6f, 1040.8f, 3.142f);
                UnitInFrostmourne->RemoveAurasDueToSpell(SPELL_HARVEST_SOUL_TELEPORT, NULL, Kill ? AURA_REMOVE_BY_EXPIRE : AURA_REMOVE_BY_DISPEL);
                if (Attack && UnitInFrostmourne->isAlive())
                {
                    m_creature->SetInCombatWith(UnitInFrostmourne);
                    m_creature->AddThreat(UnitInFrostmourne);
                }
            }
        }

        PlayersInFrostmourne.clear();
    }

    void SpellHit(Unit *pCaster, const SpellEntry *pSpell)
    {
        if (pCaster)
        {
            switch (pSpell->Id)
            {
                case SPELL_HARVEST_SOUL_DUMMY:  // Teleport into frostmourne occured
                    if (PlayersInFrostmourne.empty())
                        if (Creature *Terenas = SummonMgr.SummonCreature(NPC_TERENAS_MENETHIL_FROSTMOURNE, TerenasSummonPosition[0], TerenasSummonPosition[1], TerenasSummonPosition[2], 0.0f, TEMPSUMMON_DEAD_DESPAWN))  // CHANGE THESE LOCS!!
                        {
                            Terenas->SetOwnerGuid(m_creature->GetObjectGuid());
                            if (m_bIsHeroic)
                            {
                                m_creature->MonsterMove(515.0f, CenterPosition[1], CenterPosition[2], 2000);
                                Events.SetPhase(PHASE_FROSTMOURNE);
                            }
                            else
                                if (Creature *SpritWarden = SummonMgr.SummonCreature(NPC_SPIRIT_WARDEN, TerenasSummonPosition[0] - 10.0f, TerenasSummonPosition[1] - 10.0f, TerenasSummonPosition[2], 0.0f, TEMPSUMMON_DEAD_DESPAWN))
                                {
                                    Terenas->SetHealthPercent(50.0f);
                                    SpritWarden->SetOwnerGuid(m_creature->GetObjectGuid());
                                    SpritWarden->AddThreat(Terenas, 500000.0f);
                                    pCaster->getHostileRefManager().clearReferences();
                                }
                        }
                    PlayersInFrostmourne.push_back(pCaster->GetObjectGuid());
                    break;
                default:
                    break;
            }
            if (pSpell->SpellDifficultyId == 2296 && pCaster->GetTypeId() == TYPEID_PLAYER)    // player supposed to die inside frostmourne
            {
                PlayersToInstakill.push_back(pCaster->GetObjectGuid());   // could be still teleporting - put player on queue for killing (how wonderfully morbid)
                if (!m_bIsHeroic)
                {
                    SummonMgr.UnsummonAllWithId(NPC_TERENAS_MENETHIL_FROSTMOURNE);
                    SummonMgr.UnsummonAllWithId(NPC_SPIRIT_WARDEN);
                    DoScriptText(SAY_PLAYER_KILLED_IN_FROSTMOURNE, m_creature);
                }
                else
                    NumberOfHarvestsToApply++;
            }
        }
    }

    void ScriptMessage(WorldObject *pSender, uint32, uint32)
    {
        switch (pSender->GetEntry())
        {
            case NPC_RAGING_SPIRIT:                    // raging spirit has been summoned
                SummonMgr.AddSummonToList(pSender->GetObjectGuid());
                return;
            case NPC_SPIRIT_WARDEN:                    // spirit warden died
                RemoveAllFrostmournePlayers(false, true);
                SummonMgr.UnsummonAllWithId(NPC_TERENAS_MENETHIL_FROSTMOURNE);
                SummonMgr.UnsummonAllWithId(NPC_SPIRIT_WARDEN);
                DoScriptText(SAY_PLAYER_ESCAPE_FROSTMOURNE, m_creature);
                return;
            case NPC_TERENAS_MENETHIL_FROSTMOURNE:    // terenas died OR restore soul finished casting (heroic).
                if (m_bIsHeroic)                      // finished casting
                {
                    RemoveAllFrostmournePlayers(false, true);
                    SummonMgr.UnsummonAllWithId(NPC_TERENAS_MENETHIL_FROSTMOURNE);
                    SummonMgr.UnsummonAllWithId(NPC_VILE_SPIRIT);
                    Events.SetPhase(PHASE_THREE);
                    m_creature->SetInCombatWithZone();
                    m_creature->InterruptNonMeleeSpells(false);
                    if (NumberOfHarvestsToApply)
                    {
                        m_creature->RemoveAurasByDifficulty(SPELL_HARVESTED_SOUL);
                        for (; NumberOfHarvestsToApply; NumberOfHarvestsToApply--)
                            DoCast(m_creature, SPELL_HARVESTED_SOUL, true);
                    }
                    DoScriptText(SAY_PLAYER_ESCAPE_FROSTMOURNE, m_creature);
                }
                else                                  // he died
                {
                    RemoveAllFrostmournePlayers(true);
                    SummonMgr.UnsummonAllWithId(NPC_TERENAS_MENETHIL_FROSTMOURNE);
                    SummonMgr.UnsummonAllWithId(NPC_SPIRIT_WARDEN);
                    DoScriptText(SAY_PLAYER_KILLED_IN_FROSTMOURNE, m_creature);
                }
                return;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        UpdateTalkPhases(uiDiff);

        // kill any players ready for killing
        for (GuidList::iterator i = PlayersToInstakill.begin(); i!= PlayersToInstakill.end(); )
        {
            if (Unit *UnitToKill = m_creature->GetMap()->GetUnit(*i))
                if (UnitToKill->GetTypeId() == TYPEID_PLAYER)
                {
                    if (!static_cast<Player*>(UnitToKill)->IsBeingTeleported())
                    {
                        UnitToKill->CastSpell(UnitToKill, 72627, true);         // kills player
                        PlayersInFrostmourne.remove(*i);
                        i = PlayersToInstakill.erase(i);
                    }
                    else
                        ++i;
                }
            if (PlayersInFrostmourne.empty() && m_bIsHeroic)               // all players have died, reset
                EnterEvadeMode();
        }

        if (PlayersInFrostmourne.empty() && Events.GetPhase() == PHASE_FROSTMOURNE) // Just in case something strange happens
            Events.SetPhase(PHASE_THREE);

        if (Events.GetPhase() == PHASE_OUTRO || Events.GetPhase() == PHASE_FROSTMOURNE)       // disable AI during Outro, Frostmourne
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        if (Events.GetPhase() == PHASE_ONE && m_creature->GetHealthPercent() < 70.0f)
        {
            SummonMgr.UnsummonAllWithId(NPC_SHADOW_TRAP);
            Events.SetPhase(PHASE_TRANSITION_ONE);
            StartTransitionPhase();
        } 
        else if (Events.GetPhase() == PHASE_TWO && m_creature->GetHealthPercent() < 40.0f)
        {
            Events.SetPhase(PHASE_TRANSITION_TWO);
            StartTransitionPhase();
        }
        else if (Events.GetPhase() == PHASE_THREE && m_creature->GetHealthPercent() < 10.0f)
        {
            Events.SetPhase(PHASE_OUTRO);
            m_creature->CastSpell(m_creature->getVictim(), SPELL_FURY_OF_FROSTMOURNE, false);
            TalkPhase = 9;
            TalkTimer = 2*IN_MILLISECONDS;
            SetCombatMovement(false);
            DoStartNoMovement(m_creature->getVictim());
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
                case EVENT_SUMMON_DRUDGE_GHOULS:
                    DoCast(m_creature, SPELL_SUMMON_DRUDGE_GHOULS);
                    break;
                case EVENT_SUMMON_SHAMBLING_HORROR:
                    //DoCast(m_creature, SPELL_SUMMON_SHAMBLING_HORROR); // TODO: Spell doesnt work correctly, its summoning many overlapped horrors
                    SummonMgr.SummonCreature(NPC_SHAMBLING_HORROR, m_creature->GetPositionX() -5 + rand()%10, m_creature->GetPositionY() -5 + rand()%10, m_creature->GetPositionZ() +1.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6*IN_MILLISECONDS);
                    if (roll_chance_i(20))
                        DoScriptText(urand(0,1) ? SAY_SPECIAL : SAY_SPECIAL2, m_creature);
                    break;
                case EVENT_INFEST:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                        DoCast(Target, SPELL_INFEST);
                    if (roll_chance_i(20))
                        DoScriptText(urand(0,1) ? SAY_SPECIAL : SAY_SPECIAL2, m_creature);
                    break;
                case EVENT_SHADOW_TRAP:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(Target, SPELL_SHADOW_TRAP);
                    if (roll_chance_i(20))
                        DoScriptText(urand(0,1) ? SAY_SPECIAL : SAY_SPECIAL2, m_creature);
                    break;
                case EVENT_NECROTIC_PLAGUE:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                        DoCast(Target, SPELL_NECROTIC_PLAGUE);
                    break;
                case EVENT_SOUL_REAPER:
                    DoCast(m_creature->getVictim(), SPELL_SOUL_REAPER);
                    break;
                case EVENT_SUMMON_VALKYR:
                    DoCast(m_creature, SPELL_SUMMON_VALKYR, false);
                    DoScriptText(SAY_SUMMON_VALKYR, m_creature);
                    break;
                case EVENT_DEFILE:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                        DoCast(Target, SPELL_DEFILE_SUMMON);
                    if (roll_chance_i(20))
                        DoScriptText(urand(0,1) ? SAY_SPECIAL : SAY_SPECIAL2, m_creature);
                    DoScriptText(SAY_DEFILE, m_creature);
                    break;
                case EVENT_HARVEST_SOUL:
                    if (m_bIsHeroic)
                    {
                        Events.ScheduleEvent(EVENT_HARVEST_SOUL_HEROIC_TELE, TIMER_HARVEST_SOUL_HEROIC_TELE);
                        DoCast(m_creature->getVictim(), SPELL_HARVEST_SOULS);
                    }
                    else
                        if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                            DoCast(Target, SPELL_HARVEST_SOUL);
                    DoScriptText(SAY_HARVEST_SOUL, m_creature);
                    break;
                case EVENT_HARVEST_SOUL_HEROIC_TELE:
                {
                    Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        Player *pPlayer = itr->getSource();
                        if (!pPlayer)
                            continue;
                        if (pPlayer->isAlive() && pPlayer->HasAuraByDifficulty(SPELL_HARVEST_SOULS))
                        {
                            pPlayer->CastSpell(pPlayer, SPELL_HARVEST_SOUL_TELEPORT, true);
                            pPlayer->CastSpell(m_creature, SPELL_HARVEST_SOUL_DUMMY, true);
                        }
                    }
                    break;
                }
                case EVENT_VILE_SPIRITS:
                    DoCast(m_creature, SPELL_VILE_SPIRITS);
                    break;
                case EVENT_REMORSELESS_WINTER:
                    m_creature->CastSpell(m_creature, SPELL_REMORSELESS_WINTER, false); // need to force-interrupt any current "cast"
                    DoStartNoMovement(m_creature->getVictim());
                    SetCombatMovement(false);
                    if (Events.GetPhase() == PHASE_TRANSITION_ONE)
                        for (uint32 i = 0; i < 4; i++)
                            if (GameObject *Spike = GET_GAMEOBJECT(DATA_LICHKING_SPIKE1 + i))
                                Spike->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject *Wind = GET_GAMEOBJECT(DATA_LICHKING_FROSTY_WIND))
                        Wind->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject *EdgeWind = GET_GAMEOBJECT(DATA_LICHKING_FROSTY_EDGE))
                        EdgeWind->SetGoState(GO_STATE_READY);
                    if (Events.GetPhase() != PHASE_TRANSITION_ONE)
                        Events.ScheduleEvent(EVENT_REBUILD_PLATFORM, 3*IN_MILLISECONDS);
                    DoScriptText(SAY_REMORSELESS_WINTER_START, m_creature);
                    break;
                case EVENT_REBUILD_PLATFORM:
                    if (GameObject* Platform = GET_GAMEOBJECT(DATA_LICHKING_PLATFORM))
                        Platform->Rebuild(m_creature); // rebuild so we can destroy it again
                    break;
                case EVENT_PAIN_AND_SUFFERING:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        m_creature->SetFacingToObject(Target);
                        DoCast(Target, SPELL_PAIN_AND_SUFFERING);
                    }
                    break;
                case EVENT_RAGING_SPIRIT:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        Target->CastSpell(Target, SPELL_RAGING_SPIRIT, false);
                    break;
                case EVENT_ICE_SPHERE:
                    DoCast(m_creature, SPELL_SUMMON_ICE_SPHERE);
                    break;
                case EVENT_QUAKE:
                    m_creature->CastSpell(m_creature, SPELL_QUAKE,false);
                    Events.SetCooldown(5*IN_MILLISECONDS, COOLDOWN_TRANSITION_PHASE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Events.ScheduleEvent(EVENT_DROP_EDGES, 5*IN_MILLISECONDS);
                    if (GameObject *EdgeWarning = GET_GAMEOBJECT(DATA_LICHKING_EDGE_WARNING))
                        EdgeWarning->SetGoState(GO_STATE_ACTIVE);
                    if (Events.GetPhase() == PHASE_TRANSITION_ONE)
                        Events.SetPhase(PHASE_TWO);
                    else
                    {
                        Events.SetPhase(PHASE_THREE);
                        Events.ScheduleEvent(EVENT_HARVEST_SOUL, 5*IN_MILLISECONDS);
                    }
                    DoScriptText(SAY_REMORSELESS_WINTER_END, m_creature);
                    break;
                case EVENT_DROP_EDGES:
                    DoStartMovement(m_creature->getVictim());
                    SetCombatMovement(true);
                    if (GameObject* Platform = GET_GAMEOBJECT(DATA_LICHKING_PLATFORM))
                        Platform->DamageTaken(m_creature, 100000); // hit it like ya mean it!
                    if (GameObject *Wind = GET_GAMEOBJECT(DATA_LICHKING_FROSTY_WIND))
                        Wind->SetGoState(GO_STATE_READY);
                    if (GameObject *EdgeWind = GET_GAMEOBJECT(DATA_LICHKING_FROSTY_EDGE))
                        EdgeWind->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject *EdgeWarning = GET_GAMEOBJECT(DATA_LICHKING_EDGE_WARNING))
                        EdgeWarning->SetGoState(GO_STATE_READY);
                    break;
                default:
                    break;
            }

        if (Events.GetPhase() != PHASE_TRANSITION_ONE && Events.GetPhase() != PHASE_TRANSITION_TWO)
            DoMeleeAttackIfReady();
    }

    void UpdateTalkPhases(uint32 const uiDiff)
    {
        if (TalkPhase)
            if (TalkTimer < uiDiff)
            {
                TalkPhase++;
                Creature *Fordring = GetClosestCreatureWithEntry(m_creature, NPC_TIRION_FORDRING, 200.0f);
                switch (TalkPhase)
                {
                    case 2:
                        if (Fordring)
                            DoScriptText(SAY_TIRION_INTRO2, Fordring);
                        TalkTimer = 8*IN_MILLISECONDS;
                        break;
                    case 3:
                        DoScriptText(SAY_LICHKING_INTRO3, m_creature);
                        TalkTimer = 8*IN_MILLISECONDS;
                        m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                        break;
                    case 4:
                        m_creature->CastSpell(m_creature, 48348, false); // Emote: Exclamation
                        TalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 5:
                        m_creature->CastSpell(m_creature, 48349, false); // Emote: Point
                        TalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 6:
                        m_creature->CastSpell(m_creature, 48348, false); // Emote: Exclamation
                        TalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 7:
                        if (Fordring)
                        {
                            DoScriptText(SAY_TIRION_INTRO4, Fordring);
                            Fordring->CastSpell(Fordring, 48349, false); // Emote: Point
                        }
                        TalkTimer = 2500;
                        break;
                    case 8:
                        if (Fordring)
                            Fordring->MonsterMoveWithSpeed(476.7f, -2124.4f, 1040.9f, 1*IN_MILLISECONDS);
                        if (Fordring)
                            DoCast(Fordring, SPELL_ICE_LOCK);
                        DoScriptText(SAY_LICHKING_INTRO5, m_creature);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->SetInCombatWithZone();
                        TalkTimer = 19*IN_MILLISECONDS;
                        break;
                    case 9:
                        DoScriptText(SAY_AGGRO, m_creature);
                        HasDoneIntro = true;
                        TalkPhase = 0;
                        break;
                    case 11:    // Outro
                        SummonMgr.UnsummonAll();
                        DoScriptText(SAY_LICHKING_OUTRO1, m_creature);
                        TalkTimer = 26*IN_MILLISECONDS;
                        break;
                    case 12:
                        DoScriptText(SAY_LICHKING_OUTRO2, m_creature);
                        TalkTimer = 8*IN_MILLISECONDS;
                        break;
                    case 13:
                        DoScriptText(SAY_LICHKING_OUTRO3, m_creature);
                        TalkTimer = 23*IN_MILLISECONDS;
                        break;
                    case 14:
                        m_creature->MonsterMove(CenterPosition[0], CenterPosition[1], CenterPosition[2], 1*IN_MILLISECONDS);
                        TalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 15:
                        m_creature->MonsterMove(CenterPosition[0]-0.1f, CenterPosition[1], CenterPosition[2], 1*IN_MILLISECONDS);
                        TalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 16:
                        DoCast(m_creature, SPELL_RAISE_DEAD);
                        DoScriptText(SAY_LICHKING_OUTRO4, m_creature);
                        TalkTimer = 26*IN_MILLISECONDS;
                        break;
                    case 17:
                        DoScriptText(SAY_LICHKING_OUTRO5, m_creature);
                        TalkTimer = 6*IN_MILLISECONDS;
                        break;
                    case 18:
                        if (Fordring)
                            DoScriptText(SAY_TIRION_OUTRO6, Fordring);
                        TalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 19:
                        if (Fordring)
                            Fordring->CastSpell(Fordring, SPELL_BREAK_ICE_LOCK, false);
                        TalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 20:
                        Fordring->RemoveAllAuras();
                        Fordring->MonsterMove(529.4f, -2124.3f, 1040.8f, 4*IN_MILLISECONDS);
                        Fordring->HandleEmote(375);
                        TalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 21:
                        m_creature->InterruptNonMeleeSpells(true);
                        DoCast(m_creature, SPELL_BREAK_FROSTMOURNE, true);
                        if (Fordring)
                            Fordring->CastSpell(Fordring, SPELL_BREAK_FROSTMOURNE2, true);
                        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_UNEQUIP);
                        //m_creature->HandleEmote();
                        TalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 22:
                        DoScriptText(SAY_LICHKING_OUTRO7, m_creature);
                        TalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 23:
                        if (Creature *Frostmourne = SummonMgr.GetFirstFoundSummonWithId(NPC_FROSTMOURNE))
                        {
                            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            //m_creature->SetStandState(EMOTE_STATE_STRANGULATE);   //FIXME: what this suppoed to do?
                            m_creature->MonsterMove(Frostmourne->GetPositionX(), Frostmourne->GetPositionY(), Frostmourne->GetPositionZ()+3.0f, 100);
                            m_creature->HandleEmote(EMOTE_STATE_STRANGULATE);
                        }
                        TalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 24:
                        if (Fordring)
                        {
                            Fordring->SetFacingToObject(m_creature);
                            DoScriptText(SAY_TIRION_OUTRO8, Fordring);
                        }
                        TalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 25:
                        if (Creature *Terenas = SummonMgr.SummonCreature(NPC_TERENAS_MENETHIL_OUTRO, 518.0f, -2124.3f, m_creature->GetPositionZ()-3.0f, M_PI_F))
                            DoScriptText(SAY_TERENAS_OUTRO9, Terenas);
                        TalkTimer = 10*IN_MILLISECONDS;
                        break;
                    case 26:
                        if (Creature *Terenas = SummonMgr.GetFirstFoundSummonWithId(NPC_TERENAS_MENETHIL_OUTRO))
                        {
                            Terenas->CastSpell(Terenas, SPELL_MASS_RESSURECTION, false);
                            DoScriptText(SAY_TERENAS_OUTRO10, Terenas);
                        }
                        TalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 27:
                        if (Creature *Terenas = SummonMgr.GetFirstFoundSummonWithId(NPC_TERENAS_MENETHIL_OUTRO))
                        {
                            Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                            {
                                Player *pPlayer = itr->getSource();
                                if (!pPlayer)
                                    continue;
                                if (!pPlayer->isAlive())
                                    Terenas->CastSpell(pPlayer, SPELL_MASS_RESSURECTION_EFFECT, true);
                            }
                        }
                        if (Fordring)
                        {
                            DoScriptText(SAY_TIRION_OUTRO11, Fordring);
                            Fordring->AddThreat(m_creature, 50000.0f);
                        }
                        m_creature->HandleEmote(EMOTE_STATE_STRANGULATE);
                        TalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 28:
                        DoScriptText(SAY_LICHKING_OUTRO12, m_creature);
                        TalkTimer = 9*IN_MILLISECONDS;
                        break;
                    case 29:
                        DoScriptText(SAY_LICHKING_OUTRO13, m_creature);
                        TalkPhase = 0;
                        break;
                    default:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        HasDoneIntro = true;
                        break;
                }
            }
            else TalkTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_shambling_horrorAI: public ScriptedAI
{
    EventManager Events;
    bool m_bIsHeroic;

    mob_shambling_horrorAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_bIsHeroic(pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
    {
    }

    void Reset() {}

    void Aggro(Unit *pWho)
    {
        SCHEDULE_EVENT(ENRAGE);
        SCHEDULE_EVENT(SHOCKWAVE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsHeroic && m_creature->GetHealthPercent() < 20.0f && !m_creature->HasAura(SPELL_FRENZY))
            DoCast(m_creature, SPELL_FRENZY);

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ENRAGE:
                    DoCast(m_creature, SPELL_ENRAGE);
                    break;
                case EVENT_SHOCKWAVE:
                    DoCast(m_creature->getVictim(), SPELL_SHOCKWAVE);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_ice_sphereAI: public ScriptedAI
{
    mob_ice_sphereAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
    }

    void Reset() {}

    void JustDied(Unit *pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDist(pWho, INTERACTION_DISTANCE))
        {
            DoCast(pWho, SPELL_ICE_BURST, true);
            m_creature->ForcedDespawn();
            return;
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }
};

struct MANGOS_DLL_DECL mob_raging_spiritAI: public ScriptedAI, public ScriptEventInterface
{
    mob_raging_spiritAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature)
    {
    }

    void Reset() {}

    void Aggro(Unit *pWho)
    {
        Unit* Creator = NULL;
        if (Creator = m_creature->GetMap()->GetUnit(m_creature->GetCreatorGuid()))
            m_creature->SetDisplayId(Creator->GetDisplayId());
        BroadcastScriptMessageToEntry(m_creature, NPC_LICH_KING, 300.0f);
        SCHEDULE_EVENT(SOUL_SHRIEK);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SOUL_SHRIEK:
                    DoCast(m_creature->getVictim(), SPELL_SOUL_SHRIEK);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_valkyr_shadowguardAI: public ScriptedAI, public ScriptEventInterface
{
    ObjectGuid PrisonerGuid;

    mob_valkyr_shadowguardAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature)
    {
        // prevent it from moving too fast
        pCreature->SetSpeedRate(MOVE_WALK, 0.7f, true);
        pCreature->SetSpeedRate(MOVE_RUN, 0.7f, true);
        pCreature->SetSpeedRate(MOVE_FLIGHT, 0.7f, true);
        DoCast(m_creature, SPELL_WINGS_OF_THE_DAMNED);
    }

    void Reset() {}

    void ScriptMessage(WorldObject *pSender, uint32, uint32)
    {
        if (pSender && pSender->GetTypeId() == TYPEID_PLAYER)
            PrisonerGuid = pSender->GetObjectGuid();
        Unit* Prisoner = m_creature->GetMap()->GetUnit(PrisonerGuid);
        if (Prisoner)
        {
            DoStartNoMovement(Prisoner);
            float x, y, z;
            Prisoner->GetPosition(x, y, z);
            m_creature->NearTeleportTo(x, y, z, Prisoner->GetOrientation(), false);
        }
        float Angle, x, y;
        Angle = m_creature->GetAngle(CenterPosition[0], CenterPosition[1]);
        if (Angle < M_PI_F/4 && Angle > 7*M_PI_F/4)
            Angle = 3*M_PI_F/2;
        Angle += M_PI_F;
        GetPointOnCircle(x, y, 80.0f, Angle, CenterPosition[0], CenterPosition[1]);
        m_creature->GetMotionMaster()->MovePoint(0, x, y, m_creature->GetPositionZ());
    }

    void MoveInLineOfSight(Unit *pWho) {}

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->IsWithinDist2d(CenterPosition[0], CenterPosition[1], 75.0f))
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, 0, false);
            return;
        }

        if (Unit *Prisoner = m_creature->GetMap()->GetUnit(PrisonerGuid))
        {
            m_creature->SetTargetGuid(PrisonerGuid);
            Prisoner->NearTeleportTo(m_creature->GetPositionX(), m_creature->GetPositionY(),
                m_creature->GetPositionZ(), Prisoner->GetOrientation(), false);
            //Prisoner->Relocate(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
        }
    }
};

struct MANGOS_DLL_DECL mob_vile_wicked_spiritAI: public ScriptedAI
{
    uint32 UnfreezeTimer;

    mob_vile_wicked_spiritAI(Creature* pCreature):
        ScriptedAI(pCreature),
        UnfreezeTimer(pCreature->GetEntry() == NPC_VILE_SPIRIT ? 15*IN_MILLISECONDS : urand(5, 15)*IN_MILLISECONDS)
    {
        m_creature->SetInCombatWithZone();
        m_creature->SetSplineFlags(SPLINEFLAG_NONE);
        SetCombatMovement(false);
    }

    void Reset() {}

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!UnfreezeTimer)
        {
            if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDist(pWho, INTERACTION_DISTANCE))
            {
                DoCast(pWho,  SPELL_SPIRIT_BURST, true);
                m_creature->ForcedDespawn(1*IN_MILLISECONDS);
                return;
            }
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (UnfreezeTimer)
        {
            if (UnfreezeTimer <= uiDiff)
            {
                SetCombatMovement(true);
                m_creature->SetInCombatWithZone();
                DoStartMovement(m_creature->getVictim());
                UnfreezeTimer = 0;
            }
            else 
                UnfreezeTimer -= uiDiff;
        }
    }
};

struct MANGOS_DLL_DECL mob_terenas_menethilAI: public ScriptedAI
{
    EventManager Events;
    SummonManager SummonMgr;
    uint32 TalkPhase;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;

    mob_terenas_menethilAI(Creature* pCreature):
        ScriptedAI(pCreature),
        SummonMgr(pCreature),
        TalkPhase(0),
        m_bIsHeroic(m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC),
        m_bIs10Man(m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL)
    {
        DoCast(m_creature, SPELL_LIGHTS_FAVOR, true);
        Events.ScheduleEvent(EVENT_TALK, 1*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_TALK, (1+7)*IN_MILLISECONDS);
        if (m_bIsHeroic)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoCast(m_creature, SPELL_RESTORE_SOUL);
            Events.ScheduleEvent(EVENT_SUMMON_FROSTMOURNE_SPIRITS, 0);
            Events.ScheduleEvent(EVENT_SUMMON_FROSTMOURNE_SPIRITS, TIMER_SUMMON_FROSTMOURNE_SPIRITS);
            SCHEDULE_EVENT(TELEPORT_OUT);
        }else
            Events.ScheduleEvent(EVENT_TALK, (1+7+13)*IN_MILLISECONDS);
    }

    void Reset() {}

    void DamageDeal(Unit* pDoneTo, uint32& uiDamage)
    {
        uiDamage *= (m_creature->GetHealthPercent() - (m_creature->GetHealthPercent() > 11.0f ? 10.0f : 0.0f));
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_TELEPORT_OUT:
                    if (Unit *pOwner = m_creature->GetOwner())
                        if (pOwner->GetTypeId() == TYPEID_UNIT)
                            SendScriptMessageTo(static_cast<Creature*>(pOwner), m_creature, 0, 0);
                    SummonMgr.UnsummonAll();
                    break;
                case EVENT_SUMMON_FROSTMOURNE_SPIRITS:  // only heroic
                {
                    for (uint32 NumberOfSpirits = urand(10,20); NumberOfSpirits; NumberOfSpirits--)
                    {
                        float x,y;
                        GetRandomPointInCircle(x, y, 35.0f, TerenasSummonPosition[0], TerenasSummonPosition[1]);
                        if (Creature *Spirit = SummonMgr.SummonCreature(NPC_VILE_SPIRIT, x, y, TerenasSummonPosition[2], 0.0f, TEMPSUMMON_DEAD_DESPAWN))
                            Spirit->SetMaxHealth(m_bIs10Man ? 4000 : 8000);
                    }
                    break;
                }
                case EVENT_TALK:
                    switch (TalkPhase)
                    {
                        case 0:
                            DoScriptText(SAY_TERENAS_FROSTMOURNE_INTRO1, m_creature);
                            break;
                        case 1:
                            DoScriptText(SAY_TERENAS_FROSTMOURNE_INTRO2, m_creature);
                            break;
                        case 2:
                            DoScriptText(SAY_TERENAS_FROSTMOURNE_INTRO3, m_creature);
                            break;
                    }
                    TalkPhase++;
                    break;
            }
        if (m_creature->getVictim())
            DoMeleeAttackIfReady();
    }

    void JustDied(Unit *pKiller)
    {
        if (Unit *pOwner = m_creature->GetOwner())
            if (pOwner->GetTypeId() == TYPEID_UNIT)
                SendScriptMessageTo(static_cast<Creature*>(pOwner), m_creature, 0, 0);
    }
};

struct MANGOS_DLL_DECL mob_sprit_wardenAI: public ScriptedAI
{
    uint32 SoulRipTimer;

    mob_sprit_wardenAI(Creature* pCreature):
        ScriptedAI(pCreature),
        SoulRipTimer(TIMER_SOUL_RIP)
    {
    }

    void Reset() {}

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_DARK_HUNGER_AURA);
        SoulRipTimer = 5*IN_MILLISECONDS;
    }

    void JustDied(Unit *pKiller)
    {
        if (Unit *pOwner = m_creature->GetOwner())
            if (pOwner->GetTypeId() == TYPEID_UNIT)
                SendScriptMessageTo(static_cast<Creature*>(pOwner), m_creature, 0, 0);
    }

    void DamageDeal(Unit* pDoneTo, uint32& uiDamage)
    {
        int32 HealAmount = uiDamage / 2;
        m_creature->CastCustomSpell(m_creature, SPELL_DARK_HUNGER_HEAL, &HealAmount, NULL, NULL, true);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SoulRipTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SOUL_RIP);
            SoulRipTimer = TIMER_SOUL_RIP;
        }
        else
            SoulRipTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_shadow_trapAI: public ScriptedAI
{
    uint32 StartTimer;

    mob_shadow_trapAI(Creature* pCreature):
        ScriptedAI(pCreature),
        StartTimer(5*IN_MILLISECONDS)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        DoCast(m_creature, SPELL_SHADOW_TRAP_PRE_AURA);
        m_creature->ForcedDespawn(MINUTE*IN_MILLISECONDS);
        SetCombatMovement(false);
    }

    void Reset() {}

    void MoveInLineOfSight(Unit *pWho) {}

    void AttackStart(Unit *pAttacking) {}

    void UpdateAI(uint32 const uiDiff)
    {
        if (StartTimer)
        {
            if (StartTimer < uiDiff)
            {
                DoCast(m_creature, SPELL_SHADOW_TRAP_AURA);
                StartTimer = 0;
            }
            else
                StartTimer -= uiDiff;
        }
    }

};

void AddSC_boss_the_lich_king()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_the_lich_king);
    REGISTER_SCRIPT(mob_shambling_horror);
    REGISTER_SCRIPT(mob_ice_sphere);
    REGISTER_SCRIPT(mob_raging_spirit);
    REGISTER_SCRIPT(mob_valkyr_shadowguard);
    REGISTER_SCRIPT(mob_vile_wicked_spirit);
    REGISTER_SCRIPT(mob_terenas_menethil);
    REGISTER_SCRIPT(mob_sprit_warden);
    REGISTER_SCRIPT(mob_shadow_trap);
}
