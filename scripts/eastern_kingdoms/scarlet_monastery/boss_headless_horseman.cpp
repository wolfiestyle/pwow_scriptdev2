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
SDName: boss_headless_horseman
SD%Complete: 0
SDComment: Place Holder
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum Says
{
    // player says:
    SAY_RHYME_1            = -1189031,
    SAY_RHYME_2            = -1189032,
    SAY_RHYME_3            = -1189033,
    SAY_RHYME_4            = -1189034,

    // Headless Horseman
    SAY_EMOTE_LAUGH        = -1189030, // unused (using spell instead)

    SAY_ENTRANCE           = -1189022,
    SAY_REJOINED           = -1189023,
    SAY_BODY_DEFEAT        = -1189024,
    SAY_LOST_HEAD          = -1189025,
    SAY_CONFLAGRATION      = -1189026,
    SAY_SPROUTING_PUMPKINS = -1189027,
    SAY_SLAY               = -1189028,
    SAY_DEATH              = -1189029,

};

enum NPC_entries
{
    NPC_HEADLESS_HORSEMAN_HEAD      = 23775,
    NPC_FLAME_BUNNY                 = 23686, // unknown use (unused)
    NPC_EARTH_EXPLOSION_BUNNY       = 23758,
    // Unk use:
    NPC_FIRE                        = 23537, // [DND] unused

};

enum Spells
{
    SPELL_CREATE_PUMPKIN_TREATS = 42754,
    SPELL_CLEAVE                = 42587,
    SPELL_CONFLAGRATION         = 42380,
    SPELL_WHIRLWIND             = 43116,

    SPELL_SEND_HEAD             = 42399,
    SPELL_HEAD_LANDS            = 42400, // not used
    SPELL_RETURN_HEAD           = 42401,
    SPELL_HEAD_VISUAL           = 42413,
    SPELL_HEAD_IS_DEAD          = 42428,

    SPELL_EARTH_EXPLOSION       = 42373,

    SPELL_COMMAND_RETURN_HEAD   = 42405,
    SPELL_HEAD_STUN             = 42408,
    SPELL_HEAD_REPOSITION       = 42409,
    SPELL_COMMAND_HEAD_REPOSITION = 42410,

    SPELL_SUMMON_PUMPKINS       = 42394,

    SPELL_HEAD_PAUSE            = 42504,
    SPELL_BODY_STAGE_1          = 42547,
    SPELL_BODY_STAGE_2          = 42548,
    SPELL_BODY_STAGE_3          = 42549,
    SPELL_HEAD_IS_DEAD_2        = 42566,    // unused (using head_is_dead instead)

    SPELL_GHOST_VISUAL          = 42575,    // not used
    SPELL_HEAD_PERIODIC         = 42603,    // not used

    SPELL_SUMMONING_RHYME       = 42878,    // makes the players say their lines... (scripteffect)
    SPELL_SUMMONING_RHYME_AURA  = 42879,
    SPELL_SUMMONING_SHAKE       = 42909,
    SPELL_SUMMONING_SHAKE_2     = 42910,

    SPELL_COMMAND_HEAD_WANT_BODY = 43101,
    SPELL_BODY_REGEN            = 42403,    // immunity + unmounted visual
    SPELL_BODY_REGEN_2          = 42556,    // health regen
    SPELL_BODY_REGEN_3          = 43105,    // confuse
    SPELL_COMMAND_SELF_MOVE     = 43111,
    SPELL_HEAD_BREATH           = 43207,
    SPELL_HEAL_BODY             = 43306,    // unused (script_target)
    SPELL_BODY_LEAVES_COMBAT    = 43805,    // unused (using head_is_dead instead)
    SPELL_ON_KILL_PROC          = 43877,    // unknown use (unused)
    SPELL_HEAD_VISUAL_2         = 44241,    // unused
    SPELL_LAUGH                 = 43873,

    SPELL_PUMPKIN_AURA          = 42280,
    SPELL_PUMPKIN_AURA_GREEN    = 42294,
    SPELL_SQUASH_SOUL           = 42514,

    SPELL_SPROUTING             = 42281,
    SPELL_SPROUT_BODY           = 42285,

    SPELL_HEAD_SPEAKS           = 43129,
    SPELL_BODY_FLAME            = 42074,    // unknown use (unused)
    SPELL_HEAD_FLAME            = 42971,    // unknown use (unused)
    SPELL_ENRAGE_VISUAL         = 42438,    // unknown use (unused)
    SPELL_SMOKE                 = 42355,    // unknown use (unused)
    SPELL_BODY_DEATH            = 42429,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_WHIRLWIND,
    EVENT_CANCEL_WHIRLWIND,
    EVENT_CLEAVE,
    EVENT_CONFLAGRATE,
    EVENT_SUMMON_PUMPKINS,
    // head events
    EVENT_MOVE_HEAD,
    EVENT_HEAD_BREATH,
    EVENT_FAILED,
    // pumpkins
    EVENT_SPROUT,
    EVENT_SQUASH_SOUL,
};

enum Phases
{
    PHASE_1 = 1,
    PHASE_2,
    PHASE_3,
    PHASE_TRANSITION_1,
    PHASE_TRANSITION_2,
    PHASE_TRANSITION_3,

    PMASK_PHASE_1 = bit_mask<PHASE_1>::value,
    PMASK_PHASE_2 = bit_mask<PHASE_2>::value,
    PMASK_PHASE_3 = bit_mask<PHASE_3>::value,

    PMASK_TRANSITION_1 = bit_mask<PHASE_TRANSITION_1>::value,
    PMASK_TRANSITION_2 = bit_mask<PHASE_TRANSITION_2>::value,
    PMASK_TRANSITION_3 = bit_mask<PHASE_TRANSITION_3>::value,

    PMASK_PHASE_ALL_ACTIVE = bit_mask<PHASE_1, PHASE_2, PHASE_3>::value,
    PMASK_PHASE_T2_AND_T3  = bit_mask<PHASE_TRANSITION_2, PHASE_TRANSITION_3>::value,
};

#define TIMER_CLEAVE                10*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define TIMER_CONFLAGRATE           10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_WHIRLWIND             10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_PUMPKINS              20*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define ROOM_CENTER_X               1796.652f
#define ROOM_CENTER_Y               1351.063f
#define FLOOR_HEIGHT                18.888f

struct MANGOS_DLL_DECL boss_headless_horsemanAI : public ScriptedAI, public ScriptEventInterface
{
    Unit *m_uiPlayer;
    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;
    SummonManager SummonMgr;
    bool m_bHasDoneIntro : 1;
    bool m_bNotified : 1;
    float m_fX, m_fY, m_fZ;

    boss_headless_horsemanAI(Creature* pCreature) :
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_uiTalkPhase(0),
        m_uiTalkTimer(1000),
        m_bHasDoneIntro(false),
        m_bNotified(false),
        SummonMgr(pCreature)
    {
        pCreature->GetPosition(m_fX, m_fY, m_fZ);
        pCreature->SetDisplayId(11686); //spawn invisible to handle the "rhyme"
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Reset()
    {
        if (m_creature->HasAura(SPELL_BODY_REGEN))
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN);
        if (m_creature->HasAura(SPELL_BODY_REGEN_2))
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_2);
        if (m_creature->HasAura(SPELL_BODY_REGEN_3))
            m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_3);
        m_creature->CastSpell(m_creature, SPELL_HEAD_VISUAL, false);
        m_bNotified = false;
        Events.Reset();
        SummonMgr.UnsummonAll();
        Events.SetPhase(PHASE_1);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasDoneIntro && pWho->GetDistance2d(m_creature) < 20.0f &&
            pWho->isTargetableForAttack() && pWho->GetTypeId() == TYPEID_PLAYER &&
            !m_uiTalkPhase)
        {
            m_uiPlayer = pWho;
            m_uiTalkPhase = 1;
        }
        else
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        // summon the head somewhere close to the tanks/party
        float x, y, z;
        pWho->GetPosition(x, y, z);
        z = z + 1.0f;
        SummonMgr.SummonCreature(NPC_HEADLESS_HORSEMAN_HEAD, x, y, z, 
            m_creature->GetOrientation(),TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_CLEAVE, TIMER_CLEAVE, TIMER_CLEAVE, 0, 0, PMASK_PHASE_ALL_ACTIVE);
        Events.ScheduleEventInRange(EVENT_CONFLAGRATE, TIMER_CONFLAGRATE, TIMER_CONFLAGRATE, 0, 0, PMASK_PHASE_2);
        Events.ScheduleEventInRange(EVENT_SUMMON_PUMPKINS, TIMER_PUMPKINS, TIMER_PUMPKINS, 0, 0, PMASK_PHASE_3);
        Events.ScheduleEventInRange(EVENT_WHIRLWIND, TIMER_WHIRLWIND, TIMER_WHIRLWIND, 0, 0, PMASK_PHASE_T2_AND_T3);
        m_creature->SetInCombatWithZone();
    }

    void SpellHit(Unit* pDoneBy, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_COMMAND_HEAD_WANT_BODY) // head reached the control HP
        {
            if (Events.GetPhase() == PHASE_TRANSITION_1)
                Events.SetPhase(PHASE_2);
            else if (Events.GetPhase() == PHASE_TRANSITION_2)
            {
                Events.SetPhase(PHASE_3);
                if (m_creature->HasAura(SPELL_WHIRLWIND))
                    m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
             }
            m_creature->CastSpell(pDoneBy, SPELL_COMMAND_RETURN_HEAD, true);
        }
        else if (spell->Id == SPELL_RETURN_HEAD)
        {
            if (m_bNotified)
                m_bNotified = false;
            if (Events.GetPhase() == PHASE_3)
                DoScriptText(SAY_REJOINED, m_creature);
            if (Events.GetPhase() == PHASE_TRANSITION_1)
                Events.SetPhase(PHASE_1);
            else if (Events.GetPhase() == PHASE_TRANSITION_2)
                Events.SetPhase(PHASE_2);
            else if (Events.GetPhase() == PHASE_TRANSITION_3)
            {
                // Failed to kill head b4 body healed to full in 3rd transition
                // we warn players they must kill the head in time!
                DoScriptText(SAY_BODY_DEFEAT, m_creature, NULL);
                Events.SetPhase(PHASE_3);
            }
            if (Events.GetPhase() == PHASE_1 || Events.GetPhase() == PHASE_2)
                DoScriptText(SAY_REJOINED, m_creature);
            m_creature->CastSpell(m_creature, SPELL_HEAD_VISUAL, true);
            if (m_creature->HasAura(SPELL_BODY_REGEN))
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN);
            if (m_creature->HasAura(SPELL_BODY_REGEN_2))
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_2);
            if (m_creature->HasAura(SPELL_BODY_REGEN_3))
                m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_3);
            m_creature->SetHealthPercent(100.0f);
        }
    }

    void SummonedCreatureJustDied(Creature* pSumm)
    {
        if (pSumm->GetEntry() == NPC_HEADLESS_HORSEMAN_HEAD)
            m_creature->CastSpell(m_creature,SPELL_BODY_DEATH, false);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_SLAY, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage >= m_creature->GetHealth())
        {
            uiDamage = 0;
            if (Events.GetPhase() == PHASE_1)
            {
                // we failed to get the head down to 66% b4 horseman reached 100% hp via health regen
                if (Unit* head = SummonMgr.GetFirstFoundSummonWithId(NPC_HEADLESS_HORSEMAN_HEAD))
                    head->SetHealthPercent(100.0f);
                Events.SetPhase(PHASE_TRANSITION_1);
            }
            else if (Events.GetPhase() == PHASE_2)
            {
                if (Unit* head = SummonMgr.GetFirstFoundSummonWithId(NPC_HEADLESS_HORSEMAN_HEAD))
                    head->SetHealthPercent(66.67f);
                Events.SetPhase(PHASE_TRANSITION_2);
            }
            else if (Events.GetPhase() == PHASE_3)
            {
                if (Unit* head = SummonMgr.GetFirstFoundSummonWithId(NPC_HEADLESS_HORSEMAN_HEAD))
                    head->SetHealthPercent(33.33f);
                Events.SetPhase(PHASE_TRANSITION_3);
            }
            if (Unit* head = SummonMgr.GetFirstFoundSummonWithId(NPC_HEADLESS_HORSEMAN_HEAD))
            {
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->CastSpell(head, SPELL_SEND_HEAD, true);
            }
            m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
            m_creature->CastSpell(m_creature, SPELL_BODY_REGEN, true);
            m_creature->CastSpell(m_creature, SPELL_BODY_REGEN_2, true);
            m_creature->CastSpell(m_creature, SPELL_BODY_REGEN_3, true);
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        SummonMgr.UnsummonAll();

        Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Unit *pPlayer = itr->getSource();
            if (pPlayer)
                pPlayer->CastSpell(pPlayer, SPELL_CREATE_PUMPKIN_TREATS, true);
        }
    }

    void UpdateTalkPhase(const uint32 uiDiff)
    {
        if (m_uiTalkTimer < uiDiff)
        {
            switch(m_uiTalkPhase)
            {
                case 1:
                    if (m_uiPlayer)
                        m_uiPlayer->MonsterSay(SAY_RHYME_1, 0, 0);
                    m_uiTalkTimer = 1500;
                    m_uiTalkPhase = 2;
                    break;
                case 2:
                    if (m_uiPlayer)
                        m_uiPlayer->MonsterSay(SAY_RHYME_2, 0, 0);
                    m_creature->CastSpell(m_creature, SPELL_SUMMONING_SHAKE_2, true);
                    m_uiTalkTimer = 1500;
                    m_uiTalkPhase = 3;
                    break;
                case 3:
                    if (m_uiPlayer)
                        m_uiPlayer->MonsterSay(SAY_RHYME_3, 0, 0);
                    m_creature->CastSpell(m_creature, SPELL_SUMMONING_SHAKE_2, true);
                    m_uiTalkTimer = 1500;
                    m_uiTalkPhase = 4;
                    break;
                case 4:
                    m_creature->CastSpell(m_creature, SPELL_SUMMONING_SHAKE, true);
                    m_uiTalkTimer = 1500;
                    m_uiTalkPhase = 5;
                    break;
                case 5:
                    if (m_uiPlayer)
                        m_uiPlayer->MonsterSay(SAY_RHYME_4, 0, 0);
                    m_creature->CastSpell(m_creature, SPELL_SUMMONING_SHAKE, true);
                    m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
                    if (Unit* earth_Bunny = SummonMgr.SummonCreature(NPC_EARTH_EXPLOSION_BUNNY, m_creature->GetPositionX(),
                        m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                    {
                        earth_Bunny->SetDisplayId(11686);
                        earth_Bunny->CastSpell(earth_Bunny, SPELL_EARTH_EXPLOSION, false);
                    }
                    m_creature->CastSpell(m_creature, SPELL_HEAD_VISUAL, false);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
                    DoMoveHorseman(m_uiTalkPhase);
                    m_uiTalkTimer = 1500;
                    m_uiTalkPhase = 6;
                    break;
                case 6:
                {
                    DoMoveHorseman(m_uiTalkPhase);
                    DoScriptText(SAY_ENTRANCE, m_creature, NULL);
                    int x = urand(0,2);
                    m_uiTalkPhase = 7;
                    m_uiTalkTimer = 3000;
                    break;
                }
                case 7:
                case 8:
                case 9:
                    DoMoveHorseman(m_uiTalkPhase);
                    m_uiTalkPhase++;
                    break;
                case 10:
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                    m_creature->SetInCombatWithZone();
                    m_bHasDoneIntro = true;
                    break;
                default:
                    m_uiTalkPhase = 0;
                    m_uiTalkTimer = 0;
                    break;
            }
        }
        else m_uiTalkTimer -= uiDiff;
    }

    void DoMoveHorseman(uint32 m_uiTalkPhase)
    {
        float x, y, z;
        m_creature->GetPosition(x, y, z);

        switch(m_uiTalkPhase)
        {
            case 5:
                y = y - 30;
                z = z + 7;
                m_uiTalkTimer = 2000;
                break;
            case 6:
                m_creature->CastSpell(m_creature, SPELL_LAUGH, false);
                x = x - 30;
                m_uiTalkTimer = 1500;
                z = z + 3;
                break;
            case 7:
                y = y + 40;
                m_uiTalkTimer = 1500;
                z = z - 3;
                break;
            case 8:
                x = x + urand(25, 40);
                m_uiTalkTimer = 2000;
                z = z - 7;
                break;
            case 9:
                m_creature->CastSpell(m_creature, SPELL_LAUGH, false);
                x = m_fX;
                y = m_fY;
                m_uiTalkTimer = 2000;
                z = m_fZ;
                Events.SetPhase(PHASE_1);
                break;
            default:
                break;
        }
        z += 1.0f;
        m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bHasDoneIntro || Events.GetPhase() == PHASE_TRANSITION_1 || 
            Events.GetPhase() == PHASE_TRANSITION_2 || Events.GetPhase() == PHASE_TRANSITION_3)
            m_creature->SetTargetGUID(0);

        if (!m_bHasDoneIntro)
            UpdateTalkPhase(uiDiff);

        if ((Events.GetPhase() == PHASE_TRANSITION_1 || Events.GetPhase() == PHASE_TRANSITION_2
            || Events.GetPhase() == PHASE_TRANSITION_3) && !m_bNotified && m_creature->GetHealthPercent() >= 99.99f)
        {
            m_bNotified = true; // to prevent spamming this event
            SendEventTo(SummonMgr.GetFirstFoundSummonWithId(NPC_HEADLESS_HORSEMAN_HEAD), EVENT_FAILED, 0);
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CLEAVE:
                    // on next melee (requires melee weapon)
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_CLEAVE, false);
                    break;
                case EVENT_CONFLAGRATE:
                    // Can target tanks lasts 4 seconds
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        DoScriptText(SAY_CONFLAGRATION, m_creature, target);
                        m_creature->CastSpell(target, SPELL_CONFLAGRATION, true);
                    }
                    break;
                case EVENT_WHIRLWIND:
                    // remove confuse to allow whirlwind to trigger
                    if (m_creature->HasAura(SPELL_BODY_REGEN_3))
                        m_creature->RemoveAurasDueToSpell(SPELL_BODY_REGEN_3);
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_WHIRLWIND, false);
                    Events.RescheduleEvent(EVENT_CANCEL_WHIRLWIND, urand(5, 10)*IN_MILLISECONDS);
                    break;
                case EVENT_CANCEL_WHIRLWIND:
                    if (Events.GetPhase() == PHASE_TRANSITION_2 || Events.GetPhase() == PHASE_TRANSITION_3)
                        m_creature->CastSpell(m_creature, SPELL_BODY_REGEN_3, true);
                    if (m_creature->HasAura(SPELL_WHIRLWIND))
                        m_creature->RemoveAurasDueToSpell(SPELL_WHIRLWIND);
                    break;
                case EVENT_SUMMON_PUMPKINS:
                    DoScriptText(SAY_SPROUTING_PUMPKINS, m_creature, NULL);
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_PUMPKINS, false);
                    break;
                default:
                    break;
            }

        if (Events.GetPhase() == PHASE_1 || Events.GetPhase() == PHASE_2 || Events.GetPhase() == PHASE_3)
            DoMeleeAttackIfReady();
    }
};

#define TIMER_MOVE      10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_BREATH    5*IN_MILLISECONDS,  15*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_headless_horseman_headAI : public ScriptedAI, public ScriptEventInterface
{
    bool m_bIsActive;
    uint32 phase;
    ObjectGuid m_ogHorseman;

    boss_headless_horseman_headAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature),
        m_bIsActive(false),
        phase(1)
    {
    }

    void Reset()
    {
        phase = 1;
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_MOVE_HEAD, 0, 0, TIMER_MOVE);
        Events.ScheduleEventInRange(EVENT_HEAD_BREATH, TIMER_BREATH, TIMER_BREATH);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        m_creature->SetTargetGUID(0);
        if (!m_bIsActive)
            return;
        if (phase == 1 && m_creature->GetHealthPercent() < 66.67f)
        {
            m_creature->CastSpell(m_creature, SPELL_HEAD_IS_DEAD, true);
            m_creature->CastSpell(NULL, SPELL_COMMAND_HEAD_WANT_BODY, false);
            m_creature->CastSpell(m_creature, SPELL_HEAD_IS_DEAD, true);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (m_creature->HasAura(SPELL_HEAD_VISUAL))
                m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
            m_bIsActive = false;
            phase = 2;
        }
        if (phase == 2 && m_creature->GetHealthPercent() < 33.33f)
        {
            m_creature->CastSpell(m_creature, SPELL_HEAD_IS_DEAD, true);
            m_creature->CastSpell(NULL, SPELL_COMMAND_HEAD_WANT_BODY, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (m_creature->HasAura(SPELL_HEAD_VISUAL))
                m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
            m_bIsActive = false;
            phase = 3;

        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_MOVE_HEAD:
                {
                    DoStartNoMovement(m_creature->getVictim());
                    float x, y, z;
                    x = ROOM_CENTER_X + 20 - urand(0, 40);
                    y = ROOM_CENTER_Y + 20 - urand(0, 40);
                    z = m_creature->GetPositionZ();
                    // catch me if you can :P
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    break;
                }
                case EVENT_HEAD_BREATH:
                    m_creature->CastSpell(NULL, SPELL_HEAD_BREATH, true);
                    break;
                case EVENT_FAILED:
                    m_creature->CastSpell(m_creature->GetMap()->GetUnit(m_ogHorseman), SPELL_RETURN_HEAD, true);
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->CastSpell(m_creature, SPELL_HEAD_STUN, false);
                    m_bIsActive = false;
                    break;
            }
    }

    void SpellHit(Unit* pDoneBy, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_SEND_HEAD)
        {
            m_ogHorseman = pDoneBy->GetObjectGuid();
            if (m_creature->HasAura(SPELL_HEAD_STUN))
                m_creature->RemoveAurasDueToSpell(SPELL_HEAD_STUN);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->CastSpell(m_creature, SPELL_HEAD_VISUAL, true);
            m_creature->setFaction(14);
            m_bIsActive = true;
        }
        else if (spell->Id == SPELL_COMMAND_RETURN_HEAD)
        {
            DoScriptText(SAY_LOST_HEAD, m_creature);
            m_creature->CastSpell(m_creature, SPELL_HEAD_SPEAKS, true);
            m_creature->CastSpell(pDoneBy, SPELL_RETURN_HEAD, true);
            m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->CastSpell(m_creature, SPELL_HEAD_STUN, false);
        }
        else if (spell->Id == SPELL_COMMAND_HEAD_REPOSITION)
        {
            m_creature->CastSpell(pDoneBy, SPELL_RETURN_HEAD, true);
            m_creature->RemoveAurasDueToSpell(SPELL_HEAD_VISUAL);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->CastSpell(m_creature, SPELL_HEAD_STUN, false);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
            if (phase != 3)
                uiDamage = 0; // dont allow death until it really has to die
    }
};

struct MANGOS_DLL_DECL npc_pulsing_pumpkinAI : public ScriptedAI, public ScriptEventInterface
{
    npc_pulsing_pumpkinAI(Creature* pCreature):
        ScriptedAI(pCreature),
        ScriptEventInterface(pCreature)
    {
        pCreature->CastSpell(pCreature, SPELL_PUMPKIN_AURA, false);
        pCreature->CastSpell(pCreature, SPELL_PUMPKIN_AURA_GREEN, true);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        Events.ScheduleEvent(EVENT_SPROUT, 5*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SQUASH_SOUL, 10*IN_MILLISECONDS, 10*IN_MILLISECONDS);
    }

    void Reset()
    {
        Events.Reset();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SPROUT:
                    m_creature->RemoveAurasDueToSpell(SPELL_PUMPKIN_AURA);
                    m_creature->RemoveAurasDueToSpell(SPELL_PUMPKIN_AURA_GREEN);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->setFaction(14);
                    m_creature->CastSpell(m_creature, SPELL_SPROUT_BODY, true);
                    m_creature->SetInCombatWithZone();
                    break;
                case EVENT_SQUASH_SOUL:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_SQUASH_SOUL, false);
                    break;

            }
    }
};

CreatureAI* GetAI_boss_headless_horseman(Creature* pCreature)
{
    return new boss_headless_horsemanAI(pCreature);
}

CreatureAI* GetAI_boss_headless_horseman_head(Creature* pCreature)
{
    return new boss_headless_horseman_headAI(pCreature);
}

CreatureAI* GetAI_npc_pulsing_pumpkin(Creature* pCreature)
{
    return new npc_pulsing_pumpkinAI(pCreature);
}

void AddSC_boss_headless_horseman()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_headless_horseman";
    NewScript->GetAI = GetAI_boss_headless_horseman;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_headless_horseman_head";
    NewScript->GetAI = GetAI_boss_headless_horseman_head;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_pulsing_pumpkin";
    NewScript->GetAI = GetAI_npc_pulsing_pumpkin;
    NewScript->RegisterSelf();
}
