#include "precompiled.h"
#include "trial_of_the_crusader.h"

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
    PHASE_NONE,
    PHASE_GORMOK,
    PHASE_JORMUNGAR_TWINS,
    PHASE_ICEHOWL
};

#define GOSSIP_START_NEXT_BOSS  "We are ready to begin the next challenge."  //not the actual text, but I cant find a source for it. If you know them, change it
#define GOSSIP_TEXT_ID          45323

#define SUMMON_POSITION         562, 182, 395, 0    //FIXME: it should be different spawn positions when multiple bosses spawned at once
#define DESPAWN_TIME            4*MINUTE*IN_MILISECONDS
#define AUTOSUMMON(C_ID)        m_creature->SummonCreature(C_ID, SUMMON_POSITION, TEMPSUMMON_DEAD_DESPAWN, DESPAWN_TIME);

#define SUMMON_TIMER            3*MINUTE*IN_MILISECONDS

struct MANGOS_DLL_DECL npc_barrett_ramseyAI: public ScriptedAI
{
    ScriptedInstance *m_pInstance;
    uint32 CurrPhase;
    uint32 CurrBeastOfNortherendPhase;
    bool EncounterInProgress;
    bool m_bIsHeroic;
    uint32 uiSummonTimer;

    typedef std::list<uint64> GuidList;
    GuidList summons;

    npc_barrett_ramseyAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        CurrPhase = PHASE_NOT_STARTED;
        if(m_pInstance) //choose correct phase, in case of d/c or something happens and barrett is respawned
        {
            if (m_pInstance->GetData(TYPE_ANUBARAK) == DONE)
                CurrPhase = PHASE_ANUBARAK;
            else if(m_pInstance->GetData(TYPE_EYDIS_DARKBANE) == DONE)
                CurrPhase = PHASE_TWIN_VALKYR;
            else if(m_pInstance->GetData(TYPE_GORGRIM_SHADOWCLEAVE) == DONE)
                CurrPhase = PHASE_FACTION_CHAMPIONS;
            else if(m_pInstance->GetData(TYPE_JARAXXUS) == DONE)
                CurrPhase = PHASE_JARAXXUS;
            else if(m_pInstance->GetData(TYPE_ICEHOWL) == DONE)
                CurrPhase = PHASE_BEASTS_OF_NORTHEREND;
        }
        CurrBeastOfNortherendPhase = PHASE_NONE;
        EncounterInProgress = false;
        m_bIsHeroic = pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC;
        uiSummonTimer = SUMMON_TIMER;
        Reset();
    }

    bool IsEncounterInProgress()
    {
        return EncounterInProgress;
    }

    void RemoveAllSummons()
    {
        for(GuidList::const_iterator i = summons.begin(); i != summons.end(); ++i)
            if(Creature *summ = m_creature->GetMap()->GetCreature(*i))
                summ->ForcedDespawn();
        summons.clear();
    }

    void Reset()    //called when any creature wipes group
    {
        if (CurrPhase)
            CurrPhase--;
        if (m_pInstance)
        {
            switch (CurrPhase)
            {
                case PHASE_BEASTS_OF_NORTHEREND:
                    m_pInstance->SetData(TYPE_GORMOK, NOT_STARTED);
                    m_pInstance->SetData(TYPE_ACIDMAW, NOT_STARTED);
                    m_pInstance->SetData(TYPE_DREADSCALE, NOT_STARTED);
                    m_pInstance->SetData(TYPE_ICEHOWL, NOT_STARTED);
                    break;
                case PHASE_FACTION_CHAMPIONS:
                    for(uint32 i = FACTION_CHAMPION_START; i <= FACTION_CHAMPION_END; i++)
                        m_pInstance->SetData(i, NOT_STARTED);
                    break;
                case PHASE_TWIN_VALKYR:
                    m_pInstance->SetData(TYPE_FJOLA_LIGHTBANE, NOT_STARTED);
                    m_pInstance->SetData(TYPE_EYDIS_DARKBANE, NOT_STARTED);
                default:
                    break;
            }
        }
        CurrBeastOfNortherendPhase = PHASE_NONE;
        EncounterInProgress = false;

        RemoveAllSummons();

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void JustSummoned(Creature *pSummon)
    {
        if (pSummon)
            summons.push_back(pSummon->GetGUID());
    }

    void SummonedCreatureJustDied(Creature *who)
    {
        if (!m_pInstance)
            return;
        if (who)
            if (boss_trial_of_the_crusaderAI *bossAI = dynamic_cast<boss_trial_of_the_crusaderAI*>(who->AI()))
                m_pInstance->SetData(bossAI->m_uiBossEncounterId, DONE);

        switch(who->GetEntry())
        {
            case NPC_ANUBARAK: //called directly from JustDied in anub'arak's AI
                EncounterInProgress = false;
                CurrPhase = PHASE_DONE;
                break;
            case NPC_ICEHOWL:
            case NPC_JARAXXUS:
                EncounterInProgress = false;
                break;
            case NPC_GORMOK:
                if(!m_bIsHeroic)
                {
                    AUTOSUMMON(NPC_ACIDMAW);
                    AUTOSUMMON(NPC_DREADSCALE);
                }
                break;
            case NPC_ACIDMAW:
            case NPC_DREADSCALE:
                if(!m_bIsHeroic && !m_pInstance->IsEncounterInProgress())
                    AUTOSUMMON(NPC_ICEHOWL);
                break;
            case NPC_FJOLA_LIGHTBANE:
            case NPC_EYDIS_DARKBANE:
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
                if(!m_pInstance->IsEncounterInProgress())
                    EncounterInProgress = false;
                break;
        }

        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void StartNextPhase()
    {
        EncounterInProgress = true;
        CurrPhase++;
        GameObject *pFloor = GET_GAMEOBJECT(TYPE_COLISEUM_FLOOR);

        //remove corpses
        RemoveAllSummons();

        switch (CurrPhase)
        {
            case PHASE_BEASTS_OF_NORTHEREND:
                if (m_bIsHeroic)
                {
                    CurrBeastOfNortherendPhase = PHASE_GORMOK;
                    uiSummonTimer = SUMMON_TIMER;
                }
                AUTOSUMMON(NPC_GORMOK);
                break;
            case PHASE_JARAXXUS:
                AUTOSUMMON(NPC_JARAXXUS);   //not really how it starts
                break;
            case PHASE_FACTION_CHAMPIONS:
                if(!IS_HORDE)
                    for(uint32 i = 34441; i <= 34459; i++) //the NPC ids are rather close together
                    {
                        if(i == 34442 || i == 34443 || i==34446 || i== 34452)
                            continue;

                        AUTOSUMMON(i);
                    }
                else
                    for(uint32 i = 34460; i <= 34475; i++)
                    {
                        if(i == 34462 || i == 34464)
                            continue;

                        AUTOSUMMON(i);
                    }
                break;
            case PHASE_TWIN_VALKYR:
                AUTOSUMMON(NPC_FJOLA_LIGHTBANE);
                AUTOSUMMON(NPC_EYDIS_DARKBANE);
                break;
            case PHASE_ANUBARAK:
                if(m_pInstance && pFloor)
                    pFloor->RemoveFromWorld(); //hacky fix, type 33 (destructable building) not implemented
                break;
            default:
                CurrPhase = PHASE_NOT_STARTED;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (CurrBeastOfNortherendPhase != PHASE_NONE && m_bIsHeroic)
        {
            if (uiSummonTimer < uiDiff)
            {
                CurrBeastOfNortherendPhase++;
                switch (CurrBeastOfNortherendPhase)
                {
                    case PHASE_JORMUNGAR_TWINS:
                        AUTOSUMMON(NPC_ACIDMAW);
                        AUTOSUMMON(NPC_DREADSCALE);
                        uiSummonTimer = SUMMON_TIMER;
                        break;
                    case PHASE_ICEHOWL:
                        AUTOSUMMON(NPC_ICEHOWL);
                        uiSummonTimer = SUMMON_TIMER;
                        break;
                    default:
                        CurrBeastOfNortherendPhase = PHASE_NONE;
                        break;
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
    if(Ai && !Ai->IsEncounterInProgress())
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
        if(Ai && !Ai->IsEncounterInProgress())
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
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_barrett_ramsey";
    newscript->pGossipHello = &GossipHello_npc_barrett_ramsey;
    newscript->pGossipSelect = &GossipSelect_npc_barrett_ramsey;
    newscript->GetAI = &GetAI_npc_barrett_ramsey;
    newscript->RegisterSelf();
}
