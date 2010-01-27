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
SDName: Instance_Ulduar
SD%Complete: 100%
SDComment: 
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

struct MANGOS_DLL_DECL instance_ulduar : public ScriptedInstance
{
    instance_ulduar(Map* pMap) : ScriptedInstance(pMap) { Initialize(); }

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string m_strInstData;

    typedef std::map<uint32, uint64> GuidMap;
    GuidMap m_guidsStore;

    uint32 m_uiNumSentries;
    uint32 m_uiTeleporterData;

    void Initialize()
    {
        m_uiNumSentries     = 0;
        m_uiTeleporterData  = 0;

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
        uint32 data_id;
        switch(pCreature->GetEntry())
        {
            case NPC_LEVIATHAN:     data_id = TYPE_LEVIATHAN; break;
            case NPC_IGNIS:         data_id = TYPE_IGNIS; break;
            case NPC_RAZORSCALE:    data_id = TYPE_RAZORSCALE; break;
            case NPC_XT002:         data_id = TYPE_XT002; break;

            // Assembly of Iron
            case NPC_STEELBREAKER:  data_id = DATA_STEELBREAKER; break;
            case NPC_MOLGEIM:       data_id = DATA_MOLGEIM; break;
            case NPC_BRUNDIR:       data_id = DATA_BRUNDIR; break;

            case NPC_KOLOGARN:      data_id = TYPE_KOLOGARN; break;
            case NPC_LEFT_ARM:      data_id = DATA_LEFT_ARM; break;
            case NPC_RIGHT_ARM:     data_id = DATA_RIGHT_ARM; break;

            case NPC_AURIAYA:       data_id = TYPE_AURIAYA; break;
            case NPC_SANCTUM_SENTRY:
                if (m_uiNumSentries < 4)
                {
                     data_id = DATA_SANCTUM_SENTRY + m_uiNumSentries++;
                     break;
                }
                return;

            case NPC_MIMIRON:       data_id = TYPE_MIMIRON; break;
            case NPC_HODIR:         data_id = TYPE_HODIR; break;
            case NPC_THORIM:        data_id = TYPE_THORIM; break;
            case NPC_FREYA:         data_id = TYPE_FREYA; break;
            case NPC_VEZAX:         data_id = TYPE_VEZAX; break;
            case NPC_YOGGSARON:     data_id = TYPE_YOGGSARON; break;
            case NPC_ALGALON:       data_id = TYPE_ALGALON; break;

            default:
                return;
        }

        m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        uint32 data_id;
        switch (pGo->GetEntry())
        {
            case GO_KOLOGARN_CHEST:
            case GO_KOLOGARN_CHEST_H:
                data_id = DATA_KOLOGARN_CHEST;
                break;
            case GO_FREYA_CHEST:
            case GO_FREYA_CHEST_H:
                data_id = DATA_FREYA_CHEST;
                break;
            case GO_HODIR_CHEST:
            case GO_HODIR_CHEST_H:
                data_id = DATA_HODIR_CHEST;
                break;
            case GO_HODIR_CHEST_HARD:
            case GO_HODIR_CHEST_HARD_H:
                data_id = DATA_HODIR_CHEST_HARD;
                break;
            case GO_MIMIRON_CHEST:
            case GO_MIMIRON_CHEST_H:
                data_id = DATA_MIMIRON_CHEST;
                break;
            case GO_THORIM_CHEST:
            case GO_THORIM_CHEST_H:
                data_id = DATA_THORIM_CHEST;
                break;
            case GO_ALGALON_CHEST:
            case GO_ALGALON_CHEST_H:
                data_id = DATA_ALGALON_CHEST;
                break;

            case GO_ASSEMBLY_DOOR:  data_id = DATA_ASSEMBLY_DOOR; break;
            case GO_HODIR_DOOR:     data_id = DATA_HODIR_DOOR; break;
            case GO_VEZAX_DOOR:     data_id = DATA_VEZAX_DOOR; break;

            default:
                return;
        }

        m_guidsStore[data_id] = pGo->GetGUID();
    }

    void OnEncounterComplete(uint32 uiType)
    {
        uint32 loot_id = 0, door_id = 0;
        switch (uiType)
        {
            case TYPE_ASSEMBLY:
                door_id = DATA_ASSEMBLY_DOOR;
                break;
            case TYPE_KOLOGARN:
                //loot_id = DATA_KOLOGARN_CHEST;
                break;
            case TYPE_MIMIRON:
                //loot_id = DATA_MIMIRON_CHEST;
                break;
            case TYPE_HODIR:
                //loot_id = DATA_HODIR_CHEST;
                door_id = DATA_HODIR_DOOR;
                break;
            case TYPE_THORIM:
                //loot_id = DATA_THORIM_CHEST;
                break;
            case TYPE_FREYA:
                //loot_id = DATA_FREYA_CHEST;
                break;
            case TYPE_VEZAX:
                door_id = DATA_VEZAX_DOOR;
                break;
            case TYPE_ALGALON:
                loot_id = DATA_ALGALON_CHEST;
                break;
            default:
                return;
        }

        if (loot_id)
            if (uint64 LootGuid = GetData64(loot_id))
                DoRespawnGameObject(LootGuid, 7*DAY);
        if (door_id)
            if (uint64 DoorGuid = GetData64(door_id))
                DoUseDoorOrButton(DoorGuid);
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_LEVIATHAN:
            case TYPE_IGNIS:
            case TYPE_RAZORSCALE:
            case TYPE_XT002:
            case TYPE_ASSEMBLY:
            case TYPE_KOLOGARN:
            case TYPE_AURIAYA:
            case TYPE_MIMIRON:
            case TYPE_HODIR:
            case TYPE_THORIM:
            case TYPE_FREYA:
            case TYPE_VEZAX:
            case TYPE_YOGGSARON:
            case TYPE_ALGALON:
                m_auiEncounter[uiType] = uiData;
                switch (uiData)
                {
                    case IN_PROGRESS:
                        m_uiTeleporterData |= 1 << uiType;
                        break;
                    case DONE:
                        m_uiTeleporterData |= 0x10000 << uiType;
                        break;
                }
                break;
        }

        std::ostringstream saveStream;

        for(size_t i = 0; i < MAX_ENCOUNTER; ++i)
            saveStream << m_auiEncounter[i] << " ";
        saveStream << m_uiTeleporterData;

        m_strInstData = saveStream.str();

        if (uiData == DONE)
        {
            OnEncounterComplete(uiType);

            OUT_SAVE_INST_DATA;
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        GuidMap::const_iterator it = m_guidsStore.find(uiData);
        return it != m_guidsStore.end() ? it->second : 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_LEVIATHAN:
            case TYPE_IGNIS:
            case TYPE_RAZORSCALE:
            case TYPE_XT002:
            case TYPE_ASSEMBLY:
            case TYPE_KOLOGARN:
            case TYPE_AURIAYA:
            case TYPE_MIMIRON:
            case TYPE_HODIR:
            case TYPE_THORIM:
            case TYPE_FREYA:
            case TYPE_VEZAX:
            case TYPE_YOGGSARON:
            case TYPE_ALGALON:
                return m_auiEncounter[uiType];
            case DATA_BOSS_ENGAGED:
                return m_uiTeleporterData & 0xFFFF;
            case DATA_BOSS_DONE:
                return m_uiTeleporterData >> 16;
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
        uint32 tp_data;
        loadStream >> tp_data;
        m_uiTeleporterData |= tp_data;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_ulduar(Map* pMap)
{
    return new instance_ulduar(pMap);
}

void AddSC_instance_ulduar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_ulduar";
    newscript->GetInstanceData = &GetInstanceData_instance_ulduar;
    newscript->RegisterSelf();
}
