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
SDName: Instance_Trial_Of_the_Champion
SD%Complete: 100
SDComment: 
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

struct MANGOS_DLL_DECL instance_trial_of_the_champion : public ScriptedInstance
{
    instance_trial_of_the_champion(Map* pMap) : ScriptedInstance(pMap) { Initialize(); }

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string m_strInstData;

    uint64 m_uiEadricGUID;
    uint64 m_uiEadricLootGUID;
    uint64 m_uiPaletressGUID;
    uint64 m_uiPaletressLootGUID;
    uint64 m_uiBlackKnightGUID;
    uint64 m_uiBlackKnightMinionGUID;
    uint32 m_uiBlackKnightMinionID;
    uint64 m_uiJaerenGUID;
    uint64 m_uiArelasGUID;
    uint64 m_uiAnnouncerGUID;
    uint32 m_uiChampionId1;
    uint32 m_uiChampionId2;
    uint32 m_uiChampionId3;
    uint64 m_uiChampion1;
    uint64 m_uiChampion2;
    uint64 m_uiChampion3;
    uint64 m_uiChampionsLootGUID;
    uint64 m_uiArgentChallenger;
    uint64 m_uiArgentChallengerID;
    uint64 m_uiMemoryGUID;

    void Initialize()
    {
        m_uiEadricGUID          = 0;
        m_uiEadricLootGUID      = 0;
        m_uiPaletressGUID       = 0;
        m_uiPaletressLootGUID   = 0;
        m_uiBlackKnightGUID     = 0;
        m_uiBlackKnightMinionGUID = 0;
        m_uiBlackKnightMinionID = 0;
        m_uiJaerenGUID          = 0;
        m_uiArelasGUID          = 0;
        m_uiAnnouncerGUID       = 0;
        m_uiChampionId1         = 0;
        m_uiChampionId2         = 0;
        m_uiChampionId3         = 0;
        m_uiChampion1           = 0;
        m_uiChampion2           = 0;
        m_uiChampion3           = 0;
        m_uiChampionsLootGUID   = 0;
        m_uiArgentChallenger    = 0;
        m_uiArgentChallengerID  = 0;
        m_uiMemoryGUID          = 0;

        memset(m_auiEncounter, 0, sizeof(m_auiEncounter));
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            // Champions of the Alliance
            case NPC_JACOB:
            case NPC_AMBROSE:
            case NPC_COLOSOS:
            case NPC_JAELYNE:
            case NPC_LANA:
            // Champions of the Horde
            case NPC_MOKRA:
            case NPC_ERESSEA:
            case NPC_RUNOK:
            case NPC_ZULTORE:
            case NPC_VISCERI:
                if (!m_uiChampion1)
                    m_uiChampion1 = pCreature->GetGUID();
                else if (!m_uiChampion2)
                    m_uiChampion2 = pCreature->GetGUID();
                else if (!m_uiChampion3)
                    m_uiChampion3 = pCreature->GetGUID();
                break;

            // Argent Challenge
            case NPC_EADRIC:
                m_uiEadricGUID = pCreature->GetGUID();
                m_uiArgentChallenger = pCreature->GetGUID();
                break;
            case NPC_PALETRESS:
                m_uiPaletressGUID = pCreature->GetGUID();
                m_uiArgentChallenger = pCreature->GetGUID();
                break;

            // Black Knight
            case NPC_BLACK_KNIGHT:
                m_uiBlackKnightGUID = pCreature->GetGUID();
                break;
            case NPC_RISEN_JAEREN:
            case NPC_RISEN_ARELAS:
                m_uiBlackKnightMinionGUID = pCreature->GetGUID();
                break;

            // Coliseum Announcers
            case NPC_JAEREN:
                m_uiJaerenGUID = pCreature->GetGUID();
                break;
            case NPC_ARELAS:
                m_uiArelasGUID = pCreature->GetGUID();
                break;

            // memories
            case MEMORY_ALGALON:
            case MEMORY_ARCHIMONDE:
            case MEMORY_CHROMAGGUS:
            case MEMORY_CYANIGOSA:
            case MEMORY_DELRISSA:
            case MEMORY_ECK:
            case MEMORY_ENTROPIUS:
            case MEMORY_GRUUL:
            case MEMORY_HAKKAR:
            case MEMORY_HEIGAN:
            case MEMORY_HEROD:
            case MEMORY_HOGGER:
            case MEMORY_IGNIS:
            case MEMORY_ILLIDAN:
            case MEMORY_INGVAR:
            case MEMORY_KALITHRESH:
            case MEMORY_LUCIFRON:
            case MEMORY_MALCHEZAAR:
            case MEMORY_MUTANUS:
            case MEMORY_ONYXIA:
            case MEMORY_THUNDERAAN:
            case MEMORY_VANCLEEF:
            case MEMORY_VASHJ:
            case MEMORY_VEKNILASH:
            case MEMORY_VEZAX:
                m_uiMemoryGUID = pCreature->GetGUID();
                break;
        }
    }

    void OnObjectCreate(GameObject *pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_CHAMPIONS_LOOT:
            case GO_CHAMPIONS_LOOT_H:
                m_uiChampionsLootGUID = pGo->GetGUID();
                break;
            case GO_EADRIC_LOOT:
            case GO_EADRIC_LOOT_H:
                m_uiEadricLootGUID = pGo->GetGUID();
                break;
            case GO_PALETRESS_LOOT:
            case GO_PALETRESS_LOOT_H:
                m_uiPaletressLootGUID = pGo->GetGUID();
                break;            
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case DATA_CHAMPIONID_1:
                m_uiChampionId1 = uiData;
                break;
            case DATA_CHAMPIONID_2:
                m_uiChampionId2 = uiData;
                break;
            case DATA_CHAMPIONID_3:
                m_uiChampionId3 = uiData;
                break;
            case DATA_ARGENT_CHALLENGER:
                m_uiArgentChallengerID = uiData;
                break;
            case DATA_BLACK_KNIGHT_MINION:
                m_uiBlackKnightMinionID = uiData;
                break;
            case TYPE_GRAND_CHAMPIONS:
                m_auiEncounter[0] = uiData;
                if (uiData == DONE && m_uiChampionsLootGUID)
                    DoRespawnGameObject(m_uiChampionsLootGUID, 7*DAY);
                break;
            case TYPE_ARGENT_CHALLENGE:
                m_auiEncounter[1] = uiData;
                if (uiData == DONE)
                {
                    if (m_uiArgentChallenger == m_uiEadricGUID && m_uiEadricLootGUID)
                        DoRespawnGameObject(m_uiEadricLootGUID, 7*DAY);
                    if (m_uiArgentChallenger == m_uiPaletressGUID && m_uiPaletressLootGUID)
                        DoRespawnGameObject(m_uiPaletressLootGUID, 7*DAY);
                }
                break;
            case TYPE_BLACK_KNIGHT:
                m_auiEncounter[2] = uiData;
                break;
        }

        std::ostringstream saveStream;
        for(uint32 i = 0; i < MAX_ENCOUNTER; ++i)   //FIXME: should save more stuff to properly load in case of crash
            saveStream << m_auiEncounter[i] << " ";
        m_strInstData = saveStream.str();

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case DATA_CHAMPIONID_1:
                return m_uiChampionId1;
            case DATA_CHAMPIONID_2:
                return m_uiChampionId2;
            case DATA_CHAMPIONID_3:
                return m_uiChampionId3;
            case DATA_ARGENT_CHALLENGER:
                return m_uiArgentChallengerID;
            case DATA_BLACK_KNIGHT_MINION:
                return m_uiBlackKnightMinionID;
            case TYPE_GRAND_CHAMPIONS:
            case TYPE_ARGENT_CHALLENGE:
            case TYPE_BLACK_KNIGHT:
                return m_auiEncounter[uiType];
        }

        return 0;
    }

    void SetData64(uint32 uiType, uint64 uiData)
    {
        switch (uiType)
        {
            case DATA_TOC5_ANNOUNCER:
                m_uiAnnouncerGUID = uiData;
                break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case DATA_CHAMPION_1:
                return m_uiChampion1;
            case DATA_CHAMPION_2:
                return m_uiChampion2;
            case DATA_CHAMPION_3:
                return m_uiChampion3;
            case DATA_MEMORY:
                return m_uiMemoryGUID;
            case DATA_BLACK_KNIGHT:
                return m_uiBlackKnightGUID;
            case DATA_BLACK_KNIGHT_MINION:
                return m_uiBlackKnightMinionGUID;
            case DATA_JAEREN:
                return m_uiJaerenGUID;
            case DATA_ARELAS:
                return m_uiArelasGUID;
            case DATA_TOC5_ANNOUNCER:
                return m_uiAnnouncerGUID;
        }

        return 0;
    }

    const char* Save()
    {
        return m_strInstData.c_str();
    }

    void Load(const char* strIn)
    {
        if (!strIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(strIn);

        std::istringstream loadStream(strIn);

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            loadStream >> m_auiEncounter[i];

            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_champion(Map* pMap)
{
    return new instance_trial_of_the_champion(pMap);
}

void AddSC_instance_trial_of_the_champion()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_champion";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_champion;
    newscript->RegisterSelf();
}
