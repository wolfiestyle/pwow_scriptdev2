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
SDName: Boss_Ymiron
SD%Complete: 20%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum Says
{
    SAY_AGGRO                   = -1575031,
    SAY_SUMMON_BJORN            = -1575032,
    SAY_SUMMON_HALDOR           = -1575033,
    SAY_SUMMON_RANULF           = -1575034,
    SAY_SUMMON_TORGYN           = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040,
};

enum Spells
{
    SPELL_BANE                  = 48294,
    SPELL_BANE_H                = 59301,
    SPELL_DARK_SLASH            = 48292,
    SPELL_FETID_ROT             = 48291,
    SPELL_FETID_ROT_H           = 59300,
    SPELL_SCREAMS_OF_THE_DEAD   = 51750,
    SPELL_SPIRIT_BURST          = 48529,
    SPELL_SPIRIT_BURST_H        = 59305,
    SPELL_SPIRIT_STRIKE         = 48423,
    SPELL_SPIRIT_STRIKE_H       = 59304,

    SPELL_KING_SPIRIT_01        = 48308, // Bjorn
    SPELL_KING_SPIRIT_02        = 48311, // Haldor
    SPELL_KING_SPIRIT_03        = 48312, // Ranulf
    SPELL_KING_SPIRIT_04        = 48313, // Tor

    SPELL_CHANNEL_KING_SPIRIT   = 48307,
    SPELL_CHANNEL_SPIRIT_KING   = 48316,

    SPELL_SUMMON_AVENGING_SPIRIT = 48592,
    SPELL_SUMMON_SPIRIT_FOUNT   = 48386,

    SPELL_SPIRIT_FOUNT          = 48380,
    SPELL_SPIRIT_FOUNT_H        = 59320,

    SPELL_BOAT_FIRE             = 67302,
};

enum NPCs
{
    NPC_BJORN                   = 27303,
    NPC_BJORN_VISUAL            = 27304,
    NPC_HALDOR                  = 27307,
    NPC_HALDOR_VISUAL           = 27310,
    NPC_RANULF                  = 27308,
    NPC_RANULF_VISUAL           = 27311,
    NPC_TORGYN                  = 27309,
    NPC_TORGYN_VISUAL           = 27312,
    NPC_SPIRIT_FOUNT            = 27339,
    NPC_AVENGING_SPIRIT         = 27386,
};

enum Events
{
    EVENT_ACTIVATE_BOAT_1 = 1,  // cast scream and move to boat
    EVENT_ACTIVATE_BOAT_2,      // channel boat say boat stuff
    EVENT_ACTIVATE_BOAT_3,      // resume combat
    EVENT_BANE,
    EVENT_DARK_SLASH,
    EVENT_FETID_ROT,
    EVENT_SPIRIT_BURST,
    EVENT_SPIRIT_STRIKE,
    EVENT_SPIRIT_FOUNT,
    EVENT_AVENGING_SPIRIT,
};

enum Categories
{
    CATEGORY_GHOST = 1,
};

struct ActiveBoatStruct
{
    NPCs npc;
    Spells spell;
    Says say;
    Events event_id;
    uint32 timer_1, timer_2;    // Event to be scheduled after enabling the new "ability"
    float MoveX, MoveY, MoveZ;
    float SpawnX, SpawnY, SpawnZ, SpawnO;
};

static ActiveBoatStruct const ActiveBoat[4] =
{
    {NPC_BJORN_VISUAL,  SPELL_KING_SPIRIT_01, SAY_SUMMON_BJORN,  EVENT_SPIRIT_FOUNT,    5*IN_MILLISECONDS, 0,                   381.546f, -314.362f, 104.756f, 370.841f, -314.426f, 107.995f, 6.232f}, // confirmed
    {NPC_HALDOR_VISUAL, SPELL_KING_SPIRIT_02, SAY_SUMMON_HALDOR, EVENT_SPIRIT_STRIKE,   5*IN_MILLISECONDS, 5*IN_MILLISECONDS,   404.310f, -314.761f, 104.756f, 413.992f, -314.703f, 107.995f, 3.157f}, // confirmed
    {NPC_RANULF_VISUAL, SPELL_KING_SPIRIT_03, SAY_SUMMON_RANULF, EVENT_SPIRIT_BURST,    5*IN_MILLISECONDS, 10*IN_MILLISECONDS,  404.379f, -335.335f, 104.756f, 413.594f, -335.408f, 107.995f, 3.157f},
    {NPC_TORGYN_VISUAL, SPELL_KING_SPIRIT_04, SAY_SUMMON_TORGYN, EVENT_AVENGING_SPIRIT, 5*IN_MILLISECONDS, 0,                   380.813f, -335.069f, 104.756f, 369.994f, -334.771f, 107.995f, 6.232f},
};

/*######
## boss_ymiron
######*/

struct MANGOS_DLL_DECL boss_ymironAI : public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager Events;
    SummonManager SummonMgr;
    std::bitset<4> m_UsedSpirits;
    uint32 m_uiPhase;
    uint32 m_uiCurrentBoat;
    bool m_bIsInTransitionPhase;

    boss_ymironAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsRegularMode(pCreature->GetMap()->IsRegularDifficulty()),
        SummonMgr(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_uiPhase = 1;
        m_uiCurrentBoat = 0;
        m_bIsInTransitionPhase = false;
        SummonMgr.UnsummonAll();

        m_UsedSpirits.reset();
        Events.Reset();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YMIRON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_BANE, 20*IN_MILLISECONDS, 20*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_DARK_SLASH, 7*IN_MILLISECONDS, 11*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_FETID_ROT, 10*IN_MILLISECONDS, 13*IN_MILLISECONDS);
        if (m_pInstance) // we just started so we set achievement progress
        {
            m_pInstance->SetData(DATA_ACHIEVEMENT_KINGS_BANE, 1);
            m_pInstance->SetData(TYPE_YMIRON, IN_PROGRESS);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void SpellHitTarget(Unit* pWho, SpellEntry const* pSpell)
    {
        if (pSpell->Id == 48295 || pSpell->Id == 59302) // bane triggered (damage part)
        {
            // if damage part of bane hits anything = achievement failed
            if (m_pInstance && m_pInstance->GetData(DATA_ACHIEVEMENT_KINGS_BANE) == 1)
                m_pInstance->SetData(DATA_ACHIEVEMENT_KINGS_BANE, 0);
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_YMIRON, DONE);
    }

    void JustSummoned(Creature* pSummon)
    {
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());

        if (pSummon->GetEntry() == NPC_SPIRIT_FOUNT)
        {
            pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            pSummon->setFaction(14);
            pSummon->SetLevel(83);
            pSummon->SetSpeedRate(MOVE_RUN, 0.4f);
            pSummon->CastSpell(pSummon, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT : SPELL_SPIRIT_FOUNT_H, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsInTransitionPhase && m_creature->GetHealthPercent() <= (100.0f - (m_bIsRegularMode ? 33.0f : 20.0f) * m_uiPhase))
        {
            m_bIsInTransitionPhase = true;
            m_uiPhase++;
            m_creature->InterruptNonMeleeSpells(true);
            DoCast(m_creature, SPELL_SCREAMS_OF_THE_DEAD, false);
            DoStartNoMovement(m_creature->getVictim());

            for (uint32 i = 0; i < 4; i++)
            {
                // on transition start, burn the previous boat
                if (m_UsedSpirits[i])
                {
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(ActiveBoat[i].npc))
                    {
                        if (pSumm->HasAura(SPELL_BOAT_FIRE))
                            continue;

                        pSumm->SetDisplayId(11686);
                        pSumm->CastSpell(pSumm, SPELL_BOAT_FIRE, false);
                    }
                }
            }

            uint32 index;
            do
            {
                index = urand(0,3);
            } while (m_UsedSpirits[index]);
            m_uiCurrentBoat = index;
            m_UsedSpirits[m_uiCurrentBoat] = true;

            Events.CancelEventsWithCategory(CATEGORY_GHOST); // Every time he uses a boat, he loses the abilities from previous boat
            Events.DelayEvents(8* IN_MILLISECONDS);
            Events.ScheduleEvent(EVENT_ACTIVATE_BOAT_1, 1*IN_MILLISECONDS);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ACTIVATE_BOAT_1:
                {
                    m_creature->MonsterMoveWithSpeed(ActiveBoat[m_uiCurrentBoat].MoveX, ActiveBoat[m_uiCurrentBoat].MoveY, ActiveBoat[m_uiCurrentBoat].MoveZ, 2*IN_MILLISECONDS);
                    DoScriptText(ActiveBoat[m_uiCurrentBoat].say, m_creature);
                    Unit* pSummon = SummonMgr.SummonCreature(ActiveBoat[m_uiCurrentBoat].npc, 
                        ActiveBoat[m_uiCurrentBoat].SpawnX, ActiveBoat[m_uiCurrentBoat].SpawnY,
                        ActiveBoat[m_uiCurrentBoat].SpawnZ, ActiveBoat[m_uiCurrentBoat].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    if (pSummon)
                    {
                        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        pSummon->CastSpell(pSummon, ActiveBoat[m_uiCurrentBoat].spell, true);
                    }
                    Events.ScheduleEvent(EVENT_ACTIVATE_BOAT_2, 3*IN_MILLISECONDS);
                    break;
                }
                case EVENT_ACTIVATE_BOAT_2:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(ActiveBoat[m_uiCurrentBoat].npc))
                    {
                        pSumm->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_KING, true);
                        m_creature->CastSpell(pSumm, SPELL_CHANNEL_KING_SPIRIT, true);
                    }
                    Events.ScheduleEvent(EVENT_ACTIVATE_BOAT_3, 5*IN_MILLISECONDS);
                    break;
                case EVENT_ACTIVATE_BOAT_3:
                    m_creature->InterruptNonMeleeSpells(true);
                    m_bIsInTransitionPhase = false;
                    DoStartMovement(m_creature->getVictim());
                    Events.ScheduleEvent(ActiveBoat[m_uiCurrentBoat].event_id, ActiveBoat[m_uiCurrentBoat].timer_1, ActiveBoat[m_uiCurrentBoat].timer_2, 0, CATEGORY_GHOST);
                    Events.RescheduleEvent(EVENT_BANE, 20*IN_MILLISECONDS, 20*IN_MILLISECONDS); // Bane is cast 20 seconds after boat phase
                    break;
                // Normal Abilities
                case EVENT_BANE:
                    m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_BANE : SPELL_BANE_H, false);
                    break;
                case EVENT_DARK_SLASH:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_DARK_SLASH, false);
                    break;
                case EVENT_FETID_ROT:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FETID_ROT : SPELL_FETID_ROT_H, false);
                    break;
                // Boat Abilities
                case EVENT_SPIRIT_BURST:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_BURST : SPELL_SPIRIT_BURST_H, false);
                    break;
                case EVENT_SPIRIT_STRIKE:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE : SPELL_SPIRIT_STRIKE_H, false);
                    break;
                case EVENT_SPIRIT_FOUNT:
                    m_creature->CastSpell(m_creature, SPELL_SUMMON_SPIRIT_FOUNT, false);
                    break;
                case EVENT_AVENGING_SPIRIT:
                    for (uint32 i = 0; i < 4; i++)
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_AVENGING_SPIRIT, true);
                    break;
                default:
                    break;
            }

        if (!m_bIsInTransitionPhase)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

void AddSC_boss_ymiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ymiron";
    newscript->GetAI = &GetAI_boss_ymiron;
    newscript->RegisterSelf();
}
