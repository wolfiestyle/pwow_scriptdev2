#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL instance_trial_of_the_crusader: public ScriptedInstance
{
    instance_trial_of_the_crusader(Map *pMap): ScriptedInstance(pMap) { Initialize(); }

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string m_strInstData;
    bool IsHorde;

    typedef std::map<uint32, uint64> GuidMap;
    GuidMap m_guidsStore;

    void OnPlayerEnter(Player *pWho)
    {
        IsHorde = pWho->GetTeam() == HORDE;
    }

    void Initialize()
    {
        IsHorde = false;

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

    void OnCreatureCreate(Creature *pCreature)
    {
        if (uint32 data_id = toc_GetType(pCreature))
            m_guidsStore[data_id] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        uint32 data_id;
        switch (pGo->GetEntry())
        {
            case GO_COLISEUM_FLOOR:         data_id = TYPE_COLISEUM_FLOOR; break;
            case GO_ANUBARAK_CHEST:         data_id = TYPE_ANUBARAK_CHEST; break;
            case GO_FACTION_CHAMPION_CHEST: data_id = TYPE_FACTION_CHAMPION_CHEST; break;

            default:
                return;
        }

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
        m_auiEncounter[uiType] = uiData;

        std::ostringstream saveStream;

        for(size_t i = 0; i < MAX_ENCOUNTER; ++i)
            saveStream << m_auiEncounter[i] << " ";

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
                return IsHorde;
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

        OUT_LOAD_INST_DATA_COMPLETE;
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
