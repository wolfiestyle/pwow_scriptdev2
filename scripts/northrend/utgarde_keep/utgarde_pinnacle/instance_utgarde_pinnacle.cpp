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
SDName: instance_pinnacle
SD%Complete: 25%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

struct MANGOS_DLL_DECL instance_pinnacle : public ScriptedInstance
{
    instance_pinnacle(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiGraufGuid;
    uint64 m_uiSkadiGuid;
    uint64 m_uiSkadiDoorGUID;
    uint32 m_uiSvalaAchievement;
    uint32 m_uiSkadiAchievement;
    uint32 m_uiYmironAchievement;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
        m_uiGraufGuid = 0;
        m_uiSkadiGuid = 0;
        m_uiSkadiDoorGUID = 0;
        m_uiSvalaAchievement = 0;
        m_uiSkadiAchievement = 0;
        m_uiYmironAchievement = 0;
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_DOOR_SKADI:
                m_uiSkadiDoorGUID = pGo->GetGUID();

                if (m_auiEncounter[2] == DONE)
                    pGo->SetGoState(GO_STATE_ACTIVE);

                break;
        }
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_GRAUF:
                m_uiGraufGuid = pCreature->GetGUID();
                break;
            case NPC_SKADI:
                m_uiSkadiGuid = pCreature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnCreatureEnterCombat(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_SKADI)
        {
            if (instance->IsRegularDifficulty())
                return;

            Map::PlayerList const &players = instance->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                Player* pPlayer = itr->getSource();
                pPlayer->StartTimedAchievementCriteria(ACHIEVEMENT_TIMED_TYPE_EVENT, TIMED_ACHIEVEMENT_EVENT_SKADI);
            }
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Pinnacle: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_SVALA:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_GORTOK:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_SKADI:
                if (uiData == DONE)
                    DoUseDoorOrButton(m_uiSkadiDoorGUID);

                m_auiEncounter[2] = uiData;
                break;
            case TYPE_YMIRON:
                m_auiEncounter[3] = uiData;
                break;
            case DATA_ACHIEVEMENT_SVALA:
                m_uiSvalaAchievement = uiData;
                break;
            case DATA_ACHIEVEMENT_SKADI_MY_GIRL:
                m_uiSkadiAchievement = uiData;
                break;
            case DATA_ACHIEVEMENT_KINGS_BANE:
                m_uiYmironAchievement = uiData;
                break;
            default:
                error_log("SD2: Instance Pinnacle: SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        //saving also SPECIAL for this instance
        if (uiData == DONE || uiData == SPECIAL)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_SVALA:
                return m_auiEncounter[0];
            case TYPE_GORTOK:
                return m_auiEncounter[1];
            case TYPE_SKADI:
                return m_auiEncounter[2];
            case TYPE_YMIRON:
                return m_auiEncounter[3];
            case DATA_ACHIEVEMENT_KINGS_BANE:
                return m_uiYmironAchievement;
                break;
        }

        return 0;
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case DATA_GRAUF:
                return m_uiGraufGuid;
            case DATA_SKADI:
                return m_uiSkadiGuid;
            default:
                return 0;
        }
    }

    bool CheckAchievementCriteriaMeet(uint32 criteria_id, const Player* src, const Unit* target, uint32 miscval1)
    {
        switch (criteria_id)
        {
            case CRITERIA_THE_INCREDIBLE_HULK:
                return m_uiSvalaAchievement == 1;
            case CRITERIA_MY_GIRL:
                return m_uiSkadiAchievement == 1;
            case CRITERIA_KINGS_BANE:
                return m_uiYmironAchievement == 1;
            default:
                return false;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_pinnacle(Map* pMap)
{
    return new instance_pinnacle(pMap);
}

void AddSC_instance_pinnacle()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_pinnacle";
    newscript->GetInstanceData = &GetInstanceData_instance_pinnacle;
    newscript->RegisterSelf();
}
