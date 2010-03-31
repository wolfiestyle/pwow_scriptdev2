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
SDName: Trial Of the Champion
SD%Complete: 80%
SDComment: event script
SDCategory: trial_of_the_champion
EndScriptData */

/* ContentData
npc_toc5_announcer
EndContentData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

#define GOSSIP_START_EVENT     "I'm ready for the next challenge."

/*######
## npc_toc5_announcer
######*/
struct MANGOS_DLL_DECL npc_toc5_announcerAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;

    npc_toc5_announcerAI(Creature* pCreature): ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
    }

    void StartEvent(Player* pPlayer)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == NOT_STARTED)  // Encounter 1
        {
            m_pInstance->SetData64(DATA_TOC5_ANNOUNCER, m_creature->GetGUID());
            std::vector<uint32> ChampionEntries;
            ChampionEntries.reserve(5);
            if (m_pInstance->GetData64(DATA_TOC5_ANNOUNCER) == m_pInstance->GetData64(DATA_ARELAS))
            {
                m_pInstance->SetData(DATA_BLACK_KNIGHT_MINION, NPC_RISEN_ARELAS);
                ChampionEntries.push_back(NPC_MOKRA);
                ChampionEntries.push_back(NPC_ERESSEA);
                ChampionEntries.push_back(NPC_RUNOK);
                ChampionEntries.push_back(NPC_ZULTORE);
                ChampionEntries.push_back(NPC_VISCERI);
            }
            else if (m_pInstance->GetData64(DATA_TOC5_ANNOUNCER) == m_pInstance->GetData64(DATA_JAEREN))
            {
                m_pInstance->SetData(DATA_BLACK_KNIGHT_MINION, NPC_RISEN_JAEREN);
                ChampionEntries.push_back(NPC_JACOB);
                ChampionEntries.push_back(NPC_AMBROSE);
                ChampionEntries.push_back(NPC_COLOSOS);
                ChampionEntries.push_back(NPC_JAELYNE);
                ChampionEntries.push_back(NPC_LANA);
            }
            if (!ChampionEntries.empty())
            {
                std::random_shuffle(ChampionEntries.begin(), ChampionEntries.end());
                m_pInstance->SetData(DATA_CHAMPIONID_1, ChampionEntries[0]);
                m_pInstance->SetData(DATA_CHAMPIONID_2, ChampionEntries[1]);
                m_pInstance->SetData(DATA_CHAMPIONID_3, ChampionEntries[2]);
                m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, FAIL);   //FIXME: why fail?
            }
        }

        if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == FAIL)
        {
            m_creature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_1), 738.665771, 661.031433, 412.394623, 4.698702, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_creature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_2), 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_creature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_3), 754.360779, 660.816162, 412.395996, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
        }

        if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
        {
            if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == NOT_STARTED) // Encounter 2
            {
                m_pInstance->SetData(DATA_ARGENT_CHALLENGER, urand(0, 1) ? NPC_EADRIC : NPC_PALETRESS);
                m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, FAIL);
            }

            if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == FAIL)
            {
                m_creature->SummonCreature(m_pInstance->GetData(DATA_ARGENT_CHALLENGER), 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
                m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
            }

            if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
            {
                if (m_pInstance->GetData(TYPE_BLACK_KNIGHT) == DONE)    // Encounter 3
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                else
                {
                    m_creature->SummonCreature(NPC_BLACK_KNIGHT, 746.864441, 660.918762, 411.695465, 4.698700, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }
        }
    }

    /*void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }*/
};

CreatureAI* GetAI_npc_toc5_announcer(Creature* pCreature)
{
    return new npc_toc5_announcerAI(pCreature);
}

bool GossipHello_npc_toc5_announcer(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc5_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        if (npc_toc5_announcerAI *announcerAI = dynamic_cast<npc_toc5_announcerAI*>(pCreature->AI()))
            announcerAI->StartEvent(pPlayer);
    }

    return true;
}

boss_trial_of_the_championAI::boss_trial_of_the_championAI(Creature *pCreature):
    ScriptedAI(pCreature)
{
    m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
    m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
}

Player* boss_trial_of_the_championAI::SelectRandomPlayer()
{
    ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
    if (tlist.empty())
        return NULL;
    ThreatList::const_iterator itr = tlist.begin();
    size_t count = tlist.size();
    std::advance(itr, urand(0, count-1));
    while (count > 0)
    {
        Unit *target = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
        if (target && target->GetTypeId() == TYPEID_PLAYER && m_creature->IsInMap(target))
            return static_cast<Player*>(target);
        else
        {
            if (++itr == tlist.end())
                itr = tlist.begin();
            count--;
        }
    }
    return NULL;
}

void AddSC_trial_of_the_champion()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc5_announcer";
    NewScript->GetAI = &GetAI_npc_toc5_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc5_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc5_announcer;
    NewScript->RegisterSelf();
}
