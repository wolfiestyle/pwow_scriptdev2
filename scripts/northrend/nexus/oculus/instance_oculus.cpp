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

struct MANGOS_DLL_DECL instance_oculus : public ScriptedInstance
{
    instance_oculus(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
        m_bIsRegularMode = pMap->IsRegularDifficulty();
    }

    std::string strInstData;
    bool m_bIsRegularMode;
    uint32 m_uiEncounter[ENCOUNTERS];

    uint64 m_uiDrakosGUID;
    uint64 m_uiVarosGUID;
    uint64 m_uiUromGUID;
    uint64 m_uiEregosGUID;
    uint64 m_uiCacheOfEregosGUID;

    void Initialize()
    {
        m_uiDrakosGUID = 0;
        m_uiVarosGUID = 0;
        m_uiUromGUID = 0;
        m_uiEregosGUID = 0;
        m_uiCacheOfEregosGUID = 0;

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            m_uiEncounter[i] = NOT_STARTED;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 27654: m_uiDrakosGUID      = pCreature->GetGUID(); break;
            case 27447: m_uiVarosGUID       = pCreature->GetGUID(); break;
            case 27655: m_uiUromGUID        = pCreature->GetGUID(); break;
            case 27656: m_uiEregosGUID      = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case 191349: if (m_bIsRegularMode) m_uiCacheOfEregosGUID = pGo->GetGUID(); break;
            case 193603: if (!m_bIsRegularMode) m_uiCacheOfEregosGUID = pGo->GetGUID(); break;
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
            case DATA_DRAKOS:                    return m_uiDrakosGUID;
            case DATA_VAROS:                     return m_uiVarosGUID;
            case DATA_UROM:                      return m_uiUromGUID;
            case DATA_EREGOS:                    return m_uiEregosGUID;
			case DATA_CACHE_OF_EREGOS:           return m_uiCacheOfEregosGUID;
        }

        return 0;
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
