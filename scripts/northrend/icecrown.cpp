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
SDName: Icecrown
SD%Complete: 100
SDComment: Quest support: 12807, Vendor support: 34885
SDCategory: Icecrown
EndScriptData */

/* ContentData
npc_arete
npc_dame_evniki_kapsalis
EndContentData */

#include "precompiled.h"

/*######
## npc_arete
######*/

#define GOSSIP_ARETE_ITEM1 "Lord-Commander, I would hear your tale."
#define GOSSIP_ARETE_ITEM2 "<You nod slightly but do not complete the motion as the Lord-Commander narrows his eyes before he continues.>"
#define GOSSIP_ARETE_ITEM3 "I thought that they now called themselves the Scarlet Onslaught?"
#define GOSSIP_ARETE_ITEM4 "Where did the grand admiral go?"
#define GOSSIP_ARETE_ITEM5 "That's fine. When do I start?"
#define GOSSIP_ARETE_ITEM6 "Let's finish this!"
#define GOSSIP_ARETE_ITEM7 "That's quite a tale, Lord-Commander."

enum
{
    GOSSIP_TEXTID_ARETE1        = 13525,
    GOSSIP_TEXTID_ARETE2        = 13526,
    GOSSIP_TEXTID_ARETE3        = 13527,
    GOSSIP_TEXTID_ARETE4        = 13528,
    GOSSIP_TEXTID_ARETE5        = 13529,
    GOSSIP_TEXTID_ARETE6        = 13530,
    GOSSIP_TEXTID_ARETE7        = 13531,

    QUEST_THE_STORY_THUS_FAR    = 12807
};

bool GossipHello_npc_arete(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_THE_STORY_THUS_FAR) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE1, pCreature->GetGUID());
        return true;
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_arete(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE2, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE3, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE4, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE5, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE6, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE7, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(QUEST_THE_STORY_THUS_FAR);
            break;
    }

    return true;
}

/*######
## npc_dame_evniki_kapsalis
######*/

enum
{
    TITLE_CRUSADER    = 123
};

bool GossipHello_npc_dame_evniki_kapsalis(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->HasTitle(TITLE_CRUSADER))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_dame_evniki_kapsalis(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    return true;
}
/*######
## npc_Argent_Valiant
######*/

enum
{
    // Says
    SAY_AGGRO = -1999900,

    // Spells
    SPELL_BUFF_DEFEND = 62719,        //Spell - Defend
    SPELL_CHARGE = 63010,             //Spell - Charge
    SPELL_SHIELD_BREAKER = 64590,     //Spell - Shield Breaker

    FACTION_HOSTILE	= 128,           // Sets Faction to Enemy
    FACTION_FRIENDLY= 35,            // Sets Faction to Friendly

    // Events
    EVENT_REBUFF = 1,
    EVENT_SBREAKER,
    EVENT_CHARGE_MOVE,
    EVENT_CHARGE_MOVE_2,

    // AreaIDs
    AREA_CHAMPIONS_RING = 4669,
    AREA_ASPIRANTS_RING = 4670, // not really used
    AREA_VALIANTS_RING  = 4671,
    AREA_ALLIANCE_RING  = 4672,
    AREA_HORDE_RING     = 4673,

    // NPCs
    NPC_ARGENT_CHAMPION         = 33707,
    NPC_STORMWIND_VALIANT       = 33564,    /* Stormwind Valiant */
    NPC_IRONFORGE_VALIANT       = 33558,    /* Ironforge Valiant */
    NPC_EXODAR_VALIANT          = 33562,    /* Exodar Valiant */
    NPC_DARNASSUS_VALIANT       = 33559,    /* Darnassus Valiant */
    NPC_GNOMEREGAN_VALIANT      = 33561,    /* Gnomeregan Valiant */

    NPC_ORGRIMMAR_VALIANT       = 33306,    /* Orgrimmar Valiant */
    NPC_UNDERCITY_VALIANT       = 33384,    /* Undercity Valiant */
    NPC_SILVERMOON_VALIANT      = 33382,    /* Silvermoon Valiant */
    NPC_THUNDERBLUFF_VALIANT    = 33383,    /* Thunder Bluff Valiant */
    NPC_DARKSPEAR_VALIANT       = 33285,    /* Sen'jin Valiant */
    // credit
    ARGENT_VALIANT_QUEST_CREDIT = 33708,

};

#define GOSSIP_FIGHT "I'm Ready!"

#define TIMER_REBUFF    5*IN_MILLISECONDS, 10*IN_MILLISECONDS
#define TIMER_SBREAKER  8*IN_MILLISECONDS, 7*IN_MILLISECONDS
#define TIMER_MOVE      10*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_argent_valiantAI : public ScriptedAI, EventManager
{
    EventManager Events;
    uint32 m_uiAreaID;

    npc_argent_valiantAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        m_uiAreaID = m_creature->GetAreaId();
        if (m_uiAreaID != AREA_ALLIANCE_RING &&
            m_uiAreaID != AREA_HORDE_RING &&
            m_uiAreaID != AREA_ASPIRANTS_RING ) // Argent Champion is standing outside the ring.
            m_uiAreaID = AREA_VALIANTS_RING;

        switch(pCreature->GetEntry())
        {
            case NPC_UNDERCITY_VALIANT:
            case NPC_ORGRIMMAR_VALIANT:
            case NPC_SILVERMOON_VALIANT:
            case NPC_DARKSPEAR_VALIANT:
            case NPC_THUNDERBLUFF_VALIANT:
                m_uiAreaID = AREA_HORDE_RING;
                break;
            case NPC_GNOMEREGAN_VALIANT:
            case NPC_IRONFORGE_VALIANT:
            case NPC_DARNASSUS_VALIANT:
            case NPC_EXODAR_VALIANT:
            case NPC_STORMWIND_VALIANT:
                m_uiAreaID = AREA_ALLIANCE_RING;
                break;
        }

        if (m_uiAreaID == AREA_VALIANTS_RING)
            pCreature->GetMotionMaster()->MovePoint(1, 8541.77f, 1084.1f, 556.374f);

        pCreature->setFaction(FACTION_FRIENDLY);
    }

    void Reset()
    {
        Events.Reset();
        m_creature->setFaction(FACTION_FRIENDLY);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpellInfo)
    {
        if (pSpellInfo->Id == SPELL_SHIELD_BREAKER || pSpellInfo->Id == SPELL_CHARGE)
        {
            if (pTarget->HasAura(66482)) // player versions of "defend"
                pTarget->RemoveAuraHolderFromStack(66482);
            if (pTarget->HasAura(62552)) // player versions of "defend"
                pTarget->RemoveAuraHolderFromStack(62552);
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpellInfo)
    {
        if (pSpellInfo->Id == 64595 || pSpellInfo->Id == 62563) // player version of "shield breaker" and "charge"
        {
            if (m_creature->HasAura(SPELL_BUFF_DEFEND))
                m_creature->RemoveAuraHolderFromStack(SPELL_BUFF_DEFEND);
        }
    }
    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature, pWho);
        DoCast(m_creature, SPELL_BUFF_DEFEND, true);
        Events.ScheduleEvent(EVENT_REBUFF, TIMER_REBUFF);
        Events.ScheduleEvent(EVENT_CHARGE_MOVE, 5*IN_MILLISECONDS, TIMER_MOVE);
        Events.ScheduleEvent(EVENT_SBREAKER, TIMER_SBREAKER);
    }

    void JustDied(Unit* pSlayer)
    {
        if (m_creature->GetEntry() == NPC_ARGENT_CHAMPION)
            if (Unit* realSlayer = pSlayer->GetCharmerOrOwnerOrSelf())
                if (realSlayer->GetTypeId() == TYPEID_PLAYER)
                    ((Player*))realSlayer)->KilledMonsterCredit(ARGENT_VALIANT_QUEST_CREDIT, m_creature->GetGUID());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // return if we go too far from our fighting ring
        if (m_creature->GetAreaId() != m_uiAreaID)
            EnterEvadeMode();

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
            case EVENT_REBUFF:
                DoCast(m_creature, SPELL_BUFF_DEFEND, true);
                break;
            case EVENT_SBREAKER:
                if (!m_creature->getVictim()->IsWithinDist(m_creature, 10.0f))
                    DoCast(m_creature->getVictim(), SPELL_SHIELD_BREAKER, false);
                break;
            case EVENT_CHARGE_MOVE:
            {
                Unit* pTarget = m_creature->getVictim();
                if (pTarget)
                    DoStartNoMovement(pTarget);

                float x, y, z;
                pTarget->GetPosition(x, y, z);
                float px, py, pz;
                do
                {
                    pTarget->GetPosition(x, y, z);
                    m_creature->GetRandomPoint(x, y, z, 20, px, py, pz);
                }
                while (m_creature->GetMap()->GetTerrain()->GetAreaId(px, py, pz) != m_uiAreaID);

                m_creature->GetMotionMaster()->MovePoint(1, px, py, pz);
                Events.ScheduleEvent(EVENT_CHARGE_MOVE_2, 2*IN_MILLISECONDS);
                break;
            }
            case EVENT_CHARGE_MOVE_2:
            {
                // we assume we got to the spot    
                Unit* pTarget = m_creature->getVictim();

                if (m_creature->getVictim())
                    DoStartMovement(m_creature->getVictim());

                if (!m_creature->getVictim()->IsWithinDist(m_creature, 10.0f))
                    DoCast(m_creature->getVictim(), SPELL_CHARGE, false);
                break;
            }
            default:
                break;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_argent_valiant(Creature* pCreature)
{
    return new npc_argent_valiantAI(pCreature);
}

bool GossipSelect_npc_argent_valiant(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (!pPlayer)
        return false;

    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->setFaction(FACTION_HOSTILE);                //Set our faction to hostile towards all
        //pCreature->AI()->AttackStart(pPlayer);               // pPlayer may be unattackable (seat flags for horse mount make it that way)
    }

    return true;
}

bool GossipHello_npc_argent_valiant(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetGUID());
    return true;
}

void AddSC_icecrown()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_arete";
    newscript->pGossipHello = &GossipHello_npc_arete;
    newscript->pGossipSelect = &GossipSelect_npc_arete;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_dame_evniki_kapsalis";
    newscript->pGossipHello = &GossipHello_npc_dame_evniki_kapsalis;
    newscript->pGossipSelect = &GossipSelect_npc_dame_evniki_kapsalis;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_argent_valiant";
    newscript->pGossipHello = &GossipHello_npc_argent_valiant;
    newscript->pGossipSelect = &GossipSelect_npc_argent_valiant;
    newscript->GetAI = &GetAI_npc_argent_valiant;
    newscript->RegisterSelf();
}
