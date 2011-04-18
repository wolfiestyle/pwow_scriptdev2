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
SDName: Instance_Oculus
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"
#include "Vehicle.h"

#define INSTANCE_DATA_MAGIC "OC"
struct MANGOS_DLL_DECL instance_oculus : public ScriptedInstance
{
    instance_oculus(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
        m_bIsRegularMode = pMap->IsRegularDifficulty();
    }

    std::string strInstData;
    bool m_bIsRegularMode;
    uint32 m_uiEncounter[DATA_MAX_ENCOUNTER];

    uint32 m_uiRubyVoid;
    uint32 m_uiEmeraldVoid;
    uint32 m_uiAmberVoid;

    uint64 m_uiDrakosGUID;
    uint64 m_uiVarosGUID;
    uint64 m_uiUromGUID;
    uint64 m_uiEregosGUID;
    uint64 m_uiCacheOfEregosGUID;
    uint64 m_uiSpotlightGUID;
    uint64 m_uiVerdisaGUID;
    uint64 m_uiEternosGUID;
    uint64 m_uiBalgaristraszGUID;

    void Initialize()
    {
        m_uiRubyVoid     = 0;
        m_uiEmeraldVoid  = 0;
        m_uiAmberVoid    = 0;
        m_uiDrakosGUID  = 0;
        m_uiVarosGUID   = 0;
        m_uiUromGUID    = 0;
        m_uiEregosGUID  = 0;
        m_uiCacheOfEregosGUID = 0;
        m_uiSpotlightGUID = 0;
        m_uiVerdisaGUID = 0;
        m_uiEternosGUID = 0;
        m_uiBalgaristraszGUID = 0;

        for(uint8 i = 0; i < DATA_MAX_ENCOUNTER; i++)
            m_uiEncounter[i] = NOT_STARTED;

        // There are 10 static spawned robots
        m_uiEncounter[TYPE_ROBOTS] = 10;
    }

    void OnPlayerEnter(Player* pPlayer)
    {
        if (m_uiEncounter[TYPE_DRAKOS] == DONE && m_uiEncounter[TYPE_VAROS] == NOT_STARTED && m_uiEncounter[TYPE_ROBOTS] != 0)
        {
            pPlayer->SendUpdateWorldState(WORLD_STATE_ROBOTS_COUNT, m_uiEncounter[TYPE_ROBOTS]);
            pPlayer->SendUpdateWorldState(WORLD_STATE_ROBOTS, 1);

        }
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 27654: m_uiDrakosGUID          = pCreature->GetGUID(); break;
            case 27447: m_uiVarosGUID           = pCreature->GetGUID(); break;
            case 27655: m_uiUromGUID            = pCreature->GetGUID(); break;
            case 27656: m_uiEregosGUID          = pCreature->GetGUID(); break;
            case NPC_VERDISA: m_uiVerdisaGUID   = pCreature->GetGUID(); break;
            case NPC_ETERNOS: m_uiEternosGUID   = pCreature->GetGUID(); break;
            case NPC_BALGARISTRASZ:
            case NPC_BALGAR_IMAGE:
                m_uiBalgaristraszGUID   = pCreature->GetGUID(); break;
            case NPC_RUBY_DRAKE:
            case NPC_EMERALD_DRAKE:
            case NPC_AMBER_DRAKE:
                if (m_uiEncounter[TYPE_UROM] == DONE)
                    pCreature->UpdateEntry(pCreature->GetEntry()*10);
                break;

        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_DRAKOS)
        {
            if(instance && instance->IsDungeon() && !instance->IsRegularDifficulty())
            {
                Map::PlayerList const &PlayerList = instance->GetPlayers();

                if(PlayerList.isEmpty())
                    return;

                for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if(Player* pPlayer = i->getSource())
                    {
                        pPlayer->StartTimedAchievementCriteria(ACHIEVEMENT_TIMED_TYPE_EVENT, MAKE_IT_COUNT_START);
                    }
                }
            }
            DoUpdateWorldState(WORLD_STATE_ROBOTS_COUNT, m_uiEncounter[TYPE_ROBOTS]);
            DoUpdateWorldState(WORLD_STATE_ROBOTS, 1);
        }

        if (pCreature->GetEntry() == NPC_ROBOT)
            if (m_uiEncounter[TYPE_ROBOTS] > 0)
            {
                m_uiEncounter[TYPE_ROBOTS] -= 1;
                DoUpdateWorldState(WORLD_STATE_ROBOTS_COUNT, m_uiEncounter[TYPE_ROBOTS]);
                if (m_uiEncounter[TYPE_ROBOTS] == 0)
                    DoUpdateWorldState(WORLD_STATE_ROBOTS, 0);
            }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_EREGOS_CACHE:   if (m_bIsRegularMode) m_uiCacheOfEregosGUID = pGo->GetGUID(); break;
            case GO_EREGOS_CACHE_H: if (!m_bIsRegularMode) m_uiCacheOfEregosGUID = pGo->GetGUID(); break;
            case GO_SPOTLIGHT:      m_uiSpotlightGUID = pGo->GetGUID(); break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        if (uiType < DATA_MAX_ENCOUNTER)
            m_uiEncounter[uiType] = uiData;

        switch (uiType)
        {
            case DATA_RUBY_VOID:
                m_uiRubyVoid = uiData;
                break;
            case DATA_EMERALD_VOID:
                m_uiEmeraldVoid = uiData;
                break;
            case DATA_AMBER_VOID:
                m_uiAmberVoid = uiData;
                break;
            default:
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << INSTANCE_DATA_MAGIC << " ";

            for(uint8 i = 0; i < DATA_MAX_ENCOUNTER; ++i)
                saveStream << m_uiEncounter[i] << " ";

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    bool CheckAchievementCriteriaMeet(uint32 criteria_id, const Player* pSource, const Unit* target, uint32 miscval1)
    {
        switch (criteria_id)
        {
            // individual per-player check
            case CRITERIA_DRAKE_RIDER_RUBY:
                if (VehicleKit* pVeh = pSource->GetVehicle())
                    if (Unit* pDrake = pVeh->GetBase())
                        if (pDrake->GetEntry() == (NPC_RUBY_DRAKE*10))
                            return true;
                break;
            case CRITERIA_DRAKE_RIDER_EMERALD:
                if (VehicleKit* pVeh = pSource->GetVehicle())
                    if (Unit* pDrake = pVeh->GetBase())
                        if (pDrake->GetEntry() == (NPC_EMERALD_DRAKE*10))
                            return true;
                break;
            case CRITERIA_DRAKE_RIDER_AMBER:
                if (VehicleKit* pVeh = pSource->GetVehicle())
                    if (Unit* pDrake = pVeh->GetBase())
                        if (pDrake->GetEntry() == (NPC_AMBER_DRAKE*10))
                            return true;
                break;
            // raidwide check
            case CRITERIA_RUBY_VOID:
                return m_uiRubyVoid == 0;
            case CRITERIA_EMERALD_VOID:
                return m_uiEmeraldVoid == 0;
            case CRITERIA_AMBER_VOID:
                return m_uiAmberVoid == 0;
            default:
                return false;
        }
        return false;
    }

    uint32 GetData(uint32 uiType)
    {
        if (uiType < DATA_MAX_ENCOUNTER)
            return m_uiEncounter[uiType];
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_DRAKOS:               return m_uiDrakosGUID;
            case DATA_VAROS:                return m_uiVarosGUID;
            case DATA_UROM:                 return m_uiUromGUID;
            case DATA_EREGOS:               return m_uiEregosGUID;
            case DATA_CACHE_OF_EREGOS:      return m_uiCacheOfEregosGUID;
            case DATA_SPOTLIGHT:            return m_uiSpotlightGUID;
            case DATA_ETERNOS:              return m_uiEternosGUID;
            case DATA_VERDISA:              return m_uiVerdisaGUID;
            case DATA_BELGARISTRASZ:        return m_uiBalgaristraszGUID;
        }

        return 0;
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

        std::string magic;

        loadStream >> magic;

        if (magic != INSTANCE_DATA_MAGIC)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        for(uint8 i = 0; i < DATA_MAX_ENCOUNTER; ++i)
        {
            loadStream >> m_uiEncounter[i];

            if (m_uiEncounter[i] == IN_PROGRESS)
                m_uiEncounter[i] = NOT_STARTED;
        }

        m_uiEncounter[TYPE_ROBOTS] = 10;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_oculus(Map* pMap)
{
    return new instance_oculus(pMap);
}

void AddSC_instance_oculus()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_oculus";
    pNewScript->GetInstanceData = &GetInstanceData_instance_oculus;
    pNewScript->RegisterSelf();
}
