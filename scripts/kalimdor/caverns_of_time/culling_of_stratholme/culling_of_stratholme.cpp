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
SDName: culling_of_stratholme
SD%Complete: 5%
SDComment: Placeholder
SDCategory: Culling of Stratholme
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"
#include "culling_of_stratholme.h"

/* *************
** npc_michael_belfast
************* */
enum
{
    SAY_MICHAEL_01      = ENTRY - 0,
    SAY_MICHAEL_02      = ENTRY - 1,
    SAY_FORRESTEN_01    = ENTRY - 2,
    SAY_JAMES_01        = ENTRY - 3,
    SAY_SIABI_01        = ENTRY - 4,
    SAY_MICHAEL_03      = ENTRY - 5,
    SAY_CORRICK_01      = ENTRY - 6,
    SAY_STOUTMANTLE_01  = ENTRY - 7,
    EMOTE_CORRICK_01    = ENTRY - 8,
    SAY_CORRICK_02      = ENTRY - 9,
    SAY_JAMES_02        = ENTRY - 10,
    SAY_FORRESTEN_02    = ENTRY - 11,
    EMOTE_SIABI_01      = ENTRY - 12,
};

struct MANGOS_DLL_DECL npc_michael_belfastAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    bool m_bStarted : 1;
    bool firstWarning : 1;
    bool secondWarning : 1;

    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;
    uint32 m_uiWarningTimer;

    npc_michael_belfastAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bStarted = false;
        Reset();
    }

    void Reset()
    {
        firstWarning = false;
        secondWarning = false;

        m_uiWarningTimer = 25*IN_MILLISECONDS;
        m_uiTalkPhase = 0;
        m_uiTalkTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bStarted && pWho->GetTypeId() == TYPEID_PLAYER && !firstWarning)
        {
            m_uiWarningTimer = 10*IN_MILLISECONDS;
            firstWarning = true;
        }
        if (!m_bStarted && pWho->GetTypeId() == TYPEID_PLAYER && !secondWarning && !m_uiWarningTimer)
        {
            m_bStarted = true;
            secondWarning = true;
            m_creature->SetFacingToObject(pWho);
            m_creature->MonsterSay(urand(0,1)? SAY_MICHAEL_01 : SAY_MICHAEL_02, 0);
            m_uiTalkPhase = 1;
            m_uiTalkTimer = 15*IN_MILLISECONDS;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiWarningTimer && m_uiWarningTimer <= uiDiff)
        {
            m_uiWarningTimer = 0;
        }
        else m_uiWarningTimer -= uiDiff;
        if (m_uiTalkPhase)
        {
            if (m_uiTalkTimer <= uiDiff)
            {
                switch (m_uiTalkPhase)
                {
                    case 1:
                        if (Unit* pForresten = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_HEARTHSINGER_FORRESTEN)))
                        {
                            pForresten->HandleEmote(EMOTE_ONESHOT_TALK);
                            pForresten->MonsterSay(SAY_FORRESTEN_01, 0);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 2:
                        if (Unit* pForresten = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_HEARTHSINGER_FORRESTEN)))
                            pForresten->HandleEmote(EMOTE_ONESHOT_QUESTION);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 3:
                        if (Unit* pJames = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FOOTMAN_JAMES)))
                        {
                            pJames->MonsterSay(SAY_JAMES_01, 0);
                            pJames->HandleEmote(EMOTE_ONESHOT_TALK);
                        }
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 4:
                        if (Unit* pSiabi = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FRAS_SIABI)))
                        {
                            pSiabi->HandleEmote(EMOTE_ONESHOT_TALK);
                            pSiabi->MonsterSay(SAY_SIABI_01, 0);
                        }
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 5:
                        if (Unit* pSiabi = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FRAS_SIABI)))
                            pSiabi->HandleEmote(EMOTE_ONESHOT_QUESTION);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 6:
                        if (Unit* pSiabi = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FRAS_SIABI)))
                            pSiabi->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 7:
                        m_creature->MonsterMoveWithSpeed(1555.643f, 589.727f, 99.775f, 2*IN_MILLISECONDS);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 8:
                        m_creature->MonsterSay(SAY_MICHAEL_03, 0);
                        m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 9:
                        if (Unit* pCorrick = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MAL_CORRICKS)))
                        {
                            pCorrick->HandleEmote(EMOTE_ONESHOT_TALK);
                            pCorrick->MonsterSay(SAY_CORRICK_01, 0);
                        }
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 10:
                        if (Unit* pStoutMantle = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_GRYAN_STOUTMANTLE)))
                        {
                            pStoutMantle->HandleEmote(EMOTE_ONESHOT_TALK);
                            pStoutMantle->MonsterSay(SAY_STOUTMANTLE_01, 0);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 11:
                    case 12:
                        if (Unit* pStoutMantle = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_GRYAN_STOUTMANTLE)))
                            pStoutMantle->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 13:
                        if (Unit* pCorrick = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MAL_CORRICKS)))
                        {
                            pCorrick->HandleEmote(EMOTE_ONESHOT_TALK);
                            pCorrick->MonsterTextEmote(EMOTE_CORRICK_01,NULL, 0);
                            pCorrick->MonsterSay(SAY_CORRICK_02, NULL, 0);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 14:
                        if (Unit* pJames = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FOOTMAN_JAMES)))
                        {
                            pJames->HandleEmote(EMOTE_ONESHOT_TALK);
                            pJames->MonsterSay(SAY_JAMES_02, 0);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 15:
                        if (Unit* pJames = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FOOTMAN_JAMES)))
                            pJames->HandleEmote(EMOTE_ONESHOT_TALK);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 16:
                        if (Unit* pForresten = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_HEARTHSINGER_FORRESTEN)))
                        {
                            pForresten->HandleEmote(EMOTE_ONESHOT_TALK);
                            pForresten->MonsterSay(SAY_FORRESTEN_02, 0);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 17:
                        if (Unit* pSiabi = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_FRAS_SIABI)))
                            pSiabi->MonsterTextEmote(EMOTE_SIABI_01, NULL);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 18:
                        m_uiTalkPhase = 0;
                        EnterEvadeMode();
                        break;
                    default:
                        m_uiTalkPhase = 0;
                        break;
                }
                if (m_uiTalkPhase)
                    m_uiTalkPhase++;
            }
            else m_uiTalkTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_michael_belfast(Creature* pCreature)
{
    return new npc_michael_belfastAI(pCreature);
};

/* *************
** npc_plagued_grain_event
************* */

enum Messages
{
    MESSAGE_START = 1,
    MESSAGE_END,
    MESSAGE_SALUTE,
};

enum
{
    // NPC_ROGER_OWENS event
    SAY_OWENS_01 = ENTRY - 13,
    SAY_OWENS_02 = ENTRY - 14,
    SAY_OWENS_03 = ENTRY - 15,
    SAY_OWENS_04 = ENTRY - 16,
    SAY_OWENS_05 = ENTRY - 17,

    // NPC_SERGEANT_MORIGAN event
    SAY_MORIGAN_01  = ENTRY - 18,
    SAY_PERELI_01   = ENTRY - 19,
    SAY_MORIGAN_02  = ENTRY - 20,
    SAY_PERELI_02   = ENTRY - 21,
    SAY_MORIGAN_03  = ENTRY - 22,
    SAY_PERELI_03   = ENTRY - 23,
    SAY_MORIGAN_04  = ENTRY - 24,
    SAY_PERELI_04   = ENTRY - 25,
    SAY_MORIGAN_05  = ENTRY - 26,

    // NPC_JENA_ANDERSON event
    SAY_ANDERSON_01 = ENTRY - 27,
    SAY_GOSLIN_01   = ENTRY - 28,
    SAY_ANDERSON_02 = ENTRY - 29,
    SAY_ANDERSON_03 = ENTRY - 30,
    SAY_ANDERSON_04 = ENTRY - 31,
    SAY_GOSLIN_02   = ENTRY - 32,

    // NPC_MALCOM_MOORE event
    SAY_MOORE_01    = ENTRY - 33,
    EMOTE_SCRUFFY01 = ENTRY - 34,
    SAY_MOORE_02    = ENTRY - 35,
    SAY_MOORE_03    = ENTRY - 36,
    SAY_MOORE_04    = ENTRY - 37,

    // NPC_BARTLEBY_BATTSON event
    SAY_BATTSON_01  = ENTRY - 38,
    SAY_BATTSON_02  = ENTRY - 39,
    SAY_BATTSON_03  = ENTRY - 40,
    SAY_BATTSON_04  = ENTRY - 41,
};

struct MANGOS_DLL_DECL npc_plagued_grain_eventAI : public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance* m_pInstance;
    bool m_bStarted : 1;

    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;

    npc_plagued_grain_eventAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        m_bStarted = 0;
        m_uiTalkPhase = 0;
        m_uiTalkTimer = 0;
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_START && !m_bStarted)
        {
            m_bStarted = true;
            m_uiTalkPhase = 1;
            m_uiTalkTimer = 1*IN_MILLISECONDS;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiTalkPhase)
        {
            if (m_uiTalkTimer <= uiDiff)
            {
                switch(m_creature->GetEntry())
                {
                    case NPC_ROGER_OWENS:
                        switch (m_uiTalkPhase)
                        {
                            case 1:
                                m_creature->MonsterSay(SAY_OWENS_01, 0);
                                m_creature->MonsterMoveWithSpeed(1591.2f, 611.923f, 99.69f, 4*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 2:
                                m_creature->MonsterMoveWithSpeed(1584.74f, 625.218f, 100.06f, 4*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 3:
                                m_creature->MonsterMoveWithSpeed(1580.32f, 626.52f, 100.019f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 4:
                                m_creature->MonsterMoveWithSpeed(1577.46f, 621.85f, 99.66f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 5:
                                m_creature->MonsterSay(SAY_OWENS_02, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 6:
                                m_creature->MonsterSay(SAY_OWENS_03, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                m_uiTalkTimer = 5*IN_MILLISECONDS;
                                break;
                            case 7:
                                m_creature->MonsterSay(SAY_OWENS_04, 0);
                                m_creature->SetFacingTo(6.04f);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 8:
                                m_creature->MonsterSay(SAY_OWENS_05, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 9:
                                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 10:
                                m_creature->MonsterMoveWithSpeed(1590.175f, 651.499f, 101.494f, 4*IN_MILLISECONDS);
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 11:
                                m_uiTalkPhase = 0;
                                m_creature->ForcedDespawn();
                                break;
                            default:
                                m_uiTalkPhase = 0;
                                break;
                        }
                        if (m_uiTalkPhase)
                            m_uiTalkPhase++;
                        break;
                    case NPC_SERGEANT_MORIGAN:
                        switch (m_uiTalkPhase)
                        {
                            case 1:
                                m_creature->MonsterSay(SAY_MORIGAN_01, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 2:
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 3:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                {
                                    pPerelli->MonsterSay(SAY_PERELI_01, 0);
                                    pPerelli->HandleEmote(EMOTE_ONESHOT_NO);
                                }
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 4:
                                m_creature->MonsterSay(SAY_MORIGAN_02, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 5:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                {
                                    pPerelli->MonsterSay(SAY_PERELI_02, 0);
                                    pPerelli->HandleEmote(EMOTE_ONESHOT_NO);
                                }
                                m_creature->MonsterMoveWithSpeed(1569.26f, 666.364f, 102.146f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 6:
                                m_creature->MonsterMoveWithSpeed(1570.0287f, 668.689f, 102.207f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 7:
                                m_creature->MonsterSay(SAY_MORIGAN_03, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 8:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                    pPerelli->MonsterMoveWithSpeed(1569.75f, 665.63f, 102.138f, 1*IN_MILLISECONDS);
                                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 9:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                {
                                    pPerelli->MonsterSay(SAY_PERELI_03, 0);
                                    pPerelli->SetFacingToObject(m_creature);
                                }
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 10:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                    m_creature->SetFacingToObject(pPerelli);
                                m_uiTalkTimer = 500;
                                break;
                            case 11:
                                m_creature->MonsterSay(SAY_MORIGAN_04, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 12:
                            case 13:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 14:
                                if (Unit* pPerelli = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                {
                                    pPerelli->MonsterSay(SAY_PERELI_04, 0);
                                    pPerelli->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                }
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 15:
                                m_creature->MonsterSay(SAY_MORIGAN_05, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 16:
                                if (Creature* pPerelli = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                    pPerelli->GetMotionMaster()->MoveFollow(m_creature, 3.0f, -M_PI_F/4);
                                m_creature->MonsterMoveWithSpeed(1584.827f, 647.325f, 101.0272f, 5*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 17:
                                m_uiTalkPhase = 0;
                                if (Creature* pPerelli = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SILVIO_PERELLI)))
                                    pPerelli->ForcedDespawn();
                                m_creature->ForcedDespawn();
                                break;
                            default:
                                m_uiTalkPhase = 0;
                                break;
                        }
                        if (m_uiTalkPhase)
                            m_uiTalkPhase++;
                        break;
                    case NPC_JENA_ANDERSON:
                        switch (m_uiTalkPhase)
                        {
                            case 1:
                                m_creature->MonsterMoveWithSpeed(1603.494f, 745.877f, 114.732f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 2:
                                m_creature->MonsterMoveWithSpeed(1614.302f, 743.196f, 114.045f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 3:
                                m_creature->MonsterMoveWithSpeed( 1623.671f, 726.00f, 112.048f, 3*IN_MILLISECONDS);
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 4:
                                m_creature->MonsterMoveWithSpeed(1631.398f, 726.01f, 112.878f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 5:
                                m_creature->MonsterSay(SAY_ANDERSON_01, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 6:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 7:
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 8:
                                if (Unit* pGoslin = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                {
                                    pGoslin->SetFacingToObject(m_creature);
                                }
                                m_uiTalkTimer = 500;
                                break;
                            case 9:
                                if (Unit* pGoslin = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                {
                                    pGoslin->MonsterSay(SAY_GOSLIN_01, 0);
                                    pGoslin->HandleEmote(EMOTE_ONESHOT_TALK);
                                }
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 10:
                                if (Unit* pGoslin = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                    pGoslin->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 11:
                                m_creature->MonsterSay(SAY_ANDERSON_02, 0);
                                m_creature->MonsterMoveWithSpeed(1629.379f, 729.605f, 112.727f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 12:
                                m_creature->MonsterSay(SAY_ANDERSON_03, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 13:
                                m_creature->MonsterSay(SAY_ANDERSON_04, 0);
                                if (Unit* pGoslin = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                    pGoslin->MonsterMoveWithSpeed(1631.398f, 726.01f, 112.878f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 14:
                                if (Unit* pGoslin = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                {
                                    pGoslin->SetFacingToObject(m_creature);
                                    pGoslin->MonsterSay(SAY_GOSLIN_02, 0);
                                }
                                m_creature->MonsterMoveWithSpeed(1618.939f, 727.089f, 111.797f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 15:
                                m_creature->MonsterMoveWithSpeed(1618.939f, 727.089f, 111.797f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 16:
                                m_creature->MonsterMoveWithSpeed(1622.136f, 748.746f, 114.792f, 3*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 17:
                                m_uiTalkPhase = 0;
                                if (Creature* pGoslin = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_MARTHA_GOSLIN)))
                                    pGoslin->ForcedDespawn();
                                m_creature->ForcedDespawn();
                                break;
                            default:
                                m_uiTalkPhase = 0;
                                break;
                        }
                        if (m_uiTalkPhase)
                            m_uiTalkPhase++;
                        break;
                    case NPC_MALCOM_MOORE:
                        switch (m_uiTalkPhase)
                        {
                            case 1:
                                if (Creature* pScruffy = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SCRUFFY)))
                                    pScruffy->MonsterMoveWithSpeed(1628.327f, 815.744f, 121.188f, 3*IN_MILLISECONDS);
                                m_creature->MonsterMoveWithSpeed(1631.2f, 817.08f, 122.66f, 3*IN_MILLISECONDS);
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 2:
                                m_creature->MonsterSay(SAY_MOORE_01, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 3:
                                if (Creature* pScruffy = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SCRUFFY)))
                                    pScruffy->MonsterMoveWithSpeed(1628.517f, 813.88f, 120.93f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 4:
                                if (Creature* pScruffy = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SCRUFFY)))
                                    pScruffy->MonsterTextEmote(EMOTE_SCRUFFY01, NULL);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 5:
                                m_creature->MonsterSay(SAY_MOORE_02, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_QUESTION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 6:
                                m_creature->MonsterSay(SAY_MOORE_03, 0);
                                m_creature->MonsterMoveWithSpeed(1630.003f, 813.708f, 120.826f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 7:
                                m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 8:
                                m_creature->MonsterSay(SAY_MOORE_04, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 9:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 10:
                                m_creature->MonsterMoveWithSpeed(1637.567f, 807.985f, 119.896f, 2*IN_MILLISECONDS);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 11:
                                m_creature->MonsterMoveWithSpeed(1657.499f, 856.545f, 119.075f, 6*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 12:
                                m_uiTalkPhase = 0;
                                if (Creature* pScruffy = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_SCRUFFY)))
                                    pScruffy->ForcedDespawn();
                                m_creature->ForcedDespawn();
                                break;
                            default:
                                m_uiTalkPhase = 0;
                                break;
                        }
                        if (m_uiTalkPhase)
                            m_uiTalkPhase++;
                        break;
                    case NPC_BARTLEBY_BATTSON:
                        switch (m_uiTalkPhase)
                        {
                            case 1:
                                m_creature->MonsterMoveWithSpeed(1672.509f, 872.478f, 120.082f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 2:
                                m_creature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 3:
                                m_creature->MonsterSay(SAY_BATTSON_01, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_NONE);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 4:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 5:
                                m_creature->MonsterSay(SAY_BATTSON_02, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 3*IN_MILLISECONDS;
                                break;
                            case 6:
                                m_creature->MonsterSay(SAY_BATTSON_03, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 7:
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 8:
                                m_creature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 9:
                                m_creature->MonsterSay(SAY_BATTSON_04, 0);
                                m_creature->HandleEmote(EMOTE_ONESHOT_TALK);
                                m_uiTalkTimer = 2*IN_MILLISECONDS;
                                break;
                            case 10:
                                m_creature->MonsterMoveWithSpeed(1665.012f, 873.915f, 119.803f, 1*IN_MILLISECONDS);
                                m_uiTalkTimer = 1*IN_MILLISECONDS;
                                break;
                            case 11:
                                m_creature->MonsterMoveWithSpeed(1677.852f, 905.55f, 120.22f, 5*IN_MILLISECONDS);
                                m_uiTalkTimer = 4*IN_MILLISECONDS;
                                break;
                            case 12:
                                m_uiTalkPhase = 0;
                                m_creature->ForcedDespawn();
                                break;
                            default:
                                m_uiTalkPhase = 0;
                                break;
                        }
                        if (m_uiTalkPhase)
                            m_uiTalkPhase++;
                        break;
                }
            }
            else m_uiTalkTimer -= uiDiff;
        }
    }

};

CreatureAI* GetAI_npc_plagued_grain_event(Creature* pCreature)
{
    return new npc_plagued_grain_eventAI(pCreature);
};

/* *************
** npc_uther_cos (intro controller)
************* */

enum Says
{
    // Intro: prologue
    SAY_ARTHAS_ENTRANCE_01  = ENTRY - 50,
    SAY_UTHER_ENTRANCE_01   = ENTRY - 51,
    SAY_ARTHAS_ENTRANCE_02  = ENTRY - 52,
    SAY_ARTHAS_ENTRANCE_03  = ENTRY - 53,
    SAY_UTHER_ENTRANCE_02   = ENTRY - 54,
    SAY_ARTHAS_ENTRANCE_04  = ENTRY - 55,
    SAY_UTHER_ENTRANCE_03   = ENTRY - 56,
    SAY_ARTHAS_ENTRANCE_05  = ENTRY - 57,
    SAY_UTHER_ENTRANCE_04   = ENTRY - 58,
    SAY_ARTHAS_ENTRANCE_06  = ENTRY - 59,
    SAY_UTHER_ENTRANCE_05   = ENTRY - 60,
    SAY_ARTHAS_ENTRANCE_07  = ENTRY - 61,
    SAY_JAINA_ENTRANCE_01   = ENTRY - 62,
    SAY_ARTHAS_ENTRANCE_08  = ENTRY - 63,
    SAY_UTHER_ENTRANCE_06   = ENTRY - 64,
    SAY_ARTHAS_ENTRANCE_09  = ENTRY - 65,
    SAY_JAINA_ENTRANCE_02   = ENTRY - 66,
    SAY_ARTHAS_ENTRANCE_10  = ENTRY - 67,

    // at the gate - says

    SAY_ARTHAS_PAUSE_01     = ENTRY - 68,
    SAY_ARTHAS_GATES_01     = ENTRY - 69,
    SAY_CITIZEN_GATES_01    = ENTRY - 70,
    SAY_ARTHAS_GATES_02     = ENTRY - 71,
    SAY_CITIZEN_GATES_03    = ENTRY - 72,
    SAY_RESIDENT_GATES_01   = ENTRY - 73,
    SAY_ARTHAS_GATES_03     = ENTRY - 74,
    SAY_MALGANIS_GATES_01   = ENTRY - 75,
    SAY_MALGANIS_GATES_02   = ENTRY - 76,
    SAY_ARTHAS_GATES_04     = ENTRY - 77,
    SAY_ARTHAS_GATES_10     = ENTRY - 78,

    // Town Hall : trap
    SAY_ARTHAS_WAVES_OVER   = ENTRY - 130,
    SAY_ARTHAS_EPOCH_01     = ENTRY - 131,
    SAY_CITIZEN_EPOCH_01    = ENTRY - 132,
    SAY_ARTHAS_EPOCH_02     = ENTRY - 133,
    SAY_ARTHAS_EPOCH_03     = ENTRY - 134,
    SAY_CITIZEN_EPOCH_02    = ENTRY - 135,

    // up till Epoch
    SAY_ARTHAS_EPOCH_04     = ENTRY - 136,
    SAY_ARTHAS_EPOCH_05     = ENTRY - 137,
    SAY_ARTHAS_EPOCH_06     = ENTRY - 138,
    SAY_AGENT_EPOCH_01      = ENTRY - 139,
    SAY_ARTHAS_EPOCH_07     = ENTRY - 140,
    SAY_ARTHAS_EPOCH_08     = ENTRY - 141,
    SAY_ARTHAS_EPOCH_09     = ENTRY - 142,
    SAY_ARTHAS_EPOCH_10     = ENTRY - 143,
    SAY_ARTHAS_EPOCH_11     = ENTRY - 144,

    //  Epoch
    SAY_EPOCH_INTRO         = ENTRY - 145,
    SAY_ARTHAS_EPOCH_12     = ENTRY - 146,
    SAY_EPOCH_AGGRO         = ENTRY - 147,

    // Bookshelf till Alley
    SAY_ARTHAS_EPOCH_13     = ENTRY - 155,
    SAY_ARTHAS_EPOCH_14     = ENTRY - 156,
    SAY_ARTHAS_EPOCH_15     = ENTRY - 157,

    // Alley till Mal'Ganis
    SAY_ARTHAS_ALLEY_01     = ENTRY - 158,
    SAY_ARTHAS_ALLEY_02     = ENTRY - 159,
    SAY_ARTHAS_ALLEY_03     = ENTRY - 160,
    SAY_ARTHAS_ALLEY_04     = ENTRY - 161,

    // Mal'Ganis
    SAY_ARTHAS_MALGANIS_START   = ENTRY - 162,
    SAY_ARTHAS_MALGANIS_INTRO   = ENTRY - 163,
    SAY_MALGANIS_AGGRO          = ENTRY - 164,
    SAY_MALGANIS_ARTHAS_OUTRO_01 = ENTRY - 171,
    SAY_MALGANIS_ARTHAS_OUTRO_02 = ENTRY - 172,

    WAVES_DONE_SOUND = 14297,
};

struct MANGOS_DLL_DECL npc_uther_cosAI : public ScriptedAI, public ScriptMessageInterface
{
    ScriptedInstance* m_pInstance;
    SummonManager SummonMgr;

    ObjectGuid m_Knight1GUID;
    ObjectGuid m_Knight2GUID;
    ObjectGuid m_Knight3GUID;

    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;

    npc_uther_cosAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        m_uiTalkPhase = 0;
        m_uiTalkTimer = 0;
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_START)
        {
            m_uiTalkPhase = 1;
            m_uiTalkTimer = 4*IN_MILLISECONDS;

            if (Creature* pKnight1 = SummonMgr.SummonCreature(NPC_KNIGHT_OF_THE_SILVER_HAND, m_creature->GetPositionX(), m_creature->GetPositionY(),
                m_creature->GetPositionZ()+1.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 150*IN_MILLISECONDS))
            {
                m_Knight1GUID = pKnight1->GetObjectGuid();
                pKnight1->CastSpell(pKnight1, 23214, true); // summon charger (rank 2)
                pKnight1->MonsterMoveWithSpeed(1767.51f, 1262.426f, 138.446f, 7*IN_MILLISECONDS);
            }
            if (Creature* pKnight2 = SummonMgr.SummonCreature(NPC_KNIGHT_OF_THE_SILVER_HAND, m_creature->GetPositionX(), m_creature->GetPositionY(),
                m_creature->GetPositionZ()+1.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 150*IN_MILLISECONDS))
            {
                m_Knight2GUID = pKnight2->GetObjectGuid();
                pKnight2->CastSpell(pKnight2, 23214, true); // summon charger (rank 2)
                pKnight2->MonsterMoveWithSpeed(1769.318f, 1257.63f, 138.51f, 7*IN_MILLISECONDS);
            }
            if (Creature* pKnight3 = SummonMgr.SummonCreature(NPC_KNIGHT_OF_THE_SILVER_HAND, m_creature->GetPositionX(), m_creature->GetPositionY(),
                m_creature->GetPositionZ()+1.0f, m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 150*IN_MILLISECONDS))
            {
                m_Knight3GUID = pKnight3->GetObjectGuid();
                pKnight3->CastSpell(pKnight3, 23214, true); // summon charger (rank 2)
                pKnight3->MonsterMoveWithSpeed(1766.196f, 1265.91f, 138.471f, 7*IN_MILLISECONDS);
            }
            if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                pArthas->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            SummonMgr.SummonCreature(NPC_JAINA_PROUDMOORE, 1899.262f, 1296.945f, 143.758f, 5.721f, TEMPSUMMON_TIMED_DESPAWN, 150*IN_MILLISECONDS);
            m_creature->MonsterMoveWithSpeed(1770.825f, 1263.675f, 138.73f, 7*IN_MILLISECONDS);
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiTalkPhase)
        {
            if (m_uiTalkTimer <= uiDiff)
            {
                switch (m_uiTalkPhase)
                {
                    case 1:
                        m_creature->MonsterMoveWithSpeed(1810.577f, 1275.709f, 141.82f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight1 = m_creature->GetMap()->GetUnit(m_Knight1GUID))
                            pKnight1->MonsterMoveWithSpeed(1807.416f, 1274.519f, 141.566f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight2 = m_creature->GetMap()->GetUnit(m_Knight2GUID))
                            pKnight2->MonsterMoveWithSpeed(1808.793f, 1269.593f, 141.375f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight3 = m_creature->GetMap()->GetUnit(m_Knight3GUID))
                            pKnight3->MonsterMoveWithSpeed(1804.043f, 1278.784f, 141.638f, 7*IN_MILLISECONDS);
                        m_uiTalkTimer = 7*IN_MILLISECONDS;
                        break;
                    case 2:
                        m_creature->MonsterMoveWithSpeed(1867.416f, 1282.608f, 144.187f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight1 = m_creature->GetMap()->GetUnit(m_Knight1GUID))
                            pKnight1->MonsterMoveWithSpeed(1864.524f, 1282.171f, 144.19f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight2 = m_creature->GetMap()->GetUnit(m_Knight2GUID))
                            pKnight2->MonsterMoveWithSpeed(1865.152f, 1278.019f, 144.07f, 7*IN_MILLISECONDS);
                        if (Unit* pKnight3 = m_creature->GetMap()->GetUnit(m_Knight3GUID))
                            pKnight3->MonsterMoveWithSpeed(1864.029f, 1285.437f, 144.35f, 7*IN_MILLISECONDS);
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 3:
                        if (Creature* pSumm = ((instance_culling_of_stratholme*)m_pInstance)->GetStratIntroFootman())
                            pSumm->MonsterSay("The Lightbringer...", 0);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 4:
                        m_creature->MonsterMoveWithSpeed(1887.609f, 1285.245f, 143.722f, 3*IN_MILLISECONDS);
                        if (Unit* pKnight1 = m_creature->GetMap()->GetUnit(m_Knight1GUID))
                            pKnight1->MonsterMoveWithSpeed(1884.996f, 1285.017f, 143.778f, 3*IN_MILLISECONDS);
                        if (Unit* pKnight2 = m_creature->GetMap()->GetUnit(m_Knight2GUID))
                            pKnight2->MonsterMoveWithSpeed(1885.330f, 1281.195f, 143.726f, 3*IN_MILLISECONDS);
                        if (Unit* pKnight3 = m_creature->GetMap()->GetUnit(m_Knight3GUID))
                            pKnight3->MonsterMoveWithSpeed(1884.721f, 1288.151f, 143.864f, 3*IN_MILLISECONDS);
                        if (Creature* pSumm = ((instance_culling_of_stratholme*)m_pInstance)->GetStratIntroFootman())
                            pSumm->MonsterSay("Lord Uther...", 0);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 5:
                        m_creature->MonsterMoveWithSpeed(1897.049f, 1290.452f, 143.569f, 5*IN_MILLISECONDS);
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            pArthas->MonsterMoveWithSpeed(1905.341f, 1293.76f, 143.214f, 3*IN_MILLISECONDS);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 6:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            pArthas->SetFacingToObject(m_creature);
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 7:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                        {
                            DoScriptText(SAY_ARTHAS_ENTRANCE_01, pArthas, m_creature);
                            m_creature->SetFacingToObject(pArthas);
                        }
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 8:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_UTHER_ENTRANCE_01, m_creature, pArthas);
                        m_uiTalkTimer = 7*IN_MILLISECONDS;
                        break;
                    case 9:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                        {
                            pArthas->MonsterMoveWithSpeed(1912.994f, 1314.24f, 149.679f, 6*IN_MILLISECONDS);
                            DoScriptText(SAY_ARTHAS_ENTRANCE_02, pArthas, m_creature);
                        }
                        m_creature->MonsterMoveWithSpeed(1903.647f, 1295.82f, 143.34f, 3*IN_MILLISECONDS);
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 10:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                        {
                            if (Unit* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                                pJaina->SetFacingToObject(pArthas);
                            m_creature->SetFacingToObject(pArthas);
                        }
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 11:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                        {
                            pArthas->SetFacingTo(0.2f);
                            m_creature->SetFacingToObject(pArthas);
                        }
                        m_uiTalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 12:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_03, pArthas);
                        m_uiTalkTimer = 10*IN_MILLISECONDS;
                        break;
                    case 13:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            pArthas->SetFacingToObject(m_creature);
                        DoScriptText(SAY_UTHER_ENTRANCE_02, m_creature);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 14:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_04, pArthas, m_creature);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 15:
                        DoScriptText(SAY_UTHER_ENTRANCE_03, m_creature);
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 16:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_05, pArthas, m_creature);
                        m_uiTalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 17:
                        DoScriptText(SAY_UTHER_ENTRANCE_04, m_creature);
                        m_uiTalkTimer = 8*IN_MILLISECONDS;
                        break;
                    case 18:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_06, pArthas, m_creature);
                        m_uiTalkTimer = 3500;
                        break;
                    case 19:
                        DoScriptText(SAY_UTHER_ENTRANCE_05, m_creature);
                        m_uiTalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 20:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_07, pArthas, m_creature);
                        m_uiTalkTimer = 12*IN_MILLISECONDS;
                        break;
                    case 21:
                        break;
                    case 22:
                        if (Unit* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                            DoScriptText(SAY_JAINA_ENTRANCE_01, pJaina);
                        m_uiTalkTimer = 1*IN_MILLISECONDS;
                        break;
                    case 23:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            DoScriptText(SAY_ARTHAS_ENTRANCE_08, pArthas, m_creature);
                        m_uiTalkTimer = 8*IN_MILLISECONDS;
                        break;
                    case 24:
                        if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            pArthas->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        m_uiTalkTimer = 2*IN_MILLISECONDS;
                        break;
                    case 25:
                        DoScriptText(SAY_UTHER_ENTRANCE_06, m_creature);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 26:
                        if (Unit* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                            if (Unit* pArthas = m_pInstance->instance->GetUnit(m_pInstance->GetData64(NPC_ARTHAS)))
                            {
                                pArthas->SetFacingToObject(pJaina);
                                pJaina->MonsterMoveWithSpeed(1887.745f, 1288.047f, 143.77f, 5*IN_MILLISECONDS);
                                DoScriptText(SAY_ARTHAS_ENTRANCE_09, pArthas);
                            }
                        if (Unit* pKnight1 = m_creature->GetMap()->GetUnit(m_Knight1GUID))
                            pKnight1->MonsterMoveWithSpeed(1864.524f, 1282.171f, 144.19f, 10*IN_MILLISECONDS);
                        if (Unit* pKnight2 = m_creature->GetMap()->GetUnit(m_Knight2GUID))
                            pKnight2->MonsterMoveWithSpeed(1865.152f, 1278.019f, 144.07f, 10*IN_MILLISECONDS);
                        if (Unit* pKnight3 = m_creature->GetMap()->GetUnit(m_Knight3GUID))
                            pKnight3->MonsterMoveWithSpeed(1864.029f, 1285.437f, 144.35f, 10*IN_MILLISECONDS);

                        m_creature->MonsterMoveWithSpeed(1848.67f, 1280.26f, 144.079f, 15*IN_MILLISECONDS); // move slowly.. you just lost your job
                        m_uiTalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 27:
                        if (Unit* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                            DoScriptText(SAY_JAINA_ENTRANCE_02, pJaina);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 28:
                        if (Creature* pArthas = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_ARTHAS)))
                            SendScriptMessageTo(pArthas, m_creature, MESSAGE_START, 0);
                        if (Unit* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                            pJaina->MonsterMoveWithSpeed(1848.67f, 1280.26f, 144.079f, 7*IN_MILLISECONDS);
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    case 29:
                        m_uiTalkPhase = 0;
                        if (Creature* pJaina = SummonMgr.GetFirstFoundSummonWithId(NPC_JAINA_PROUDMOORE))
                            pJaina->ForcedDespawn();
                        if (Creature* pKnight1 = m_creature->GetMap()->GetCreature(m_Knight1GUID))
                            pKnight1->ForcedDespawn();
                        if (Creature* pKnight2 = m_creature->GetMap()->GetCreature(m_Knight2GUID))
                            pKnight2->ForcedDespawn();
                        if (Creature* pKnight3 = m_creature->GetMap()->GetCreature(m_Knight3GUID))
                            pKnight3->ForcedDespawn();
                        m_creature->ForcedDespawn();
                        break;
                    default:
                        m_uiTalkPhase = 0;
                        break;
                }
                if (m_uiTalkPhase)
                    m_uiTalkPhase++;
            } else m_uiTalkTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_uther_cos(Creature* pCreature)
{
    return new npc_uther_cosAI(pCreature);
};

/* *************
** npc_arthas_cs (main instance escort)
************* */

struct MANGOS_DLL_DECL npc_arthas_csAI: public npc_escortAI, public ScriptMessageInterface
{
    instance_culling_of_stratholme* m_pInstance;
    SummonManager SummonMgr;
    bool m_bIsRegularDifficulty : 1;
    bool m_bWavesDoneAnnounced : 1;
    bool m_bEpochDone : 1;
    bool m_bEscortDone : 1;

    uint32 m_uiPosition;
    uint32 m_uiTalkPhase;
    uint32 m_uiTalkTimer;

    npc_arthas_csAI(Creature* pCreature) : npc_escortAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<instance_culling_of_stratholme*>(pCreature->GetInstanceData());
        m_bIsRegularDifficulty = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetActiveObjectState(true);

        m_bWavesDoneAnnounced = false;
        m_bEpochDone = false;
        m_bEscortDone = false;
        Reset();
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_START)
        {
            npc_escortAI::Start();
        }
        else if (data1 == MESSAGE_END)
        {
            m_uiTalkPhase = 60;
            m_uiTalkTimer = 2*IN_MILLISECONDS;
        }
    }

    void Reset()
    {
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
    }

    void JustRespawned()
    {
        switch (uint32 pos = ((instance_culling_of_stratholme*)m_pInstance)->GetInstancePosition())
        {
            case POS_ARTHAS_WAVES:
            case POS_ARTHAS_INTRO:
                m_bWavesDoneAnnounced = false;
                break;
            case POS_ARTHAS_TOWNHALL:
                m_bWavesDoneAnnounced = true;
                m_bEpochDone = false;
                break;
            case POS_ARTHAS_ESCORTING:
                m_bWavesDoneAnnounced = true;
                m_bEpochDone = true;
                m_bEscortDone = false;
                break;
            case POS_ARTHAS_MALGANIS:
                m_bWavesDoneAnnounced = true;
                m_bEpochDone = true;
                m_bEscortDone = true;
                break;
            case POS_INSTANCE_FINISHED:
                m_bWavesDoneAnnounced = true;
                m_bEpochDone = true;
                m_bEscortDone = true;
                break;
        }
    }

    void WaypointStart(uint32 uiWP)
    {
        switch (uiWP)
        {
            case 7:
                BroadcastScriptMessage(m_creature, 20.0f, MESSAGE_START, 0, false);
                DoScriptText(SAY_ARTHAS_GATES_01, m_creature);
                SummonMgr.SummonCreature(NPC_STRATHOLME_CITIZEN, 2090.755f, 1274.257f, 141.316f, 0.333f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_STRATHOLME_RESIDENT, 2096.261f, 1276.116f, 139.867f, 3.494f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                break;
            case 16:
                SummonMgr.UnsummonAll();
                DoScriptText(SAY_ARTHAS_EPOCH_01, m_creature);
                SummonMgr.SummonCreature(NPC_AGIATED_STRATHOLME_CITIZEN, 2398.239f, 1206.76f, 134.039f, 3.56f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_AGIATED_STRATHOLME_RESIDENT, 2398.138f, 1201.868f, 134.035f, 2.81f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_AGIATED_STRATHOLME_CITIZEN, 2390.868f, 1206.281f, 134.039f, 5.773f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                break;
            case 18:
                DoScriptText(SAY_ARTHAS_EPOCH_05, m_creature);
                break;
            case 20:
                DoScriptText(SAY_ARTHAS_EPOCH_07, m_creature);
                break;
            case 24:
                DoScriptText(SAY_ARTHAS_EPOCH_09, m_creature);
                break;
            case 40:
                DoScriptText(SAY_ARTHAS_ALLEY_01, m_creature);
                break;
            case 44:
                DoScriptText(SAY_ARTHAS_ALLEY_03, m_creature);
                break;
            case 47:
                DoScriptText(SAY_ARTHAS_MALGANIS_START, m_creature);
                SummonMgr.SummonCreature(NPC_MALGANIS, 2297.658f, 1499.281f, 128.362f, 5.019f, TEMPSUMMON_MANUAL_DESPAWN);
                break;
        }
    }

    void WaypointReached(uint32 wp)
    {
        switch (wp)
        {
            case 1:
                SetRun(true);
                m_creature->SetFacingTo(3.152759f);
                DoScriptText(SAY_ARTHAS_ENTRANCE_10, m_creature);
                BroadcastScriptMessageToEntry(m_creature, NPC_LORDAERON_FOOTMAN, 75.0f, MESSAGE_END, 10*IN_MILLISECONDS);
                break;
            case 6:
                SetRun(false);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_creature->SetFacingTo(3.152759f);
                DoScriptText(SAY_ARTHAS_PAUSE_01, m_creature);
                SetEscortPaused(true);
                BroadcastScriptMessage(m_creature, 10.0f, MESSAGE_SALUTE, 4*IN_MILLISECONDS);
                break;
            case 8:
                m_uiTalkPhase = 1;
                m_uiTalkTimer = 2*IN_MILLISECONDS;
                SetEscortPaused(true);
                break;
            case 15:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetRun(false);
                SetEscortPaused(true);
                break;
            case 17:
                m_uiTalkPhase = 20;
                m_uiTalkTimer = 1*IN_MILLISECONDS;
                SetEscortPaused(true);
                break;
            case 18:
                DoScriptText(SAY_ARTHAS_EPOCH_06, m_creature);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_SMALL,  2421.153f, 1195.286f, 148.076f, 0, TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_AGENT,   2421.153f, 1195.286f, 148.076f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_HUNTER,  2421.153f, 1195.286f, 148.076f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_HUNTER,  2421.153f, 1195.286f, 148.076f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_ADVERSARY,  2421.153f, 1195.286f, 148.076f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                break;
            case 19:
            {
                uint32 pos = 0;
                std::list<ObjectGuid> summons = SummonMgr.GetSummonList();
                for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                {
                    if (Unit* pSumm = m_creature->GetMap()->GetUnit(*itr))
                    {
                        if (pSumm->GetEntry() == NPC_TIME_RIFT_SMALL)
                            continue;
                        pos++;
                        switch(pos)
                        {
                            case 1:
                                pSumm->MonsterMove(2427.3176f, 1186.786f, 148.076f, 2*IN_MILLISECONDS);
                                break;
                            case 2:
                                pSumm->MonsterMove(2433.919f, 1191.574f, 148.076f, 2*IN_MILLISECONDS);
                                break;
                            case 3:
                                pSumm->MonsterMove(2433.548f, 1196.474f, 148.076f, 2*IN_MILLISECONDS);
                                break;
                            case 4:
                                pSumm->MonsterMove(2431.926f, 1201.899f, 148.076f, 2*IN_MILLISECONDS);
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (Unit* Summ = SummonMgr.GetFirstFoundSummonWithId(NPC_INFINITE_AGENT))
                    DoScriptText(SAY_AGENT_EPOCH_01, Summ);
                break;
            }
            case 23:
                DoScriptText(SAY_ARTHAS_EPOCH_08, m_creature);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_SMALL, 2412.549f, 1141.698f, 148.076f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_HUNTER, 2414.511f, 1142.005f, 148.076f, 1.784f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_HUNTER, 2410.737f, 1141.517f, 148.076f, 1.784f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_SMALL, 2406.540f, 1170.591f, 148.076f, 0, TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_AGENT, 2403.867f, 1170.189f, 148.076f, 4.863f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_INFINITE_AGENT, 2408.231f, 1171.091f, 148.076f, 4.863f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS);
                break;
            case 24:
                DoScriptText(SAY_ARTHAS_EPOCH_10, m_creature);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_SMALL, 2446.32f, 1111.868f, 148.076f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS);
                if (Unit* pSumm = SummonMgr.SummonCreature(NPC_INFINITE_HUNTER, 2446.32f, 1111.868f, 148.076f, 3.24f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS))
                    pSumm->AddThreat(m_creature);
                if (Unit* pSumm = SummonMgr.SummonCreature(NPC_INFINITE_HUNTER, 2446.32f, 1111.868f, 148.076f, 3.24f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS))
                    pSumm->AddThreat(m_creature);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_SMALL, 2438.72f, 1097.07f, 149.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS);
                if (Unit* pSumm = SummonMgr.SummonCreature(NPC_INFINITE_AGENT, 2438.72f, 1097.07f, 149.0f, 4.863f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS))
                    pSumm->AddThreat(m_creature);
                if (Unit* pSumm = SummonMgr.SummonCreature(NPC_INFINITE_AGENT, 2438.72f, 1097.07f, 149.0f, 4.863f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2*IN_MILLISECONDS))
                    pSumm->AddThreat(m_creature);
                break;
            case 25:
                SetEscortPaused(true);
                m_pInstance->SetData(TYPE_EPOCH_EVENT, IN_PROGRESS);
                DoScriptText(SAY_ARTHAS_EPOCH_11, m_creature);
                SummonMgr.SummonCreature(NPC_TIME_RIFT_BIG, 2446.32f, 1111.868f, 148.076f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS);
                SummonMgr.SummonCreature(NPC_CHRONO_LORD_EPOCH, 2446.32f, 1111.868f, 148.076f, 3.24f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
                m_uiTalkPhase = 30;
                m_uiTalkTimer = 3*IN_MILLISECONDS;
                break;
            case 30:
                m_uiTalkPhase = 40;
                m_uiTalkTimer = 1*IN_MILLISECONDS;
                SetEscortPaused(true);
                break;
            case 39:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetEscortPaused(true);
                break;
            case 43:
                DoScriptText(SAY_ARTHAS_ALLEY_02, m_creature);
                break;
            case 46:
                m_pInstance->SetData(TYPE_ARTHAS_ESCORT_EVENT, DONE);
                DoScriptText(SAY_ARTHAS_ALLEY_04, m_creature);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetEscortPaused(true);
                break;
            case 48:
                m_pInstance->SetData(TYPE_MALGANIS_EVENT, IN_PROGRESS);
                m_uiTalkPhase = 50;
                m_uiTalkTimer = 2*IN_MILLISECONDS;
                SetEscortPaused(true);
                break;
            case 49:
                m_creature->SetFacingTo(2.16f);
                m_uiTalkPhase = 65;
                m_uiTalkTimer = 0;
                SetEscortPaused(true);
                break;
        }
    }
    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiTalkPhase)
            UpdateTalk(uiDiff);

        if (!m_bWavesDoneAnnounced && m_pInstance->GetInstancePosition() == POS_ARTHAS_TOWNHALL)
        {
            m_bWavesDoneAnnounced = true;
            m_creature->MonsterYellToZone(SAY_ARTHAS_WAVES_OVER, 0, NULL);
            SetRun(true);
            SetEscortPaused(false);
        }
        else if (!m_bEpochDone && m_pInstance->GetInstancePosition() == POS_ARTHAS_ESCORTING)
        {
            m_bEpochDone = true;
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        // must.
        npc_escortAI::UpdateAI(uiDiff);

        if (!m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }

    void UpdateTalk(uint32 const uiDiff)
    {
        if (m_uiTalkTimer <= uiDiff)
        {
            switch(m_uiTalkPhase)
            {
                // city gates
                case 1:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_CITIZEN))
                    {
                        m_creature->SetFacingToObject(pSumm);
                        pSumm->MonsterMoveWithSpeed(2089.191f, 1278.218f, 140.79f, 2*IN_MILLISECONDS);
                        DoScriptText(SAY_CITIZEN_GATES_01, pSumm);
                    }
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_RESIDENT))
                    {
                        pSumm->MonsterMoveWithSpeed(2092.647f, 1278.791f, 140.121f, 2*IN_MILLISECONDS);
                    }
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 2:
                {
                    std::list<ObjectGuid> summons = SummonMgr.GetSummonList();
                    for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    {
                        if (Unit* pSumm = m_creature->GetMap()->GetUnit(*itr))
                            pSumm->SetFacingToObject(m_creature);
                    }
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_CITIZEN))
                    {
                        m_creature->MonsterMoveWithSpeed(2088.746f, 1279.583f, 140.665f, 2*IN_MILLISECONDS);
                    }
                    DoScriptText(SAY_ARTHAS_GATES_02, m_creature);
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                }
                case 3:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_CITIZEN))
                    {
                        DoScriptText(SAY_CITIZEN_GATES_03, pSumm);
                    }
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 4:
                    m_creature->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_CITIZEN))
                    {
                        pSumm->DealDamage(pSumm, pSumm->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    }
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_RESIDENT))
                    {
                        pSumm->SetFacingToObject(m_creature);
                        DoScriptText(SAY_RESIDENT_GATES_01, pSumm); // oh no!
                    }
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 5:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_RESIDENT))
                    {
                        m_creature->MonsterMoveWithSpeed(2091.316f, 1278.935f, 140.325f, 1*IN_MILLISECONDS);
                    }
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 6:
                    m_creature->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_STRATHOLME_RESIDENT))
                    {
                        pSumm->DealDamage(pSumm, pSumm->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                    }
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 7:
                    DoScriptText(SAY_ARTHAS_GATES_03, m_creature);
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 8:
                    SummonMgr.SummonCreature(NPC_MALGANIS, 2137.171f, 1284.192f, 135.311f, 3.04f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 9:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_MALGANIS))
                    {
                        m_creature->SetFacingToObject(pSumm);
                        DoScriptText(SAY_MALGANIS_GATES_01, pSumm);
                    }
                    m_uiTalkTimer = 12*IN_MILLISECONDS;
                    break;
                case 10:
                    if (Unit* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_MALGANIS))
                    {
                        DoScriptText(SAY_MALGANIS_GATES_02, pSumm);
                    }
                    m_uiTalkTimer = 16*IN_MILLISECONDS;
                    break;
                case 11:
                    DoScriptText(SAY_ARTHAS_GATES_04, m_creature);
                    m_uiTalkTimer = 4*IN_MILLISECONDS;
                    break;
                case 12:
                    if (Unit* pMalg = SummonMgr.GetFirstFoundSummonWithId(NPC_MALGANIS))
                        pMalg->HandleEmote(EMOTE_ONESHOT_OMNICAST_GHOUL);
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 13:
                    SummonMgr.UnsummonAllWithId(NPC_MALGANIS);
                    m_uiTalkTimer = 3*IN_MILLISECONDS;
                    break;
                case 14:
                    m_pInstance->SetData(TYPE_MEATHOOK_EVENT, IN_PROGRESS);
                    DoScriptText(SAY_ARTHAS_GATES_10, m_creature);
                    m_uiTalkTimer = 20*IN_MILLISECONDS;
                    break;
                case 15:
                    m_uiTalkPhase = 0;
                    m_pInstance->SummonNextWave();
                    m_pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, DONE);
                    break;
                    // TownHall - start
                case 20:
                {
                    std::list<ObjectGuid> summons = SummonMgr.GetSummonList();
                    for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    {
                        if (Unit* pSumm = m_creature->GetMap()->GetUnit(*itr))
                            pSumm->SetFacingToObject(m_creature);
                    }
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                }
                case 21:
                    if (Creature* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_AGIATED_STRATHOLME_CITIZEN))
                        DoScriptText(SAY_CITIZEN_EPOCH_01, pSumm);
                    m_uiTalkTimer = 4*IN_MILLISECONDS;
                    break;
                case 22:
                    m_creature->MonsterMoveWithSpeed(2395.604f, 1205.556f, 134.126f, 2*IN_MILLISECONDS);
                    DoScriptText(SAY_ARTHAS_EPOCH_02, m_creature);
                    m_uiTalkTimer = 3*IN_MILLISECONDS;
                    break;
                case 23:
                    m_creature->HandleEmote(EMOTE_ONESHOT_ATTACK2HTIGHT);
                    m_uiTalkTimer = 1*IN_MILLISECONDS;
                    break;
                case 24:
                    DoScriptText(SAY_ARTHAS_EPOCH_03, m_creature);
                    if (Creature* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_AGIATED_STRATHOLME_CITIZEN))
                        pSumm->HandleEmote(EMOTE_ONESHOT_LAUGH);
                    m_uiTalkTimer = 3*IN_MILLISECONDS;
                    break;
                case 25:
                {
                    if (Creature* pSumm = SummonMgr.GetFirstFoundSummonWithId(NPC_AGIATED_STRATHOLME_CITIZEN))
                        DoScriptText(SAY_CITIZEN_EPOCH_02, pSumm);
                    std::list<ObjectGuid> summons = SummonMgr.GetSummonList();
                    for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
                    {
                        if (Creature* pSumm = m_creature->GetMap()->GetCreature(*itr))
                            switch(pSumm->GetEntry())
                            {
                                case NPC_AGIATED_STRATHOLME_CITIZEN:
                                    pSumm->UpdateEntry(urand(0,1)? NPC_INFINITE_ADVERSARY : NPC_INFINITE_HUNTER);
                                    pSumm->AddThreat(m_creature);
                                    break;
                                case NPC_AGIATED_STRATHOLME_RESIDENT:
                                    pSumm->UpdateEntry(NPC_INFINITE_AGENT);
                                    pSumm->AddThreat(m_creature);
                                    break;
                            }
                    }
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                }
                case 26:
                    SetEscortPaused(false);
                    m_uiTalkPhase = 0;
                    break;
                // Epoch intro
                case 30:
                    if (Creature* pEpoch = SummonMgr.GetFirstFoundSummonWithId(NPC_CHRONO_LORD_EPOCH))
                    {
                        m_creature->SetFacingToObject(pEpoch);
                        pEpoch->SetFacingToObject(m_creature);
                        DoScriptText(SAY_EPOCH_INTRO, pEpoch);
                    }
                    m_uiTalkTimer = 15*IN_MILLISECONDS;
                    break;
                case 31:
                    DoScriptText(SAY_ARTHAS_EPOCH_12, m_creature);
                    m_uiTalkTimer = 7*IN_MILLISECONDS;
                    break;
                case 32:
                    if (Creature* pEpoch = SummonMgr.GetFirstFoundSummonWithId(NPC_CHRONO_LORD_EPOCH))
                    {
                        DoScriptText(SAY_EPOCH_AGGRO, pEpoch);
                        pEpoch->SetInCombatWithZone();
                    }
                    m_uiTalkTimer = 5*IN_MILLISECONDS;
                    break;
                case 33:
                    m_uiTalkPhase = 0;
                    break;
                // At Bookshelf
                case 40:
                    DoScriptText(SAY_ARTHAS_EPOCH_13, m_creature);
                    m_uiTalkTimer = 4*IN_MILLISECONDS;
                    break;
                case 41:
                    if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_DOOR_BOOKCASE)))
                        m_creature->SetFacingToObject(pGo);
                    DoScriptText(SAY_ARTHAS_EPOCH_14, m_creature);
                    m_uiTalkTimer = 5*IN_MILLISECONDS;
                    break;
                case 42:
                    if (GameObject* pGo = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_DOOR_BOOKCASE)))
                        pGo->SetGoState(GO_STATE_ACTIVE);
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 43:
                    DoScriptText(SAY_ARTHAS_EPOCH_15, m_creature);
                    m_uiTalkTimer = 2*IN_MILLISECONDS;
                    break;
                case 44:
                    m_uiTalkPhase = 0;
                    SetEscortPaused(false);
                    break;
                // Mal'Ganis intro
                case 50:
                    if (Creature* pMalganis = SummonMgr.GetFirstFoundSummonWithId(NPC_MALGANIS))
                    {
                        DoScriptText(SAY_ARTHAS_MALGANIS_INTRO, m_creature);
                        m_creature->SetFacingToObject(pMalganis);
                        pMalganis->SetFacingToObject(m_creature);
                    }
                    m_uiTalkTimer = 6*IN_MILLISECONDS;
                    break;
                case 51:
                    if (Creature* pMalganis = SummonMgr.GetFirstFoundSummonWithId(NPC_MALGANIS))
                    {
                        DoScriptText(SAY_MALGANIS_AGGRO, pMalganis);
                        pMalganis->SetInCombatWithZone();
                    }
                    m_uiTalkPhase = 0;
                    break;
                // outro
                case 60:
                   SetEscortPaused(false);
                   SetRun(true);
                   m_uiTalkPhase = 0;
                   break;
                case 65:
                   DoScriptText(SAY_MALGANIS_ARTHAS_OUTRO_01, m_creature);
                   m_uiTalkTimer = 13*IN_MILLISECONDS;
                   break;
                case 66:
                   m_creature->SetFacingTo(5.268f);
                   DoScriptText(SAY_MALGANIS_ARTHAS_OUTRO_02, m_creature);
                   m_uiTalkTimer = 15*IN_MILLISECONDS;
                   break;
                case 67: // handle credits
                {
                    Map::PlayerList const& players = m_pInstance->instance->GetPlayers();
                    if (!players.isEmpty())
                    {
                        for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                        {
                            if (Player* pPlayer = itr->getSource())
                            {
                                pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, MALGANIS_KILL_CREDIT, 0, m_creature);
                                pPlayer->KilledMonsterCredit(MALGANIS_KILL_CREDIT_BUNNY, m_creature->GetObjectGuid());
                            }
                        }
                    }
                    m_pInstance->SetData(TYPE_MALGANIS_EVENT, DONE); //spawns chest for loot and summons chromie
                   m_uiTalkTimer = 5*IN_MILLISECONDS;
                   break;
                }
                case 68:
                   m_uiTalkPhase = 0;
                   break;
                default:
                    break;
            }
            if (m_uiTalkPhase)
                m_uiTalkPhase++;
        }
        else m_uiTalkTimer -= uiDiff;
    }
};

#define GOSSIP_ESCORT_START_GATES_01    "Yes, my Prince. We're ready."
#define GOSSIP_ESCORT_START_TOWNHALL_01 "We're only doing what is best for Lordaeron, your Highness."
#define GOSSIP_ESCORT_START_TOWNHALL_02 "Lead the way Prince Arthas"
#define GOSSIP_ESCORTING_01             "I'm Ready"
#define GOSSIP_ESCORTING_02             "For Lordaeron!"
#define GOSSIP_MALGANIS_01              "I'm ready to battle the dreadlord, sire."

enum
{
    TEXT_ID_ESCORT_START_01     = 13076,
    TEXT_ID_ESCORT_TOWNHALL_01  = 13125,
    TEXT_ID_ESCORT_TOWNHALL_02  = 13126,
    TEXT_ID_ESCORTING_01        = 13177,
    TEXT_ID_ESCORTING_02        = 13179,
    TEXT_ID_MALGANIS_START_01   = 13287,
};

bool GossipHello_npc_arthas_cs(Player *pPlayer, Creature *pCreature)
{
    if (instance_culling_of_stratholme* m_pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
    {
        switch(m_pInstance->GetInstancePosition())
        {
            case POS_ARTHAS_INTRO:
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ESCORT_START_GATES_01, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORT_START_01, pCreature->GetGUID());
                break;
            case POS_ARTHAS_WAVES:
                break;
            case POS_ARTHAS_TOWNHALL:
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ESCORT_START_TOWNHALL_01, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORT_TOWNHALL_01, pCreature->GetGUID());
                break;
            case POS_ARTHAS_ESCORTING:
                if (pCreature->GetPositionX() > 2500.0f)
                {
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ESCORTING_02, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORTING_02, pCreature->GetGUID());
                }
                else
                {
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ESCORTING_01, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORTING_01, pCreature->GetGUID());
                }
                break;
            case POS_ARTHAS_MALGANIS:
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_MALGANIS_01, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_MALGANIS_START_01, pCreature->GetGUID());
                break;
            case POS_INSTANCE_FINISHED:
                break;
        }
    }
    return true;
}

bool GossipSelect_npc_arthas_cs(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            if (npc_arthas_csAI* pAI = ((npc_arthas_csAI*)pCreature->AI()))
            {
                if (!pAI->HasEscortState(STATE_ESCORT_PAUSED))
                {
                    pAI->Start();
                    pAI->SetEscortPaused(true);
                    pAI->SetCurrentWaypoint(1);
                    pAI->SetEscortPaused(false);
                } else pAI->SetEscortPaused(false);
            }
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->CLOSE_GOSSIP_MENU();
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ESCORT_START_TOWNHALL_02, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ESCORT_TOWNHALL_02, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            if (npc_arthas_csAI* pAI = ((npc_arthas_csAI*)pCreature->AI()))
            {
                if (!pAI->HasEscortState(STATE_ESCORT_PAUSED))
                {
                    pAI->Start();
                    pAI->SetEscortPaused(true);
                    pAI->SetCurrentWaypoint(16);
                    pAI->SetEscortPaused(false);
                }
                else pAI->SetEscortPaused(false);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->CLOSE_GOSSIP_MENU();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            if (npc_arthas_csAI* pAI = ((npc_arthas_csAI*)pCreature->AI()))
            {
                if (!pAI->HasEscortState(STATE_ESCORT_PAUSED))
                {
                    pAI->Start();
                    pAI->SetEscortPaused(true);
                    pAI->SetCurrentWaypoint(26);
                    pAI->SetEscortPaused(false);
                }
                else pAI->SetEscortPaused(false);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->CLOSE_GOSSIP_MENU();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            if (npc_arthas_csAI* pAI = ((npc_arthas_csAI*)pCreature->AI()))
            {
                if (!pAI->HasEscortState(STATE_ESCORT_PAUSED))
                {
                    pAI->Start();
                    pAI->SetEscortPaused(true);
                    pAI->SetCurrentWaypoint(40);
                    pAI->SetEscortPaused(false);
                }
                else pAI->SetEscortPaused(false);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->CLOSE_GOSSIP_MENU();
            }
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            if (npc_arthas_csAI* pAI = ((npc_arthas_csAI*)pCreature->AI()))
            {
                if (!pAI->HasEscortState(STATE_ESCORT_PAUSED))
                {
                    pAI->Start();
                    pAI->SetEscortPaused(true);
                    pAI->SetCurrentWaypoint(47);
                    pAI->SetEscortPaused(false);
                }
                else pAI->SetEscortPaused(false);
            pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pPlayer->CLOSE_GOSSIP_MENU();
            }
            break;
    }
    return true;
}

CreatureAI* GetAI_npc_arthas_cs(Creature* pCreature)
{
    return new npc_arthas_csAI(pCreature);
};
/* *************
** npc_lordaeron_army
************* */

struct MANGOS_DLL_DECL npc_lordaeron_armyAI: public ScriptedAI,public ScriptMessageInterface
{
    uint32 m_uiEmoteTimer;
    uint32 m_uiEmote2Timer;
    uint32 m_uiUnfollowTimer;

    npc_lordaeron_armyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_uiEmoteTimer = 0;
        m_uiEmote2Timer = 0;
        m_uiUnfollowTimer = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_UNIT && (pWho->GetEntry() == NPC_STRATHOLME_RESIDENT || pWho->GetEntry() == NPC_STRATHOLME_CITIZEN || pWho->GetEntry() == NPC_ZOMBIE))
            m_creature->AddThreat(pWho);
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (data1 == MESSAGE_SALUTE)
        {
            m_uiEmoteTimer = data2;
        }
        else if (data1 == MESSAGE_START)
        {
            m_creature->GetMotionMaster()->MoveFollow((Unit*)pSender, m_creature->GetDistance2d(pSender), m_creature->GetAngle(pSender));
            m_uiUnfollowTimer = 40*IN_MILLISECONDS;
        }
        else if (data1 == MESSAGE_END)
        {
            m_uiEmote2Timer = data2;
        }
    }
    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiEmoteTimer)
            if (m_uiEmoteTimer <= uiDiff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_SALUTE);
                m_uiEmoteTimer = 0;
            } else m_uiEmoteTimer -= uiDiff;

        if (m_uiEmote2Timer)
            if (m_uiEmote2Timer <= uiDiff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_CHEER);
                m_uiEmote2Timer = 0;
            } else m_uiEmote2Timer -= uiDiff;

        if (m_uiUnfollowTimer)
            if (m_uiUnfollowTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                m_uiUnfollowTimer = 0;
            } else m_uiUnfollowTimer -= uiDiff;
    }
};
CreatureAI* GetAI_npc_lordaeron_army(Creature* pCreature)
{
    return new npc_lordaeron_armyAI(pCreature);
};

/* *************
** npc_lordaeron_citizen
************* */
enum
{
    SAY_CITIZEN_GATES_02 = ENTRY - 79,
    SAY_CITIZEN_GATES_04 = ENTRY - 80,
    SAY_CITIZEN_GATES_05 = ENTRY - 81,
    SAY_RESIDENT_GATES_02 = ENTRY - 82,
    SAY_RESIDENT_GATES_03 = ENTRY - 89,
    SAY_RESIDENT_GATES_04 = ENTRY - 90,
    SAY_RESIDENT_GATES_05 = ENTRY - 91,
};

struct MANGOS_DLL_DECL npc_lordaeron_citizenAI: public ScriptedAI
{
    instance_culling_of_stratholme* m_pInstance;
    bool m_bSaidStuff;

    npc_lordaeron_citizenAI(Creature* pCreature): ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<instance_culling_of_stratholme*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        m_bSaidStuff = false;
    };

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bSaidStuff && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDist(m_creature, 10.0f))
        {
            m_bSaidStuff = true;
            switch(urand(0, 13)) // 33% chance to say stuff
            {
                case 0:
                    DoScriptText(SAY_CITIZEN_GATES_02, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_CITIZEN_GATES_04, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_CITIZEN_GATES_05, m_creature);
                    break;
                case 3:
                    DoScriptText(SAY_RESIDENT_GATES_02, m_creature);
                    break;
                case 4:
                    DoScriptText(SAY_RESIDENT_GATES_03, m_creature);
                    break;
                case 5:
                    DoScriptText(SAY_RESIDENT_GATES_04, m_creature);
                    break;
                case 6:
                    DoScriptText(SAY_RESIDENT_GATES_05, m_creature);
                    break;
                default:
                    break;
            }
            if (m_pInstance->GetData(TYPE_MEATHOOK_EVENT) == IN_PROGRESS || m_pInstance->GetData(TYPE_SALRAMM_EVENT) == IN_PROGRESS)
            {
                m_creature->UpdateEntry(NPC_ZOMBIE);
                m_creature->AddThreat(pWho);
            }
        }
    }
};

CreatureAI* GetAI_npc_lordaeron_citizen(Creature* pCreature)
{
    return new npc_lordaeron_citizenAI(pCreature);
};

/* *************
** npc_chromie (gossip, quest-accept)
************* */

enum
{
    QUEST_DISPELLING_ILLUSIONS  = 13149,
    QUEST_A_ROYAL_ESCORT        = 13151,

    ITEM_ARCANE_DISRUPTOR       = 37888,

    TEXT_ID_ENTRANCE_1          = 12992,
    TEXT_ID_ENTRANCE_2          = 12993,
    TEXT_ID_ENTRANCE_3          = 12994,
    TEXT_ID_ENTRANCE_4          = 12995,

    TEXT_ID_INN_1               = 12939,
    TEXT_ID_INN_2               = 12949,
    TEXT_ID_INN_3               = 12950,
    TEXT_ID_INN_4               = 12952,
};
/**
* Gossip menu ID is known and present in DB (9586, 9594, 9595)
**/
#define GOSSIP_ITEM_INN_1   "Why have I been sent back to this particular place and time?"
#define GOSSIP_ITEM_INN_2   "What was this decision?"
#define GOSSIP_ITEM_INN_3   "So how does the Infinite Dragonflight plan to interfere?"


/**
* Gossip menu ID is unknown
* texts from video-reference (Youtube)
**/
#define GOSSIP_ITEM_ENTRANCE_1  "What do you think they're up to?"
#define GOSSIP_ITEM_ENTRANCE_2  "You want me to do what?"
#define GOSSIP_ITEM_ENTRANCE_3  "Very well, Chromie."

bool GossipHello_npc_chromie(Player *pPlayer, Creature *pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (instance_culling_of_stratholme* m_pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
    {
        switch (pCreature->GetEntry())
        {
            case NPC_CHROMIE_INN:
                if (m_pInstance->GetData(TYPE_GRAIN_EVENT) != DONE)
                {
                    if (!pPlayer->HasItemCount(ITEM_ARCANE_DISRUPTOR, 1))
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                }
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_1, pCreature->GetGUID());
                break;
            case NPC_CHROMIE_ENTRANCE:
                if (m_pInstance->GetData(TYPE_GRAIN_EVENT) == DONE && m_pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED)
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_1, pCreature->GetGUID());
                break;
        }
    }
    return true;
}

bool GossipSelect_npc_chromie(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 uiAction)
{
    switch (pCreature->GetEntry())
    {
        case NPC_CHROMIE_INN:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_2, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_3, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_INN_4, pCreature->GetGUID());
                    if (!pPlayer->HasItemCount(ITEM_ARCANE_DISRUPTOR, 1))
                    {
                        if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_ARCANE_DISRUPTOR, 1))
                        {
                            pPlayer->SendNewItem(pItem, 1, true, false);
                            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
                            {
                                if (pInstance->GetData(TYPE_GRAIN_EVENT) == NOT_STARTED)
                                    pInstance->SetData(TYPE_GRAIN_EVENT, SPECIAL);
                            }
                        }
                    }
                    break;
            }
            break;
        case NPC_CHROMIE_ENTRANCE:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_2, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ENTRANCE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_3, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    pPlayer->SEND_GOSSIP_MENU(TEXT_ID_ENTRANCE_4, pCreature->GetGUID());
                    if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
                    {
                        if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED)
                        {
                            pInstance->SetData(TYPE_ARTHAS_INTRO_EVENT, IN_PROGRESS);
                            if (Creature* pUther = pCreature->SummonCreature(NPC_UTHER_THE_LIGHTBRINGER, 1741.429f, 1243.964f, 137.583f, 0.663f, TEMPSUMMON_TIMED_DESPAWN, 150*IN_MILLISECONDS, true))
                                if (npc_uther_cosAI* pAI = ((npc_uther_cosAI*)pUther->AI()))
                                    pAI->ScriptMessage(pCreature, MESSAGE_START, 0);
                        }
                    }
                    break;
            }
            break;
    }
    return true;
}

bool QuestAccept_npc_chromie(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    switch (pQuest->GetQuestId())
    {
        case QUEST_DISPELLING_ILLUSIONS:
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
            {
                if (pInstance->GetData(TYPE_GRAIN_EVENT) == NOT_STARTED)
                    pInstance->SetData(TYPE_GRAIN_EVENT, SPECIAL);
            }
            break;
        case QUEST_A_ROYAL_ESCORT:
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pCreature->GetInstanceData())
            {
                if (pInstance->GetData(TYPE_ARTHAS_INTRO_EVENT) == NOT_STARTED)
                    pInstance->DoSpawnArthasIfNeeded();
            }
            break;
    }
    return true;
}

/* *************
** npc_crates_bunny (spell aura effect dummy)
************* */

enum
{
    SPELL_ARCANE_DISRUPTION = 49590
};

bool EffectAuraDummy_spell_aura_dummy_npc_crates_dummy(const Aura* pAura, bool bApply)
{
    if (pAura->GetId() == SPELL_ARCANE_DISRUPTION && pAura->GetEffIndex() == EFFECT_INDEX_0 && bApply)
    {
        if (Creature* pTarget = (Creature*)pAura->GetTarget())
        {
            std::list<Creature*> lCrateBunnyList;
            if (instance_culling_of_stratholme* pInstance = (instance_culling_of_stratholme*)pTarget->GetInstanceData())
            {
                pInstance->GetCratesBunnyOrderedList(lCrateBunnyList);
                uint8 i = 0;
                for (std::list<Creature*>::const_iterator itr = lCrateBunnyList.begin(); itr != lCrateBunnyList.end(); ++itr)
                {
                    i++;
                    if (*itr == pTarget)
                        break;
                }

                switch (i)
                {
                    case 1:
                        // Start NPC_ROGER_OWENS Event
                        if (Creature* pRoger = pInstance->instance->GetCreature(pInstance->GetData64(NPC_ROGER_OWENS)))
                            if (npc_plagued_grain_eventAI* pAI = ((npc_plagued_grain_eventAI*)pRoger->AI()))
                                pAI->ScriptMessage(pTarget, MESSAGE_START, 0);
                        break;
                    case 2:
                        // Start NPC_SERGEANT_MORIGAN  Event
                        if (Creature* pMorigan = pInstance->instance->GetCreature(pInstance->GetData64(NPC_SERGEANT_MORIGAN)))
                            if (npc_plagued_grain_eventAI* pAI = ((npc_plagued_grain_eventAI*)pMorigan->AI()))
                                pAI->ScriptMessage(pTarget, MESSAGE_START, 0);
                        break;
                    case 3:
                        // Start NPC_JENA_ANDERSON Event
                        if (Creature* pAnderson = pInstance->instance->GetCreature(pInstance->GetData64(NPC_JENA_ANDERSON)))
                            if (npc_plagued_grain_eventAI* pAI = ((npc_plagued_grain_eventAI*)pAnderson->AI()))
                                pAI->ScriptMessage(pTarget, MESSAGE_START, 0);
                        break;
                    case 4:
                        // Start NPC_MALCOM_MOORE Event
                    {
                        Creature* pScruffy =  pTarget->SummonCreature(NPC_SCRUFFY, 1608.633f, 827.474f, 123.987f,  5.866f, TEMPSUMMON_CORPSE_DESPAWN, 0, true);
                        if (Creature* pMalcolm =  pTarget->SummonCreature(NPC_MALCOM_MOORE, 1608.633f, 827.474f, 123.987f,  5.866f, TEMPSUMMON_CORPSE_DESPAWN, 0, true))
                            if (npc_plagued_grain_eventAI* pAI = ((npc_plagued_grain_eventAI*)pMalcolm->AI()))
                                pAI->ScriptMessage(pTarget, MESSAGE_START, 0);
                        break;
                    }
                    case 5:
                        // Start NPC_BARTLEBY_BATTSON Event
                        if (Creature* pBattson = pInstance->instance->GetCreature(pInstance->GetData64(NPC_BARTLEBY_BATTSON)))
                            if (npc_plagued_grain_eventAI* pAI = ((npc_plagued_grain_eventAI*)pBattson->AI()))
                                pAI->ScriptMessage(pTarget, MESSAGE_START, 0);
                        break;
                }

                if (pInstance->GetData(TYPE_GRAIN_EVENT) != DONE)
                    pInstance->SetData(TYPE_GRAIN_EVENT, IN_PROGRESS);
                // pTarget->ForcedDespawn();    // direct despawn has influence on visual effects,
                                                // but despawning makes it impossible to multi-use the spell at the same place
                // perhaps some add. GO-Visual
            }
        }
    }
    return true;
}

void AddSC_culling_of_stratholme()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_chromie";
    pNewScript->pGossipHello = &GossipHello_npc_chromie;
    pNewScript->pGossipSelect = &GossipSelect_npc_chromie;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_chromie;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "spell_dummy_npc_crates_bunny";
    pNewScript->pEffectAuraDummy = &EffectAuraDummy_spell_aura_dummy_npc_crates_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_plagued_grain_event";
    pNewScript->GetAI = &GetAI_npc_plagued_grain_event;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lordaeron_army";
    pNewScript->GetAI = &GetAI_npc_lordaeron_army;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lordaeron_citizen";
    pNewScript->GetAI = &GetAI_npc_lordaeron_citizen;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_michael_belfast";
    pNewScript->GetAI = &GetAI_npc_michael_belfast;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_uther_cos";
    pNewScript->GetAI = &GetAI_npc_uther_cos;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arthas_cs";
    pNewScript->pGossipHello = &GossipHello_npc_arthas_cs;
    pNewScript->pGossipSelect = &GossipSelect_npc_arthas_cs;
    pNewScript->GetAI = &GetAI_npc_arthas_cs;
    pNewScript->RegisterSelf();
}
