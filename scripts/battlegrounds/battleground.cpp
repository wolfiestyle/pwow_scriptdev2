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
SDName: Battleground
SD%Complete: 100
SDComment: Spirit guides in battlegrounds will revive all players every 30 sec
SDCategory: Battlegrounds
EndScriptData */

#include "precompiled.h"

// **** Script Info ****
// Spiritguides in battlegrounds resurrecting many players at once
// every 30 seconds - through a channeled spell, which gets autocasted
// the whole time
// if spiritguide despawns all players around him will get teleported
// to the next spiritguide
// here i'm not sure, if a dummyspell exist for it

// **** Quick Info ****
// battleground spiritguides - this script handles gossipHello
// and JustDied also it let autocast the channel-spell

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel

    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT      = 2584                  // players who cancel this aura don't want a resurrection
};

struct MANGOS_DLL_DECL npc_spirit_guideAI : public ScriptedAI
{
    npc_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // auto cast the whole time this spell
        if (!m_creature->IsNonMeleeSpellCasted(false))
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, false);
    }

    void CorpseRemoved(uint32 &)
    {
        // TODO: would be better to cast a dummy spell
        Map* pMap = m_creature->GetMap();

        if (!pMap || !pMap->IsBattleGround())
            return;

        Map::PlayerList const &PlayerList = pMap->GetPlayers();

        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (!pPlayer || !pPlayer->IsWithinDistInMap(m_creature, 20.0f) || !pPlayer->HasAura(SPELL_WAITING_TO_RESURRECT))
                continue;

            // repop player again - now this node won't be counted and another node is searched
            pPlayer->RepopAtGraveyard();
        }
    }

    void SpellHitTarget (Unit* pUnit, const SpellEntry* pSpellEntry)
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER
            && pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, true);
    }
};

bool GossipHello_npc_spirit_guide(Player* pPlayer, Creature* pCreature)
{
    pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);
    return true;
}

CreatureAI* GetAI_npc_spirit_guide(Creature* pCreature)
{
    return new npc_spirit_guideAI(pCreature);
}

void AddSC_battleground()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_spirit_guide";
    newscript->GetAI = &GetAI_npc_spirit_guide;
    newscript->pGossipHello = &GossipHello_npc_spirit_guide;
    newscript->RegisterSelf();
}
