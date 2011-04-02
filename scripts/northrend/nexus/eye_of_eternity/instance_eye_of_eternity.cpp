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
    }

    //std::string strInstData;
    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint64 m_uiMalygosGUID;
    uint64 m_uiAlexstraszaGiftGUID;

    void Initialize()
    {
        m_uiMalygosGUID = 0;
        m_uiAlexstraszaGiftGUID = 0;

        memset(m_auiEncounter, 0, sizeof(m_auiEncounter));
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_MALYGOS:
                m_uiMalygosGUID = pCreature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch (pGo->GetEntry())
        {
            case GO_MALYGOS_LOOT_10:
            case GO_MALYGOS_LOOT_25:
                m_uiAlexstraszaGiftGUID = pGo->GetGUID();
                break;
            default:
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
            case TYPE_MALYGOS:
                m_auiEncounter[uiType] = uiData;
            default:
                break;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch (uiType)
        {
            case TYPE_MALYGOS:
                return m_auiEncounter[uiType];
            default:
                return 0;
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        switch (uiData)
        {
            case TYPE_MALYGOS:
                return m_uiMalygosGUID;
            case DATA_MALYGOS_LOOT:
                return m_uiAlexstraszaGiftGUID;
            default:
                return 0;
        }
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
