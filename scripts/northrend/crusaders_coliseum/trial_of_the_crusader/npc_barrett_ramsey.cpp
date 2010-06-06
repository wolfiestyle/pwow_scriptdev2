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
SDName: NPC Barret Ramsey
SD%Complete: 90
SDComment: event not fully implemented, just the necessary parts to make it doable
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
npc_barrett_ramsey
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    SPELL_SUBMERGE          = 56422,
    SPELL_OPEN_PORTAL       = 67864,
    SPELL_WILFRED_PORTAL    = 68424,
    SPELL_FEL_LIGHTNING     = 67888,
};

enum Phases
{
    PHASE_NOT_STARTED,
    PHASE_BEASTS_OF_NORTHEREND,
    PHASE_JARAXXUS,
    PHASE_FACTION_CHAMPIONS,
    PHASE_TWIN_VALKYR,
    PHASE_ANUBARAK,
    PHASE_DONE
};

enum BeastsOfNortherendPhases
{
    PHASE_BEASTS_NONE,
    PHASE_GORMOK,
    PHASE_JORMUNGAR_TWINS,
    PHASE_ICEHOWL,
    PHASE_BEASTS_DONE,
};

enum Says
{
    SAY_TIRION_TRIAL_OF_THE_CRUSADER_INTRO_1        = -1300364,
    SAY_TIRION_TRIAL_OF_THE_CRUSADER_INTRO_2        = -1300363,
    // Northrend Beasts
    SAY_TIRION_GORMOK_SPAWN                         = -1300299,
    SAY_TIRION_TWIN_JORMUNGAR_SPAWN                 = -1300300,
    SAY_TIRION_ICEHOWL_SPAWN                        = -1300301,
    SAY_TIRION_BEASTS_OF_NORTHREND_WIPE             = -1300302,
    SAY_TIRION_BEASTS_OF_NOTHREND_DEFEATED          = -1300303,
    // Jaraxxus
    SAY_TIRION_JARAXXUS_INTRO1                      = -1300304,
    SAY_WILFRED_JARAXXUS_INTRO2                     = -1300305,
    SAY_WILFRED_JARAXXUS_INTRO3                     = -1300306,
    SAY_WILFRED_JARAXXUS_INTRO4                     = -1300307,
    SAY_JARAXXUS_INTRO5                             = -1300308,
    SAY_WILFRED_JARAXXUS_INTRO6                     = -1300309,
    SAY_TIRION_JARAXXUS_INTRO7                      = -1300310,
    SAY_TIRION_JARAXXUS_OUTRO1                      = -1300318,
    SAY_GARROSH_JARAXXUS_HORDE_OUTRO2               = -1300319,
    SAY_VARIAN_JARAXXUS_HORDE_OUTRO3                = -1300320,
    SAY_TIRION_JARAXXUS_HORDE_OUTRO4                = -1300321,
    // Faction champions
    SAY_TIRION_FACTION_CHAMPIONS_INTRO1             = -1300322,
    SAY_GARROSH_FACTION_CHAMPIONS_ALLIANCE_INTRO2   = -1300323,
    SAY_TIRION_FACTION_CHAMPIONS_INTRO3             = -1300324,
    SAY_GARROSH_FACTION_CHAMPIONS_ALLIANCE_INTRO4   = -1300325,
    SAY_VARIAN_FACTION_CHAMPIONS_HORDE_INTRO2       = -1300326,
    SAY_VARIAN_FACTION_CHAMPIONS_HORDE_INTRO4       = -1300327,
    SAY_VARIAN_ALLIANCE_VICTORY                     = -1300336,
    SAY_GARROSH_HORDE_VICTORY                       = -1300337,
    SAY_TIRION_OUTRO                                = -1300338,
    // Valkyr twins
    SAY_TIRION_TWIN_VALKYR_INTRO                    = -1300339,
    SAY_GARROSH_TWIN_VALKYR_OUTRO                   = -1300348,
    // Anub'arak
    SAY_TIRION_ANUBARAK_INTRO1                      = -1300349,
    SAY_LICHKING_ANUBARAK_INTRO2                    = -1300350,
    SAY_TIRION_ANUBARAK_INTRO3                      = -1300351,
    SAY_LICHKING_ANUBARAK_INTRO4                    = -1300352,
    SAY_LICHKING_ANUBARAK_INTRO5                    = -1300353,
    SAY_TIRION_ANUBARACK_OUTRO                      = -1300362,
};

#define GOSSIP_START_NEXT_BOSS  "We are ready to begin the next challenge."  //not the actual text, but I cant find a source for it. If you know them, change it
#define GOSSIP_TEXT_ID          45323

#define DESPAWN_TIME            4*MINUTE*IN_MILLISECONDS
#define SUMMON_TIMER            3*MINUTE*IN_MILLISECONDS
#define AGGRO_TIMER             urand(10, 15)*IN_MILLISECONDS

static const float summon_pos[4] = {563.8f, 182.0f, 395.0f, 3*M_PI/2};
static const float RoomCenter[3] = {563.67f, 139.57f, 393.83f };

struct MANGOS_DLL_DECL npc_barrett_ramseyAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    uint32 CurrPhase;
    uint32 CurrBeastOfNortherendPhase;
    bool EncounterInProgress :1;
    bool m_bIsHeroic :1;
    bool m_bIs10Man :1;
    bool m_bCombatStart :1;
    bool m_bIsInOutroTalk :1;
    uint32 uiSummonTimer;
    uint32 m_uiAggroTimer;
    uint32 m_uiBeastsDead;
    uint32 m_uiTalkTimer;
    uint32 m_uiTalkCounter;
    InstanceVar<uint32> m_AttemptCounter;
    InstanceVar<uint32> m_AchievementCounter;
    InstanceVar<uint32> m_bIsInTalkPhase;

    typedef std::list<uint64> GuidList;
    GuidList summons;

    npc_barrett_ramseyAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        CurrPhase(PHASE_NOT_STARTED),
        CurrBeastOfNortherendPhase(PHASE_BEASTS_NONE),
        EncounterInProgress(false),
        m_bCombatStart(false),
        m_bIsInOutroTalk(false),
        uiSummonTimer(0),
        m_uiAggroTimer(0),
        m_uiBeastsDead(0),
        m_uiTalkTimer(0),
        m_uiTalkCounter(0),
        m_AttemptCounter(DATA_ATTEMPT_COUNTER, m_pInstance),
        m_AchievementCounter(DATA_ACHIEVEMENT_COUNTER, m_pInstance),
        m_bIsInTalkPhase(DATA_IN_TALK_PHASE, m_pInstance)
    {
        if (m_pInstance)    //choose correct phase, in case of d/c or something happens and barrett is respawned
        {
            if (m_pInstance->GetData(TYPE_ANUBARAK) == DONE)
                CurrPhase = PHASE_ANUBARAK;
            else if (m_pInstance->GetData(TYPE_EYDIS_DARKBANE) == DONE)
                CurrPhase = PHASE_TWIN_VALKYR;
            else if (m_pInstance->GetData(TYPE_GORGRIM_SHADOWCLEAVE) == DONE)
                CurrPhase = PHASE_FACTION_CHAMPIONS;
            else if (m_pInstance->GetData(TYPE_JARAXXUS) == DONE)
                CurrPhase = PHASE_JARAXXUS;
            else if (m_pInstance->GetData(TYPE_ICEHOWL) == DONE)
                CurrPhase = PHASE_BEASTS_OF_NORTHEREND;
        }
        if (CurrPhase == PHASE_TWIN_VALKYR)
            DestroyFloor();
        Difficulty diff = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
        m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
        Reset();
    }

    bool IsEncounterInProgress() const
    {
        return EncounterInProgress;
    }

    void RemoveAllSummons()
    {
        for (GuidList::const_iterator i = summons.begin(); i != summons.end(); ++i)
            if (Creature *summ = m_creature->GetMap()->GetCreature(*i))
                summ->ForcedDespawn();
        summons.clear();
    }

    void DestroyFloor()
    {
        if (GameObject *pFloor = GET_GAMEOBJECT(TYPE_COLISEUM_FLOOR))
            pFloor->Delete();   //hacky fix, type 33 (destructable building) not implemented
    }

    void Reset()    //called when any creature wipes group
    {
        if (m_pInstance && m_bIsHeroic) //because of the order how data is saved into the instance, we first update the wipe counter
            --m_AttemptCounter;

        if (m_pInstance)
        {
            switch (CurrPhase)
            {
                case PHASE_BEASTS_OF_NORTHEREND:
                    m_pInstance->SetData(TYPE_GORMOK, NOT_STARTED);
                    m_pInstance->SetData(TYPE_ACIDMAW, NOT_STARTED);
                    m_pInstance->SetData(TYPE_DREADSCALE, NOT_STARTED);
                    m_pInstance->SetData(TYPE_ICEHOWL, NOT_STARTED);
                    m_uiBeastsDead = 0;
                    if (Creature *fordring = GET_CREATURE(TYPE_TIRION_FORDRING))
                        DoScriptText(SAY_TIRION_BEASTS_OF_NORTHREND_WIPE, fordring);
                    break;
                case PHASE_JARAXXUS:
                    m_pInstance->SetData(TYPE_JARAXXUS, NOT_STARTED);
                    break;
                case PHASE_FACTION_CHAMPIONS:
                    for (uint32 i = FACTION_CHAMPION_START; i <= FACTION_CHAMPION_END; i++)
                        m_pInstance->SetData(i, NOT_STARTED);
                    break;
                case PHASE_TWIN_VALKYR:
                    m_pInstance->SetData(TYPE_FJOLA_LIGHTBANE, NOT_STARTED);
                    m_pInstance->SetData(TYPE_EYDIS_DARKBANE, NOT_STARTED);
                default:
                    break;
            }
        }
        if (CurrPhase && CurrPhase != PHASE_ANUBARAK)
            CurrPhase--;
        if (CurrBeastOfNortherendPhase != PHASE_BEASTS_DONE)
            CurrBeastOfNortherendPhase = PHASE_BEASTS_NONE;
        EncounterInProgress = false;

        RemoveAllSummons();

        if (GameObject *Door = GET_GAMEOBJECT(TYPE_ENTRANCE_DOOR))
            Door->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

        if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
            Gate->SetGoState(GO_STATE_READY);

        if(CurrPhase != PHASE_ANUBARAK)
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (!pSummon)
            return;
        pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pSummon->SetSpeedRate(MOVE_WALK, 2.0f);
        summons.push_back(pSummon->GetGUID());
        if (!m_bCombatStart)
        {
            m_bCombatStart = true;
            m_uiAggroTimer = AGGRO_TIMER;
        }
        if (CurrPhase == PHASE_BEASTS_OF_NORTHEREND && CurrBeastOfNortherendPhase != PHASE_BEASTS_NONE)
            m_bIsInTalkPhase = true;    // do intro for individual beasts
        if (pSummon->GetEntry() == NPC_ACIDMAW)
            pSummon->CastSpell(pSummon, SPELL_SUBMERGE, false);
        if (pSummon->GetEntry() == NPC_JARAXXUS)
            m_uiAggroTimer = 35*IN_MILLISECONDS;
        if (pSummon->GetEntry() != NPC_JARAXXUS)
            pSummon->GetMotionMaster()->MovePoint(0, pSummon->GetPositionX(), summon_pos[1], summon_pos[2]);
    }

    void SpawnBoss(uint32 entry, int32 slot = 0)
    {
        if (entry == NPC_JARAXXUS)
        {
            m_creature->SummonCreature(entry, RoomCenter[0], RoomCenter[1], RoomCenter[2], summon_pos[3], TEMPSUMMON_DEAD_DESPAWN, DESPAWN_TIME);
            return;
        }
        int32 x = slot & 1 ? (slot + 1) / 2 : -(slot + 1) / 2;
        m_creature->SummonCreature(entry, summon_pos[0] + float(x)*2.5f, summon_pos[1]+45.0f, summon_pos[2]+1.0f, summon_pos[3], TEMPSUMMON_DEAD_DESPAWN, DESPAWN_TIME);
    }

    void NorthrendBeastsEncounterCheck()
    {
        if (++m_uiBeastsDead >= 4)
        {
            EncounterInProgress = false;
            if (Creature *fordring = GET_CREATURE(TYPE_TIRION_FORDRING))
                DoScriptText(SAY_TIRION_BEASTS_OF_NOTHREND_DEFEATED, fordring);
            CurrBeastOfNortherendPhase = PHASE_BEASTS_DONE;
        }
    }

    void EncounterCheck()
    {
        if (!m_pInstance->IsEncounterInProgress())
        {
            EncounterInProgress = false;
            m_bIsInOutroTalk = true;
            m_uiTalkCounter = 0;
            m_uiTalkTimer = 4*IN_MILLISECONDS;
        }
    }

    void SummonedCreatureJustDied(Creature *who)
    {
        if (!who || !m_pInstance)
            return;

        if (boss_trial_of_the_crusaderAI *bossAI = dynamic_cast<boss_trial_of_the_crusaderAI*>(who->AI()))
            bossAI->m_BossEncounter = DONE;

        switch (who->GetEntry())
        {
            case NPC_ANUBARAK:  //called directly from JustDied in anub'arak's AI
                EncounterInProgress = false;
                CurrPhase = PHASE_DONE;
                break;
            case NPC_JARAXXUS:
                EncounterInProgress = false;
                m_uiTalkCounter = 0;
                m_bIsInOutroTalk = true;
                break;
            case NPC_GORMOK:
                if (!m_bIsHeroic)
                {
                    CurrBeastOfNortherendPhase = PHASE_JORMUNGAR_TWINS;
                    SpawnBoss(NPC_ACIDMAW, 1);
                    SpawnBoss(NPC_DREADSCALE, 2);
                }
                else
                    if (uiSummonTimer < (SUMMON_TIMER - 15*IN_MILLISECONDS))
                        uiSummonTimer = 0;
                NorthrendBeastsEncounterCheck();
                break;
            case NPC_ACIDMAW:
            case NPC_DREADSCALE:
                if (!m_bIsHeroic && !m_pInstance->IsEncounterInProgress())
                {
                    CurrBeastOfNortherendPhase = PHASE_ICEHOWL;
                    SpawnBoss(NPC_ICEHOWL);
                }
                else
                    if (m_bIsHeroic && !m_pInstance->IsEncounterInProgress())
                        uiSummonTimer = 0;
                // (no break)
            case NPC_ICEHOWL:
                NorthrendBeastsEncounterCheck();
                break;
            // Twin Val'kyr
            case NPC_FJOLA_LIGHTBANE:
            case NPC_EYDIS_DARKBANE:
            // faction champions
            case NPC_TYRIUS_DUSKBLADE:
            case NPC_KAVINA_GROVESONG:
            case NPC_MELADOR_VALESTRIDER:
            case NPC_ALYSSIA_MOONSTALKER:
            case NPC_NOOZLE_WHIZZLESTICK:
            case NPC_VELNAA:
            case NPC_BAELNOR_LIGHTBEARER:
            case NPC_ANTHAR_FORGEMENDER:
            case NPC_BRIENNA_NIGHTFELL:
            case NPC_IRIETH_SHADOWSTEP:
            case NPC_SHAABAD:
            case NPC_SAAMUL:
            case NPC_SERISSA_GRIMDABBLER:
            case NPC_SHOCUUL:
            case NPC_GORGRIM_SHADOWCLEAVE:
            case NPC_BIRANA_STORMHOOF:
            case NPC_ERIN_MISTHOOF:
            case NPC_RUJKAH:
            case NPC_GINSELLE_BLIGHTSLINGER:
            case NPC_LIANDRA_SUNCALLER:
            case NPC_MALITHAS_BRIGHTBLADE:
            case NPC_CAIPHUS_THE_STERN:
            case NPC_VIVIENNE_BLACKWHISPER:
            case NPC_MAZDINAH:
            case NPC_BROLN_STOUTHORN:
            case NPC_THRAKGAR:
            case NPC_HARKZOG:
                EncounterCheck();
                break;
            default:
                break;
        }

        if (!EncounterInProgress)
        {
            if (GameObject *Door = GET_GAMEOBJECT(TYPE_ENTRANCE_DOOR))
                Door->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
                Gate->SetGoState(GO_STATE_READY);

            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void StartNextPhase()
    {
        if (GameObject *Door = GET_GAMEOBJECT(TYPE_ENTRANCE_DOOR))
            Door->SetGoState(GO_STATE_READY);

        if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
            Gate->SetGoState(GO_STATE_ACTIVE);
        CurrPhase++;
        EncounterInProgress = true;
        m_AchievementCounter = 0;   // we reset the achievement progress on every new phase
        m_uiTalkCounter = 0;
        m_uiTalkTimer = 0;
        m_bIsInTalkPhase = true;
        m_bIsInOutroTalk = false;
        if (CurrPhase == PHASE_BEASTS_OF_NORTHEREND && m_bIsHeroic) //start timed summons
        {
            if (m_AttemptCounter == 50) 
            {
                uiSummonTimer = 27*IN_MILLISECONDS;
            } // if they wiped once, we just spawn him
            else uiSummonTimer = 0;
        }
    }

    void StartNextBoss()
    {
        //remove corpses
        RemoveAllSummons();

        switch (CurrPhase)
        {
            case PHASE_BEASTS_OF_NORTHEREND:
                CurrBeastOfNortherendPhase = PHASE_GORMOK;
                if (!m_bIsHeroic)               //summon each one after the previous died
                    SpawnBoss(NPC_GORMOK);
                break;
            case PHASE_JARAXXUS:
                SpawnBoss(NPC_JARAXXUS);
                break;
            case PHASE_FACTION_CHAMPIONS:
            {
                std::vector<uint32> ChampionEntries;
                ChampionEntries.reserve(14);
                if (!IS_HORDE)
                    for (uint32 id = 34441; id <= 34459; id++) //the NPC ids are rather close together
                    {
                        if (id == 34442 || id == 34443 || id == 34446 || id == 34452)
                            continue;

                        ChampionEntries.push_back(id);
                    }
                else
                    for (uint32 id = 34460; id <= 34475; id++)
                    {
                        if (id == 34462 || id == 34464)
                            continue;

                        ChampionEntries.push_back(id);
                    }
                std::random_shuffle(ChampionEntries.begin(), ChampionEntries.end());
                uint32 amount = m_bIs10Man ? 6 : 10;
                for (uint32 i = 0 ; i < amount; i++)
                    SpawnBoss(ChampionEntries[i], i);
                break;
            }
            case PHASE_TWIN_VALKYR:
                SpawnBoss(NPC_FJOLA_LIGHTBANE, 1);
                SpawnBoss(NPC_EYDIS_DARKBANE, 2);
                break;
            default:
                CurrPhase = PHASE_NOT_STARTED;
                break;
        }
    }

    void UpdateIntros(const uint32 uiDiff)
    {
        if (m_uiTalkTimer < uiDiff)
        {
            Creature *Fordring = GET_CREATURE(TYPE_TIRION_FORDRING);
            switch (CurrPhase)
            {
                case PHASE_ANUBARAK:
                    switch (m_uiTalkCounter)
                    {
                        case 0:
                            RemoveAllSummons();
                            if (Fordring)
                                DoScriptText(SAY_TIRION_ANUBARAK_INTRO1, Fordring);
                            m_uiTalkTimer = 19*IN_MILLISECONDS;
                            break;
                        case 1:
                            Creature *LichKing;
                            if (Fordring)   //cant use m_creature to summon king, otherwise lich king will be considered as a boss and it will get complicated
                                LichKing = Fordring->SummonCreature(NPC_LICH_KING, summon_pos[0], summon_pos[1], summon_pos[2], summon_pos[3], TEMPSUMMON_CORPSE_DESPAWN, 0);
                            if (LichKing)
                                DoScriptText(SAY_LICHKING_ANUBARAK_INTRO2, LichKing);
                            m_uiTalkTimer = 5*IN_MILLISECONDS;
                            break;
                        case 2:
                            if (Fordring)
                                DoScriptText(SAY_TIRION_ANUBARAK_INTRO3, Fordring);
                            m_uiTalkTimer = 7*IN_MILLISECONDS;
                            break;
                        case 3:
                            if (Creature *LichKing = GET_CREATURE(TYPE_LICH_KING))
                                DoScriptText(SAY_LICHKING_ANUBARAK_INTRO4, LichKing);
                            m_uiTalkTimer = 20*IN_MILLISECONDS;
                            break;
                        case 4:
                            if (Creature *LichKing = GET_CREATURE(TYPE_LICH_KING))
                                DoScriptText(SAY_LICHKING_ANUBARAK_INTRO5, LichKing);
                            DestroyFloor();
                            m_uiTalkTimer = 9*IN_MILLISECONDS;
                            break;
                        case 5:
                            if (Creature *LichKing = GET_CREATURE(TYPE_LICH_KING))
                                LichKing->ForcedDespawn();
                            m_bIsInTalkPhase = false;
                            break;
                        default:
                            m_bIsInTalkPhase = false;
                            break;
                    }
                    m_uiTalkCounter++;
                    break;
                case PHASE_FACTION_CHAMPIONS:
                    switch (m_uiTalkCounter)
                    {
                        case 0:
                            if (Fordring)
                                DoScriptText(SAY_TIRION_FACTION_CHAMPIONS_INTRO1, Fordring);
                            m_uiAggroTimer = 35*IN_MILLISECONDS;
                            m_uiTalkTimer = 9*IN_MILLISECONDS;
                            break;
                        case 1:
                            if (IS_HORDE)
                            {
                                if (Creature* Varian = GET_CREATURE(TYPE_VARIAN_WYRM))
                                    DoScriptText(SAY_VARIAN_FACTION_CHAMPIONS_HORDE_INTRO2, Varian);
                                m_uiTalkTimer = 16*IN_MILLISECONDS;
                            }
                            else
                            {
                                if (Creature* Garrosh = GET_CREATURE(TYPE_GARROSH_HELLSCREAM))
                                    DoScriptText(SAY_GARROSH_FACTION_CHAMPIONS_ALLIANCE_INTRO2, Garrosh);
                                m_uiTalkTimer = 15*IN_MILLISECONDS;
                            }
                            break;
                        case 2:
                            if (Fordring)
                                DoScriptText(SAY_TIRION_FACTION_CHAMPIONS_INTRO3, Fordring);
                            StartNextBoss();
                            m_uiTalkTimer = 4.5*IN_MILLISECONDS;
                            break;
                        case 3:
                            if (IS_HORDE)
                            {
                                if (Creature* Varian = GET_CREATURE(TYPE_VARIAN_WYRM))
                                    DoScriptText(SAY_VARIAN_FACTION_CHAMPIONS_HORDE_INTRO4, Varian);
                                m_uiTalkTimer =10*IN_MILLISECONDS;
                            }
                            else
                            {
                                if (Creature* Garrosh = GET_CREATURE(TYPE_GARROSH_HELLSCREAM))
                                    DoScriptText(SAY_GARROSH_FACTION_CHAMPIONS_ALLIANCE_INTRO4, Garrosh);
                                m_uiTalkTimer = 10*IN_MILLISECONDS;
                            }
                            break;
                        default:
                            m_bIsInTalkPhase = false;
                            m_bCombatStart = true;
                            break;
                    }
                    m_uiTalkCounter++;
                    break;
                case PHASE_JARAXXUS:
                    switch (m_uiTalkCounter)
                    {
                        case 0:
                            RemoveAllSummons();
                            m_bIsInTalkPhase = true;
                            m_bCombatStart = false;
                            if (Fordring)
                                DoScriptText(SAY_TIRION_JARAXXUS_INTRO1, Fordring);
                            Creature *Fizzlebang;
                            if (Fordring)
                                Fizzlebang = Fordring->SummonCreature(NPC_WILFRED_FIZZLEBANG, summon_pos[0], summon_pos[1]+40.0f, summon_pos[2], summon_pos[3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
                            if (Fizzlebang)
                                Fizzlebang->GetMotionMaster()->MovePoint(0, RoomCenter[0], RoomCenter[1]+10.0f, RoomCenter[2]);
                            m_uiTalkTimer = 30*IN_MILLISECONDS;
                            break;
                        case 1:
                            if (Creature *Fizzlebang = GET_CREATURE(TYPE_FIZZLEBANG))
                                DoScriptText(SAY_WILFRED_JARAXXUS_INTRO2, Fizzlebang);
                            if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
                                Gate->SetGoState(GO_STATE_READY);
                            m_uiTalkTimer = 12*IN_MILLISECONDS;
                            break;
                        case 2:
                            if (Creature *Fizzlebang = GET_CREATURE(TYPE_FIZZLEBANG))
                            {
                                DoScriptText(SAY_WILFRED_JARAXXUS_INTRO3, Fizzlebang);
                                if (Creature* Portal = GET_CREATURE(TYPE_PORTAL_TARGET))
                                {
                                    Portal->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    Portal->SetPhaseMask(1, true);
                                    Fizzlebang->CastSpell(Portal, SPELL_OPEN_PORTAL, false);
                                }
                                // the rune circle and the portal are not visible for some reason (only visible in GM mode)
                                if (Creature *Runes = GET_CREATURE(TYPE_PURPLE_RUNE))
                                {
                                    Runes->SetPhaseMask(1, true);
                                    Runes->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); 
                                }
                            }
                            m_uiTalkTimer = 5*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 3:
                            if (Creature* Portal = GET_CREATURE(TYPE_PORTAL_TARGET))
                            {
                                Portal->CastSpell(Portal, SPELL_WILFRED_PORTAL, false);
                            }
                            m_uiTalkTimer = 6*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 4:
                            if (Creature *Fizzlebang = GET_CREATURE(TYPE_FIZZLEBANG))
                                DoScriptText(SAY_WILFRED_JARAXXUS_INTRO4, Fizzlebang);
                            StartNextBoss();
                            if (Creature *Jaraxxus = GET_CREATURE(TYPE_JARAXXUS))
                                Jaraxxus->GetMotionMaster()->MovePoint(0, Jaraxxus->GetPositionX(), RoomCenter[1] - 10.0f, Jaraxxus->GetPositionZ());
                            m_uiTalkTimer = 7*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 5:
                            if (Creature* Runes = GET_CREATURE(TYPE_PURPLE_RUNE))
                                Runes->SetPhaseMask(256, true);
                            if (Creature* Portal = GET_CREATURE(TYPE_PORTAL_TARGET))
                            {
                                Portal->RemoveAllAuras();
                                Portal->SetPhaseMask(256, true);
                            }
                            m_uiTalkTimer = 7*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 6:
                            if (Creature *Jaraxxus = GET_CREATURE(TYPE_JARAXXUS))
                            {
                                Jaraxxus->GetMotionMaster()->MovePoint(0, Jaraxxus->GetPositionX(), Jaraxxus->GetPositionY()+0.1f, Jaraxxus->GetPositionZ()); 
                                DoScriptText(SAY_JARAXXUS_INTRO5, Jaraxxus);
                            }
                            m_uiTalkTimer = 5*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 7:
                            if (Creature *Fizzlebang = GET_CREATURE(TYPE_FIZZLEBANG))
                                DoScriptText(SAY_WILFRED_JARAXXUS_INTRO6, Fizzlebang);
                            m_uiTalkTimer = 1*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 8:
                            if (Creature* Jaraxxus = GET_CREATURE(TYPE_JARAXXUS))
                            {                             
                                if (Creature* Fizzlebang = GET_CREATURE(TYPE_FIZZLEBANG))
                                    Jaraxxus->CastSpell(Fizzlebang, SPELL_FEL_LIGHTNING, false);
                            }
                            m_uiTalkTimer = 4.5*IN_MILLISECONDS;
                            m_bIsInTalkPhase = true;
                            break;
                        case 9:
                            if (Fordring)
                            {
                                DoScriptText(SAY_TIRION_JARAXXUS_INTRO7, Fordring);
                                if (Creature *Jaraxxus = GET_CREATURE(TYPE_JARAXXUS))
                                    Jaraxxus->GetMotionMaster()->MovePoint(0, Jaraxxus->GetPositionX(), Jaraxxus->GetPositionY()-0.1f, Jaraxxus->GetPositionZ()); 
                            }
                            m_uiTalkTimer = 5.5*IN_MILLISECONDS;
                            m_bCombatStart = true;
                            break;
                        default:
                            m_bIsInTalkPhase = false;
                            break;
                    }
                    m_uiTalkCounter++;
                    break;
                case PHASE_BEASTS_OF_NORTHEREND:
                    switch (CurrBeastOfNortherendPhase)
                    {
                        case PHASE_BEASTS_NONE:
                        case PHASE_GORMOK:
                            if (m_bIsHeroic && m_AttemptCounter != 50) // if they wiped once, we skip the long intro
                                m_uiTalkCounter = 2;
                            switch (m_uiTalkCounter)
                            {
                                case 0:
                                    if (Fordring)
                                        DoScriptText(SAY_TIRION_TRIAL_OF_THE_CRUSADER_INTRO_1, Fordring);
                                    m_uiTalkTimer = 22*IN_MILLISECONDS;
                                    m_bIsInTalkPhase = true;
                                    break;
                                case 1:
                                    if (Fordring)
                                        DoScriptText(SAY_TIRION_TRIAL_OF_THE_CRUSADER_INTRO_2, Fordring);
                                    m_uiTalkTimer = 5*IN_MILLISECONDS;
                                    break;
                                case 2:
                                    if(!m_bIsHeroic)
                                        StartNextBoss();
                                    if (Fordring)
                                        DoScriptText(SAY_TIRION_GORMOK_SPAWN, Fordring);
                                    m_bIsInTalkPhase = false;
                                    break;
                                default:
                                    m_bIsInTalkPhase = false;
                                    break;
                            }
                            m_uiTalkCounter++;
                            break;
                        case PHASE_JORMUNGAR_TWINS:
                            if (Fordring)
                                DoScriptText(SAY_TIRION_TWIN_JORMUNGAR_SPAWN, Fordring);
                            if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
                                Gate->SetGoState(GO_STATE_ACTIVE);
                            m_bIsInTalkPhase = false;
                            break;
                        case PHASE_ICEHOWL:
                            if (Fordring)
                                DoScriptText(SAY_TIRION_ICEHOWL_SPAWN, Fordring);
                            if (GameObject *Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
                                Gate->SetGoState(GO_STATE_ACTIVE);
                            m_bIsInTalkPhase = false;
                            break;
                    }
                    break;
                case PHASE_TWIN_VALKYR:
                    if (Fordring)
                        DoScriptText(SAY_TIRION_TWIN_VALKYR_INTRO, Fordring);
                    StartNextBoss();
                    m_bIsInTalkPhase = false;
                    break;
                default:
                    break;
            }
        }
        else
            m_uiTalkTimer -= uiDiff;
    }

    void UpdateOutros(uint32 const uiDiff)
    {
        if (m_uiTalkTimer < uiDiff)
        {
            Creature *Fordring = GET_CREATURE(TYPE_TIRION_FORDRING);
            switch (CurrPhase)
            {
                case PHASE_JARAXXUS:
                    switch (m_uiTalkCounter)
                    {
                        case 0:
                            m_uiTalkTimer = 5000;
                            break;
                        case 1:
                            if (Unit* Fordring = GET_CREATURE(TYPE_TIRION_FORDRING))
                                DoScriptText(SAY_TIRION_JARAXXUS_OUTRO1, Fordring);
                            m_uiTalkTimer = 15000;
                            break;
                        case 2:
                            if (Unit* Garrosh = GET_CREATURE(TYPE_GARROSH_HELLSCREAM))
                                DoScriptText(SAY_GARROSH_JARAXXUS_HORDE_OUTRO2, Garrosh);
                            m_uiTalkTimer = 10500;
                            break;
                        case 3:
                            if (Unit* Wrynn = GET_CREATURE(TYPE_VARIAN_WYRM))
                                DoScriptText(SAY_VARIAN_JARAXXUS_HORDE_OUTRO3, Wrynn);
                            m_uiTalkTimer = 7000;
                            break;
                        case 4:
                            if (Unit* Fordring = GET_CREATURE(TYPE_TIRION_FORDRING))
                                DoScriptText(SAY_TIRION_JARAXXUS_HORDE_OUTRO4, Fordring);
                            break;
                        default:
                            m_bIsInOutroTalk = false;
                            break;
                    }
                    m_uiTalkCounter++;
                    break;
                case PHASE_FACTION_CHAMPIONS:
                    switch (m_uiTalkCounter)
                    {
                        case 0:
                            if (!IS_HORDE)
                            {
                                if (Creature* Varian = GET_CREATURE(TYPE_VARIAN_WYRM))
                                    DoScriptText(SAY_VARIAN_ALLIANCE_VICTORY, Varian);
                                m_uiTalkTimer =3*IN_MILLISECONDS;
                            }
                            else
                            {
                                if (Creature* Garrosh = GET_CREATURE(TYPE_GARROSH_HELLSCREAM))
                                    DoScriptText(SAY_GARROSH_HORDE_VICTORY, Garrosh);
                                m_uiTalkTimer = 5.5*IN_MILLISECONDS;
                            }
                            break;
                        case 1:
                            if (Unit* Fordring = GET_CREATURE(TYPE_TIRION_FORDRING))
                                DoScriptText(SAY_TIRION_OUTRO, Fordring);
                            m_bIsInOutroTalk = false;
                            break;
                        default:
                            m_bIsInOutroTalk = false;
                            break;
                    }
                    m_uiTalkCounter++;
                    break;
                case PHASE_TWIN_VALKYR:
                    if (IS_HORDE)
                        if (Creature *Garrosh = GET_CREATURE(TYPE_GARROSH_HELLSCREAM))
                            DoScriptText(SAY_GARROSH_TWIN_VALKYR_OUTRO, Garrosh);
                    m_bIsInOutroTalk = false;
                    break;
                default:
                    m_bIsInOutroTalk = false;
                    break;
           }
        }
        else
            m_uiTalkTimer -= uiDiff;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsInTalkPhase)
            UpdateIntros(uiDiff);

        if (m_bIsInOutroTalk)
            UpdateOutros(uiDiff);

        if (m_AttemptCounter == 0)
        {
            if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            if (Creature* Anub = GET_CREATURE(TYPE_ANUBARAK))
                Anub->ForcedDespawn();
        }

        if (m_bCombatStart)
        {
            if (m_uiAggroTimer < uiDiff)
            {
                if (GameObject* Gate = GET_GAMEOBJECT(TYPE_MAIN_GATE))
                    Gate->SetGoState(GO_STATE_READY);
                for (GuidList::const_iterator i = summons.begin(); i != summons.end(); ++i)
                    if (Creature *pSummon = m_creature->GetMap()->GetCreature(*i))
                        if (pSummon->isAlive())
                        {
                            pSummon->SetSpeedRate(MOVE_WALK, 1.0f);
                            pSummon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pSummon->SetInCombatWithZone();
                        }
                m_bCombatStart = false;
            }
            else
                m_uiAggroTimer -= uiDiff;
        }

        if (m_bIsHeroic && CurrPhase == PHASE_BEASTS_OF_NORTHEREND)
        {
            if (uiSummonTimer < uiDiff)
            {
                switch (CurrBeastOfNortherendPhase)
                {
                    case PHASE_BEASTS_NONE:
                        SpawnBoss(NPC_GORMOK);
                        uiSummonTimer = SUMMON_TIMER;
                        CurrBeastOfNortherendPhase++;
                        break;
                    case PHASE_GORMOK:
                        SpawnBoss(NPC_ACIDMAW, 1);
                        SpawnBoss(NPC_DREADSCALE, 2);
                        uiSummonTimer = SUMMON_TIMER;
                        CurrBeastOfNortherendPhase++;
                        break;
                    case PHASE_JORMUNGAR_TWINS:
                        SpawnBoss(NPC_ICEHOWL);
                        uiSummonTimer = SUMMON_TIMER;
                        CurrBeastOfNortherendPhase++;
                        break;
                    default:
                        return;
                }
            }
            else
                uiSummonTimer-= uiDiff;
        }
    }
};

bool GossipHello_npc_barrett_ramsey(Player *pPlayer, Creature *pCreature)
{
    npc_barrett_ramseyAI *Ai = dynamic_cast<npc_barrett_ramseyAI*>(pCreature->AI());
    if (Ai && !Ai->IsEncounterInProgress())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_NEXT_BOSS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_barrett_ramsey(Player *pPlayer, Creature *pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        npc_barrett_ramseyAI *Ai = dynamic_cast<npc_barrett_ramseyAI*>(pCreature->AI());
        if (Ai && !Ai->IsEncounterInProgress())
            Ai->StartNextPhase();
    }
    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    return true;
}

CreatureAI* GetAI_npc_barrett_ramsey(Creature* pCreature)
{
    return new npc_barrett_ramseyAI(pCreature);
}

void AddSC_npc_barrett_ramsey()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_barrett_ramsey";
    newscript->pGossipHello = &GossipHello_npc_barrett_ramsey;
    newscript->pGossipSelect = &GossipSelect_npc_barrett_ramsey;
    newscript->GetAI = &GetAI_npc_barrett_ramsey;
    newscript->RegisterSelf();
}
