/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss Drakos the Interrogator
SD%Complete: 100%
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"
#include "Vehicle.h"

enum Spells
{
    SPELL_THUNDERING_STOMP          = 50774,
    SPELL_THUNDERING_STOMP_H        = 59370,

    SPELL_MAGIC_PULL                = 51336,
    SPELL_MAGIC_PULL_EFFECT         = 50770,

    SPELL_UNSTABLE_SPHERE_PASSIVE   = 50756,
    SPELL_UNSTABLE_SPHERE_PULSE     = 50757,

    SPELL_UNSTABLE_SPHERE_TIMER     = 50758,
    SPELL_UNSTABLE_SPHERE_EXPLODE   = 50759,

    SPELL_SUMMON_VISUAL             = 36400,
};

enum Says
{
    SAY_AGGRO   = -1578000,
    SAY_KILL_1  = -1578001,
    SAY_KILL_2  = -1578002,
    SAY_KILL_3  = -1578003,
    SAY_DEATH   = -1578004,
    SAY_PULL_1  = -1578005,
    SAY_PULL_2  = -1578006,
    SAY_PULL_3  = -1578007,
    SAY_PULL_4  = -1578008,
    SAY_STOMP_1 = -1578009,
    SAY_STOMP_2 = -1578010,
    SAY_STOMP_3 = -1578011,

    SAY_VAROS_SPAWN = -1578022,
    SAY_EREGOS_SPAWN = -1578030,

    SAY_BELGARISTRASZ_DRAKOS    = -1578040,
    SAY_BELGARISTRASZ_VAROS     = -1578041,
    SAY_BELGARISTRASZ_UROM_01   = -1578042,
    SAY_BELGARISTRASZ_UROM_02   = -1578043,
};

enum Items
{
    RUBY_ESSENCE        = 37860,
    EMERALD_ESSENCE     = 37815,
    AMBER_ESSENCE       = 37859,
};

enum NPCs
{
    NPC_BELGARISTRASZ   = 27658,
    NPC_UNSTABLE_SPHERE = 28166,
};

enum Events
{
    EVENT_STOMP = 1,
    EVENT_PULL,
    EVENT_SUMMON,
};

#define CENTER_X 960.120f
#define CENTER_Y 1049.413f

#define MAGIC_PULL_EMOTE "Drakos the Interrogator begins to cast Magic Pull!"

#define TIMER_STOMP 15*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define TIMER_PULL  27*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_SUMMON 7*IN_MILLISECONDS, 3*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_drakosAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    EventManager Events;

    boss_drakosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        if (m_pInstance && m_creature->isAlive())
            m_pInstance->SetData(TYPE_DRAKOS, NOT_STARTED);
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature, NULL);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_DRAKOS, IN_PROGRESS);
        Events.ScheduleEvent(EVENT_STOMP,   TIMER_STOMP);
        Events.ScheduleEvent(EVENT_PULL,    TIMER_PULL);
        Events.ScheduleEvent(EVENT_SUMMON,  TIMER_SUMMON);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            switch (urand(0, 2))
            {
                case 0:
                    DoScriptText(SAY_KILL_1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_KILL_2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_KILL_3, m_creature);
                    break;
            }
        }
    }

    void JustDied(Unit* pSlayer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_DRAKOS, DONE);
            if (Creature* pBelgaristrasz = m_pInstance->instance->GetCreature(m_pInstance->GetData64(DATA_BELGARISTRASZ)))
                SendScriptMessageTo(pBelgaristrasz, pSlayer, MESSAGE_DRAKOS, 0);
        }
    }

    void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_MAGIC_PULL)
            if (target->GetTypeId() == TYPEID_PLAYER)
                DoCast(target, SPELL_MAGIC_PULL_EFFECT, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_STOMP:
                    switch (urand(0, 2))
                    {
                       case 0:
                           DoScriptText(SAY_STOMP_1, m_creature);
                           break;
                       case 1:
                           DoScriptText(SAY_STOMP_2, m_creature);
                           break;
                       case 2:
                           DoScriptText(SAY_STOMP_3, m_creature);
                           break;
                    }
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_THUNDERING_STOMP : SPELL_THUNDERING_STOMP_H);
                    break;
                case EVENT_PULL:
                    m_creature->MonsterTextEmote(MAGIC_PULL_EMOTE, m_creature, true);
                    switch (urand(0, 3))
                    {
                       case 0:
                           DoScriptText(SAY_PULL_1, m_creature);
                           break;
                       case 1:
                           DoScriptText(SAY_PULL_2, m_creature);
                           break;
                       case 2:
                           DoScriptText(SAY_PULL_3, m_creature);
                           break;
                       case 3:
                           DoScriptText(SAY_PULL_4, m_creature);
                           break;
                    }
                    // After pull more adds spawn
                    Events.ScheduleEvent(EVENT_SUMMON, 2*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_SUMMON, 3*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_SUMMON, 4*IN_MILLISECONDS);
                    DoCast(m_creature, SPELL_MAGIC_PULL);
                    break;
                case EVENT_SUMMON:
                {
                    for (int i = 0 ; i < 2 ; i++)
                    {
                        Creature* Summon = m_creature->SummonCreature(NPC_UNSTABLE_SPHERE, m_creature->GetPositionX()+urand(0,10) -5,
                            m_creature->GetPositionY() +urand(0,10) -5, m_creature->GetPositionZ()+urand(0,3), 0, TEMPSUMMON_TIMED_DESPAWN, 19*IN_MILLISECONDS);
                        Summon->CastSpell(Summon, SPELL_SUMMON_VISUAL, true);
                    }
                    break;
                }
            }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakos(Creature* pCreature)
{
    return new boss_drakosAI(pCreature);
}

struct MANGOS_DLL_DECL npc_unstable_sphereAI : public ScriptedAI
{
    npc_unstable_sphereAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiPulseTimer;

    void Reset()
    {
        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
        m_creature->GetMotionMaster()->MovePoint(0, (CENTER_X-35)+rand()%70, (CENTER_Y-35)+rand()%70, m_creature->GetPositionZ());
        m_creature->SetSpeedRate(MOVE_RUN, 2, true);
        m_creature->setFaction(14);
        DoCast(m_creature, SPELL_UNSTABLE_SPHERE_TIMER, true);
        DoCast(m_creature, SPELL_UNSTABLE_SPHERE_PASSIVE, true);
        m_uiPulseTimer = 3000;
        m_creature->ForcedDespawn(19000);
    }

    void AttackStart(Unit* pWho)
    {
    }

    void UpdateAI(const uint32 diff)
    {

        if (m_pInstance && m_pInstance->GetData(TYPE_DRAKOS) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (m_uiPulseTimer < diff)
        {
            DoCast(m_creature, SPELL_UNSTABLE_SPHERE_PULSE, true);
            m_uiPulseTimer = 3000;
        }
        else
            m_uiPulseTimer -= diff;
    }
};

CreatureAI* GetAI_npc_unstable_sphere(Creature* pCreature)
{
    return new npc_unstable_sphereAI (pCreature);
}

struct MANGOS_DLL_DECL npc_belgaristraszAI : public ScriptedAI, ScriptMessageInterface
{
    ScriptedInstance* m_pInstance;
    uint32 m_uiTalkTimer;
    uint32 m_uiTalkPhase;

    npc_belgaristraszAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        m_uiTalkTimer = 0;
        m_uiTalkPhase = 0;
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_DRAKOS:
                m_uiTalkPhase = 1;
                m_uiTalkTimer = 1*IN_MILLISECONDS;
                break;
            case MESSAGE_VAROS:
                m_uiTalkPhase = 5;
                m_uiTalkTimer = 1*IN_MILLISECONDS;
                break;
            case MESSAGE_UROM:
                m_uiTalkPhase = 10;
                m_uiTalkTimer = 1*IN_MILLISECONDS;
                break;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiTalkPhase)
        {
            if (m_uiTalkTimer && m_uiTalkTimer <= uiDiff)
            {
                switch (m_uiTalkPhase)
                {
                    case 1:
                    {
                        std::list<GameObject*> go_list;
                        GetGameObjectListWithEntryInGrid(go_list, m_creature, GO_DRAGON_CAGE_DOOR, DEFAULT_VISIBILITY_INSTANCE);
                        for (std::list<GameObject*>::const_iterator itr = go_list.begin(); itr != go_list.end(); ++itr)
                        {
                            (*itr)->SetGoState(GO_STATE_ACTIVE);
                        }
                        m_uiTalkPhase = 2;
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    }
                    case 2:
                    {
                        Creature* pEternos = GetClosestCreatureWithEntry(m_creature, NPC_ETERNOS, 30.0f);
                        Creature* pVerdisa = GetClosestCreatureWithEntry(m_creature, NPC_VERDISA, 30.0f);
                        if (pEternos)
                            pEternos->MonsterMove(943.39f, 1059.63f, 359.967f, 3*IN_MILLISECONDS);

                        if (pVerdisa)
                            pVerdisa->MonsterMove(949.252f, 1031.807f, 359.967f, 3*IN_MILLISECONDS);

                        m_creature->MonsterMove(941.397f, 1043.55f, 359.967f, 3*IN_MILLISECONDS);
                        m_uiTalkPhase = 3;
                        m_uiTalkTimer = 3*IN_MILLISECONDS;
                        break;
                    }
                    case 3:
                        DoScriptText(SAY_BELGARISTRASZ_DRAKOS, m_creature);
                        m_uiTalkPhase = 4;
                        m_uiTalkTimer = 5*IN_MILLISECONDS;
                        break;
                    case 4:
                        if (Unit* pVaros = m_pInstance->instance->GetCreature(m_pInstance->GetData64(TYPE_VAROS)))
                            pVaros->MonsterYellToZone(SAY_VAROS_SPAWN, 0, NULL);
                        m_creature->PlayDirectSound(13648);
                        m_uiTalkPhase = 0;
                        m_uiTalkTimer = 0;
                        break;
                    case 5:
                        DoScriptText(SAY_BELGARISTRASZ_VAROS, m_creature);
                        m_uiTalkPhase++;
                        m_uiTalkTimer = 7*IN_MILLISECONDS;
                        break;
                    case 6:
                        m_uiTalkPhase = 0;
                        m_uiTalkTimer = 0;
                        break;
                    case 10:
                        DoScriptText(SAY_BELGARISTRASZ_UROM_01, m_creature);
                        m_uiTalkPhase++;
                        m_uiTalkTimer = 7*IN_MILLISECONDS;
                        break;
                    case 11:
                        DoScriptText(SAY_BELGARISTRASZ_UROM_02, m_creature);
                        m_uiTalkPhase++;
                        m_uiTalkTimer = 4*IN_MILLISECONDS;
                        break;
                    case 12:
                        if (Unit* Eregos = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(TYPE_EREGOS)))
                            Eregos->MonsterYellToZone(SAY_EREGOS_SPAWN, 0, NULL);
                        m_creature->PlayDirectSound(13622);
                        m_uiTalkPhase = 0;
                        m_uiTalkTimer = 0;
                        break;
                    default:
                        m_uiTalkPhase = 0;
                        m_uiTalkTimer = 0;
                        break;
                }
            }
            else
                m_uiTalkTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_belgaristrasz(Creature* pCreature)
{
    return new npc_belgaristraszAI (pCreature);
}

#define GOSSIP_NEXT_STEP "What should we do next?"
// emerald
#define GOSSIP_R_T_E    "I want to exchange my Ruby Essence for Emerald Essence."
#define GOSSIP_A_T_E    "I want to exchange my Amber Essence for Emerald Essence."
#define GOSSIP_E        "I want to fly on the wings of the Green Flight."
#define GOSSIP_A_E      "What abilities do Emerald Drakes have?"
#define GOSSIP_U_A_E    "What is the ultimate ability of the Emerald Drake?"
// ruby
#define GOSSIP_E_T_R    "I want to exchange my Emerald Essence for Ruby Essence."
#define GOSSIP_A_T_R    "I want to exchange my Amber Essence for Ruby Essence."
#define GOSSIP_R        "I want to fly on the wings of the Red Flight."
#define GOSSIP_A_R      "What abilities do Ruby Drakes have?"
#define GOSSIP_U_A_R    "What is the ultimate ability of the Ruby Drake?"
// amber
#define GOSSIP_E_T_A    "I want to exchange my Emerald Essence for Amber Essence."
#define GOSSIP_R_T_A    "I want to exchange my Ruby Essence for Amber Essence."
#define GOSSIP_A        "I want to fly on the wings of the Bronze Flight."
#define GOSSIP_A_A      "What abilities do Amber Drakes have?"
#define GOSSIP_U_A_A    "What is the ultimate ability of the Amber Drake?"

enum Gossips
{
    GOSSIP_VERDISA          = 12915,
    GOSSIP_ETERNOS          = 12917,
    GOSSIP_BELGARISTRASZ    = 12916,

    GOSSIP_NEXT_UROM            = 13237,
    GOSSIP_NEXT_EREGOS          = 13238,
    GOSSIP_INSTANCE_COMPLETE    = 13239,

    GOSSIP_A_R_T    = 13254,
    GOSSIP_U_A_R_T  = 13255,

    GOSSIP_A_E_T    = 13258,
    GOSSIP_U_A_E_T  = 13259,

    GOSSIP_A_A_T    = 13256,
    GOSSIP_U_A_A_T  = 13257,
};

bool GossipHello_oculus_drake_handler(Player *pPlayer, Creature *pCreature)
{
    if (pCreature->isQuestGiver() && pCreature->GetEntry() == NPC_BALGAR_IMAGE)
    {
        if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
        {
            pPlayer->PrepareQuestMenu(pCreature->GetGUID());
            if (m_pInstance->GetData(TYPE_EREGOS) == DONE)
            {
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_INSTANCE_COMPLETE, pCreature->GetGUID());
            }
            else if (m_pInstance->GetData(TYPE_UROM) == DONE)
            {
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_NEXT_EREGOS, pCreature->GetGUID());
            }
            else if (m_pInstance->GetData(TYPE_VAROS) == DONE)
            {
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_NEXT_UROM, pCreature->GetGUID());
            }
            return true;
        }
    }

    if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
        if (m_pInstance->GetData(TYPE_DRAKOS) == DONE)
        switch (pCreature->GetEntry())
        {
            case NPC_VERDISA:
                if (pPlayer->GetItemCount(EMERALD_ESSENCE) == 0)
                {
                    if (pPlayer->GetItemCount(RUBY_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_R_T_E, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    else
                    if (pPlayer->GetItemCount(AMBER_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A_T_E, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    else
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_E, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                }
                // abilities
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A_E, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                // ultimate ability
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_U_A_E, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_VERDISA, pCreature->GetGUID());
                break;
            case NPC_BALGARISTRASZ:
                if (m_pInstance->GetData(TYPE_VAROS) == DONE)
                    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_NEXT_STEP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                if (pPlayer->GetItemCount(RUBY_ESSENCE) == 0)
                {
                    if (pPlayer->GetItemCount(EMERALD_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_E_T_R, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
                    else
                    if (pPlayer->GetItemCount(AMBER_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A_T_R, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+8);
                    else
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_R, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);
                }
                // abilities
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A_R, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);
                // ultimate ability
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_U_A_R, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+11);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_BELGARISTRASZ, pCreature->GetGUID());
                break;
            case NPC_ETERNOS:
                if (pPlayer->GetItemCount(AMBER_ESSENCE) == 0)
                {
                    if (pPlayer->GetItemCount(RUBY_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_R_T_A, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+12);
                    else
                    if (pPlayer->GetItemCount(EMERALD_ESSENCE) == 1)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_E_T_A, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+13);
                    else
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+14);
                }
                // abilities
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_A_A, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+15);
                // ultimate ability
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_U_A_A, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+16);
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_ETERNOS, pCreature->GetGUID());
                break;
        }
    }
    return true;
}

bool GossipSelect_oculus_drake_handler(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 uiAction)
{
    switch (pCreature->GetEntry())
    {
        case NPC_VERDISA:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF + 2: // R_T_E
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(EMERALD_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(RUBY_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 3: // A_T_E
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(EMERALD_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(AMBER_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 4: // E
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(EMERALD_ESSENCE, 1))
                    {
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 5:
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_A_E_T, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 6:
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_U_A_E_T, pCreature->GetGUID());
                    break;
            }
            break;
        case NPC_BALGARISTRASZ:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF + 1: // what next?
                    if (ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
                    {
                        if (m_pInstance->GetData(TYPE_EREGOS) == DONE)
                        {
                            pPlayer->SEND_GOSSIP_MENU(GOSSIP_INSTANCE_COMPLETE, pCreature->GetGUID());
                        }
                        else if (m_pInstance->GetData(TYPE_UROM) == DONE)
                        {
                            pPlayer->SEND_GOSSIP_MENU(GOSSIP_NEXT_EREGOS, pCreature->GetGUID());
                        }
                        else if (m_pInstance->GetData(TYPE_VAROS) == DONE)
                        {
                            pPlayer->SEND_GOSSIP_MENU(GOSSIP_NEXT_UROM, pCreature->GetGUID());
                        }
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF + 7:  // E_T_R
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(RUBY_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(EMERALD_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 8:  // A_T_R
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(RUBY_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(AMBER_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 9:  // R
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(RUBY_ESSENCE, 1))
                    {
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 10: // A_R
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_A_R_T, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 11: // U_A_R
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_U_A_R_T, pCreature->GetGUID());
                    break;
            }
            break;
        case NPC_ETERNOS:
            switch (uiAction)
            {
                case GOSSIP_ACTION_INFO_DEF + 12: // R_T_A
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(AMBER_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(RUBY_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 13: // E_T_A
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(AMBER_ESSENCE, 1))
                    {
                        pPlayer->DestroyItemCount(EMERALD_ESSENCE, 1, true);
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 14: // A
                    if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(AMBER_ESSENCE, 1))
                    {
                        pPlayer->SendNewItem(pItem, 1, true, false);
                    }
                    pPlayer->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF + 15: // A_A
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_A_A_T, pCreature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF + 16: // U_A_A
                    pPlayer->SEND_GOSSIP_MENU(GOSSIP_U_A_A_T, pCreature->GetGUID());
                    break;
            }
            break;
    }
    return true;
}

#define PLATFORM_X  996.5733f
#define PLATFORM_Y  1051.945f
#define PLATFORM_Z  359.5285f

bool GO_use_oculus_inner_portal(Player* pPlayer, GameObject* pGo)
{
    pPlayer->NearTeleportTo(PLATFORM_X, PLATFORM_Y, PLATFORM_Z, M_PI_F, true);
    pPlayer->CastSpell(pPlayer, SPELL_SUMMON_VISUAL, true);
    return false;
}

void AddSC_boss_drakos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_drakos";
    newscript->GetAI = &GetAI_boss_drakos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_unstable_sphere";
    newscript->GetAI = &GetAI_npc_unstable_sphere;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_belgaristrasz";
    newscript->GetAI = &GetAI_npc_belgaristrasz;
    newscript->pGossipHello = &GossipHello_oculus_drake_handler;
    newscript->pGossipSelect = &GossipSelect_oculus_drake_handler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "oculus_drake_handler";
    newscript->pGossipHello = &GossipHello_oculus_drake_handler;
    newscript->pGossipSelect = &GossipSelect_oculus_drake_handler;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_oculus_inner_portal";
    newscript->pGOUse = &GO_use_oculus_inner_portal;
    newscript->RegisterSelf();
}
