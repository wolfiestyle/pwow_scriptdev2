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
SDName: Instance Icecrown Citadel
SD%Complete: 90
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

/* ContentData
instance_icecrown_citadel
EndContentData */

#include "precompiled.h"
#include "icecrown_citadel.h"
#include <iterator>

struct MANGOS_DLL_DECL instance_icecrown_citadel: public ScriptedInstance
{
    std::vector<uint32> m_auiEncounter;
    std::string m_strInstData;

    typedef std::vector<uint64> GuidContainer;  // data_id => guid
    GuidContainer m_guidsStore;

    instance_icecrown_citadel(Map *pMap):
        ScriptedInstance(pMap),
        m_auiEncounter(MAX_ENCOUNTER, 0),
        m_guidsStore(DATA_MAX, 0)
    {
        Initialize();
    }

    void OnPlayerEnter(Player *pWho)
    {
    }

    void Initialize()
    {
    }

    bool IsEncounterInProgress() const
    {
        return std::find(m_auiEncounter.begin(), m_auiEncounter.end(), IN_PROGRESS) != m_auiEncounter.end();
    }

    void OnCreatureCreate(Creature *pCreature)
    {
        uint32 data_id = icc::GetType(pCreature);
        if (data_id < DATA_MAX)
            m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        uint32 data_id = icc::GetType(pGo);
        if (data_id < DATA_MAX)
            m_guidsStore[data_id] = pGo->GetGUID();
    }

    void OnEncounterComplete(uint32 uiType)
    {
        uint32 loot_id = 0, door_id = 0, door_id2 = 0;
        switch (uiType)
        {
            case TYPE_MARROWGAR:    
                door_id = DATA_MARROWGAR_DOOR_1;
                door_id2 = DATA_MARROWGAR_DOOR_2;
                break;
            case TYPE_SAURFANG:
                loot_id = DATA_SAURFANG_CHEST;
                door_id = DATA_SAURFANG_DOOR;
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
        if (door_id2)
            if (uint64 DoorGuid = GetData64(door_id2))
                DoUseDoorOrButton(DoorGuid);
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
            default:
                if (uiType < MAX_ENCOUNTER)
                    m_auiEncounter[uiType] = uiData;
                break;
        }

        std::ostringstream saveStream;
        std::copy(m_auiEncounter.begin(), m_auiEncounter.end(), std::ostream_iterator<uint32>(saveStream, " "));

        m_strInstData = saveStream.str();

        if (uiType < MAX_ENCOUNTER && uiData == DONE)
        {
            OnEncounterComplete(uiType);

            OUT_SAVE_INST_DATA;
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        return uiData < DATA_MAX ? m_guidsStore[uiData] : 0;
    }

    uint32 GetData(uint32 uiType)
    {
        if (uiType < MAX_ENCOUNTER)
            return m_auiEncounter[uiType];

        switch(uiType)
        {
            default:
                return 0;
        }
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

        for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
            loadStream >> m_auiEncounter[i];

        std::replace(m_auiEncounter.begin(), m_auiEncounter.end(), IN_PROGRESS, NOT_STARTED);

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    bool CheckAchievementCriteriaMeet(uint32 CriteriaId, Player const* pPlayer, Unit const* pWho, uint32 misc1)
    {
        switch (CriteriaId)
        {
            default:
                return false;
        }
    }
};

InstanceData* GetInstanceData_instance_icecrown_citadel(Map* pMap)
{
    return new instance_icecrown_citadel(pMap);
}

void AddSC_instance_icecrown_citadel()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_icecrown_citadel";
    newscript->GetInstanceData = &GetInstanceData_instance_icecrown_citadel;
    newscript->RegisterSelf();
}
