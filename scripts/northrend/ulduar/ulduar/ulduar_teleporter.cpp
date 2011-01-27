#include "precompiled.h"
#include "ulduar.h"

enum
{
    TELEPORT_EXPEDITION_BASE_CAMP  = 64014,
    TELEPORT_FORMATION_GROUNDS     = 64032,
    TELEPORT_COLOSSAL_FORGE        = 64028,
    TELEPORT_SCRAPYARD             = 64031,
    TELEPORT_ANTECHAMBER_OF_ULDUAR = 64030,
    TELEPORT_SHATTERED_WALKWAY     = 64029,
    TELEPORT_CONSERVATORY_OF_LIFE  = 64024,
    TELEPORT_SPARK_OF_IMAGINATION  = 65061,
    TELEPORT_PRISON_OF_YOGG_SARON  = 65042,

    GOSSIP_TEXT_ID                 = 14424
};

#define G_EXPEDITION_BASE_CAMP  "Expedition Base Camp"
#define G_FORMATION_GROUNDS     "Formation Grounds"
#define G_COLOSSAL_FORGE        "Colossal Forge"
#define G_SCRAPYARD             "Scrapyard"
#define G_ANTECHAMBER_OF_ULDUAR "Antechamber of Ulduar"
#define G_SHATTERED_WALKWAY     "Shattered Walkway"
#define G_CONSERVATORY_OF_LIFE  "Conservatory of Life"
#define G_SPARK_OF_IMAGINATION  "Spark of Imagination"
#define G_PRISON_OF_YOGG_SARON  "Prison of Yogg-Saron"

bool GOHello_ulduar_teleporter(Player *pPlayer, GameObject *pGo)
{
    ScriptedInstance *pInstance = dynamic_cast<ScriptedInstance*>(pGo->GetInstanceData());
    if (!pInstance)
        return true;
    std::bitset<16> bossEngaged(pInstance->GetData(DATA_BOSS_ENGAGED));
    std::bitset<16> bossDone(pInstance->GetData(DATA_BOSS_DONE));
    bool skipCheck = pPlayer->isGameMaster();  // allow GM to teleport anywhere

    if (!skipCheck && pPlayer->isInCombat())   // dont allow using in combat
    {
        WorldPacket data(SMSG_CAST_FAILED, (4+1+1));
        data << uint8(0);
        data << uint32(7791);
        data << uint8(SPELL_FAILED_AFFECTING_COMBAT);
        pPlayer->GetSession()->SendPacket(&data);
        return true;
    }

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_EXPEDITION_BASE_CAMP,  GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    if (skipCheck || bossEngaged[TYPE_LEVIATHAN])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_FORMATION_GROUNDS,     GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
    if (skipCheck || bossDone[TYPE_LEVIATHAN] || bossEngaged[TYPE_IGNIS] || bossEngaged[TYPE_RAZORSCALE] || bossEngaged[TYPE_XT002])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_COLOSSAL_FORGE,        GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
    if (skipCheck || bossDone[TYPE_XT002])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_SCRAPYARD,             GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
    if (skipCheck || bossDone[TYPE_IGNIS] || bossDone[TYPE_RAZORSCALE] || bossDone[TYPE_XT002])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_ANTECHAMBER_OF_ULDUAR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
    if (skipCheck || bossDone[TYPE_KOLOGARN] || bossEngaged[TYPE_AURIAYA] || bossEngaged[TYPE_HODIR] || bossEngaged[TYPE_VEZAX])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_SHATTERED_WALKWAY,     GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
    if (skipCheck || bossDone[TYPE_AURIAYA] || bossEngaged[TYPE_FREYA] || bossEngaged[TYPE_THORIM])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_CONSERVATORY_OF_LIFE,  GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
    if (skipCheck || bossEngaged[TYPE_MIMIRON])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_SPARK_OF_IMAGINATION,  GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+8);
    if (skipCheck || bossDone[TYPE_VEZAX] || bossEngaged[TYPE_YOGGSARON])
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, G_PRISON_OF_YOGG_SARON,  GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID, pGo->GetGUID());

    return true;
}

bool GOSelect_ulduar_teleporter(Player* pPlayer, GameObject* pGo, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CastSpell(pPlayer, TELEPORT_EXPEDITION_BASE_CAMP, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CastSpell(pPlayer, TELEPORT_FORMATION_GROUNDS, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CastSpell(pPlayer, TELEPORT_COLOSSAL_FORGE, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CastSpell(pPlayer, TELEPORT_SCRAPYARD, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CastSpell(pPlayer, TELEPORT_ANTECHAMBER_OF_ULDUAR, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->CastSpell(pPlayer, TELEPORT_SHATTERED_WALKWAY, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CastSpell(pPlayer, TELEPORT_CONSERVATORY_OF_LIFE, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            pPlayer->CastSpell(pPlayer, TELEPORT_SPARK_OF_IMAGINATION, true);
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            pPlayer->CastSpell(pPlayer, TELEPORT_PRISON_OF_YOGG_SARON, true);
            break;
        default:
            break;
    }

    pPlayer->CLOSE_GOSSIP_MENU();

    return true;
}

void AddSC_ulduar_teleporter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_ulduar_teleporter";
    newscript->pGossipHelloGO = &GOHello_ulduar_teleporter;
    newscript->pGossipSelectGO = &GOSelect_ulduar_teleporter;
    newscript->RegisterSelf();
}
