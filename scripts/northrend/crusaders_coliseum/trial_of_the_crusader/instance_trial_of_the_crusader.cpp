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
SDName: Instance Trial of the Crusader
SD%Complete: 100
SDComment:
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
instance_trial_of_the_crusader
EndContentData */

#include <iterator>
#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL instance_trial_of_the_crusader: public ScriptedInstance
{
    std::vector<uint32> m_auiEncounter;
    std::string m_strInstData;
    uint32 m_uiPlayerTeam;
    uint32 m_uiAchievementProgressCounter;
    uint32 m_uiAttemptCounter;
    uint32 m_uiInTalkPhase;

    typedef UNORDERED_MAP<uint32 /*entry*/, uint64 /*guid*/> GuidMap;
    GuidMap m_guidsStore;

    instance_trial_of_the_crusader(Map *pMap):
        ScriptedInstance(pMap),
        m_auiEncounter(MAX_ENCOUNTER, 0)
    {
        Initialize();
    }

    void OnPlayerEnter(Player *pWho)
    {
        if (!m_uiPlayerTeam)
            m_uiPlayerTeam = pWho->GetTeam();
        if ((instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC) &&
            (m_uiAttemptCounter != 50 || IsEncounterInProgress()))
            return;
        pWho->SendUpdateWorldState(WORLD_STATE_TOTGC, 0);
    }

    void Initialize()
    {
        m_uiAttemptCounter              = 51; //1 is lost cause reset is called at npc spawn
        m_uiPlayerTeam                  = 0;
        m_uiAchievementProgressCounter  = 0;
        m_uiInTalkPhase                 = 0;
    }

    void InitWorldState(bool Enable = true)
    {
        DoUpdateWorldState(WORLD_STATE_TOTGC, Enable ? 1 : 0);
        DoUpdateWorldState(WORLD_STATE_TOTGC_ATTEMPT_COUNTER, m_uiAttemptCounter);
    }

    bool IsEncounterInProgress() const
    {
        return std::find(m_auiEncounter.begin(), m_auiEncounter.end(), IN_PROGRESS) != m_auiEncounter.end();
    }

    void OnCreatureCreate(Creature *pCreature)
    {
        if (uint32 data_id = toc::GetType<Creature>(pCreature))
            m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        if (uint32 data_id = toc::GetType<GameObject>(pGo))
            m_guidsStore[data_id] = pGo->GetGUID();
    }

    void OnEncounterComplete(uint32 uiType)
    {
        uint32 loot_id = 0;
        switch (uiType)
        {
            case TYPE_ANUBARAK:
                //loot_id = TYPE_ANUBARAK_CHEST;
                break;
            case TYPE_GORGRIM_SHADOWCLEAVE:
            case TYPE_BIRANA_STORMHOOF:
            case TYPE_ERIN_MISTHOOF:
            case TYPE_RUJKAH:
            case TYPE_GINSELLE_BLIGHTSLINGER:
            case TYPE_LIANDRA_SUNCALLER:
            case TYPE_MALITHAS_BRIGHTBLADE:
            case TYPE_CAIPHUS_THE_STERN:
            case TYPE_VIVIENNE_BLACKWHISPER:
            case TYPE_MAZDINAH:
            case TYPE_THRAKGAR:
            case TYPE_BROLN_STOUTHORN:
            case TYPE_HARKZOG:
            case TYPE_NARRHOK_STEELBREAKER:
                if(!IsEncounterInProgress())
                    loot_id = TYPE_FACTION_CHAMPION_CHEST;
                break;
        }

        if (loot_id)
            if (uint64 LootGuid = GetData64(loot_id))
                DoRespawnGameObject(LootGuid, 7*DAY);
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
            case DATA_ACHIEVEMENT_COUNTER:
                m_uiAchievementProgressCounter = uiData;
                return;
            case DATA_ATTEMPT_COUNTER:
                m_uiAttemptCounter = uiData;
                DoUpdateWorldState(WORLD_STATE_TOTGC_ATTEMPT_COUNTER, m_uiAttemptCounter);
                return;
            case DATA_IN_TALK_PHASE:
                m_uiInTalkPhase = uiData;
                return;
        }

        if (uiType < MAX_ENCOUNTER)
            m_auiEncounter[uiType] = uiData;

        if (IsEncounterInProgress())
            if (instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || instance->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                InitWorldState();

        std::ostringstream saveStream;
        std::copy(m_auiEncounter.begin(), m_auiEncounter.end(), std::ostream_iterator<uint32>(saveStream, " "));
        saveStream << m_uiAttemptCounter << " ";

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
            case TYPE_GORMOK:
            case TYPE_ACIDMAW:
            case TYPE_DREADSCALE:
            case TYPE_ICEHOWL:
            case TYPE_JARAXXUS:
            case TYPE_GORGRIM_SHADOWCLEAVE:
            case TYPE_BIRANA_STORMHOOF:
            case TYPE_ERIN_MISTHOOF:
            case TYPE_RUJKAH:
            case TYPE_GINSELLE_BLIGHTSLINGER:
            case TYPE_LIANDRA_SUNCALLER:
            case TYPE_MALITHAS_BRIGHTBLADE:
            case TYPE_CAIPHUS_THE_STERN:
            case TYPE_VIVIENNE_BLACKWHISPER:
            case TYPE_MAZDINAH:
            case TYPE_BROLN_STOUTHORN:
            case TYPE_THRAKGAR:
            case TYPE_HARKZOG:
            case TYPE_NARRHOK_STEELBREAKER:
            case TYPE_FJOLA_LIGHTBANE:
            case TYPE_EYDIS_DARKBANE:
            case TYPE_ANUBARAK:
                return m_auiEncounter[uiType];
            case DATA_FACTION:
                return m_uiPlayerTeam;
            case DATA_ACHIEVEMENT_COUNTER:
                return m_uiAchievementProgressCounter;
            case DATA_ATTEMPT_COUNTER:
                return m_uiAttemptCounter;
            case DATA_IN_TALK_PHASE:
                return m_uiInTalkPhase;
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

        for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
            loadStream >> m_auiEncounter[i];
        loadStream >> m_uiAttemptCounter;

        std::replace(m_auiEncounter.begin(), m_auiEncounter.end(), IN_PROGRESS, NOT_STARTED);

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    bool CheckAchievementCriteriaMeet(uint32 CriteriaId, Player const* pPlayer, Unit const* pWho, uint32 misc1)
    {
        switch(CriteriaId)
        {
            case THREE_SIXTY_CRITERIA_N10:
            case THREE_SIXTY_CRITERIA_N25:
            case THREE_SIXTY_CRITERIA_H10:
            case THREE_SIXTY_CRITERIA_H25:
            case UPPER_BACK_PAIN_CRITERIA_N10:
            case UPPER_BACK_PAIN_CRITERIA_H10:
                return m_uiAchievementProgressCounter >= 2;
            case UPPER_BACK_PAIN_CRITERIA_N25:
            case UPPER_BACK_PAIN_CRITERIA_H25:
                return m_uiAchievementProgressCounter >= 4;
            default:
                return false;
        }
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* pMap)
{
    return new instance_trial_of_the_crusader(pMap);
}

void AddSC_instance_trial_of_the_crusader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();
}
