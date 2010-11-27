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
SDName: boss_halion
SD%Complete:
SDComment: placeholder
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum Says
{
    HALION_SPAWN01      = -1620009,
    HALION_AGGRO01      = -1620010,
    HALION_SLAY01       = -1620011,
    HALION_SLAY02       = -1620012,
    HALION_DEATH01      = -1620013,
    HALION_BERSERK01    = -1620014,
    HALION_SPECIAL01    = -1620015,
    HALION_SPECIAL02    = -1620016,
    HALION_PHASE2       = -1620017,
    HALION_PHASE3       = -1620018,
};

enum Events
{
    MESSAGE_INIT_SUMMONING = 1,
    EVENT_SPAWN_HALION,
    MESSAGE_ENGAGE,
    EVENT_ENGAGE,
    EVENT_BERSERK,
    MESSAGE_PHASE_2,
    EVENT_PHASE_3,
    MESSAGE_PHASE_3,
    MESSAGE_RESET,

    // Meteor Strike
    EVENT_BLOW,
    EVENT_METEOR_SPREAD,

    // Halion Both
    EVENT_RESET,
    EVENT_CLEAVE,
    EVENT_TAIL_LASH,
    EVENT_CALCULATE_DPS,

    // Halion Phisycal
    EVENT_COMBUSTION,
    EVENT_PHYSICAL_SPECIAL,
    EVENT_FLAME_BREATH,

    // Halion Twilight
    EVENT_CONSUMPTION,
    EVENT_DARK_BREATH,
    EVENT_TWILIGHT_SPECIAL,
    EVENT_TWILIGHT_SPECIAL_2,
};

enum Phases
{
    PHASE_PHYSICAL = 1,
    PHASE_TWILIGHT,
    PHASE_3,

    PMASK_P1        = bit_mask<PHASE_PHYSICAL>::value,
    PMASK_P2        = bit_mask<PHASE_TWILIGHT>::value,
    PMASK_P3        = bit_mask<PHASE_3>::value,
    PMASK_PHYSICAL  = bit_mask<PHASE_PHYSICAL, PHASE_3>::value,
    PMASK_TWILIGHT  = bit_mask<PHASE_TWILIGHT, PHASE_3>::value,
};

enum Spells
{
    // Intro
    SPELL_RED_SKYBEAM           = 63772,
    SPELL_HUGE_EXPLOSION        = 76010,

    // Halion: both realms
    SPELL_TWILIGHT_REALM        = 74807,
    SPELL_TWILIGHT_PRECISION    = 78243,
    SPELL_CORPOREALITY_50       = 74826,
    SPELL_CORPOREALITY_60       = 74827,
    SPELL_CORPOREALITY_70       = 74828,
    SPELL_CORPOREALITY_80       = 74829,
    SPELL_CORPOREALITY_90       = 74830,
    SPELL_CORPOREALITY_100      = 74831,
    SPELL_CORPOREALITY_40       = 74832,
    SPELL_CORPOREALITY_30       = 74833,
    SPELL_CORPOREALITY_20       = 74834,
    SPELL_CORPOREALITY_10       = 74835,
    SPELL_CORPOREALITY_0        = 74836,
    SPELL_BERSERK               = 26662,

    // Halion: Physical Realm
    SPELL_TWILIGHT_PHASING      = 74808,
    SPELL_CLEAVE                = 74524,
    SPELL_FIRE_BREATH           = 74525,
    SPELL_METEOR_STRIKE         = 74637,
    SPELL_METEOR_STRIKE_FIRE    = 74713,
    SPELL_METEOR_STRIKE_SOURCE  = 74718,
    SPELL_METEOR_STRIKE_TARGET  = 74641,
    SPELL_METEOR_STRIKE_BLAST   = 74648,
    SPELL_TAIL_LASH             = 74531,
    SPELL_FIERY_COMBUSTION      = 74562, // FIXME: add triggered spells, summon NPC with scale = stack amount
                                         // add NPC script, fix spell radius based on creature scale 

    // Halion: Twilight Realm
    SPELL_TWILIGHT_DIVISION     = 75063,
    //SPELL_CLEAVE                = 74524,
    //SPELL_TAIL_LASH             = 74531,
    SPELL_DARK_BREATH           = 74806,
    SPELL_DUSK_SHROUD           = 75476,
    SPELL_TWILIGHT_CUTTER       = 74768, // FIXME: make sure hit target is really standing between both orbs
    SPELL_SOUL_CONSUMPTION      = 74792, // FIXME: same as fiery combustion

    // Aditional spells
    SPELL_COMBUSTION            = 74629,
    SPELL_CONSUMPTION           = 74803,
};

uint32 Corporeality [] = 
{
    SPELL_CORPOREALITY_0,
    SPELL_CORPOREALITY_10,
    SPELL_CORPOREALITY_20,
    SPELL_CORPOREALITY_30,
    SPELL_CORPOREALITY_40,
    SPELL_CORPOREALITY_50,
    SPELL_CORPOREALITY_60,
    SPELL_CORPOREALITY_70,
    SPELL_CORPOREALITY_80,
    SPELL_CORPOREALITY_90,
    SPELL_CORPOREALITY_100,
};

enum Adds
{
    NPC_METEOR_STRIKE           = 40029, // Summoned by SPELL_METEOR_STRIKE
    NPC_METEOR_STRIKE_BUNNY     = 40041, // the one who casts SPELL_METEOR_STRIKE summoned at (X,Y,Z) coords
    NPC_METEOR_STRIKE_FLAME     = 40042, // the one which leaves a trail of fire (this is a fire-patch on the ground)

    NPC_SHADOW_ORB              = 40100, // 2(4 in heroic) of these are spawned 180 degrees of each other
    NPC_COMBUSTION              = 40001,
    NPC_CONSUMPTION             = 40135,
};

#define TRACK_RADIUS        45.0f
#define MAX_STEPS           36
#define FLAME_STEP_DIST     5
#define FLAME_START_DIST    3
#define FLAME_MAX_DIST      55
#define TIMER_METEOR_SPREAD 1*IN_MILLISECONDS
#define ROOM_CENTER_X       3152.08f
#define ROOM_CENTER_Y       532.163f

struct MANGOS_DLL_DECL npc_meteor_strikeAI: public Scripted_NoMovementAI, ScriptEventInterface
{
    typedef std::list<std::pair<WorldLocation /*start point, direction*/, uint32 /*dist*/> > FlameList;
    FlameList FlameAttribs;
    SummonManager SummonMgr;
    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    npc_meteor_strikeAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        ScriptEventInterface(pCreature),
        SummonMgr(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsHeroic(m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC || m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE);
        if (pCreature->GetEntry() == NPC_METEOR_STRIKE_BUNNY)
            DoCast(pCreature, SPELL_METEOR_STRIKE);
        if (pCreature->GetEntry() == NPC_METEOR_STRIKE_FLAME)
            DoCast(pCreature, SPELL_METEOR_STRIKE_FIRE);
        if (pCreature->GetEntry() == NPC_METEOR_STRIKE)
        {
            pCreature->SetObjectScale(1.0f);
            DoCast(pCreature, SPELL_METEOR_STRIKE_TARGET, false);
            Events.ScheduleEvent(EVENT_BLOW, 6*IN_MILLISECONDS);
        }
    }

    void Reset()
    {
    }

    void SpellHit(Unit* pCaster, const SpellEntry* spellProto)
    {
        if (m_creature->GetEntry() == NPC_METEOR_STRIKE_BUNNY && spellProto->Id == SPELL_METEOR_STRIKE)
            Events.ScheduleEvent(EVENT_BLOW, 6*IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BLOW:
                    if (m_creature->GetEntry() == NPC_METEOR_STRIKE_BUNNY)
                    {
                        WorldLocation StartPos;
                        m_creature->GetPosition(StartPos);
                        m_creature->SetObjectScale(1.0f);
                        DoCast(m_creature, SPELL_METEOR_STRIKE_SOURCE, false);
                        for (uint32 i = 0; i < 4; i++)
                        {
                            StartPos.orientation += i*(M_PI/2);
                            FlameAttribs.push_back(std::make_pair(StartPos, 0));
                        }
                    }
                    else if (m_creature->GetEntry() == NPC_METEOR_STRIKE)
                    {
                        DoCast(m_creature, SPELL_METEOR_STRIKE_BLAST);
                        break;
                    }
                // no break
                case EVENT_METEOR_SPREAD:
                {
                    for (FlameList::iterator i = FlameAttribs.begin(); i != FlameAttribs.end();  ++i)
                    {
                        WorldLocation curPos = i->first;
                        float x, y;
                        i->second += FLAME_STEP_DIST;
                        GetPointOnCircle(x, y, i->second + FLAME_START_DIST, i->first.orientation);
                        if (i->second <= FLAME_MAX_DIST &&
                            std::sqrt(((x+curPos.coord_x)- ROOM_CENTER_X)*((x+curPos.coord_x)- ROOM_CENTER_X) + ((y+curPos.coord_y)- ROOM_CENTER_Y)*((y+curPos.coord_y)- ROOM_CENTER_Y)) < 55.0f)
                        {
                            float x, y;
                            GetPointOnCircle(x, y, i->second + FLAME_START_DIST, i->first.orientation);
                            Creature *flame = SummonMgr.SummonCreatureAt(i->first, NPC_METEOR_STRIKE_FLAME, TEMPSUMMON_TIMED_DESPAWN, m_bIsHeroic ? 20*IN_MILLISECONDS : 15*IN_MILLISECONDS, x, y);
                            if (flame)
                                flame->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE);
                            Events.ScheduleEvent(EVENT_METEOR_SPREAD, TIMER_METEOR_SPREAD);
                        }
                        else
                            i = FlameAttribs.erase(i);
                    }
                    break;
                }
            }
    }
};

struct MANGOS_DLL_DECL npc_shadow_orb_rsAI: public ScriptedAI
{
    uint32 m_uiMoveTimer;
    uint32 m_movePhase;
    float  m_fInitialAngle;

    npc_shadow_orb_rsAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiMoveTimer(1000),
        m_movePhase(0),
        m_fInitialAngle(pCreature->GetAngle(ROOM_CENTER_X, ROOM_CENTER_Y))
    {
        pCreature->SetPhaseMask(32, true);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_UNK_15);
    }

    void Reset(){}

    void Aggro(Unit* pWho){}

    void GoToNextSpot(uint32 i)
    {
        float angle = i * M_PI_F / MAX_STEPS + m_fInitialAngle;
        m_creature->GetMotionMaster()->MovePoint(0, ROOM_CENTER_X + TRACK_RADIUS*cos(angle),
            ROOM_CENTER_Y + TRACK_RADIUS*sin(angle), m_creature->GetPositionZ());
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiMoveTimer < uiDiff)
        {
           GoToNextSpot(m_movePhase++);
           m_uiMoveTimer = 500;
        }
        else m_uiMoveTimer -= uiDiff;
    }
};

#define PATCH_LIFETIME 40*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_special_rsAI: public Scripted_NoMovementAI
{
    uint32 m_uiLifeTimer;
    npc_special_rsAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_uiLifeTimer(PATCH_LIFETIME)
    {
        if (pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
            pCreature->SetPhaseMask(0x0001 | 0x0020, true);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PASSIVE);
        DoCast(pCreature, pCreature->GetEntry() == NPC_COMBUSTION ? SPELL_COMBUSTION : SPELL_CONSUMPTION, true);
    }

    void Reset(){}

    void Aggro(Unit* pWho){}

    void UpdateAI(uint32 const uiDiff)
    {
        m_uiLifeTimer -= uiDiff;
        if (m_uiLifeTimer <= uiDiff)
            m_creature->RemoveFromWorld();
    }
};

// Both
#define TIMER_CLEAVE            15*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_TAIL_LASH         30*IN_MILLISECONDS
#define TIMER_DPS_CHECK         5*IN_MILLISECONDS
// Physical Halion
#define TIMER_PHYSICAL_SPECIAL  40*IN_MILLISECONDS
#define TIMER_FLAME_BREATH      19*IN_MILLISECONDS, 25*IN_MILLISECONDS

// Twilight Halion
#define TIMER_DARK_BREATH       19*IN_MILLISECONDS, 25* IN_MILLISECONDS
#define TIMER_TWILIGHT_SPECIAL  40*IN_MILLISECONDS

#define HALION_TE_PHYS_DEC  "Your efforts have forced Halion further out of the Physical realm!"
#define HALION_TE_PHYS_INC  "Your companion's efforts have forced Halion further into the Physical realm!"
#define HALION_TE_TWIL_INC  "Your companion's efforts have forced Halion further into the Twilight realm!"
#define HALION_TE_TWIL_DEC  "Your efforts have forced Halion further out of the Twilight realm!"

struct MANGOS_DLL_DECL boss_halion_physicalAI: public boss_ruby_sanctumAI
{
    uint32 m_uiDamageTaken;
    SummonManager SummonMgr;
    uint32 m_uiCorporeality;
    bool m_bCantDie;
    uint32 prev_dps;

    boss_halion_physicalAI(Creature* pCreature):
        boss_ruby_sanctumAI(pCreature),
        SummonMgr(pCreature),
        m_uiDamageTaken(0),
        m_uiCorporeality(5),
        prev_dps(0),
        m_bCantDie(true)
    {
    }

    void GetDPS(uint32& curr_corp, uint32 &curr_dps, uint32 &pdps)
    {
        curr_corp = m_uiCorporeality; curr_dps = m_uiDamageTaken; pdps = prev_dps;
        prev_dps = m_uiDamageTaken;
        m_uiDamageTaken = 0;
    }

    void UpdateCorporeality(uint32 newIdx)
    {
        if (m_uiCorporeality != newIdx)
        {
            m_creature->MonsterTextEmote(m_uiCorporeality > newIdx ? HALION_TE_PHYS_DEC : HALION_TE_PHYS_INC, 0, true);
            m_uiCorporeality = newIdx;
            DoCast(m_creature, Corporeality[m_uiCorporeality], true);
        }
    }

    void Reset()
    {
        RemoveEncounterAuras(SPELL_TWILIGHT_REALM);
        m_creature->RemoveAllAuras();
        m_bCantDie = true;
        SummonMgr.UnsummonAll();
        Events.SetPhase(PHASE_PHYSICAL);
        boss_ruby_sanctumAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        DoCast(m_creature, SPELL_TWILIGHT_PRECISION, true);
        Events.SetPhase(PHASE_PHYSICAL);
        if (Creature* pCreature = GET_CREATURE(DATA_HALION_T))
            if (pCreature->GetHealthPercent() <= 50.0f)
                Events.SetPhase(PHASE_TWILIGHT);
        if (Events.GetPhase() == PHASE_PHYSICAL)
        {
            DoScriptText(HALION_AGGRO01, m_creature);
            if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
                SendScriptMessageTo(Controller, m_creature, MESSAGE_ENGAGE, 0);
        }
        Events.ScheduleEvent(EVENT_COMBUSTION, 15*IN_MILLISECONDS, 25*IN_MILLISECONDS, 0, 0, PMASK_PHYSICAL);
        Events.ScheduleEvent(EVENT_PHYSICAL_SPECIAL, 20*IN_MILLISECONDS, TIMER_PHYSICAL_SPECIAL, 0, 0, PMASK_PHYSICAL);
        Events.ScheduleEventInRange(EVENT_CLEAVE, TIMER_CLEAVE, TIMER_CLEAVE, 0, 0, PMASK_PHYSICAL);
        Events.ScheduleEvent(EVENT_TAIL_LASH, TIMER_TAIL_LASH, TIMER_TAIL_LASH, 0, 0, PMASK_PHYSICAL);
        Events.ScheduleEventInRange(EVENT_FLAME_BREATH, 10*IN_MILLISECONDS, 12*IN_MILLISECONDS, TIMER_FLAME_BREATH, 0, 0, PMASK_PHYSICAL);
        m_BossEncounter = IN_PROGRESS;
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_PHASE_3)
        {
            Events.SetPhase(PHASE_3);
            DoCast(m_creature, SPELL_CORPOREALITY_50, true);
            if (Creature* pHalion = GET_CREATURE(DATA_HALION_T))
                m_creature->SetHealthPercent(pHalion->GetHealthPercent());
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_PHASING);
        }
        else if (pSender->GetEntry() == NPC_HALION_TWILIGHT)
        {
            m_bCantDie = false;
        }
        return;
    }

    void KilledUnit(Unit* pWho)
    {
        if (!pWho)
            return;
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0,1) ? HALION_SLAY01: HALION_SLAY02, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        m_BossEncounter = DONE;
        DoScriptText(HALION_DEATH01, m_creature);
        RemoveEncounterAuras(SPELL_TWILIGHT_REALM);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (Events.GetPhase() == PHASE_TWILIGHT)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
        {
            if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
                SendScriptMessageTo(Controller, m_creature, MESSAGE_RESET, 0);
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_FLAME_BREATH:
                    DoCast(m_creature->getVictim(), SPELL_FIRE_BREATH, false);
                    break;
                case EVENT_PHYSICAL_SPECIAL:
                    DoScriptText(HALION_SPECIAL01, m_creature);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1)) // dont target MT
                    {
                        float x, y, z;
                        pTarget->GetPosition(x, y, z);
                        SummonMgr.SummonCreature(NPC_METEOR_STRIKE_BUNNY, x, y, z + 1.0f, urand(0.0f, 2*M_PI_F), TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS);
                    }
                    break;
                case EVENT_TAIL_LASH:
                    DoCast(m_creature, SPELL_TAIL_LASH, false);
                    break;
                case EVENT_CLEAVE:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE, false);
                    break;
                case EVENT_COMBUSTION: // TODO: fix the spell
                    if (Unit* pTarget  = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1)) // dont get tanks
                        DoCast(pTarget, SPELL_FIERY_COMBUSTION, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (Events.GetPhase() == PHASE_PHYSICAL && m_creature->GetHealthPercent() <= 75.0f)
        {
            if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
                SendScriptMessageTo(Controller, m_creature, MESSAGE_PHASE_2, 0);
            if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_1))
                pPortal->SetPhaseMask(1, true);
            DoScriptText(HALION_PHASE2, m_creature);
            DoCast(m_creature, SPELL_TWILIGHT_PHASING, false);
            Events.SetPhase(PHASE_TWILIGHT);
        }
        if (Events.GetPhase() == PHASE_3 && pDoneBy != m_creature)
        {
            m_uiDamageTaken += uiDamage;
            if (uiDamage > m_creature->GetHealth() && m_bCantDie)
                uiDamage = 0;
            if (Creature* pHalion = GET_CREATURE(DATA_HALION_T))
                pHalion->DealDamage(pHalion, uiDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
};

struct MANGOS_DLL_DECL boss_halion_twilightAI: public boss_ruby_sanctumAI
{
    uint32 m_uiDamageTaken;
    SummonManager SummonMgr;
    uint32 m_uiCorporeality;
    uint32 prev_dps;

    ObjectGuid m_DarkSphere1;
    ObjectGuid m_DarkSphere2;
    ObjectGuid m_DarkSphere3;
    ObjectGuid m_DarkSphere4;

    boss_halion_twilightAI(Creature* pCreature):
        boss_ruby_sanctumAI(pCreature),
        SummonMgr(pCreature),
        m_uiDamageTaken(0),
        m_uiCorporeality(5),
        prev_dps(0)
    {
    }

    void GetDPS(uint32& curr_corp, uint32 &curr_dps, uint32 &pdps)
    {
        curr_corp = m_uiCorporeality; curr_dps = m_uiDamageTaken; pdps = prev_dps;
        prev_dps = m_uiDamageTaken;
        m_uiDamageTaken = 0;
    }

    void UpdateCorporeality(uint32 newIdx)
    {
        if (m_uiCorporeality != newIdx)
        {
            m_creature->MonsterTextEmote(m_uiCorporeality > newIdx ? HALION_TE_TWIL_DEC : HALION_TE_TWIL_INC, 0, true);
            m_uiCorporeality = newIdx;
            DoCast(m_creature, Corporeality[m_uiCorporeality], true);
        }
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        return;
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        m_creature->RemoveAllAuras();
        RemoveEncounterAuras(SPELL_TWILIGHT_REALM);
        Events.SetPhase(PHASE_PHYSICAL);
        boss_ruby_sanctumAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* pOrb = SummonMgr.SummonCreature(NPC_SHADOW_ORB, ROOM_CENTER_X + TRACK_RADIUS,
            ROOM_CENTER_Y, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5*IN_MILLISECONDS))
            m_DarkSphere1 = pOrb->GetObjectGuid();
        if (Creature* pOrb = SummonMgr.SummonCreature(NPC_SHADOW_ORB, ROOM_CENTER_X - TRACK_RADIUS,
            ROOM_CENTER_Y, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5*IN_MILLISECONDS))
            m_DarkSphere2 = pOrb->GetObjectGuid();
        if (m_bIsHeroic)
        {
            if (Creature* pOrb = SummonMgr.SummonCreature(NPC_SHADOW_ORB, ROOM_CENTER_X,
                ROOM_CENTER_Y + TRACK_RADIUS, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5*IN_MILLISECONDS))
                m_DarkSphere3 = pOrb->GetObjectGuid();
            if (Creature* pOrb = SummonMgr.SummonCreature(NPC_SHADOW_ORB, ROOM_CENTER_X,
                ROOM_CENTER_Y - TRACK_RADIUS, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5*IN_MILLISECONDS))
                m_DarkSphere4 = pOrb->GetObjectGuid();
        }
        DoCast(m_creature, SPELL_TWILIGHT_PRECISION, true);
        DoCast(m_creature, SPELL_DUSK_SHROUD, true);
        Events.SetPhase(PHASE_TWILIGHT);
        if (Creature* pHalion = GET_CREATURE(TYPE_HALION))
            m_creature->SetHealthPercent(pHalion->GetHealthPercent());
        Events.ScheduleEvent(EVENT_CONSUMPTION, 15*IN_MILLISECONDS, 25*IN_MILLISECONDS, 0, 0, PMASK_TWILIGHT);
        Events.ScheduleEvent(EVENT_TWILIGHT_SPECIAL, m_bIsHeroic ? 30*IN_MILLISECONDS : 35*IN_MILLISECONDS, TIMER_TWILIGHT_SPECIAL, 0, 0, PMASK_TWILIGHT);
        Events.ScheduleEvent(EVENT_TAIL_LASH, TIMER_TAIL_LASH, TIMER_TAIL_LASH, 0, 0, PMASK_TWILIGHT);
        Events.ScheduleEventInRange(EVENT_CLEAVE, TIMER_CLEAVE, TIMER_CLEAVE, 0, 0, PMASK_TWILIGHT);
        Events.ScheduleEventInRange(EVENT_DARK_BREATH, TIMER_DARK_BREATH,TIMER_DARK_BREATH,0,0, PMASK_TWILIGHT);
    }

    void KilledUnit(Unit* pWho)
    {
        if (!pWho)
            return;
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0,1) ? HALION_SLAY01: HALION_SLAY02, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(HALION_DEATH01, m_creature);
        RemoveEncounterAuras(SPELL_TWILIGHT_REALM);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
        {
            if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
                SendScriptMessageTo(Controller, m_creature, MESSAGE_RESET, 0);
            return;
        }


        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_DARK_BREATH:
                    DoCast(m_creature->getVictim(), SPELL_DARK_BREATH, false);
                    break;
                case EVENT_TWILIGHT_SPECIAL:
                    DoScriptText(HALION_SPECIAL02, m_creature);
                    m_creature->MonsterTextEmote("The orbiting spheres pulse with dark energy!", 0, true);
                    Events.ScheduleEvent(EVENT_TWILIGHT_SPECIAL_2, 2*IN_MILLISECONDS);
                    break;
                case EVENT_TWILIGHT_SPECIAL_2:
                    if (Creature* pOrb1 = m_creature->GetMap()->GetCreature(m_DarkSphere1))
                        if (Creature* pOrb2 = m_creature->GetMap()->GetCreature(m_DarkSphere2))
                            pOrb1->CastSpell(pOrb2, SPELL_TWILIGHT_CUTTER, true);
                    if (m_bIsHeroic)
                    {
                        if (Creature* pOrb1 = m_creature->GetMap()->GetCreature(m_DarkSphere3))
                            if (Creature* pOrb2 = m_creature->GetMap()->GetCreature(m_DarkSphere4))
                                pOrb1->CastSpell(pOrb2, SPELL_TWILIGHT_CUTTER, true);
                    }
                    break;
                case EVENT_TAIL_LASH:
                    DoCast(m_creature, SPELL_TAIL_LASH, false);
                    break;
                case EVENT_CLEAVE:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE, false);
                    break;
                case EVENT_CONSUMPTION: // TODO: fix the spell
                    if (Unit* pTarget  = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1)) // dont get tanks
                        DoCast(pTarget, SPELL_SOUL_CONSUMPTION, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (Events.GetPhase() == PHASE_TWILIGHT && m_creature->GetHealthPercent() <= 50.0f)
        {
            if (Creature* Controller = GET_CREATURE(DATA_HALION_CONTROLLER))
                SendScriptMessageTo(Controller, m_creature, MESSAGE_PHASE_3, 0);
            DoScriptText(HALION_PHASE3, m_creature);
            DoCast(m_creature, SPELL_TWILIGHT_DIVISION, false);
            Events.SetPhase(PHASE_3);
        }
        if (Events.GetPhase() == PHASE_3 && pDoneBy != m_creature)
        {
            if (uiDamage > m_creature->GetHealth())
                RemoveEncounterAuras(SPELL_TWILIGHT_REALM);
            m_uiDamageTaken += uiDamage;
            if (Creature* pHalion = GET_CREATURE(TYPE_HALION))
                pHalion->DealDamage(pHalion, uiDamage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }
};

struct MANGOS_DLL_DECL npc_halion_controllerAI: public Scripted_NoMovementAI, ScriptEventInterface
{
    bool m_bIsCombatAllowed;
    uint32 m_uiSummonPhase;
    SummonManager SummonMgr;
    ScriptedInstance* m_pInstance;

    npc_halion_controllerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        ScriptEventInterface(pCreature),
        SummonMgr(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_uiSummonPhase(0),
        m_bIsCombatAllowed(false)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetObjectScale(3.0f); // just so its big!
    }

    void Reset()
    {
        Events.Reset();
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_INIT_SUMMONING:
                Events.ScheduleEvent(EVENT_SPAWN_HALION, 2*IN_MILLISECONDS);
                return;
            case MESSAGE_PHASE_2:
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_1))
                    pPortal->SetPhaseMask(1, true);
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_2))
                    pPortal->SetPhaseMask(64, true);
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_FROM_TWILIGHT_2))
                    pPortal->SetPhaseMask(64, true);
                return;
            case MESSAGE_PHASE_3:
                if (Creature* pHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                    SendScriptMessageTo(pHalion, m_creature, MESSAGE_PHASE_3, 0);
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_1))
                    pPortal->SetPhaseMask(64, true);
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_2))
                    pPortal->SetPhaseMask(1, true);
                if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_FROM_TWILIGHT_2))
                    pPortal->SetPhaseMask(32, true);
                Events.ScheduleEvent(EVENT_CALCULATE_DPS, 5*IN_MILLISECONDS);
                return;
            case MESSAGE_ENGAGE:
                Events.ScheduleEvent(EVENT_ENGAGE, 1*IN_MILLISECONDS);
                return;
            case MESSAGE_RESET:
                m_creature->RemoveAllAttackers();
                m_creature->ClearInCombat();
                return;
        }
        return;
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SPAWN_HALION:
                    {
                    uint32 m_uiPhaseTimer = 0;
                    switch (m_uiSummonPhase)
                        {
                        case 0:
                            DoCast(m_creature, SPELL_RED_SKYBEAM, false);
                            m_uiPhaseTimer = 15*IN_MILLISECONDS;
                            break;
                        case 1:
                            if (GameObject* Tree = GET_GAMEOBJECT(DATA_BURNING_TREE_1))
                                Tree->UseDoorOrButton();
                            if (GameObject* Tree = GET_GAMEOBJECT(DATA_BURNING_TREE_2))
                                Tree->UseDoorOrButton();
                            if (GameObject* Tree = GET_GAMEOBJECT(DATA_BURNING_TREE_3))
                                Tree->UseDoorOrButton();
                            if (GameObject* Tree = GET_GAMEOBJECT(DATA_BURNING_TREE_4))
                                Tree->UseDoorOrButton();
                            m_uiPhaseTimer = 2*IN_MILLISECONDS;
                            break;
                        case 2:
                            m_creature->RemoveAurasDueToSpell(SPELL_RED_SKYBEAM);
                            m_uiPhaseTimer = 2*IN_MILLISECONDS;
                            break;
                        case 3:
                            DoCast(m_creature, SPELL_HUGE_EXPLOSION);
                            m_uiPhaseTimer = 2*IN_MILLISECONDS;
                            break;
                        case 4:
                            SummonMgr.SummonCreature(NPC_HALION_PHYSICAL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(),
                                M_PI_F, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS);
                            if (Creature* pTwilightHalion = SummonMgr.SummonCreature(NPC_HALION_TWILIGHT, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(),
                                M_PI_F, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS))
                                pTwilightHalion->SetPhaseMask(32, true);
                            m_uiPhaseTimer = 2*IN_MILLISECONDS;
                            break;
                        case 5:
                            if (Creature* Halion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                                DoScriptText(HALION_SPAWN01, Halion);
                            m_uiPhaseTimer = 3*IN_MILLISECONDS;
                            break;
                        case 6:
                            if (Creature* Halion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                                Halion->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                            if (Creature* Halion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_TWILIGHT))
                                Halion->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                            break;
                        default:
                            break;
                        }
                    if (m_uiSummonPhase < 6)
                        Events.ScheduleEvent(EVENT_SPAWN_HALION, m_uiPhaseTimer);
                    m_uiSummonPhase++;
                    break;
                    }
                case EVENT_ENGAGE: // controller will track players during combat
                    if (GameObject* HalionFlameRing = GET_GAMEOBJECT(DATA_HALION_FLAME_RING))
                        HalionFlameRing->SetGoState(GO_STATE_READY);
                    m_creature->SetPhaseMask(0xFFFF, true);// make him exist on all realms
                    m_creature->SetInCombatWithZone();
                    Events.ScheduleEvent(EVENT_BERSERK, 8*MINUTE*IN_MILLISECONDS);
                    m_bIsCombatAllowed = true;
                    break;
                case EVENT_BERSERK:
                    if (Creature* tHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_TWILIGHT))
                        tHalion->CastSpell(tHalion, SPELL_BERSERK, true);
                    if (Creature* pHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                        pHalion->CastSpell(pHalion, SPELL_BERSERK, true);
                    break;
                case EVENT_CALCULATE_DPS:
                    {
                        uint32 p_corp_b1, p_corp_b2, dps_b1, dps_b2, pdps_b1, pdps_b2;
                        if (Creature* tHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_TWILIGHT))
                            dynamic_cast<boss_halion_twilightAI*>(tHalion->AI())->GetDPS(p_corp_b1, dps_b1, pdps_b1);
                        if (Creature* pHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                            dynamic_cast<boss_halion_physicalAI*>(pHalion->AI())->GetDPS(p_corp_b2, dps_b2, pdps_b2);
                        float b1 = (dps_b1 + pdps_b1);
                        float b2 = (dps_b2 + pdps_b2);
                        if (b2 == 0)
                            b2 = 1;
                        float dps_ratio = b1 / b2;
                        uint32 new_corp1, new_corp2;
                        if (dps_ratio > 3.0f) // be very "nice" 100-0
                        {
                            new_corp1 = 0;
                            new_corp2 = 10;
                        }
                        else if (dps_ratio > 2.6f) // 90 - 10
                        {
                            new_corp1 = 1;
                            new_corp2 = 9;
                        }
                        else if (dps_ratio > 2.2f) // 80 - 20
                        {
                            new_corp1 = 2;
                            new_corp2 = 8;
                        }
                        else if (dps_ratio > 1.8f) // 70 - 30
                        {
                            new_corp1 = 3;
                            new_corp2 = 7;
                        }
                        else if (dps_ratio > 1.4f) // 60 - 40
                        {
                            new_corp1 = 4;
                            new_corp2 = 6;
                        }
                        else if (dps_ratio > 1.0f) // 50 - 50
                        {
                            new_corp1 = 5;
                            new_corp2 = 5;
                        }
                        else if (dps_ratio < 1/3.0f)// 0 - 100
                        {
                            new_corp1 = 10;
                            new_corp2 = 0;
                        }
                        else if (dps_ratio < 1/2.6f)// 10 - 90
                        {
                            new_corp1 = 9;
                            new_corp2 = 1;
                        }
                        else if (dps_ratio < 1/2.2f)// 20 - 80
                        {
                            new_corp1 = 2;
                            new_corp2 = 8;
                        }
                        else if (dps_ratio < 1/1.8f)// 30 - 70
                        {
                            new_corp1 = 3;
                            new_corp2 = 7;
                        }
                        else if (dps_ratio < 1/1.4f)// 40 - 60
                        {
                            new_corp1 = 4;
                            new_corp2 = 6;
                        }
                        else if (dps_ratio < 1/1.0f)// 50 - 50
                        {
                            new_corp1 = 5;
                            new_corp2 = 5;
                        }
                        if (Creature* tHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_TWILIGHT))
                            dynamic_cast<boss_halion_twilightAI*>(tHalion->AI())->UpdateCorporeality(new_corp1);
                        if (Creature* pHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                            dynamic_cast<boss_halion_physicalAI*>(pHalion->AI())->UpdateCorporeality(new_corp2);
                    }
                    break;
                default:
                    break;
            }
        // if we engaged and there are no targets -> wipe
        if (m_bIsCombatAllowed && (!m_creature->SelectHostileTarget() || !m_creature->getVictim()))
        {
            if (GameObject* HalionFlameRing = GET_GAMEOBJECT(DATA_HALION_FLAME_RING))
                HalionFlameRing->SetGoState(GO_STATE_ACTIVE);
            if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_1))
                pPortal->SetPhaseMask(64, true);
            if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_TO_TWILIGHT_2))
                pPortal->SetPhaseMask(64, true);
            if (GameObject* pPortal = GET_GAMEOBJECT(DATA_PORTAL_FROM_TWILIGHT_2))
                pPortal->SetPhaseMask(64, true);
            if (Creature* pHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_PHYSICAL))
                pHalion->AI()->EnterEvadeMode();
            if (Creature* tHalion = SummonMgr.GetFirstFoundSummonWithId(NPC_HALION_TWILIGHT))
                tHalion->AI()->EnterEvadeMode();
            Events.CancelAllEvents();
            m_bIsCombatAllowed = false;
        }
    }
};

void AddSC_boss_halion()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_halion_physical);
    REGISTER_SCRIPT(boss_halion_twilight);
    REGISTER_SCRIPT(npc_shadow_orb_rs);
    REGISTER_SCRIPT(npc_halion_controller);
    REGISTER_SCRIPT(npc_meteor_strike);
    REGISTER_SCRIPT(npc_special_rs);
}
