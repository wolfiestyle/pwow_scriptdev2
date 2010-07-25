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

#define INSTANCE_DATA_MAGIC "ICC"

using namespace serialize;

struct MANGOS_DLL_DECL instance_icecrown_citadel: public ScriptedInstance
{
    typedef std::map<uint32, uint32> InstanceDataMap;

    std::string m_strInstData;
    InstanceDataMap m_InstanceVars;     // data_id => value
    std::vector<uint64> m_guidsStore;   // data_id => guid

    instance_icecrown_citadel(Map *pMap):
        ScriptedInstance(pMap),
        m_guidsStore(DATA_GUID_MAX, 0)
    {
        Initialize();
    }

    void OnPlayerEnter(Player *pWho)
    {
        if (!m_InstanceVars[DATA_FACTION])
            m_InstanceVars[DATA_FACTION] = pWho->GetTeam();
    }

    void Initialize()
    {
        m_InstanceVars[DATA_FACTION] = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (InstanceDataMap::const_iterator i = m_InstanceVars.begin(); i != m_InstanceVars.end(); ++i)
        {
            if (i->first >= MAX_ENCOUNTER)
                break;
            if (i->second == IN_PROGRESS)
                return true;
        }
        return false;
    }

    void OnCreatureCreate(Creature *pCreature)
    {
        uint32 data_id = icc::GetType(pCreature);
        if (data_id < DATA_GUID_MAX)
            m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        uint32 data_id = icc::GetType(pGo);
        if (data_id < DATA_GUID_MAX)
        {
            m_guidsStore[data_id] = pGo->GetGUID();

            // restore door state on create
            if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR)
            {
                uint32 state = map_find(m_InstanceVars, data_id, MAX_GO_STATE);
                if (state < MAX_GO_STATE)
                    pGo->SetGoState(GOState(state));
            }
        }
    }

    void OnEncounterComplete(uint32 uiType)
    {
        uint32 loot_id = 0, phased_id = 0;
        std::list<uint32> door_ids;
        switch (uiType)
        {
            case TYPE_MARROWGAR:    
                door_ids.push_back(DATA_MARROWGAR_DOOR_1);
                door_ids.push_back(DATA_MARROWGAR_DOOR_2);
                break;
            case TYPE_DEATHWHISPER:
                phased_id = DATA_DEATHWHISPER_ELEV;
                break;
            case TYPE_SAURFANG:
                loot_id = DATA_SAURFANG_CHEST;
                door_ids.push_back(DATA_SAURFANG_DOOR);
                break;
            case TYPE_VALITHRIA:
                door_ids.push_back(DATA_VALITHRIA_DOOR_ENTRANCE);
                door_ids.push_back(DATA_VALITHRIA_DOOR_EXIT);
                door_ids.push_back(DATA_VALITHRIA_DOOR_LEFT_1);
                door_ids.push_back(DATA_VALITHRIA_DOOR_RIGHT_1);
                if (instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
                {
                    door_ids.push_back(DATA_VALITHRIA_DOOR_LEFT_2);
                    door_ids.push_back(DATA_VALITHRIA_DOOR_RIGHT_2);
                }
                loot_id = DATA_VALITHRIA_CHEST;
                break;
            case TYPE_ROTFACE:
            case TYPE_FESTERGUT:
                if (m_InstanceVars[TYPE_ROTFACE] == DONE && m_InstanceVars[TYPE_FESTERGUT] == DONE)
                {
                    door_ids.push_back(DATA_PUTRICIDE_UPPER_DOOR);
                    door_ids.push_back(DATA_PUTRICIDE_LOWER_DOOR);        // Note: does not animate opening of door.
                    door_ids.push_back(DATA_PUTRICIDE_LOWER_DOOR_ORANGE);
                    door_ids.push_back(DATA_PUTRICIDE_LOWER_DOOR_GREEN);
                }
                break;
            default:
                return;
        }

        if (loot_id)
            if (uint64 LootGuid = GetData64(loot_id))
                DoRespawnGameObject(LootGuid, 7*DAY);

        if (phased_id)
            if (uint64 ObjGuid = GetData64(phased_id))
                if (WorldObject *pObj = instance->GetWorldObject(ObjGuid))
                    pObj->SetPhaseMask(1, true);

        for (std::list<uint32>::const_iterator i = door_ids.begin(); i != door_ids.end(); ++i)
            if (uint64 DoorGuid = GetData64(*i))
            {
                DoUseDoorOrButton(DoorGuid);
                // store door state for saving
                if (GameObject *pGo = instance->GetGameObject(DoorGuid))
                    m_InstanceVars[*i] = pGo->GetGoState();
            }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        m_InstanceVars[uiType] = uiData;

        bool do_save = false;
        if (uiType < MAX_ENCOUNTER && uiData == DONE)
        {
            OnEncounterComplete(uiType);
            do_save = true;
        }

        std::ostringstream saveStream;
        saveStream << INSTANCE_DATA_MAGIC << ' ' << m_InstanceVars;

        m_strInstData = saveStream.str();

        if (do_save)
        {
            OUT_SAVE_INST_DATA;
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        return uiData < DATA_GUID_MAX ? m_guidsStore[uiData] : 0;
    }

    uint32 GetData(uint32 uiType)
    {
        return map_find(m_InstanceVars, uiType);
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

        std::istringstream loadStream(strIn);
        std::string magic;
        loadStream >> magic;
        if (magic != INSTANCE_DATA_MAGIC)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(strIn);

        loadStream >> m_InstanceVars;

        InstanceDataMap::iterator i = m_InstanceVars.begin();
        for (; i != m_InstanceVars.end(); ++i)
        {
            if (i->first >= MAX_ENCOUNTER)
                break;
            if (i->second == IN_PROGRESS)
                i->second = NOT_STARTED;
        }
        // restore door state on load
        for (; i != m_InstanceVars.end(); ++i)
        {
            if (i->first >= DATA_GUID_MAX)
                break;
            if (i->second >= MAX_GO_STATE)
                continue;
            if (uint64 guid = GetData64(i->first))
                if (GameObject *pGO = instance->GetGameObject(guid))
                    if (pGO->GetGoType() == GAMEOBJECT_TYPE_DOOR)
                        pGO->SetGoState(GOState(i->second));
        }

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
