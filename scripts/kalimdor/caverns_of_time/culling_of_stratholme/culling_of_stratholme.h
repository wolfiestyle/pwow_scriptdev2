/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_CULLING_OF_STRATHOLME_H
#define DEF_CULLING_OF_STRATHOLME_H

#define ENTRY -1595000

enum
{
    MAX_ENCOUNTER                   = 9,

    TYPE_GRAIN_EVENT                = 0,                    // crates with plagued grain identified
    TYPE_ARTHAS_INTRO_EVENT         = 1,                    // Arthas Speech and Walk to Gates and short intro with MalGanis
    TYPE_MEATHOOK_EVENT             = 2,                    // Waves 1-5
    TYPE_SALRAMM_EVENT              = 3,                    // Waves 6-10
    TYPE_EPOCH_EVENT                = 4,                    // Townhall Event, Boss Killed
    TYPE_ARTHAS_ESCORT_EVENT        = 5,                    // Townhall to Malganis
    TYPE_MALGANIS_EVENT             = 6,                    // Malganis
    TYPE_INFINITE_CORRUPTER_TIME    = 7,                    // Time for 25min Timer
    TYPE_INFINITE_CORRUPTER         = 8,

    // Main Encounter NPCs
    NPC_CHROMIE_INN                 = 26527,
    NPC_CHROMIE_ENTRANCE            = 27915,
    NPC_CHROMIE_END                 = 30997,
    NPC_HOURGLASS                   = 28656,
    NPC_ARTHAS                      = 26499,
    NPC_MEATHOOK                    = 26529,
    NPC_SALRAMM_THE_FLESHCRAFTER    = 26530,
    NPC_CHRONO_LORD_EPOCH           = 26532,
    NPC_MALGANIS                    = 26533,
    NPC_INFINITE_CORRUPTER          = 32273,
    NPC_LORDAERON_CRIER             = 27913,
    NPC_ZOMBIE                      = 27737,

    // Inn Event related NPC
    NPC_MICHAEL_BELFAST             = 30571,
    NPC_HEARTHSINGER_FORRESTEN      = 30551,
    NPC_FRAS_SIABI                  = 30552,
    NPC_FOOTMAN_JAMES               = 30553,
    NPC_MAL_CORRICKS                = 31017,
    NPC_GRYAN_STOUTMANTLE           = 30561,

    // Grain Event NPCs
    NPC_ROGER_OWENS                 = 27903,
    NPC_SERGEANT_MORIGAN            = 27877,
    NPC_JENA_ANDERSON               = 27885,
    NPC_MALCOM_MOORE                = 27891,                // Not (yet?) spawned
    NPC_BARTLEBY_BATTSON            = 27907,
    NPC_CRATES_BUNNY                = 30996,

    // Grain Event Aditional NPCs
    NPC_SILVIO_PERELLI              = 27876,
    NPC_MARTHA_GOSLIN               = 27884,
    NPC_SCRUFFY                     = 27892,

    // Intro Event NPCs
    NPC_JAINA_PROUDMOORE            = 26497,
    NPC_UTHER_THE_LIGHTBRINGER      = 26528,
    NPC_KNIGHT_OF_THE_SILVER_HAND   = 28612,
    NPC_LORDAERON_FOOTMAN           = 27745,
    NPC_STRATHOLME_CITIZEN          = 28167,
    NPC_STRATHOLME_RESIDENT         = 28169,

    // Mobs in Stratholme (to despawn) -- only here for sake of completeness handling remains open (mangos feature)
    NPC_MAGISTRATE_BARTHILAS        = 30994,
    NPC_STEPHANIE_SINDREE           = 31019,
    NPC_LEEKA_TURNER                = 31027,
    NPC_SOPHIE_AAREN                = 31021,
    NPC_ROBERT_PIERCE               = 31025,
    NPC_GEORGE_GOODMAN              = 31022,

    // Others NPCs in Stratholme
    NPC_EMERY_NEILL                 = 30570,
    NPC_EDWARD_ORRICK               = 31018,
    NPC_OLIVIA_ZENITH               = 31020,
    NPC_SALRAMM_GHOUL               = 27733,
    NPC_DEVOURING_GHOUL             = 28249,
    NPC_ENRAGING_GHOUL              = 27729,
    NPC_MASTER_NECROMANCER          = 27732,
    NPC_CRYPT_FIEND                 = 27734,
    NPC_TOMB_STALKER                = 28199,
    NPC_PATCHWORK_CONSTRUCT         = 27736,
    NPC_ACOLYTE                     = 27731,

    // Townhall Event NPCs
    NPC_AGIATED_STRATHOLME_CITIZEN  = 31126,
    NPC_AGIATED_STRATHOLME_RESIDENT = 31127,
    NPC_PATRICIA_O_REILLY           = 31028,
    NPC_INFINITE_ADVERSARY          = 27742,
    NPC_INFINITE_AGENT              = 27744,
    NPC_INFINITE_HUNTER             = 27743,
    NPC_TIME_RIFT_SMALL             = 28409,
    NPC_TIME_RIFT_BIG               = 28439,

    // Gameobjects
    GO_DOOR_BOOKCASE                = 188686,
    GO_DARK_RUNED_CHEST             = 190663,
    GO_DARK_RUNED_CHEST_H           = 193597,
    GO_CITY_GATE                    = 191788,

    // World States
    WORLD_STATE_CRATES              = 3479,
    WORLD_STATE_CRATES_COUNT        = 3480,
    WORLD_STATE_WAVE                = 3504,
    WORLD_STATE_TIME                = 3932,
    WORLD_STATE_TIME_COUNTER        = 3931,

    // misc
    MALGANIS_KILL_CREDIT            = 58630,
    MALGANIS_KILL_CREDIT_BUNNY      = 31006,

    // positions
    POSITION_FESTIVALS_LANE_GATE    = 0,
    ELDERS_SQUARE_GATE              = 1,
    KINGS_SQUARE_FOUNTAIN           = 2,
    TOWN_HALL                       = 3,
    MARKET_ROW_GATE                 = 4,

    // Areatrigger
    AREATRIGGER_INN                 = 5291,
    /*
    5085 before bridge - could be Uther SpawnPos
    5148 ini entrance
    5181 ini exit
    5249 fras siabis store
    5250 leeking shields...(store)
    5251 bar in stratholme
    5252 Aaren flowers
    5253 Angelicas boutique
    5256 townhall
    5291 Inn */
};

enum eInstancePosition
{
    POS_ARTHAS_INTRO        = 1,
    POS_ARTHAS_WAVES        = 2,
    POS_ARTHAS_TOWNHALL     = 3,
    POS_ARTHAS_ESCORTING    = 4,
    POS_ARTHAS_MALGANIS     = 5,
    POS_INSTANCE_FINISHED   = 6
};

class MANGOS_DLL_DECL instance_culling_of_stratholme : public ScriptedInstance
{
    public:
        instance_culling_of_stratholme(Map* pMap);
        ~instance_culling_of_stratholme() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);
        void OnCreatureDeath(Creature* pCreature);
        void OnCreatureEnterCombat(Creature* pCreature);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);
        uint64 GetData64(uint32 uiData);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

        void Update(uint32 uiDiff);
        void SummonNextWave();

        void GetStratAgiatedCitizenList(std::list<uint64> &lList){ lList = m_lAgiatedCitizenGUIDList; };
        void GetStratAgiatedResidentList(std::list<uint64> &lList){ lList = m_lAgiatedResidentGUIDList; };

        void GetCratesBunnyOrderedList(std::list<Creature*> &lList);
        Creature* GetStratIntroFootman();
        void GetResidentOrderedList(std::list<Creature*> &lList);
        void DoSpawnArthasIfNeeded();
        void DoSpawnChromieIfNeeded();
        uint8 GetInstancePosition();
        void ArthasJustDied();

    protected:
        void OnPlayerEnter(Player* pPlayer);
        void UpdateQuestCredit();
        void DoChromieSpeech(int32 uiSpeech);

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

        uint8 m_uiGrainCrateCount;
        uint32 m_uiWaveCount;
        uint32 m_uiRemoveCrateStateTimer;
        uint32 m_uiArthasRespawnTimer;

        uint64 m_uiChromieInnGUID;
        uint64 m_uiChromieEntranceGUID;
        uint64 m_uiChromieEndGUID;
        uint64 m_uiHourglassGUID;
        uint64 m_uiArthasGUID;
        uint64 m_uiMeathookGUID;
        uint64 m_uiSalrammGUID;
        uint64 m_uiEpochGUID;
        uint64 m_uiMalganisGUID;
        uint64 m_uiCorrupterGUID;
        uint64 m_uiLordaeronCrierGUID;

        uint64 m_uiBelfastGUID;
        uint64 m_uiForrestenGUID;
        uint64 m_uiSiabiGUID;
        uint64 m_uiJamesGUID;
        uint64 m_uiCorricksGUID;
        uint64 m_uiStoutmantleGUID;

        uint64 m_uiOwensGUID;
        uint64 m_uiMoriganGUID;
        uint64 m_uiAndersonGUID;
        uint64 m_uiMooreGUID;
        uint64 m_uiBattsonGUID;

        uint64 m_uiPerelliGUID;
        uint64 m_uiGoslinGUID;
        uint64 m_uiScruffyGUID;

        uint64 m_uiOReillyGUID;

        std::list<uint64> m_luiCratesBunnyGUIDs;
        std::list<uint64> m_luiFootmanGUIDs;
        std::list<uint64> m_luiResidentGUIDs;
        std::list<uint64> m_luiWaveAdds;

        std::list<uint64> m_lAgiatedCitizenGUIDList;
        std::list<uint64> m_lAgiatedResidentGUIDList;

        uint64 m_uiDoorBookcaseGUID;
        uint64 m_uiDarkRunedChestGUID;
        uint64 m_uiCityGateGUID;
};

#endif
