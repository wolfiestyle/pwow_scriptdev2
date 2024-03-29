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
SDName: Boss_Gortok
SD%Complete: 20%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    // script texts
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_DEATH               = -1575018,

    // spells
    SPELL_FREEZE_ANIM       = 16245,

    SPELL_IMPALE            = 48261,
    SPELL_IMPALE_H          = 59268,
    SPELL_WITHERING_ROAR    = 48256,
    SPELL_WITHERING_ROAR_H  = 59267,
    SPELL_ARCING_SMASH      = 48260,

    SPELL_ENRAGE_SOFT       = 48138,
    SPELL_ENRAGE_HARD       = 48142,
    SPELL_MORTAL_WOUND      = 48137,
    SPELL_MORTAL_WOUND_H    = 59265,

    SPELL_CHAIN_LIGHTNING   = 48140,
    SPELL_CHAIN_LIGHTNING_H = 59273,
    SPELL_CRAZED            = 48139,
    SPELL_TERRIFYING_ROAR   = 48144,

    SPELL_ACID_SPIT         = 48132,
    SPELL_ACID_SPLATTER     = 48136,
    SPELL_ACID_SPLATTER_H   = 59272,
    SPELL_POISON_BREATH     = 48133,
    SPELL_POISON_BREATH_H   = 59271,

    SPELL_GORE              = 48130,
    SPELL_GORE_H            = 59264,
    SPELL_GRIEVOUS_WOUND    = 48105,
    SPELL_GRIEVOUS_WOUND_H  = 59263,
    SPELL_STOMP             = 48131,

    SPELL_AWAKEN_SUBBOSS    = 47669,
    SPELL_AWAKEN_GORTOK     = 47670,

    SPELL_ORB_VISUAL        = 48044,

    NPC_GORTOK_CONTROLLER   = 25640, //TODO: find correct npc unused "target orb"

    // events
    EVENT_BEGIN_MOVING = 1,
    EVENT_AWAKEN_SUBBOSS,
    EVENT_AWAKEN_GORTOK,

    EVENT_ARCING_SMASH,
    EVENT_IMPALE,
    EVENT_WITHERING_ROAR,

    EVENT_ENRAGE_SOFT,
    EVENT_MORTAL_WOUND,

    EVENT_CHAIN_LIGHTNING,
    EVENT_TERRIFYING_ROAR,

    EVENT_ACID_SPIT,
    EVENT_POISON_BREATH,

    EVENT_GORE,
    EVENT_STOMP,

    // script messages
    MESSAGE_AWAKEN,
    MESSAGE_ACK,
    MESSAGE_WIPE,
    MESSAGE_DIED,
};

#define MAX_BEAST   4

static uint32 const BeastEntry[MAX_BEAST] = {NPC_WORGEN, NPC_FURBOLG, NPC_JORMUNGAR, NPC_RHINO};

typedef UNORDERED_MAP<uint32 /*entry*/, uint32 /*index*/> EntryToIndexMap;
static EntryToIndexMap const BeastIndex = map_initializer<EntryToIndexMap>
    (NPC_WORGEN, 0)(NPC_FURBOLG, 1)(NPC_JORMUNGAR, 2)(NPC_RHINO, 3);

/*######
## npc_gortok_controller
######*/

struct MANGOS_DLL_DECL npc_gortok_controllerAI : public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    EventManager Events;
    uint32 m_uiBossesToSummon;
    std::bitset<MAX_BEAST> CreaturesUsed;
    ObjectGuid BeastGuid[MAX_BEAST];

    npc_gortok_controllerAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsRegularMode(pCreature->GetMap()->IsRegularDifficulty()),
        m_uiBossesToSummon(m_bIsRegularMode ? 2 : 4)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_GORTOK) != NOT_STARTED)
        {
            pCreature->ForcedDespawn(100);
            return;
        }

        pCreature->SetSpeedRate(MOVE_WALK, 2.0f, true);
        pCreature->SetSpeedRate(MOVE_RUN, 2.0f, true);
        pCreature->SetSpeedRate(MOVE_FLIGHT, 2.0f, true);

        pCreature->AddSplineFlag(SPLINEFLAG_FLYING);

        pCreature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NOT_SELECTABLE);
        pCreature->CastSpell(pCreature, SPELL_ORB_VISUAL, true);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, IN_PROGRESS);

        Events.ScheduleEvent(EVENT_BEGIN_MOVING, 5*IN_MILLISECONDS);
    }

    void Reset()
    {
        CreaturesUsed.reset();
        Events.Reset();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_GORTOK) == DONE)
            m_creature->ForcedDespawn();

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BEGIN_MOVING:
                    m_creature->GetMotionMaster()->MovePoint(0, 276.068f, -451.88f, 113.172f);
                    Events.ScheduleEvent(EVENT_AWAKEN_SUBBOSS, 10*IN_MILLISECONDS);
                    break;
                case EVENT_AWAKEN_SUBBOSS:
                {
                    uint32 index;
                    do
                    {
                        index = urand(0, MAX_BEAST-1);
                    } while (CreaturesUsed[index]); // true = used
                    BroadcastScriptMessageToEntry(m_creature, BeastEntry[index], 50.0f, MESSAGE_AWAKEN);
                    CreaturesUsed[index] = true;
                    m_uiBossesToSummon--;
                    break;
                }
                case EVENT_AWAKEN_GORTOK:
                    BroadcastScriptMessageToEntry(m_creature, NPC_GORTOK, 50.0f, MESSAGE_AWAKEN);
                    break;
                default:
                    break;
        }
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        uint32 index = map_find(BeastIndex, pSender->GetEntry(), MAX_BEAST);
        if (index < MAX_BEAST)
            BeastGuid[index] = pSender->GetObjectGuid();

        if (data1 == MESSAGE_WIPE)
        {
            for (uint32 i = 0; i < MAX_BEAST; ++i)
                if (!BeastGuid[i].IsEmpty())
                    if (Creature *pBeast = m_creature->GetMap()->GetCreature(BeastGuid[i]))
                        if (!pBeast->isAlive())
                            pBeast->Respawn();
            m_creature->ForcedDespawn(100);

            if (m_pInstance)
                m_pInstance->SetData(TYPE_GORTOK, NOT_STARTED);
        }
        else if (data1 == MESSAGE_ACK)
        {
            if (pSender->GetEntry() != NPC_GORTOK)
                m_creature->CastSpell(pSender->GetPositionX(), pSender->GetPositionY(), pSender->GetPositionZ(), SPELL_AWAKEN_SUBBOSS, false);
            else if (pSender->isType(TYPEMASK_UNIT))
                m_creature->CastSpell(static_cast<Unit*>(pSender), SPELL_AWAKEN_GORTOK, false);
        }
        else if (data1 == MESSAGE_DIED)
        {
            if (pSender->GetEntry() == NPC_GORTOK)
                return;
            Events.ScheduleEvent(m_uiBossesToSummon ? EVENT_AWAKEN_SUBBOSS : EVENT_AWAKEN_GORTOK, 5*IN_MILLISECONDS);
        }
    }
};

/*######
## boss_gortok_beasts
######*/

struct MANGOS_DLL_DECL boss_gortok_beastsAI : public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance *m_pInstance;
    EventManager Events;
    bool m_bIsRegularMode :1;
    bool m_bSpecialUsed :1;
    bool m_bAwaken :1;
    uint32 m_uiAwakeTimer;

    boss_gortok_beastsAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsRegularMode(pCreature->GetMap()->IsRegularDifficulty()),
        m_bSpecialUsed(false),
        m_bAwaken(false),
        m_uiAwakeTimer(0)
    {
        DoCast(pCreature, SPELL_FREEZE_ANIM, false);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Reset()
    {
        m_bAwaken = false;
        m_bSpecialUsed = false;
        Events.Reset();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        BroadcastScriptMessageToEntry(m_creature, NPC_GORTOK_CONTROLLER, 100.0f, MESSAGE_WIPE);
    }

    void JustReachedHome()
    {        
        DoCast(m_creature, SPELL_FREEZE_ANIM, false);
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_AWAKEN:
                if (pSender->GetTypeId() == TYPEID_UNIT)
                    SendScriptMessageTo(static_cast<Creature*>(pSender), m_creature, MESSAGE_ACK, 0);
                m_uiAwakeTimer = 7*IN_MILLISECONDS;
                m_bAwaken = true;
                break;
            default:
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_WORGEN:
                Events.ScheduleEventInRange(EVENT_ENRAGE_SOFT, 16*IN_MILLISECONDS, 24*IN_MILLISECONDS, 24*IN_MILLISECONDS, 29*IN_MILLISECONDS);
                Events.ScheduleEventInRange(EVENT_MORTAL_WOUND, 4*IN_MILLISECONDS, 7*IN_MILLISECONDS, 9*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                break;
            case NPC_FURBOLG:
                Events.ScheduleEventInRange(EVENT_CHAIN_LIGHTNING, 5*IN_MILLISECONDS, 10*IN_MILLISECONDS, 12*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                Events.ScheduleEventInRange(EVENT_TERRIFYING_ROAR, 12*IN_MILLISECONDS, 19*IN_MILLISECONDS, 23*IN_MILLISECONDS, 27*IN_MILLISECONDS);
                break;
            case NPC_JORMUNGAR:
                Events.ScheduleEventInRange(EVENT_ACID_SPIT, 3*IN_MILLISECONDS, 6*IN_MILLISECONDS, 12*IN_MILLISECONDS, 18*IN_MILLISECONDS);
                Events.ScheduleEventInRange(EVENT_POISON_BREATH, 13*IN_MILLISECONDS, 20*IN_MILLISECONDS, 17*IN_MILLISECONDS, 25*IN_MILLISECONDS);
                break;
            case NPC_RHINO:
                m_creature->CastSpell(pWho, m_bIsRegularMode ? SPELL_GRIEVOUS_WOUND : SPELL_GRIEVOUS_WOUND_H, false);
                Events.ScheduleEventInRange(EVENT_GORE, 5*IN_MILLISECONDS, 9*IN_MILLISECONDS, 18*IN_MILLISECONDS, 24*IN_MILLISECONDS);
                Events.ScheduleEventInRange(EVENT_STOMP, 10*IN_MILLISECONDS, 15*IN_MILLISECONDS, 12*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bAwaken)
        {
            if (m_uiAwakeTimer <= uiDiff)
            {
                m_bAwaken = false;
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetInCombatWithZone();
            }
            else
                m_uiAwakeTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bSpecialUsed)
            switch (m_creature->GetEntry())
            {
                case NPC_WORGEN:
                    if (m_creature->GetHealthPercent() <= 20.0f)
                    {
                        m_bSpecialUsed = true;
                        DoCast(m_creature, SPELL_ENRAGE_HARD, true);
                    }
                    break;
                case NPC_FURBOLG:
                    if (m_creature->GetHealthPercent() <= 30.0f)
                    {
                        m_bSpecialUsed = true;
                        DoCast(m_creature, SPELL_CRAZED, true);
                    }
                    break;
                case NPC_JORMUNGAR:
                    if (m_creature->GetHealthPercent() <= 50.0f)
                    {
                        m_bSpecialUsed = true;
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_ACID_SPLATTER : SPELL_ACID_SPLATTER_H, true);
                    }
                    break;
                default:
                    break;
            }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ENRAGE_SOFT:
                    m_creature->CastSpell(m_creature, SPELL_ENRAGE_SOFT, false);
                    break;
                case EVENT_MORTAL_WOUND:
                    m_creature->CastSpell(m_creature->getVictim(),m_bIsRegularMode ? SPELL_MORTAL_WOUND : SPELL_MORTAL_WOUND_H, false);
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    m_creature->CastSpell(m_creature->getVictim(),m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H, false);
                    break;
                case EVENT_TERRIFYING_ROAR:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_TERRIFYING_ROAR, true);
                    break;
                case EVENT_ACID_SPIT:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_ACID_SPIT, true);
                    break;
                case EVENT_POISON_BREATH:
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POISON_BREATH : SPELL_POISON_BREATH_H, true);
                    break;
                case EVENT_GORE:
                    m_creature->CastSpell(m_creature->getVictim(),m_bIsRegularMode ? SPELL_GORE : SPELL_GORE_H, true);
                    break;
                case EVENT_STOMP:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_STOMP, true);
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pSlayer)
    {
        BroadcastScriptMessageToEntry(m_creature, NPC_GORTOK_CONTROLLER, 100.0f, MESSAGE_DIED);
    }
};

/*######
## boss_gortok
######*/

struct MANGOS_DLL_DECL boss_gortokAI : public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance *m_pInstance;
    EventManager Events;
    bool m_bIsRegularMode :1;
    bool m_bAwaken :1;
    uint32 m_uiAwakeTimer;

    boss_gortokAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsRegularMode(pCreature->GetMap()->IsRegularDifficulty()),
        m_bAwaken(false),
        m_uiAwakeTimer(0)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_AWAKEN:
                m_bAwaken = true;
                m_uiAwakeTimer = 7*IN_MILLISECONDS;
                if (pSender->GetTypeId() == TYPEID_UNIT)
                    SendScriptMessageTo(static_cast<Creature*>(pSender), m_creature, MESSAGE_ACK, 0);
                break;
            default:
                break;
        }
    }

    void Reset()
    {
        m_bAwaken = false;
        m_uiAwakeTimer = 0;
        Events.Reset();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        BroadcastScriptMessageToEntry(m_creature, NPC_GORTOK_CONTROLLER, 100.0f, MESSAGE_WIPE);
    }

    void JustReachedHome()
    {        
        DoCast(m_creature, SPELL_FREEZE_ANIM, false);
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_ARCING_SMASH, 5*IN_MILLISECONDS, 7*IN_MILLISECONDS, 10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        Events.ScheduleEventInRange(EVENT_IMPALE, 5*IN_MILLISECONDS, 7*IN_MILLISECONDS, 10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_WITHERING_ROAR, 10*IN_MILLISECONDS, 10*IN_MILLISECONDS);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bAwaken)
        {
            if (m_uiAwakeTimer <= uiDiff)
            {
                m_bAwaken = false;
                m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetInCombatWithZone();
            }
            else
                m_uiAwakeTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ARCING_SMASH:
                    DoCast(m_creature->getVictim(), SPELL_ARCING_SMASH, false);
                    break;
                case EVENT_IMPALE:
                    DoCast(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0), m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H, false);
                    break;
                case EVENT_WITHERING_ROAR:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_WITHERING_ROAR : SPELL_WITHERING_ROAR_H, false);
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

CreatureAI* GetAI_boss_gortok_beasts(Creature* pCreature)
{
    return new boss_gortok_beastsAI(pCreature);
}

CreatureAI* GetAI_npc_gortok_controller(Creature* pCreature)
{
    return new npc_gortok_controllerAI(pCreature);
}

void AddSC_boss_gortok()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_gortok";
    newscript->GetAI = &GetAI_boss_gortok;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_gortok_beasts";
    newscript->GetAI = &GetAI_boss_gortok_beasts;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_gortok_controller";
    newscript->GetAI = &GetAI_npc_gortok_controller;
    newscript->RegisterSelf();
}
