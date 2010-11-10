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
SDName: Instance Ruby Sanctum
SD%Complete: 0%
SDComment:
SDCategory: Ruby Sanctum
EndScriptData */

/* ContentData
instance_icecrown_citadel
EndContentData */

#include "precompiled.h"
#include "ruby_sanctum.h"

#define INSTANCE_DATA_MAGIC "RS"

using namespace serialize;

struct MANGOS_DLL_DECL instance_ruby_sanctum: public ScriptedInstance
{
    typedef std::map<uint32, uint32> InstanceDataMap;
    std::string m_strInstData;
    InstanceDataMap m_InstanceVars;     // data_id => value
    std::vector<uint64> m_guidsStore;   // data_id => guid

    instance_ruby_sanctum(Map *pMap):
        ScriptedInstance(pMap),
        m_guidsStore(DATA_GUID_MAX, 0)
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
        uint32 data_id = rs::GetType(pCreature);
        if (data_id < DATA_GUID_MAX)
            m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        uint32 data_id = rs::GetType(pGo);
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

    bool HalionCheck() const
    {
        return  map_find(m_InstanceVars, TYPE_SAVIANA) == DONE &&
                map_find(m_InstanceVars, TYPE_ZARITHRIAN) == DONE &&
                map_find(m_InstanceVars, TYPE_BALTHARUS) == DONE;
    }

    void OnEncounterComplete(uint32 uiType)
    {
        std::deque<uint32> go_ids;

        switch (uiType)
        {
            case TYPE_BALTHARUS:
                go_ids.push_back(DATA_FIRE_FIELD);
                if (HalionCheck())
                {
                    go_ids.push_back(DATA_BURNING_TREE_1);
                    go_ids.push_back(DATA_BURNING_TREE_2);
                    go_ids.push_back(DATA_BURNING_TREE_3);
                    go_ids.push_back(DATA_BURNING_TREE_4);
                }
                break;
            case TYPE_ZARITHRIAN:
                go_ids.push_back(DATA_FLAME_WALLS);
                // (no break)
            case TYPE_SAVIANA:
                if (HalionCheck())
                {
                    go_ids.push_back(DATA_BURNING_TREE_1);
                    go_ids.push_back(DATA_BURNING_TREE_2);
                    go_ids.push_back(DATA_BURNING_TREE_3);
                    go_ids.push_back(DATA_BURNING_TREE_4);
                }
                break;
            case TYPE_HALION:
                go_ids.push_back(DATA_HALION_FLAME_RING);
                break;
            default:
                break;
        }

        for (std::deque<uint32>::const_iterator i = go_ids.begin(); i != go_ids.end(); ++i)
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
};

InstanceData* GetInstanceData_instance_ruby_sanctum(Map* pMap)
{
    return new instance_ruby_sanctum(pMap);
}

void AddSC_instance_ruby_sanctum()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_ruby_sanctum";
    newscript->GetInstanceData = &GetInstanceData_instance_ruby_sanctum;
    newscript->RegisterSelf();
}
