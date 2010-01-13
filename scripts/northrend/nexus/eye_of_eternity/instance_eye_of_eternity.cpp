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
SDName: Instance_Eye_of_Eternity
SD%Complete: 0
SDComment:
SDCategory: Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "def_eye_of_eternity.h"

struct MANGOS_DLL_DECL instance_eye_of_eternity : public ScriptedInstance
{
    instance_eye_of_eternity(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
        m_bIsRegularMode = pMap->IsRegularDifficulty();
    }

    std::string strInstData;
    bool m_bIsRegularMode;
    uint32 m_uiEncounter[ENCOUNTERS];

    uint64 m_uiMalygosGUID;
    uint64 m_uiAlexstraszaGiftGUID;

    void Initialize()
    {
        m_uiMalygosGUID = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            m_uiEncounter[i] = NOT_STARTED;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 28859: m_uiMalygosGUID    = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case 193905: if (m_bIsRegularMode) m_uiAlexstraszaGiftGUID = pGo->GetGUID(); break;
            case 193967: if (!m_bIsRegularMode) m_uiAlexstraszaGiftGUID = pGo->GetGUID(); break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
    }

    uint32 GetData(uint32 uiType)
    {
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case DATA_MALYGOS:                  return m_uiMalygosGUID;
            case DATA_ALEXSTRASZAGIFT:          return m_uiAlexstraszaGiftGUID;
        }

        return 0;
    }
};

InstanceData* GetInstanceData_instance_eye_of_eternity(Map* pMap)
{
    return new instance_eye_of_eternity(pMap);
}

void AddSC_instance_eye_of_eternity()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_eye_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_eye_of_eternity;
    pNewScript->RegisterSelf();
}
