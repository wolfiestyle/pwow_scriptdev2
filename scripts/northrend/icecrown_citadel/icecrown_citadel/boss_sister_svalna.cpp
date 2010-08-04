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
SDName: boss_sister_svalna
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"
#include "escort_ai.h"

enum Spells
{
    SPELL_BERSERK               = 64238,
    SPELL_IMPALING_SPEAR_CRU    = 70193,
    SPELL_AETHER_BURST          = 71468,
    SPELL_AETHER_SHIELD         = 71463,
    SPELL_DIVINE_SURGE          = 71465,
    SPELL_REVIVE_CHAMPION       = 70053,
    SPELL_IMPALING_SPEAR_PLA    = 71443,
    SPELL_CARESS_OF_LIGHT       = 70078,
    // Arnath
    SPELL_DOMINATE_MIND         = 14515, // possibly bugged... Mind Control
    SPELL_FLASH_HEAL_ALIVE      = 71595, // making it cast on lowesthpfriendly can make server crash unexpectedly so left it as self heal 
    SPELL_FLASH_HEAL_UNDEAD     = 71782,
    SPELL_SMITE_ALIVE           = 71546,
    SPELL_SMITE_UNDEAD          = 71778,
    SPELL_PWS_ALIVE             = 71548,
    SPELL_PWS_UNDEAD            = 71780,
    // Brandon
    SPELL_CRUSADERS_STRIKE      = 71549,
    SPELL_DIVINE_SHIELD         = 71550,
    SPELL_JUDGEMENT_OF_COMMAND  = 71551,
    SPELL_HAMMER_OF_BETRAYAL    = 71784,
    // Grondel
    SPELL_CHARGE                = 71553,
    SPELL_MORTAL_STRIKE         = 71552,
    SPELL_SUNDER_ARMOR          = 71554,
    SPELL_CONFLAGRATION         = 71785,
    // Rupert
    SPELL_ROCKET_LAUNCH_ALIVE   = 71590,
    SPELL_ROCKET_LAUNCH_UNDEAD  = 71786,
    SPELL_FEL_IRON_BOMB_ALIVE   = 71592,
    SPELL_FEL_IRON_BOMB_UNDEAD  = 71787,
    SPELL_MACHINE_GUN_ALIVE     = 71594,
    SPELL_MACHINE_GUN_UNDEAD    = 71788,
    // Crok
    SPELL_DEATH_STRIKE          = 71489,
    SPELL_ICEBOUND_ARMOR        = 70714,
    SPELL_DEATH_COIL            = 71490,
    SPELL_SCOURGE_STRIKE        = 71488,
    // all argent champions
    SPELL_UNDEATH               = 70089,
    // Trash mobs
    // casters:
    // Frostbinders
    SPELL_ARCTIC_CHILL          = 71270,
    SPELL_FROZEN_ORB            = 71274,
    SPELL_FROZEN_ORB_SHOOT      = 71285,
    // Deathbringers
    SPELL_BANISH                = 71298,
    SPELL_DEATHS_EMBRACE        = 71299,
    SPELL_SHADOW_BOLT           = 71296,
    SPELL_SUMMON_YMIRJAR        = 71303,
    // shared for casters:
    SPELL_SPIRIT_STREAM         = 69929,
    SPELL_TWISTED_WINDS         = 71306,
    // non-casters:
    // Battle-Maidens
    SPELL_ADRENALINE_RUSH       = 71258,
    SPELL_BARBARIC_STRIKE       = 71257,
    // Huntresses
    SPELL_ICE_TRAP              = 71249,
    SPELL_RAPID_SHOT            = 71251,
    SPELL_SHOOT                 = 71253,
    SPELL_SUMMON_WARHAWK        = 71705,
    SPELL_VOLLEY                = 71252,
    // Warlords
    SPELL_WHIRLWIND             = 41056,
    // shared spell for all adds
    SPELL_SOUL_FEAST            = 71203,
};

enum Events
{
    // Sister Svalna
    EVENT_BERSERK = 1,
    EVENT_RESET,
    EVENT_AETHER_BURST,
    EVENT_SPEAR,
    EVENT_SHIELD,
    EVENT_WIN,
    // FWH Intro
    EVENT_UPDATE_TEXT,
    EVENT_RESURRECT,
    EVENT_SAY_RESURRECT,
    EVENT_START,        // Controlled by the area triggers
    EVENT_ADVANCE,
    // Boss AI Argent Crusaders
    // Arnath
    EVENT_DOMINATE_MIND, //possibly bugged... Mind Control
    EVENT_FLASH_HEAL,
    EVENT_SMITE,
    EVENT_PWS,
    // Brandon
    EVENT_CRUSADERS_STRIKE,
    EVENT_DIVINE_SHIELD,
    EVENT_JUDGEMENT_OF_COMMAND,
    EVENT_HAMMER_OF_BETRAYAL,
    // Grondel
    EVENT_CHARGE,
    EVENT_MORTAL_STRIKE,
    EVENT_SUNDER_ARMOR,
    EVENT_CONFLAGRATION,
    // Rupert
    EVENT_ROCKET_LAUNCH,
    EVENT_FEL_IRON_BOMB,
    EVENT_MACHINE_GUN,
    // Crok
    EVENT_DEATH_STRIKE,
    EVENT_ICEBOUND_ARMOR,
    EVENT_DEATH_COIL,
    EVENT_SCOURGE_STRIKE,
    EVENT_TIMED_DEATH,
    // Outro
    EVENT_SAY_ALIVE,
    // Trash mobs
    // Frostbinders
    EVENT_ARCTIC_CHILL,
    EVENT_FROZEN_ORB,
    EVENT_FROZEN_ORB_SHOOT,
    // Deathbringers
    EVENT_BANISH,
    EVENT_DEATHS_EMBRACE,
    EVENT_SHADOW_BOLT,
    EVENT_SUMMON_YMIRJAR,
    // shared for casters:
    EVENT_SPIRIT_STREAM,
    EVENT_TWISTED_WINDS,
    // non-casters:
    // Battle-Maidens
    EVENT_ADRENALINE_RUSH,
    EVENT_BARBARIC_STRIKE,
    // Huntresses
    EVENT_ICE_TRAP,
    EVENT_RAPID_SHOT,
    EVENT_SHOOT,
    EVENT_SUMMON_WARHAWK,
    EVENT_VOLLEY,
    // Warlords
    EVENT_WHIRLWIND,
};

enum Says
{
// Captain Grondel
    FWH_GRONDEL_SAY_DEATH       = -1301050,
    FWH_GRONDEL_SAY_RESURRECT   = -1301051,
    FWH_GRONDEL_SAY_SLAY        = -1301052,
    FWH_GRONDEL_SAY_REALDEATH   = -1301053,
    TELE_GRONDEL_SAY_01         = -1301054,
    FWH_GRONDEL_SAY_ALIVE       = -1301055,
// Captain Brandon
    FWH_BRANDON_SAY_DEATH       = -1301056,
    FWH_BRANDON_SAY_RESURRECT   = -1301057,
    FWH_BRANDON_SAY_SLAY        = -1301058,
    FWH_BRANDON_SAY_REALDEATH   = -1301059,
    TELE_BRANDON_SAY_01         = -1301060,
    FWH_BRANDON_SAY_ALIVE       = -1301061,
// Captain Rupert
    FWH_RUPERT_SAY_AGGRO        = -1301062,
    FWH_RUPERT_SAY_DEATH        = -1301063,
    FWH_RUPERT_SAY_RESURRECT    = -1301064,
    FWH_RUPERT_SAY_SLAY         = -1301065,
    FWH_RUPERT_SAY_REALDEATH    = -1301066,
    TELE_RUPERT_SAY_01          = -1301067,
    FWH_RUPERT_SAY_ALIVE        = -1301068,
// Captain Arnath
    FWH_ARNATH_SAY_01           = -1301069,
    FWH_ARNATH_SAY_DEATH        = -1301070,
    FWH_ARNATH_SAY_RESURRECT    = -1301071,
    FWH_ARNATH_SAY_SLAY         = -1301072,
    FWH_ARNATH_SAY_REALDEATH    = -1301073,
    FWH_ARNATH_SAY_ALIVE        = -1301074,
// Crok Scourgebane
    FWH_CROK_SAY_01             = -1301075,
    FWH_CROK_SAY_02             = -1301076,
    FWH_CROK_SAY_03             = -1301077,
    FWH_CROK_SAY_ADVANCE_01     = -1301078,
    FWH_CROK_SAY_ADVANCE_02     = -1301079,
    FWH_CROK_SAY_PRE_ENGAGE     = -1301080,
    FWH_CROK_PRE_ENGAGE         = -1301081,
    FWH_CROK_SAY_LOW_HEALTH     = -1301082,
    FWH_CROK_SAY_LOW_HEALTHBOSS = -1301083,
    FWH_CROK_SAY_WIPE           = -1301084,
    FWH_CROK_SAY_TRAP           = -1301085,
    FWH_CROK_SAY_WIN            = -1301086,
    TELE_CROK_SAY_01            = -1301087,
// Sister Svalna
    FWH_SVALNA_SAY_01           = -1301088,
    FWH_SVALNA_SAY_SLAY_01      = -1301089,
    FWH_SVALNA_SAY_RESURRECT    = -1301090,
    FWH_SVALNA_SAY_PRE_AGGRO    = -1301091,
    FWH_SVALNA_SAY_SLAY_02      = -1301092,
    FWH_SVALNA_SLAY_CPTN        = -1301093,
    FWH_SVALNA_SAY_DEATH_01     = -1301094,
    FWH_SVALNA_SAY_DEATH_02     = -1301095,
};

enum Adds
{
    NPC_IMPALING_SPEAR          = 38248,
    // Halls adds
    NPC_YRMIJAR_WARLORD         = 37133,
    NPC_YMIRJAR_HUNTRESS        = 37134,
    NPC_WARHAWK                 = 38154,
    NPC_YMIRJAR_BATTLE_MAIDEN   = 37132,
    NPC_YMIRJAR_FROSTBINDER     = 37127,
    NPC_YMIRJAR_DEATHBRINGER    = 38125,
};

enum PositionInfo
{
    POSITION_TELEPORTER = 0,
    POSITION_FROSTWING,
};

enum DisplayID          // Used by the "fallen" champions (RIP)
{
       CPTN_ARNATH_RIP_DID  = 30618,           // fallen Captain Arnath
       CPTN_BRANDON_RIP_DID = 30619,           // fallen Captain Brandon
       CPTN_GRONDEL_RIP_DID = 30620,           // fallen Captain Grondel
       CPTN_RUPERT_RIP_DID  = 30621,           // fallen Captain Rupert
};

static const float teleporterloc[2][3] = 
{
    {4234.19f,  2768.7f,    350.96f},   // Area Trigger
    {4199.25f,  2769.26f,   351.06f},   // Teleporter Coords
};

#define COMBAT_CATEGORY 1

struct MANGOS_DLL_DECL boss_sister_svalnaAI: public boss_icecrown_citadelAI
{
    boss_sister_svalnaAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_NON_ATTACKABLE);
    }

    void Reset()
    {
        BroadcastEvent(EVENT_RESET, 1*IN_MILLISECONDS, 150.0f); // reset from boss = escort reset = you fail!

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_NON_ATTACKABLE);

        if (GameObject* FWHDoor = GET_GAMEOBJECT(DATA_FROSTWING_DOOR_ENTRANCE))
            FWHDoor->SetGoState(GO_STATE_ACTIVE); // we leave the door open so ppl can keep trying
    }

    void Aggro(Unit* pWho)
    {
        if (GameObject* FWHDoor = GET_GAMEOBJECT(DATA_FROSTWING_DOOR_ENTRANCE))
            FWHDoor->SetGoState(GO_STATE_READY);
        DoCast(m_creature, SPELL_DIVINE_SURGE, true);
        Events.ScheduleEvent(EVENT_BERSERK, 6*MINUTE*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_AETHER_BURST, 15*IN_MILLISECONDS, 30*IN_MILLISECONDS, 30*IN_MILLISECONDS, 1*MINUTE*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_SPEAR, 45*IN_MILLISECONDS, 45*IN_MILLISECONDS, 60*IN_MILLISECONDS, 60*IN_MILLISECONDS);
    }

    void KilledUnit(Unit* pWho)
    {
        if (!roll_chance_i(66))
            return;

        if (roll_chance_i(50))
                DoScriptText(FWH_SVALNA_SAY_SLAY_01, m_creature);
            else
                DoScriptText(FWH_SVALNA_SAY_SLAY_02, m_creature);
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        if (pSummon->GetEntry() == NPC_IMPALING_SPEAR)
        {
            if (m_creature->HasAura(SPELL_AETHER_SHIELD))
                m_creature->RemoveAurasDueToSpell(SPELL_AETHER_SHIELD);
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (roll_chance_i(50))
            DoScriptText(FWH_SVALNA_SAY_DEATH_01, m_creature);
        else 
            DoScriptText(FWH_SVALNA_SAY_DEATH_02, m_creature);
        //m_BossEncounter = DONE;
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
                    break;
                case EVENT_SPEAR:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER,1))
                    {
                        m_creature->CastSpell(pTarget, SPELL_IMPALING_SPEAR_PLA, false);
                        m_creature->MonsterTextEmote("Sister Svalna has impaled $N!", pTarget->GetGUID(), true);
                        if (Unit* Spear = m_creature->SummonCreature(NPC_IMPALING_SPEAR, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6*IN_MILLISECONDS))
                            Spear->AddThreat(pTarget, 10000.0f);
                        Events.ScheduleEvent(EVENT_SHIELD, 0);
                    }
                    break;
                case EVENT_SHIELD:
                    m_creature->CastSpell(m_creature, SPELL_AETHER_SHIELD, true);
                    break;
                case EVENT_AETHER_BURST:
                    DoCast(m_creature->getVictim(), SPELL_AETHER_BURST);
                    break;
                default:
                    break;
            }
        DoMeleeAttackIfReady();
    }
};

// Frostwing Halls Gauntlet-Event
#define RESET_TIMER             1*IN_MILLISECONDS
#define KILL_CAPTAIN_TIMER      2*MINUTE*IN_MILLISECONDS,   3*MINUTE*IN_MILLISECONDS
#define DEATH_STRIKE_TIMER      10*IN_MILLISECONDS,         15*IN_MILLISECONDS
#define DEATH_COIL_TIMER        20*IN_MILLISECONDS,         30*IN_MILLISECONDS
#define SCOURGE_STRIKE_TIMER    20*IN_MILLISECONDS,         30*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_crok_scourgebaneAI: public npc_escortAI, public ScriptEventInterface
{
    ScriptedInstance* m_pInstance;
    InstanceVar<uint32> m_BossEncounter;
    uint32 currPosition;
    uint32 currWaypoint;
    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;
    bool HasDoneIntro   :1;
    bool m_bIsIntroTalk :1;
    bool m_bLowHealth   :1;
    std::bitset<4> CaptainsSpawned; 
    
    npc_crok_scourgebaneAI(Creature* pCreature):
        npc_escortAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_BossEncounter(TYPE_SVALNA, m_pInstance),
        m_uiTalkPhase(0),
        m_uiTalkTimer(0),
        HasDoneIntro(false),
        m_bIsIntroTalk(true),
        m_bLowHealth(false),
        currPosition((pCreature->GetPositionX() - teleporterloc[0][0]) < 30.0f ? POSITION_TELEPORTER : POSITION_FROSTWING)
    {
        pCreature->SetSpeedRate(MOVE_WALK, 1.6f, true);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || ((Player*)pWho)->isGameMaster())
            return;

        if ((m_creature->GetPositionX() - teleporterloc[0][0]) < 20.0f && (m_creature->GetPositionY() - teleporterloc[0][1]) < 20.0f)
            currPosition = POSITION_TELEPORTER;
        else
            currPosition = POSITION_FROSTWING;

        if (!HasDoneIntro && pWho && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDist(pWho, currPosition == POSITION_TELEPORTER ? 50.0f : 25.0f) && !m_uiTalkPhase)
        {
            m_uiTalkPhase = 1;
            if (currPosition == POSITION_TELEPORTER)
                m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX() + 50.0f, m_creature->GetPositionY(), m_creature->GetPositionZ());
        }
        npc_escortAI::MoveInLineOfSight(pWho);
    }

    void DoStart()
    {
        Events.ScheduleEventInRange(EVENT_TIMED_DEATH, KILL_CAPTAIN_TIMER, KILL_CAPTAIN_TIMER);
        BroadcastEvent(EVENT_START, 0, 30.0f);
        npc_escortAI::Start();
    }

    void JustDied(Unit* pKiller)
    {
        BroadcastEvent(EVENT_RESET, RESET_TIMER, 150.0f);
        if (m_BossEncounter != DONE)
        {
            DoScriptText(FWH_CROK_SAY_WIPE, m_creature);
            m_creature->setDeathState(JUST_DIED);
            m_creature->Respawn();
            m_BossEncounter = NOT_STARTED;
        }
    }

    void Reset()
    {
     /*   if ( m_BossEncounter != IN_PROGRESS )
            EnterEvadeMode();*/
        Events.CancelEventsWithCategory(COMBAT_CATEGORY);
        m_bLowHealth = false;
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->GetEntry() == NPC_SVALNA)
        {
            pWho->AddThreat(m_creature, 30000.0f);
        }
        Events.ScheduleEventInRange(EVENT_DEATH_STRIKE, DEATH_STRIKE_TIMER, DEATH_STRIKE_TIMER, 0, COMBAT_CATEGORY);
        Events.ScheduleEventInRange(EVENT_DEATH_COIL, DEATH_COIL_TIMER, DEATH_COIL_TIMER, 0, COMBAT_CATEGORY);
        Events.ScheduleEventInRange(EVENT_SCOURGE_STRIKE, SCOURGE_STRIKE_TIMER, SCOURGE_STRIKE_TIMER, 0, COMBAT_CATEGORY);
    }

    void UpdateTalk(uint32 const uiDiff)
    {
        if (!HasDoneIntro && m_bIsIntroTalk)
        {
            if (m_uiTalkTimer < uiDiff)
            {
                if (currPosition == POSITION_FROSTWING)         // Svalna's Intro on approach
                {
                    switch (m_uiTalkPhase)
                    {
                        case 1:
                            DoScriptText(FWH_CROK_SAY_01, m_creature);
                            m_uiTalkTimer = 6.5*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 2:
                            BroadcastEventToEntry(NPC_ARNATH, EVENT_UPDATE_TEXT, 0, 30.0f, false);
                            m_uiTalkTimer = 5*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 3:
                            DoScriptText(FWH_CROK_SAY_02, m_creature);
                            m_uiTalkTimer = 8*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 4:
                            if (Creature* Svalna = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(TYPE_SVALNA)))
                                DoScriptText(FWH_SVALNA_SAY_01, Svalna);
                            if (GameObject* Door = GET_GAMEOBJECT(DATA_FROSTWING_DOOR_ENTRANCE))
                                Door->SetGoState(GO_STATE_ACTIVE);
                            m_creature->PlayDirectSound(17017); // Svalna should say this but she is too far away;
                            m_uiTalkTimer = 60*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 5:
                            DoScriptText(FWH_CROK_SAY_03, m_creature);
                            HasDoneIntro = true;
                            m_uiTalkPhase = 0;
                            m_uiTalkTimer = 5*IN_MILLISECONDS;
                            break;
                        default:
                            break;
                    }
                } 
                else if (currPosition == POSITION_TELEPORTER)   // only for teleporter Event after the "Blue Boss"
                {
                    if (CaptainsSpawned[0] && CaptainsSpawned[1] && CaptainsSpawned[2]) //Brandon, Rupert, Grondel
                        CaptainsSpawned.reset();

                    int index;
                    do 
                    {
                        index = urand(0,2);
                    } 
                    while (CaptainsSpawned[index]);

                    CaptainsSpawned[index] = true;
                    if (m_uiTalkPhase > 2)
                        index = 3;
                    switch (index)
                    {
                        case 0:
                            if (Creature* Brandon = m_creature->SummonCreature(NPC_BRANDON, teleporterloc[1][0], teleporterloc[1][1], teleporterloc[1][2], 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
                            {
                                Brandon->GetMotionMaster()->MovePoint(0, Brandon->GetPositionX()+50.0f, Brandon->GetPositionY() + 5.0f*(m_uiTalkPhase == 1? 1 : -1), Brandon->GetPositionZ());
                                DoScriptText(TELE_BRANDON_SAY_01, Brandon);
                            }
                            m_uiTalkTimer = 3*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 1:
                            if (Creature* Rupert = m_creature->SummonCreature(NPC_RUPERT, teleporterloc[1][0], teleporterloc[1][1], teleporterloc[1][2], 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
                            {
                                
                                Rupert->GetMotionMaster()->MovePoint(0, Rupert->GetPositionX()+50.0f, Rupert->GetPositionY() + 5.0f*(m_uiTalkPhase == 1? 1 : -1), Rupert->GetPositionZ());
                                DoScriptText(TELE_RUPERT_SAY_01, Rupert);
                            }
                            m_uiTalkTimer = 4.5*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 2:
                            if (Creature* Grondel = m_creature->SummonCreature(NPC_GRONDEL, teleporterloc[1][0], teleporterloc[1][1], teleporterloc[1][2], 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
                            {
                                Grondel->GetMotionMaster()->MovePoint(0, Grondel->GetPositionX()+50.0f, Grondel->GetPositionY() + 5.0f*(m_uiTalkPhase == 1? 1 : -1), Grondel->GetPositionZ());
                                DoScriptText(TELE_GRONDEL_SAY_01, Grondel);
                            }
                            m_uiTalkTimer = 6*IN_MILLISECONDS;
                            m_uiTalkPhase++;
                            break;
                        case 3:
                            {
                                DoScriptText(TELE_CROK_SAY_01, m_creature);
                                HasDoneIntro = true;
                                m_bIsIntroTalk = false;
                                m_uiTalkPhase = 0;
                            }
                            break;
                        default:
                            break;
                    }
                    
                }
            } else m_uiTalkTimer -= uiDiff;
        } 
        else if (HasDoneIntro && m_bIsIntroTalk) // Svalna's pre-fight phase
        {
            if (m_uiTalkTimer < uiDiff)
            {
                switch (m_uiTalkPhase)
                {
                    case 1:
                        Events.CancelEvent(EVENT_TIMED_DEATH);
                        DoScriptText(FWH_CROK_SAY_PRE_ENGAGE, m_creature);
                        m_uiTalkTimer = 5*IN_MILLISECONDS;
                        m_uiTalkPhase++;
                        break;
                    case 2:
                        if (Creature* Svalna = GET_CREATURE(TYPE_SVALNA))
                        {
                            // if at least 1 captain is dead we cast resurrection on him
                            if (CaptainsSpawned[0] || CaptainsSpawned[1] || CaptainsSpawned[2] || CaptainsSpawned[3])
                            {
                                DoScriptText(FWH_SVALNA_SAY_RESURRECT, Svalna);
                                Svalna->CastSpell(Svalna, SPELL_REVIVE_CHAMPION, false);
                                BroadcastEvent(EVENT_RESURRECT, 3*IN_MILLISECONDS, 100.0f);
                                m_uiTalkTimer = 8*IN_MILLISECONDS;
                            } else // otherwise we skip the resurrection 
                                m_uiTalkPhase++;
                        }
                        m_uiTalkPhase++;
                        break; 
                    case 3:
                        {
                            // since there is a captain who didnt survive, we make it say his stuff
                            int index;
                            do 
                            {
                                index = urand(0,3);
                            } while (!CaptainsSpawned[index]); // false = alive

                            switch (index)
                            {
                                case 0:
                                    BroadcastEventToEntry(NPC_BRANDON, EVENT_SAY_RESURRECT, 0, 50.0f);
                                    m_uiTalkTimer = 12.5*IN_MILLISECONDS;
                                    break;
                                case 1:
                                    BroadcastEventToEntry(NPC_RUPERT, EVENT_SAY_RESURRECT, 0, 50.0f);
                                    m_uiTalkTimer = 11*IN_MILLISECONDS;
                                    break;
                                case 2:
                                    BroadcastEventToEntry(NPC_GRONDEL, EVENT_SAY_RESURRECT, 0, 50.0f);
                                    m_uiTalkTimer = 9.5*IN_MILLISECONDS;
                                    break;
                                case 3:
                                    BroadcastEventToEntry(NPC_ARNATH, EVENT_SAY_RESURRECT, 0, 50.0f);
                                    m_uiTalkTimer = 5*IN_MILLISECONDS;
                                    break;
                            }
                            m_uiTalkPhase++;
                        }
                    case 4:
                        DoScriptText(FWH_CROK_PRE_ENGAGE, m_creature);
                        if (Creature* Svalna = GET_CREATURE(TYPE_SVALNA))
                        {
                            Svalna->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_NON_ATTACKABLE);
                            Svalna->AddThreat(m_creature, 30000.0f);
                        }
                        SetEscortPaused(false);
                        m_uiTalkTimer = 3000;
                        m_bIsIntroTalk = false;
                        m_uiTalkPhase = 0;
                        break;
                    default:
                        break;
                }
             } else m_uiTalkTimer -= uiDiff;
        }
    }
    void WaypointStart(uint32 uiWP)
    {
        switch (uiWP)
        {
            case 2:
            case 3:
            case 4:
                if (!roll_chance_i(66))
                    return;
                if (roll_chance_i(50))
                    DoScriptText(FWH_CROK_SAY_ADVANCE_01, m_creature);
                else
                    DoScriptText(FWH_CROK_SAY_ADVANCE_02, m_creature);
                break;
        }
    }

    void WaypointReached(uint32 uiWP)
    {
        switch (uiWP)
        {
            case 0:
                break;
            case 1: // First Stop
            case 2: // Second Stop
            case 3: // Third Stop
            case 4: // Fourth Stop
                break;
            case 5: // @ Svalna's spot
                SetEscortPaused(true);
                m_uiTalkPhase = 1;
                break;
            case 6: // Win Spot
                DoScriptText(FWH_CROK_SAY_WIN, m_creature);
                break;
            case 7:
                break;
            case 8:
                if (!CaptainsSpawned[0] || !CaptainsSpawned[1] || !CaptainsSpawned[2] || !CaptainsSpawned[3]) // false = alive
                {
                    int index;
                    do 
                    {
                        index = urand(0,3);
                    }
                    while (CaptainsSpawned[index]); // if Cap[index] = dead, we retry
                    switch(index)
                    {   
                        case 0:
                            BroadcastEventToEntry(NPC_BRANDON, EVENT_SAY_ALIVE, 0, 50.0f);
                            break;
                        case 1:
                            BroadcastEventToEntry(NPC_RUPERT, EVENT_SAY_ALIVE, 0, 50.0f);
                            break;
                        case 2:
                            BroadcastEventToEntry(NPC_GRONDEL, EVENT_SAY_ALIVE, 0, 50.0f);
                            break;
                        case 3:
                            BroadcastEventToEntry(NPC_ARNATH, EVENT_SAY_ALIVE, 0, 50.0f);
                            break;
                    }
                }
                m_BossEncounter = DONE;
                break;
        }    
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if ( m_uiTalkPhase )
            UpdateTalk(uiDiff); // this way intro-outro are always executed

        // Must update npc_escortAI
        npc_escortAI::UpdateAI(uiDiff);
        
        Events.Update(uiDiff); // these ones will be queued and executed regardless of combat
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_TIMED_DEATH:
                    int index;
                    do 
                    {
                        index = urand(0,3);
                    } 
                    while (CaptainsSpawned[index]);
                    CaptainsSpawned[index] = true;
                    if (CaptainsSpawned[0] && CaptainsSpawned[1] && CaptainsSpawned[2] && CaptainsSpawned[3]) //Brandon, Rupert, Grondel,Arnath
                        Events.CancelEvent(EVENT_TIMED_DEATH);
                    switch(index)
                    {   
                        case 0:
                            BroadcastEventToEntry(NPC_BRANDON, EVENT_TIMED_DEATH, 0, 50.0f);
                            break;
                        case 1:
                            BroadcastEventToEntry(NPC_RUPERT, EVENT_TIMED_DEATH, 0, 50.0f);
                            break;
                        case 2:
                            BroadcastEventToEntry(NPC_GRONDEL, EVENT_TIMED_DEATH, 0, 50.0f);
                            break;
                        case 3:
                            BroadcastEventToEntry(NPC_ARNATH, EVENT_TIMED_DEATH, 0, 50.0f);
                            break;
                    }
                    if (roll_chance_i(40))
                        if (Creature* Svalna = GET_CREATURE(TYPE_SVALNA))
                        {
                            DoScriptText(FWH_SVALNA_SAY_SLAY_01, Svalna);
                            m_creature->PlayDirectSound(17018); // Svalna is too far, so we just make crok play the sounds
                        }
                    break;
                case EVENT_RESURRECT:
                    BroadcastEvent(EVENT_RESURRECT, 0, 75.0f);
                    break;
                case EVENT_WIN:                    
                    if (!CaptainsSpawned[0] && !CaptainsSpawned[1] && !CaptainsSpawned[2] && !CaptainsSpawned[3]) //Brandon, Rupert, Grondel,Arnath
                        break; // if all died by svalna's Hand... then there is no-one to report the win.
                    BroadcastEvent(EVENT_WIN, 0, 75.0f);
                    break;
                case EVENT_DEATH_STRIKE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_DEATH_STRIKE);
                    break;
                case EVENT_ICEBOUND_ARMOR:
                    DoCastSpellIfCan(m_creature, SPELL_ICEBOUND_ARMOR);
                    break;
                case EVENT_DEATH_COIL:
                    DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0), SPELL_DEATH_COIL);
                    break;
                case EVENT_SCOURGE_STRIKE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SCOURGE_STRIKE);
                    break;
            }
        // Combat Check
        if (m_creature->getVictim())
        {
            DoMeleeAttackIfReady();
        // HP check only in combat
            if (m_creature->GetHealthPercent() < 25.0f && !m_bLowHealth)
            {
                m_bLowHealth = true;
                Events.ScheduleEvent(EVENT_ICEBOUND_ARMOR, 5*IN_MILLISECONDS, 1*MINUTE*IN_MILLISECONDS, 0, COMBAT_CATEGORY);
                if (m_creature->getVictim()->GetEntry() == NPC_SVALNA)
                    DoScriptText(FWH_CROK_SAY_LOW_HEALTHBOSS, m_creature);
                else
                    DoScriptText(FWH_CROK_SAY_LOW_HEALTH, m_creature);
            }
            if (m_creature->GetHealthPercent() > 25.0f && m_bLowHealth && m_creature->HasAura(SPELL_ICEBOUND_ARMOR))
            {
                m_bLowHealth = false;
                Events.CancelEvent(EVENT_ICEBOUND_ARMOR);
                m_creature->RemoveAurasDueToSpell(SPELL_ICEBOUND_ARMOR);
            }
        }
        if (m_creature->GetHealthPercent() >= 100.0f && !m_creature->isInCombat() && m_creature->HasAura(SPELL_ICEBOUND_ARMOR))
            m_creature->RemoveAurasDueToSpell(SPELL_ICEBOUND_ARMOR);
    }

};

// Brandon
#define C_S_TIMER   15*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define J_O_C_TIMER 10*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define H_O_B_TIMER 40*IN_MILLISECONDS, 60*IN_MILLISECONDS
// Grondel
#define CHA_TIMER   15*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define M_S_TIMER   10*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define S_A_TIMER   7*IN_MILLISECONDS,  15*IN_MILLISECONDS
#define CON_TIMER   30*IN_MILLISECONDS, 45*IN_MILLISECONDS
// Rupert
#define R_L_TIMER   15*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define F_I_B_TIMER 25*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define M_G_TIMER   20*IN_MILLISECONDS, 30*IN_MILLISECONDS
// Arnath
#define D_M_TIMER   30*IN_MILLISECONDS, 60*IN_MILLISECONDS
#define F_H_TIMER   5*IN_MILLISECONDS,  5*IN_MILLISECONDS
#define SMI_TIMER   10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define P_W_S_TIMER 15*IN_MILLISECONDS, 15*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_icecrown_argent_captainAI: public npc_escortAI, public ScriptEventInterface
{
    bool fallen : 1;
    ScriptedInstance* m_pInstance;
    uint32 currWaypoint;
    bool m_bLowHealth   :1;

    npc_icecrown_argent_captainAI(Creature* pCreature):
        npc_escortAI(pCreature),
        ScriptEventInterface(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        fallen(false),
        currWaypoint(0),
        m_bLowHealth(false)
    {
        if (m_pInstance->GetData(DATA_TP_UNLOCKED) & (1<<TP_UPPER_SPIRE) && m_pInstance->GetData(TYPE_SVALNA) == DONE)
            pCreature->SetPhaseMask(16, true);
    }

    void Reset()
    {
        Events.CancelEventsWithCategory(COMBAT_CATEGORY);
        m_bLowHealth = false;
    }

    void JustRespawned()
    {
        currWaypoint = 0;
        fallen = false;
        m_creature->RemoveAllAuras();
        m_creature->setDeathState(ALIVE);
        m_creature->setFaction(2230); // Argent Crusade
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (m_creature->HasFlag(UNIT_DYNAMIC_FLAGS,UNIT_DYNFLAG_DEAD))
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        Reset();
    }

    void WaypointReached(uint32 uiWP){}

    void WaypointStart(uint32 uiWP){}

    void Aggro(Unit* pWho)
    {
        if (fallen && m_creature->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD))
            DoStartNoMovement(pWho);

        if (pWho->GetTypeId() != TYPEID_PLAYER)
            if (m_creature->GetEntry() == NPC_RUPERT && roll_chance_i(20))
                DoScriptText(FWH_RUPERT_SAY_AGGRO, m_creature);

        switch (m_creature->GetEntry()) // prepare their skills
        {
            case NPC_BRANDON:
                Events.ScheduleEventInRange(EVENT_CRUSADERS_STRIKE, C_S_TIMER, C_S_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_JUDGEMENT_OF_COMMAND, J_O_C_TIMER, J_O_C_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_HAMMER_OF_BETRAYAL, H_O_B_TIMER, H_O_B_TIMER, 0, COMBAT_CATEGORY);
                break;
            case NPC_GRONDEL:
                Events.ScheduleEventInRange(EVENT_CHARGE, CHA_TIMER, CHA_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_MORTAL_STRIKE, M_S_TIMER, M_S_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_SUNDER_ARMOR, S_A_TIMER, S_A_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_CONFLAGRATION, CON_TIMER, CON_TIMER, 0, COMBAT_CATEGORY);
                break;
            case NPC_RUPERT:
                Events.ScheduleEventInRange(EVENT_ROCKET_LAUNCH, R_L_TIMER, R_L_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_FEL_IRON_BOMB, F_I_B_TIMER, F_I_B_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_MACHINE_GUN, M_G_TIMER, M_G_TIMER, 0, COMBAT_CATEGORY);
                break;
            case NPC_ARNATH:
                /*if (!fallen) // Mind Control
                    Events.ScheduleEventInRange(EVENT_DOMINATE_MIND, D_M_TIMER,  D_M_TIMER);*/
                Events.ScheduleEventInRange(EVENT_FLASH_HEAL, F_H_TIMER, F_H_TIMER, 0, COMBAT_CATEGORY); // self heal only
                Events.ScheduleEventInRange(EVENT_SMITE, SMI_TIMER, SMI_TIMER, 0, COMBAT_CATEGORY);
                Events.ScheduleEventInRange(EVENT_PWS, P_W_S_TIMER, P_W_S_TIMER, 0, COMBAT_CATEGORY); // self buff 
                break;
        }
        DoStartMovement(pWho);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && roll_chance_i(33)) // to prevent spam everytime a captain kills a player
        {
            switch(m_creature->GetEntry())
            {
                case NPC_BRANDON:
                    DoScriptText(FWH_BRANDON_SAY_SLAY, m_creature);
                    break;
                case NPC_GRONDEL:
                    DoScriptText(FWH_GRONDEL_SAY_SLAY, m_creature);
                    break;
                case NPC_RUPERT:
                    DoScriptText(FWH_RUPERT_SAY_SLAY, m_creature);
                    break;
                case NPC_ARNATH:
                    DoScriptText(FWH_ARNATH_SAY_SLAY, m_creature);
                    break;
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (fallen && pKiller->GetObjectGuid() != m_creature->GetObjectGuid())
            switch(m_creature->GetEntry())
            {
                case NPC_BRANDON:
                    DoScriptText(FWH_BRANDON_SAY_REALDEATH, m_creature);
                    break;
                case NPC_GRONDEL:
                    DoScriptText(FWH_GRONDEL_SAY_REALDEATH, m_creature);
                    break;
                case NPC_RUPERT:
                    DoScriptText(FWH_RUPERT_SAY_REALDEATH, m_creature);
                    break;
                case NPC_ARNATH:
                    DoScriptText(FWH_ARNATH_SAY_REALDEATH, m_creature);
                    break;
            }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        // Must update npc_escortAI
        npc_escortAI::UpdateAI(uiDiff);
        
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                // this one will be broadcasted to all the champions (checks the "fallen" flag)
                case EVENT_RESURRECT:
                    if (fallen) // we were feigning death till this point
                    {
                        m_creature->setFaction(14);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE | UNIT_FLAG_STUNNED );
                        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                        m_creature->SetHealth(m_creature->GetMaxHealth());
                        m_creature->SetInCombatWithZone();
                        m_creature->CastSpell(m_creature, SPELL_UNDEATH, false);
                    }
                    break;
                //random risen captain will say his "raise" lines
                case EVENT_SAY_RESURRECT:
                    switch(m_creature->GetEntry())
                    {
                        case NPC_BRANDON:
                            DoScriptText(FWH_BRANDON_SAY_RESURRECT, m_creature);
                            break;
                        case NPC_GRONDEL:
                            DoScriptText(FWH_GRONDEL_SAY_RESURRECT, m_creature);
                            break;
                        case NPC_RUPERT:
                            DoScriptText(FWH_RUPERT_SAY_RESURRECT, m_creature);
                            break;
                        case NPC_ARNATH:
                            DoScriptText(FWH_ARNATH_SAY_RESURRECT, m_creature);
                    }
                    break;
                // only arnath will receive this one
                case EVENT_UPDATE_TEXT:
                    DoScriptText(FWH_ARNATH_SAY_01, m_creature);
                    break;
                // all champions receive this on start (so they start moving)
                case EVENT_START:
                    npc_escortAI::Start();
                    break;
                // all champions will receive this one on wipe
                case EVENT_RESET:
                    m_creature->setDeathState(JUST_DIED);
                    m_creature->Respawn();
                    break;
                // sent on Svalna's Death to all the champions in order to kill (argent guys who died in svalna's fight and not mid-escort)
                case EVENT_WIN:
                    //if unit died while in the event and didnt got ressed it'll stay "unattackable" for easy tracking
                    if (fallen && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                        m_creature->DealDamage(m_creature, m_creature->GetHealth()+1, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    else
                        break;
                    break;
                // sent to a random live champion at the end of the fight (once Crok leads to Valithria)
                case EVENT_SAY_ALIVE: //this can bug if captain died mid-encounter @ valithria (dead body will say stuff)
                    switch(m_creature->GetEntry())
                    {
                        case NPC_BRANDON:
                            DoScriptText(FWH_BRANDON_SAY_ALIVE, m_creature);
                            fallen = true;
                            break;
                        case NPC_GRONDEL:
                            DoScriptText(FWH_GRONDEL_SAY_ALIVE, m_creature);
                            fallen = true;
                            break;
                        case NPC_RUPERT:
                            DoScriptText(FWH_RUPERT_SAY_ALIVE, m_creature);
                            fallen = true;
                            break;
                        case NPC_ARNATH:
                            DoScriptText(FWH_ARNATH_SAY_ALIVE, m_creature);
                            fallen = true;
                            break;
                    }
                    break;
                // sent to a random live champion during the escort (this guy "feigns death")
                case EVENT_TIMED_DEATH: // make it feign death for now
                    SetEscortPaused(true);
                    m_creature->StopMoving();
                    m_creature->ClearInCombat();
                    m_creature->SetHealth(0);
                    m_creature->RemoveAllAuras();
                    m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE | UNIT_FLAG_STUNNED); // TODO: prevent it "moving" while dead
                    switch(m_creature->GetEntry())
                    {
                        case NPC_BRANDON:
                            m_creature->SetDisplayId(CPTN_BRANDON_RIP_DID);
                            DoScriptText(FWH_BRANDON_SAY_DEATH, m_creature);
                            fallen = true;
                            break;
                        case NPC_GRONDEL:
                            m_creature->SetDisplayId(CPTN_GRONDEL_RIP_DID);
                            DoScriptText(FWH_GRONDEL_SAY_DEATH, m_creature);
                            fallen = true;
                            break;
                        case NPC_RUPERT:
                            m_creature->SetDisplayId(CPTN_RUPERT_RIP_DID);
                            DoScriptText(FWH_RUPERT_SAY_DEATH, m_creature);
                            fallen = true;
                            break;
                        case NPC_ARNATH:
                            m_creature->SetDisplayId(CPTN_ARNATH_RIP_DID);
                            DoScriptText(FWH_ARNATH_SAY_DEATH, m_creature);
                            fallen = true;
                            break;
                    }
                    break;
                // Individual AI spell events
                // Arnath
                case EVENT_DOMINATE_MIND: //possibly bugged... Mind Control
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_DOMINATE_MIND);
                    break;
                case EVENT_FLASH_HEAL:
                    DoCastSpellIfCan(m_creature, fallen ? SPELL_FLASH_HEAL_UNDEAD : SPELL_FLASH_HEAL_ALIVE);
                    break;
                case EVENT_SMITE:
                    DoCastSpellIfCan(m_creature->getVictim(), fallen ? SPELL_SMITE_UNDEAD : SPELL_SMITE_ALIVE);
                    break;
                case EVENT_PWS:
                    DoCastSpellIfCan(m_creature, fallen ? SPELL_PWS_UNDEAD : SPELL_PWS_ALIVE);
                    break;
                // Brandon
                case EVENT_CRUSADERS_STRIKE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CRUSADERS_STRIKE);
                    break;
                case EVENT_DIVINE_SHIELD:
                    DoCastSpellIfCan(m_creature, SPELL_DIVINE_SHIELD);
                    break;
                case EVENT_JUDGEMENT_OF_COMMAND:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_JUDGEMENT_OF_COMMAND);
                    break;
                case EVENT_HAMMER_OF_BETRAYAL:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_HAMMER_OF_BETRAYAL);
                    break;
                // Grondel
                case EVENT_CHARGE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHARGE);
                    break;
                case EVENT_MORTAL_STRIKE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                    break;
                case EVENT_SUNDER_ARMOR:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUNDER_ARMOR);
                    break;
                case EVENT_CONFLAGRATION:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CONFLAGRATION);
                    break;
                // Rupert
                case EVENT_ROCKET_LAUNCH:
                    DoCastSpellIfCan(m_creature->getVictim(), fallen ? SPELL_ROCKET_LAUNCH_UNDEAD : SPELL_ROCKET_LAUNCH_ALIVE);
                    break;
                case EVENT_FEL_IRON_BOMB:
                    DoCastSpellIfCan(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0), fallen ? SPELL_FEL_IRON_BOMB_UNDEAD : SPELL_FEL_IRON_BOMB_ALIVE);
                    break;
                case EVENT_MACHINE_GUN:
                    DoCastSpellIfCan(m_creature->getVictim(), fallen ? SPELL_MACHINE_GUN_UNDEAD : SPELL_MACHINE_GUN_ALIVE);
                    break;
                default:
                    break;
            }
        //Combat check
        if (m_creature->getVictim())
        {
            DoMeleeAttackIfReady();
            if (m_creature->GetHealthPercent() < 25.0f && !m_bLowHealth)
            {
                m_bLowHealth = true;
                if(m_creature->GetEntry() == NPC_BRANDON)
                    Events.ScheduleEvent(EVENT_DIVINE_SHIELD, 1*IN_MILLISECONDS, 5*MINUTE*IN_MILLISECONDS, 0, COMBAT_CATEGORY); //Period = cooldown
            }
            if (m_creature->GetHealthPercent() > 25.0f && m_bLowHealth)
            {
                m_bLowHealth = false;
                if(m_creature->GetEntry() == NPC_BRANDON)
                    Events.CancelEvent(EVENT_DIVINE_SHIELD);
            }
        }
    }
};

// for the spear: we either click it or we kill it, both ways, the "bubble" will burst from Svalna
struct MANGOS_DLL_DECL npc_icecrown_impaling_spearAI: public Scripted_NoMovementAI 
{
    npc_icecrown_impaling_spearAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature)
    {
        pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        pCreature->setFaction(7); // FACTION_NEUTRAL
    }
    void Reset(){}
    void UpdateAI(uint32 const uiDiff){}

};
// Trash spells timers:
#define QUEST_FEAST_OF_SOULS 24547

    // Frostbinders
#define FROZEN_ORB_TIMER        10*IN_MILLISECONDS,    15*IN_MILLISECONDS
#define TWISTED_WINDS_TIMER     30*IN_MILLISECONDS,    45*IN_MILLISECONDS
    // Deathbringers
#define BANISH_TIMER            15*IN_MILLISECONDS,    45*IN_MILLISECONDS
#define DEATHS_EMBRACE_TIMER    40*IN_MILLISECONDS,    45*IN_MILLISECONDS
#define SHADOW_BOLT_TIMER       5*IN_MILLISECONDS,     10*IN_MILLISECONDS
#define SUMMON_YMIRJAR_TIMER    60*IN_MILLISECONDS,    65*IN_MILLISECONDS
    // shared for casters:
#define SPIRIT_STREAM_TIMER     25*IN_MILLISECONDS,    45*IN_MILLISECONDS
    // non-casters:
    // Battle-Maidens
#define ADRENALINE_RUSH_TIMER   15*IN_MILLISECONDS,    45*IN_MILLISECONDS
#define BARBARIC_STRIKE_TIMER   2*IN_MILLISECONDS,     5*IN_MILLISECONDS
    // Huntresses
#define ICE_TRAP_TIMER          30*IN_MILLISECONDS,    45*IN_MILLISECONDS
#define RAPID_SHOT_TIMER        30*IN_MILLISECONDS,    45*IN_MILLISECONDS
#define SHOOT_TIMER             3*IN_MILLISECONDS,     5*IN_MILLISECONDS
#define SUMMON_WARHAWK_TIMER    45*IN_MILLISECONDS,    60*IN_MILLISECONDS
#define VOLLEY_TIMER            10*IN_MILLISECONDS,    25*IN_MILLISECONDS
    // Warlords
#define WHIRLWIND_TIMER         20*IN_MILLISECONDS,    25*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_fwh_trash_nmAI: public Scripted_NoMovementAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;

    npc_fwh_trash_nmAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        if (m_pInstance->GetData(TYPE_SVALNA) == DONE) // we make it invisible if event is done and creature wants to respawn
            pCreature->SetPhaseMask(16, true);
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        switch(m_creature->GetEntry())
        {
            case NPC_YMIRJAR_FROSTBINDER:
                DoCast(m_creature, SPELL_ARCTIC_CHILL, false);
                Events.ScheduleEventInRange(EVENT_FROZEN_ORB, 0, 0, FROZEN_ORB_TIMER); // Instant cast on aggro
                //Events.ScheduleEventInRange(EVENT_SPIRIT_STREAM, SPIRIT_STREAM_TIMER, SPIRIT_STREAM_TIMER); // not used by the mob but is listed in his abilities
                Events.ScheduleEventInRange(EVENT_TWISTED_WINDS, TWISTED_WINDS_TIMER, TWISTED_WINDS_TIMER);
                break;
            case NPC_YMIRJAR_DEATHBRINGER:
                Events.ScheduleEventInRange(EVENT_BANISH, BANISH_TIMER, BANISH_TIMER); 
                Events.ScheduleEventInRange(EVENT_DEATHS_EMBRACE, DEATHS_EMBRACE_TIMER, DEATHS_EMBRACE_TIMER);
                Events.ScheduleEventInRange(EVENT_SHADOW_BOLT, SHADOW_BOLT_TIMER, SHADOW_BOLT_TIMER);
                Events.ScheduleEventInRange(EVENT_SUMMON_YMIRJAR, SUMMON_YMIRJAR_TIMER, SUMMON_YMIRJAR_TIMER);
                break;
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
                case EVENT_FROZEN_ORB:
                    DoCast(m_creature, SPELL_FROZEN_ORB);
                        Events.ScheduleEvent(EVENT_FROZEN_ORB_SHOOT, 10*IN_MILLISECONDS);
                    break;
                case EVENT_FROZEN_ORB_SHOOT:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        float x, y, z;
                        pTarget->GetPosition(x, y, z);
                        m_creature->CastSpell(x, y, z, SPELL_FROZEN_ORB_SHOOT, true);
                    }
                    break;
                case EVENT_SPIRIT_STREAM:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SPIRIT_STREAM, CAST_TRIGGERED);
                    break;
                case EVENT_TWISTED_WINDS: // not sure how to just "swap their threat lists" perhaps core should handle the spell
                    DoCast(m_creature, SPELL_TWISTED_WINDS, false);
                    if (Unit* pSwapped = GetClosestCreatureWithEntry(m_creature, NPC_YMIRJAR_DEATHBRINGER, 30.0f))
                    {
                        float x,y,z,o;
                        pSwapped->GetPosition(x, y, z);
                        o = pSwapped->GetOrientation();
                        pSwapped->NearTeleportTo(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());
                        m_creature->NearTeleportTo(x, y, z, o);
                        // TODO: play with threat stuff here.
                    }
                    break;
                case EVENT_BANISH:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_BANISH);
                    break;
                case EVENT_DEATHS_EMBRACE:
                    DoCastSpellIfCan(m_creature, SPELL_DEATHS_EMBRACE, false);
                    break;
                case EVENT_SHADOW_BOLT:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
                    break;
                case EVENT_SUMMON_YMIRJAR:
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_YMIRJAR, false);
                    break;
                default:
                    break;
            }
        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        Map* pMap = m_creature->GetMap();
        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();
            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin();i != PlayerList.end();++i)
                {
                    if (Unit* pPlayer = i->getSource())
                        if (pPlayer->IsWithinDist(m_creature, 50.0f) && pPlayer->hasQuest(QUEST_FEAST_OF_SOULS))
                            DoCast(pPlayer, SPELL_SOUL_FEAST, false);
                }
            }
        }
    }
};

struct MANGOS_DLL_DECL npc_fwh_trashAI: public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;

    npc_fwh_trashAI(Creature* pCreature):
        ScriptedAI(pCreature),        
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))

    {
        if (m_pInstance->GetData(TYPE_SVALNA) == DONE) // we make it invisible if event is done
            pCreature->SetPhaseMask(16, true);
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        switch(m_creature->GetEntry())
        {
            case NPC_YRMIJAR_WARLORD:
                Events.ScheduleEventInRange(EVENT_WHIRLWIND, WHIRLWIND_TIMER, WHIRLWIND_TIMER);
                break;
            case NPC_YMIRJAR_BATTLE_MAIDEN:
                Events.ScheduleEventInRange(EVENT_ADRENALINE_RUSH, ADRENALINE_RUSH_TIMER, ADRENALINE_RUSH_TIMER);
                Events.ScheduleEventInRange(EVENT_BARBARIC_STRIKE,BARBARIC_STRIKE_TIMER,BARBARIC_STRIKE_TIMER);
                break;
            case NPC_YMIRJAR_HUNTRESS:
                Events.ScheduleEventInRange(EVENT_ICE_TRAP, 0, 5*IN_MILLISECONDS, ICE_TRAP_TIMER); // on aggro place a trap 
                Events.ScheduleEventInRange(EVENT_RAPID_SHOT, RAPID_SHOT_TIMER, RAPID_SHOT_TIMER);
                Events.ScheduleEventInRange(EVENT_SHOOT, SHOOT_TIMER, SHOOT_TIMER);
                //Events.ScheduleEventInRange(EVENT_SUMMON_WARHAWK, SUMMON_WARHAWK_TIMER, SUMMON_WARHAWK_TIMER); // makes Escort NPCs stop moving (bird despawns, doesnt attack anyways)
                Events.ScheduleEventInRange(EVENT_VOLLEY, VOLLEY_TIMER, VOLLEY_TIMER);
                break;
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
                case EVENT_WHIRLWIND:
                    m_creature->CastSpell(m_creature, SPELL_WHIRLWIND, false);
                    break;
                case EVENT_ADRENALINE_RUSH:
                    m_creature->CastSpell(m_creature, SPELL_ADRENALINE_RUSH, true);
                    break;
                case EVENT_BARBARIC_STRIKE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_BARBARIC_STRIKE, false);
                    break;
                case EVENT_ICE_TRAP:
                    DoCastSpellIfCan(m_creature, SPELL_ICE_TRAP);
                    break;
                case EVENT_RAPID_SHOT:
                    DoCastSpellIfCan(m_creature, SPELL_RAPID_SHOT);
                    break;
                case EVENT_SHOOT:
                    if (m_creature->getVictim()->GetDistance2d(m_creature) > 5.0f)
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOOT);
                    break;
                case EVENT_SUMMON_WARHAWK:
                    DoCastSpellIfCan(m_creature, SPELL_SUMMON_WARHAWK);
                    break;
                case EVENT_VOLLEY:
                    if (m_creature->getVictim()->GetDistance2d(m_creature) > 10.0f) // getting crashes cause of distance... (too close / deadline)
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_VOLLEY);
                    break;
                default:
                    break;
            }
        DoMeleeAttackIfReady();
    }
    
    void JustDied(Unit* pKiller)
    {
        Map* pMap = m_creature->GetMap();
        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();
            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin();i != PlayerList.end();++i)
                {
                    if (Unit* pPlayer = i->getSource())
                        if (pPlayer->IsWithinDist(m_creature, 50.0f) && pPlayer->hasQuest(QUEST_FEAST_OF_SOULS))
                            DoCast(pPlayer, SPELL_SOUL_FEAST, false);
                }
            }
        }
    }
};

bool AT_fwh_svalna_hall(Player* pPlayer, AreaTriggerEntry *pAt)
{
    if (!pPlayer || pPlayer->isGameMaster())
        return false;

    if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData()))
    {
        if (m_pInstance->GetData(TYPE_SVALNA) == DONE) // disable the trigger, encounter complete
            return true;
        if (m_pInstance->GetData(TYPE_SVALNA) == IN_PROGRESS)
            return false;
        else
        {
            if (Creature* Crok = GetClosestCreatureWithEntry(pPlayer, NPC_CROK_SCOURGEBANE, 50.0f))
            {
                if (Crok->isInCombat())
                    return false;
                ((npc_crok_scourgebaneAI*)Crok->AI())->DoStart();
                m_pInstance->SetData(TYPE_SVALNA, IN_PROGRESS); 
                return false;
            }
        }
    }
    return false;
};

bool GossipHello_npc_impaling_spear(Player *pPlayer, Creature* pCreature)
{
    if (!pPlayer || pPlayer->isGameMaster())
        return false;
    if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        pPlayer->CastSpell(GET_CREATURE(TYPE_SVALNA),SPELL_IMPALING_SPEAR_CRU, true);
        pCreature->DealDamage(pCreature, pCreature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false); // Svalna Summons this one (SummonCreatureJustDied will call it and remove the shield from Svalna // Either ppl kill the creature, or "use it")
        pCreature->ForcedDespawn(100);
        return true;
    }
    return false;
}

CreatureAI* GetAI_npc_icecrown_impaling_spearAI(Creature* pCreature)
{
    return new npc_icecrown_impaling_spearAI(pCreature);
}

void AddSC_boss_sister_svalna()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_sister_svalna);
    REGISTER_SCRIPT(npc_icecrown_argent_captain);
    REGISTER_SCRIPT(npc_crok_scourgebane);
    REGISTER_SCRIPT(npc_fwh_trash);
    REGISTER_SCRIPT(npc_fwh_trash_nm);

    newscript = new Script;
    newscript->Name = "npc_icecrown_impaling_spear";
    newscript->pGossipHello = &GossipHello_npc_impaling_spear;
    newscript->GetAI = &GetAI_npc_icecrown_impaling_spearAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_fwh_svalna_hall";
    newscript->pAreaTrigger = &AT_fwh_svalna_hall;
    newscript->RegisterSelf();
}
