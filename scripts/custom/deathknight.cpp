  // *************************************//
 // Phoenix Development Team July, 2009  //
//**************************************//

#include "precompiled.h"
#include "escort_ai.h"
#include "WorldPacket.h"

//Quest:The Gift That Keeps On Giving
struct MANGOS_DLL_DECL npc_scarlet_minerAI : public ScriptedAI
{
    npc_scarlet_minerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && pSpell->Id == 52481)
        {
            if(((Player*)pCaster)->GetQuestStatus(12698) == QUEST_STATUS_INCOMPLETE)
            {
                //We summon the ghoul for 60 seconds
                Unit* pGhoul = m_creature->SummonCreature(28845,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),m_creature->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,60000);

                //We kill the mob since he is now a ghoul
                m_creature->SetHealth(0);
                m_creature->setDeathState(DEAD);

                //We increase the quest counter
                ((Player*)pCaster)->KilledMonsterCredit(28845,pGhoul->GetGUID());
            }
        }
    }
};

CreatureAI* GetAI_npc_scarlet_miner(Creature* pCreature)
{
    return new npc_scarlet_minerAI(pCreature);
}

//Quest:Massacre At Light's Point
struct MANGOS_DLL_DECL npc_scarlet_defenderAI : public ScriptedAI
{
    npc_scarlet_defenderAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12701,28849) < 100)
                pKiller->KilledMonsterCredit(28849,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_scarlet_defender(Creature* pCreature)
{
    return new npc_scarlet_defenderAI(pCreature);
}

//Quest:Nowhere To Run And Nowhere To Hide
struct MANGOS_DLL_DECL npc_mayor_quimbyAI : public ScriptedAI
{
    npc_mayor_quimbyAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12719,28945) < 1)
                pKiller->KilledMonsterCredit(28945,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_mayor_quimby(Creature* pCreature)
{
    return new npc_mayor_quimbyAI(pCreature);
}

//Quest:Lambs To The Slaughter
struct MANGOS_DLL_DECL npc_scarlet_crusade_soldierAI : public ScriptedAI
{
    npc_scarlet_crusade_soldierAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12722,28984) < 10)
                pKiller->KilledMonsterCredit(28984,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_scarlet_crusade_soldier(Creature* pCreature)
{
    return new npc_scarlet_crusade_soldierAI(pCreature);
}

struct MANGOS_DLL_DECL npc_citizen_of_new_avalonAI : public ScriptedAI
{
    npc_citizen_of_new_avalonAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12722,28986) < 15)
                pKiller->KilledMonsterCredit(28986,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_citizen_of_new_avalon(Creature* pCreature)
{
    return new npc_citizen_of_new_avalonAI(pCreature);
}

/* Script in ebon_hold.cpp
//Quest:A Special Surprise
enum
{
    SAY_EXECUTE01                     = -1609101,
    SAY_EXECUTE02                     = -1609102,
    SAY_EXECUTE03                     = -1609103,
    SAY_EXECUTE04                     = -1609104,
    SAY_EXECUTE05                     = -1609105,
    SAY_EXECUTE06                     = -1609106,
    SAY_EXECUTE07                     = -1609107,
    SAY_EXECUTE08                     = -1609108,
    SAY_EXECUTE09                     = -1609109,
    SAY_EXECUTE10                     = -1609110,
    SAY_EXECUTE11                     = -1609111,
    SAY_EXECUTE12                     = -1609112,
    SAY_EXECUTE13                     = -1609113,
    SAY_EXECUTE14                     = -1609114,
    SAY_EXECUTE15                     = -1609115,
    SAY_EXECUTE16                     = -1609116,
    SAY_EXECUTE17                     = -1609117,
    SAY_EXECUTE18                     = -1609118,
    SAY_EXECUTE19                     = -1609119,
    SAY_EXECUTE20                     = -1609120,
    SAY_EXECUTE21                     = -1609121,
    SAY_EXECUTE22                     = -1609122,
    SAY_EXECUTE23                     = -1609123,
    SAY_EXECUTE24                     = -1609124,
    SAY_EXECUTE25                     = -1609125,
    SAY_EXECUTE26                     = -1609126,
    SAY_EXECUTE27                     = -1609127,
    SAY_EXECUTE28                     = -1609128,
    SAY_EXECUTE29                     = -1609129,
    SAY_EXECUTE30                     = -1609130,
    SAY_EXECUTE31                     = -1609131,
    SAY_EXECUTE32                     = -1609132,
    SAY_EXECUTE33                     = -1609133,
    SAY_EXECUTE34                     = -1609134,
    SAY_EXECUTE35                     = -1609135,
    SAY_EXECUTE36                     = -1609136,
    SAY_EXECUTE37                     = -1609137,
    SAY_EXECUTE38                     = -1609138,
    SAY_EXECUTE39                     = -1609139,
    SAY_EXECUTE40                     = -1609140,
    SAY_EXECUTE41                     = -1609141,
    SAY_EXECUTE42                     = -1609142,
    SAY_EXECUTE43                     = -1609143,
    SAY_EXECUTE44                     = -1609144,
    SAY_EXECUTE45                     = -1609145,
    SAY_EXECUTE46                     = -1609146,
    SAY_EXECUTE47                     = -1609147,
    SAY_EXECUTE48                     = -1609148,
    SAY_EXECUTE49                     = -1609149,
    SAY_EXECUTE50                     = -1609150,
    SAY_EXECUTE51                     = -1609151,
    SAY_EXECUTE52                     = -1609152,
    SAY_EXECUTE53                     = -1609153,
    SAY_EXECUTE54                     = -1609154,
    SAY_EXECUTE55                     = -1609155,
    SAY_EXECUTE56                     = -1609156,
    SAY_EXECUTE57                     = -1609157,
    SAY_EXECUTE58                     = -1609158,
    SAY_EXECUTE59                     = -1609159
};

//Quest:A Special Surprise - Blood Elf
struct MANGOS_DLL_DECL npc_lady_eonysAI : public ScriptedAI
{
    npc_lady_eonysAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12747,29074) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12747,29074) < 1)
                pKiller->KilledMonsterCredit(29074,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE04, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE13, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE30, m_creature, pPlayer); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE43, m_creature, pPlayer); Talk_Counter++; break;
                case 10:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_lady_eonys(Creature* pCreature)
{
    return new npc_lady_eonysAI(pCreature);
}

//Quest:A Special Surprise - Draenei
struct MANGOS_DLL_DECL npc_valok_the_righteousAI : public ScriptedAI
{
    npc_valok_the_righteousAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12746,29070) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12746,29070) < 1)
                pKiller->KilledMonsterCredit(29070,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE04, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE14, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE19, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE31, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE44, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_valok_the_righteous(Creature* pCreature)
{
    return new npc_valok_the_righteousAI(pCreature);
}

//Quest:A Special Surprise - Dwarf
struct MANGOS_DLL_DECL npc_donovan_pulfrostAI : public ScriptedAI
{
    npc_donovan_pulfrostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12744,29067) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12744,29067) < 1)
                pKiller->KilledMonsterCredit(29067,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE02, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE05, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE15, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE20, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE32, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE39, m_creature, pPlayer); Talk_Counter++; break;
                case 9:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE45, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_donovan_pulfrost(Creature* pCreature)
{
    return new npc_donovan_pulfrostAI(pCreature);
}

//Quest:A Special Surprise - Undead
struct MANGOS_DLL_DECL npc_antoine_brackAI : public ScriptedAI
{
    npc_antoine_brackAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12750,29071) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12750,29071) < 1)
                pKiller->KilledMonsterCredit(29071,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE06, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE16, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE21, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE28, m_creature); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE40, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE46, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_antoine_brack(Creature* pCreature)
{
    return new npc_antoine_brackAI(pCreature);
}

//Quest:A Special Surprise - Gnome
struct MANGOS_DLL_DECL npc_goby_blastenheimerAI : public ScriptedAI
{
    npc_goby_blastenheimerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12745,29068) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12745,29068) < 1)
                pKiller->KilledMonsterCredit(29068,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE07, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE13, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE22, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE33, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE47, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_goby_blastenheimer(Creature* pCreature)
{
    return new npc_goby_blastenheimerAI(pCreature);
}

//Quest:A Special Surprise - Human
struct MANGOS_DLL_DECL npc_ellen_stanbridgeAI : public ScriptedAI
{
    npc_ellen_stanbridgeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12742,29061) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12742,29061) < 1)
                pKiller->KilledMonsterCredit(29061,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE08, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE13, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE23, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE34, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE48, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_ellen_stanbridge(Creature* pCreature)
{
    return new npc_ellen_stanbridgeAI(pCreature);
}

//Quest:A Special Surprise - Night Elf
struct MANGOS_DLL_DECL npc_yazmina_oakenthornAI : public ScriptedAI
{
    npc_yazmina_oakenthornAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12743,29065) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12743,29065) < 1)
                pKiller->KilledMonsterCredit(29065,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE08, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE13, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE24, m_creature, pPlayer); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE29, m_creature); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE49, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_yazmina_oakenthorn(Creature* pCreature)
{
    return new npc_yazmina_oakenthornAI(pCreature);
}

//Quest:A Special Surprise - Orc
struct MANGOS_DLL_DECL npc_kug_ironjawAI : public ScriptedAI
{
    npc_kug_ironjawAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12748,29072) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12748,29072) < 1)
                pKiller->KilledMonsterCredit(29072,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE09, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE13, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE25, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE35, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE50, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_kug_ironjaw(Creature* pCreature)
{
    return new npc_kug_ironjawAI(pCreature);
}

//Quest:A Special Surprise - Tauren
struct MANGOS_DLL_DECL npc_malar_bravehornAI : public ScriptedAI
{
    npc_malar_bravehornAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        KnightCommanderPlaguefist = GetClosestCreatureWithEntry(m_creature, 29053, 300.0f);
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    Creature* KnightCommanderPlaguefist;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12739,29032) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12739,29032) < 1)
                pKiller->KilledMonsterCredit(29032,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE01, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE04, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE11, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE17, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE26, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE36, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE38, m_creature, pPlayer); Talk_Counter++; break;
                case 9: DoScriptText(SAY_EXECUTE42, KnightCommanderPlaguefist, pPlayer); Talk_Counter++; break;
                case 10: DoScriptText(SAY_EXECUTE51, m_creature, pPlayer); Talk_Counter++; break;
                case 11:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE53, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_malar_bravehorn(Creature* pCreature)
{
    return new npc_malar_bravehornAI(pCreature);
}

//Quest:A Special Surprise - Troll 
struct MANGOS_DLL_DECL npc_iggy_darktuskAI : public ScriptedAI
{
    npc_iggy_darktuskAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 Talk_Timer;
    uint32 Talk_Counter;
    uint64 PlayerGUID;

    void Reset()
    {
        Talk_Timer = 0;
        Talk_Counter = 0;
        PlayerGUID = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if(((Player*)who)->GetReqKillOrCastCurrentCount(12749,29073) < 1)
        {
            if (Talk_Counter == 0)
            {
                PlayerGUID = who->GetGUID();
                Talk_Counter++;
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12749,29073) < 1)
                pKiller->KilledMonsterCredit(29073,m_creature->GetGUID());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID);
        if (Talk_Timer < diff)
        {
            switch(Talk_Counter)
            {
                case 1: DoScriptText(SAY_EXECUTE03, m_creature); Talk_Counter++; break;
                case 2: ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND); Talk_Counter++; break;
                case 3: DoScriptText(SAY_EXECUTE10, m_creature); Talk_Counter++; break;
                case 4: DoScriptText(SAY_EXECUTE12, m_creature, pPlayer); Talk_Counter++; break;
                case 5: DoScriptText(SAY_EXECUTE18, m_creature, pPlayer); Talk_Counter++; break;
                case 6: DoScriptText(SAY_EXECUTE27, m_creature); Talk_Counter++; break;
                case 7: DoScriptText(SAY_EXECUTE37, m_creature, pPlayer); Talk_Counter++; break;
                case 8: DoScriptText(SAY_EXECUTE41, m_creature); Talk_Counter++; break;
                case 9:
                    ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                    DoScriptText(SAY_EXECUTE52, m_creature, pPlayer);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Talk_Counter++;
                break;
            }

            Talk_Timer = 2000;
        }else Talk_Timer -= diff;
    }
};

CreatureAI* GetAI_npc_iggy_darktusk(Creature* pCreature)
{
    return new npc_iggy_darktuskAI(pCreature);
}
*/

//Quest:An End To All Things...
struct MANGOS_DLL_DECL npc_scarlet_soldierAI : public ScriptedAI
{
    npc_scarlet_soldierAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12779,29150) < 150)
                pKiller->KilledMonsterCredit(29150,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_scarlet_soldier(Creature* pCreature)
{
    return new npc_scarlet_soldierAI(pCreature);
}

struct MANGOS_DLL_DECL npc_scarlet_ballistaAI : public ScriptedAI
{
    npc_scarlet_ballistaAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(12779,29104) < 10)
                pKiller->KilledMonsterCredit(29104,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_scarlet_ballista(Creature* pCreature)
{
    return new npc_scarlet_ballistaAI(pCreature);
}

//Quest:How To Win Friends And Influence Enemies
enum
{
    SAY_PERSUADE1                     = -1609751,
    SAY_PERSUADE2                     = -1609752,
    SAY_PERSUADE3                     = -1609753,
    SAY_PERSUADE4                     = -1609754,
    SAY_PERSUADE5                     = -1609755,
    SAY_PERSUADE6                     = -1609756,
    SAY_PERSUADE7                     = -1609757,
    SAY_CRUSADER1                     = -1609758,
    SAY_CRUSADER2                     = -1609759,
    SAY_CRUSADER3                     = -1609760,
    SAY_CRUSADER4                     = -1609761,
    SAY_CRUSADER5                     = -1609762,
    SAY_CRUSADER6                     = -1609763,
    SAY_PERSUADED1                    = -1609764,
    SAY_PERSUADED2                    = -1609765,
    SAY_PERSUADED3                    = -1609766,
    SAY_PERSUADED4                    = -1609767,
    SAY_PERSUADED5                    = -1609768,
    SAY_PERSUADED6                    = -1609769,
    SPELL_PERSUASIVE_STRIKE           = 52781
};

struct MANGOS_DLL_DECL npc_persuade_scarletAI : public ScriptedAI
{
    npc_persuade_scarletAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 uiSpeech_timer;
    uint32 uiSpeech_counter;
    uint32 uiCrusade_faction;
    uint64 uiPlayerGUID;

    void Reset()
    {
        uiSpeech_timer = 0;
        uiSpeech_counter = 0;
        uiCrusade_faction = 0;
        uiPlayerGUID = 0;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == SPELL_PERSUASIVE_STRIKE && uiSpeech_counter == 0)
        {
            if(((Player*)caster)->GetQuestStatus(12720) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 > 75) // chance
                {
                    uiPlayerGUID = ((Player*)caster)->GetGUID();
                    uiCrusade_faction = m_creature->getFaction();
                    uiSpeech_timer = 1000;
                    uiSpeech_counter = 1;
                    m_creature->setFaction(35);
                }
                else if (uiSpeech_counter == 0)
                {
                    switch(rand()%6)
                    {
                        case 0: DoScriptText(SAY_PERSUADE1, caster);break;
                        case 1: DoScriptText(SAY_PERSUADE2, caster);break;
                        case 2: DoScriptText(SAY_PERSUADE3, caster);break;
                        case 3: DoScriptText(SAY_PERSUADE4, caster);break;
                        case 4: DoScriptText(SAY_PERSUADE5, caster);break;
                        case 5: DoScriptText(SAY_PERSUADE6, caster);break;
                        case 6: DoScriptText(SAY_PERSUADE7, caster);break;
                    }
                    switch(rand()%5)
                    {
                        case 0: DoScriptText(SAY_CRUSADER1, m_creature);break;
                        case 1: DoScriptText(SAY_CRUSADER2, m_creature);break;
                        case 2: DoScriptText(SAY_CRUSADER3, m_creature);break;
                        case 3: DoScriptText(SAY_CRUSADER4, m_creature);break;
                        case 4: DoScriptText(SAY_CRUSADER5, m_creature);break;
                        case 5: DoScriptText(SAY_CRUSADER6, m_creature);break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiSpeech_counter >= 1 && uiSpeech_counter <= 6)
            if (uiSpeech_timer < diff)
            {
                m_creature->CombatStop(true);
                m_creature->StopMoving();
                Unit* pPlayer = Unit::GetUnit(*m_creature, uiPlayerGUID);

                switch(uiSpeech_counter)
                {
                    case 1: DoScriptText(SAY_PERSUADED1, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 2: DoScriptText(SAY_PERSUADED2, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 3: DoScriptText(SAY_PERSUADED3, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 4: DoScriptText(SAY_PERSUADED4, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 5: DoScriptText(SAY_PERSUADED5, pPlayer); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 6:
                        DoScriptText(SAY_PERSUADED6, m_creature);
                        m_creature->setFaction(uiCrusade_faction);
                        uiSpeech_timer = 0;
                        uiCrusade_faction = 0;
                        uiSpeech_counter++;
                        AttackStart(pPlayer);
                        if(((Player*)pPlayer)->GetQuestStatus(12720) == QUEST_STATUS_INCOMPLETE)
                            ((Player*)pPlayer)->AreaExploredOrEventHappens(12720);
                        break;
                }
            }else uiSpeech_timer -= diff;
        else
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_persuade_scarlet(Creature* pCreature)
{
    return new npc_persuade_scarletAI(pCreature);
}

//Quest:The Battle For The Ebon Hold
struct MANGOS_DLL_DECL npc_patchwerkAI : public ScriptedAI
{
    npc_patchwerkAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator i = tlist.begin(); i != tlist.end(); ++i)
        {
            Unit* TheKiller = Unit::GetUnit((*m_creature),(*i)->getUnitGuid());
            if(TheKiller->GetTypeId()==TYPEID_PLAYER)
            {
                Player* pKiller = (Player*)TheKiller;

                if(pKiller->GetReqKillOrCastCurrentCount(13166,31099) < 1)
                    pKiller->KilledMonsterCredit(31099,m_creature->GetGUID());
            }
        }
    }
};

CreatureAI* GetAI_npc_patchwerk(Creature* pCreature)
{
    return new npc_patchwerkAI(pCreature);
}

struct MANGOS_DLL_DECL npc_scourgeAI : public ScriptedAI
{
    npc_scourgeAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset(){}

    void JustDied(Unit *killer)
    {
        if(killer->GetTypeId()==TYPEID_PLAYER){
            Player* pKiller = (Player*)killer;

            if(pKiller->GetReqKillOrCastCurrentCount(13166,31100) < 10)
                pKiller->KilledMonsterCredit(31100,m_creature->GetGUID());
        }
    }
};

CreatureAI* GetAI_npc_scourge(Creature* pCreature)
{
    return new npc_scourgeAI(pCreature);
}

//Quest:Noth's Special Brew
bool GOHello_go_plague_cauldron(Player* pPlayer, GameObject* _GO)
{
    if(pPlayer->GetQuestStatus(12717) == QUEST_STATUS_INCOMPLETE)
    {
        if (pPlayer->HasItemCount(39329,1))
            pPlayer->AreaExploredOrEventHappens(12717);
    }

    return false;
}

//CKEGG SCRIPTS BELOW

// use 28957 Scarlet Crusader Test Dummy Guy to start it
enum scarletcourier
{
    SAY_TREE1                          = -1609770,
    SAY_TREE2                          = -1609771,
    SPELL_SHOOT                        = 52818,
    GO_INCONSPICUOUS_TREE              = 191144,
    NPC_SCARLET_COURIER                = 29076
};

struct MANGOS_DLL_DECL mob_scarlet_courierAI : public ScriptedAI
{
    mob_scarlet_courierAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiStage;
    uint32 uiStage_timer;
    uint64 pPlayer;

    void Reset() {
        uiStage = 0;
        uiStage_timer = 3000;
        pPlayer = 0;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(type != POINT_MOTION_TYPE)
                return;

        switch(id)
        {
            case 0:
                uiStage = 1;
                break;
            case 1:
                uiStage = 2;
                break;
        }
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiStage_timer < diff)
        {
            switch(uiStage)
            {
                case 1:
                {
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);

                    if (GameObject* treeGO = GetClosestGameObjectWithEntry(m_creature, GO_INCONSPICUOUS_TREE, 40.0f))
                    {
                        DoScriptText(SAY_TREE1, m_creature);
                        m_creature->GetMotionMaster()->MovePoint(1, treeGO->GetPositionX(), treeGO->GetPositionY(), treeGO->GetPositionZ());
                    }
                    uiStage = 0;
                } break;
                case 2:
                {
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    DoScriptText(SAY_TREE2, m_creature);
                    m_creature->Unmount();
                    //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    uiStage = 0;
                } break;
            }
            uiStage_timer = 3000;
        }else uiStage_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_scarlet_courier(Creature* pCreature)
{
    return new mob_scarlet_courierAI (pCreature);
}

struct MANGOS_DLL_DECL mob_scarlet_courier_controllerAI : public ScriptedAI
{
    mob_scarlet_courier_controllerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool bAmbush_overlook;

    void Reset() {
        bAmbush_overlook = false;
    }

    void UpdateAI(const uint32 diff) 
    {
        GameObject* treeGO = GetClosestGameObjectWithEntry(m_creature, GO_INCONSPICUOUS_TREE, 40.0f);
        if(treeGO && bAmbush_overlook == false)
        {
            Creature* pCourier = m_creature->SummonCreature(NPC_SCARLET_COURIER, 1461.65, -6010.34, 116.369, 0, TEMPSUMMON_TIMED_DESPAWN, 180000);
            pCourier->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            pCourier->Mount(14338); // not sure about this id
            pCourier->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            bAmbush_overlook = true;
        }
        if(!treeGO && bAmbush_overlook == true)
            bAmbush_overlook = false;
    }
};

CreatureAI* GetAI_mob_scarlet_courier_controller(Creature* pCreature)
{
    return new mob_scarlet_courier_controllerAI (pCreature);
}

/*######
## Npc Koltira Deathweaver
######*/
/*enum koltira
{
    SAY_BREAKOUT1                     = -1609772,
    SAY_BREAKOUT2                     = -1609773,
    SAY_BREAKOUT3                     = -1609774,
    SAY_BREAKOUT4                     = -1609775,
    SAY_BREAKOUT5                     = -1609776,
    SAY_BREAKOUT6                     = -1609777,
    SAY_BREAKOUT7                     = -1609778,
    SAY_BREAKOUT8                     = -1609779,
    SAY_BREAKOUT9                     = -1609780,
    SAY_BREAKOUT10                    = -1609781,
    SPELL_KOLTIRA_TRANSFORM           = 52899,
    SPELL_ANTI_MAGIC_ZONE             = 52894,
    NPC_CRIMSON_ACOLYTE               = 29007,
    NPC_HIGH_INQUISITOR_VALROTH       = 29001,
    MODEL_KOLTIRA                     = 28447
};

struct MANGOS_DLL_DECL npc_koltira_deathweaverAI : public npc_escortAI
{
    npc_koltira_deathweaverAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    bool bIsBattle;
    uint32 uiWave;
    uint32 uiWave_timer;
    uint64 uiValrothGUID;

    void Reset() {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            bIsBattle = false;
            uiWave = 0;
            uiWave_timer = 3000;
            uiValrothGUID = 0;
        }
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
            case 0:
                DoScriptText(SAY_BREAKOUT1, m_creature);
                break;
            case 1:
                ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 2:
                ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(m_creature, SPELL_KOLTIRA_TRANSFORM);
                break;
            case 3:
                SetHoldState(true);
                ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetRun(true);
                bIsBattle = true;
                DoScriptText(SAY_BREAKOUT2, m_creature);
                DoCast(m_creature, SPELL_ANTI_MAGIC_ZONE); // cast again that makes bubble up
                break;
            case 9:
                m_creature->Mount(25678); // not sure about this id
                break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if (Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID))
        {
            summoned->AI()->AttackStart(pPlayer);
            summoned->AddThreat(m_creature, 0.0f);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (bIsBattle)
        {
            if (uiWave_timer < diff)
            {
                if (uiWave == 0)
                    DoScriptText(SAY_BREAKOUT3, m_creature);
                else if (uiWave == 1)
                    DoScriptText(SAY_BREAKOUT4, m_creature);
                else if (uiWave == 2)
                {
                    DoScriptText(SAY_BREAKOUT5, m_creature);
                    m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329, -6045.818, 127.583, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                }

                if (uiWave >= 0 && uiWave <= 2)
                {
                    m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329, -6045.818, 127.583, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                    m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329, -6045.818, 127.583, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                    m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329, -6045.818, 127.583, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000);
                    uiWave++;
                    uiWave_timer = 20000;
                }
                else if (uiWave == 3)
                {
                    DoScriptText(SAY_BREAKOUT6, m_creature);
                    if (Creature* pNPC = m_creature->SummonCreature(NPC_HIGH_INQUISITOR_VALROTH, 1642.329, -6045.818, 127.583, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                        uiValrothGUID = pNPC->GetGUID();
                    uiWave++;
                    uiWave_timer = 1000;
                }
                else if (uiWave == 4)
                {
                    Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiValrothGUID));
                    if (!pTemp)
                    {
                        DoScriptText(SAY_BREAKOUT8, m_creature);

                        // just makes him stronger and be able to kill out
                        const CreatureInfo *cinfo = m_creature->GetCreatureInfo();
                        m_creature->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->mindmg * 100);
                        m_creature->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->maxdmg * 100);
                        m_creature->UpdateDamagePhysical(BASE_ATTACK);

                        m_creature->SetDisplayId(MODEL_KOLTIRA); // somehow he becomes nake again

                        uiWave++;
                        uiWave_timer = 5000;
                    }
                }
                else if (uiWave == 5)
                {
                    DoScriptText(SAY_BREAKOUT9, m_creature);
                    uiWave++;
                    uiWave_timer = 1000;
                }
                else if (uiWave == 6)
                {
                    DoScriptText(SAY_BREAKOUT10, m_creature);
                    SetHoldState(false);
                    uiWave++;
                    bIsBattle = false;
                }
            }else uiWave_timer -= diff;

        //DoMeleeAttackIfReady();
        }
    }
};

bool QuestAccept_npc_koltira_deathweaver(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == 12727)
    {
        ((Unit*)pCreature)->SetStandState(UNIT_STAND_STATE_STAND);
        ((npc_escortAI*)(pCreature->AI()))->Start(false, true, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_koltira_deathweaver(Creature* pCreature)
{
    npc_koltira_deathweaverAI* tempAI = new npc_koltira_deathweaverAI(pCreature);

    //tempAI->FillPointMovementListForCreature();

    return (CreatureAI*)tempAI;
}*/

/*######
## Mob High Inquisitor Valroth
######*/
enum valroth
{
    SAY_VALROTH1                      = -1609782,
    SAY_VALROTH2                      = -1609783,
    SAY_VALROTH3                      = -1609784,
    SAY_VALROTH4                      = -1609785,
    SAY_VALROTH5                      = -1609786,
    SAY_VALROTH6                      = -1609787,
    SPELL_RENEW                       = 38210,
    SPELL_INQUISITOR_PENANCE          = 52922,
    SPELL_VALROTH_SMITE               = 52926,
    SPELL_SUMMON_VALROTH_REMAINS      = 52929
};

struct MANGOS_DLL_DECL mob_high_inquisitor_valrothAI : public ScriptedAI
{
    mob_high_inquisitor_valrothAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiRenew_timer;
    uint32 uiInquisitor_Penance_timer;
    uint32 uiValroth_Smite_timer;

    void Reset()
    {
        uiRenew_timer = 1000;
        uiInquisitor_Penance_timer = 2000;
        uiValroth_Smite_timer = 1000;
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_VALROTH2, m_creature);
        DoCast(who, SPELL_VALROTH_SMITE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiRenew_timer < diff)
        {
            Shout();
            DoCast(m_creature, SPELL_RENEW);
            uiRenew_timer = 1000 + rand()%5000;
        }else uiRenew_timer -= diff;

        if (uiInquisitor_Penance_timer < diff)
        {
            Shout();
            DoCast(m_creature->getVictim(), SPELL_INQUISITOR_PENANCE);
            uiInquisitor_Penance_timer = 2000 + rand()%5000;
        }else uiInquisitor_Penance_timer -= diff;

        if (uiValroth_Smite_timer < diff)
        {
            Shout();
            DoCast(m_creature->getVictim(), SPELL_VALROTH_SMITE);
            uiValroth_Smite_timer = 1000 + rand()%5000;
        }else uiValroth_Smite_timer -= diff;

        DoMeleeAttackIfReady();
    }

    void Shout()
    {
        switch(rand()%20)
        {
            case 0: DoScriptText(SAY_VALROTH3, m_creature);break;
            case 1: DoScriptText(SAY_VALROTH4, m_creature);break;
            case 2: DoScriptText(SAY_VALROTH5, m_creature);break;
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_VALROTH6, m_creature);
        killer->CastSpell(m_creature, SPELL_SUMMON_VALROTH_REMAINS, true);
    }
};

CreatureAI* GetAI_mob_high_inquisitor_valroth(Creature* pCreature)
{
    return new mob_high_inquisitor_valrothAI (pCreature);
}

/*######
## Npc Highlord Darion Mograine
######*/
enum mograine
{
    ENCOUNTER_DK_NUMBER               = 5,  // how many player queue to start the quest , or -
    ENCOUNTER_DK_TIMER                = 10, // *every 5 minutes. These have to be done in instance data
    ENCOUNTER_DEFENDER_NUMBER         = 20, // how many of defender
    ENCOUNTER_EARTHSHATTER_NUMBER     = 20, // how many of earthshatter
    ENCOUNTER_ABOMINATION_NUMBER      = 3,  // how many of abomination
    ENCOUNTER_BEHEMOTH_NUMBER         = 2,  // how many of behemoth
    ENCOUNTER_GHOUL_NUMBER            = 10, // how many of ghoul
    ENCOUNTER_WARRIOR_NUMBER          = 2,  // how many of warrior
    ENCOUNTER_TOTAL_DAWN              = 300,  // Total number
    ENCOUNTER_TOTAL_SCOURGE           = 10000,

    WORLD_STATE_REMAINS               = 3592,
    WORLD_STATE_COUNTDOWN             = 3603,
    WORLD_STATE_EVENT_BEGIN           = 3605,

    SAY_LIGHT_OF_DAWN01               = -1609901, // pre text
    SAY_LIGHT_OF_DAWN02               = -1609902,
    SAY_LIGHT_OF_DAWN03               = -1609903,
    SAY_LIGHT_OF_DAWN04               = -1609904, // intro
    SAY_LIGHT_OF_DAWN05               = -1609905,
    SAY_LIGHT_OF_DAWN06               = -1609906,
    SAY_LIGHT_OF_DAWN07               = -1609907, // During the fight - Korfax, Champion of the Light
    SAY_LIGHT_OF_DAWN08               = -1609908, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN09               = -1609909, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN10               = -1609910,
    SAY_LIGHT_OF_DAWN11               = -1609911,
    SAY_LIGHT_OF_DAWN12               = -1609912,
    SAY_LIGHT_OF_DAWN13               = -1609913,
    SAY_LIGHT_OF_DAWN14               = -1609914,
    SAY_LIGHT_OF_DAWN15               = -1609915,
    SAY_LIGHT_OF_DAWN16               = -1609916,
    SAY_LIGHT_OF_DAWN17               = -1609917,
    SAY_LIGHT_OF_DAWN18               = -1609918,
    SAY_LIGHT_OF_DAWN19               = -1609919,
    SAY_LIGHT_OF_DAWN20               = -1609920,
    SAY_LIGHT_OF_DAWN21               = -1609921,
    SAY_LIGHT_OF_DAWN22               = -1609922,
    SAY_LIGHT_OF_DAWN23               = -1609923,
    SAY_LIGHT_OF_DAWN24               = -1609924,
    SAY_LIGHT_OF_DAWN25               = -1609925, // After the fight
    SAY_LIGHT_OF_DAWN26               = -1609926, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN27               = -1609927, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN28               = -1609928, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN29               = -1609929, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN30               = -1609930, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN31               = -1609931, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN32               = -1609932, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN33               = -1609933, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN34               = -1609934, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN35               = -1609935, // Darion Mograine
    SAY_LIGHT_OF_DAWN36               = -1609936, // Darion Mograine
    SAY_LIGHT_OF_DAWN37               = -1609937, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN38               = -1609938, // Darion Mograine
    SAY_LIGHT_OF_DAWN39               = -1609939, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN40               = -1609940, // Darion Mograine
    SAY_LIGHT_OF_DAWN41               = -1609941, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN42               = -1609942, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN43               = -1609943, // The Lich King
    SAY_LIGHT_OF_DAWN44               = -1609944, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN45               = -1609945, // The Lich King
    SAY_LIGHT_OF_DAWN46               = -1609946, // The Lich King
    SAY_LIGHT_OF_DAWN47               = -1609947, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN48               = -1609948, // The Lich King
    SAY_LIGHT_OF_DAWN49               = -1609949, // The Lich King
    SAY_LIGHT_OF_DAWN50               = -1609950, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN51               = -1609951, // The Lich King
    SAY_LIGHT_OF_DAWN52               = -1609952, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN53               = -1609953, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN54               = -1609954, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN55               = -1609955, // The Lich King
    SAY_LIGHT_OF_DAWN56               = -1609956, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN57               = -1609957, // The Lich King
    SAY_LIGHT_OF_DAWN58               = -1609958, // The Lich King
    SAY_LIGHT_OF_DAWN59               = -1609959, // The Lich King
    SAY_LIGHT_OF_DAWN60               = -1609960, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN61               = -1609961, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN62               = -1609962, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN63               = -1609963, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN64               = -1609964, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN65               = -1609965, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN66               = -1609966, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN67               = -1609967, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN68               = -1609968, // Highlord Darion Mograine

    EMOTE_LIGHT_OF_DAWN01             = -1609969,  // Emotes
    EMOTE_LIGHT_OF_DAWN02             = -1609970,
    EMOTE_LIGHT_OF_DAWN03             = -1609971,
    EMOTE_LIGHT_OF_DAWN04             = -1609972,
    EMOTE_LIGHT_OF_DAWN05             = -1609973,
    EMOTE_LIGHT_OF_DAWN06             = -1609974,
    EMOTE_LIGHT_OF_DAWN07             = -1609975,
    EMOTE_LIGHT_OF_DAWN08             = -1609976,
    EMOTE_LIGHT_OF_DAWN09             = -1609977,
    EMOTE_LIGHT_OF_DAWN10             = -1609978,
    EMOTE_LIGHT_OF_DAWN11             = -1609979,
    EMOTE_LIGHT_OF_DAWN12             = -1609980,
    EMOTE_LIGHT_OF_DAWN13             = -1609981,
    EMOTE_LIGHT_OF_DAWN14             = -1609982,
    EMOTE_LIGHT_OF_DAWN15             = -1609983,
    EMOTE_LIGHT_OF_DAWN16             = -1609984,
    EMOTE_LIGHT_OF_DAWN17             = -1609985,
    EMOTE_LIGHT_OF_DAWN18             = -1609986,

    GO_LIGHT_OF_DAWN                  = 191330,
    SPELL_THE_LIGHT_OF_DAWN_Q         = 53606, // quest credit

    // ---- Dark Knight npc --------------------
    // Highlord Darion Mograine
    NPC_HIGHLORD_DARION_MOGRAINE      = 29173,
    SPELL_ANTI_MAGIC_ZONE1            = 52893,
    SPELL_DEATH_STRIKE                = 53639,
    SPELL_DEATH_EMBRACE               = 53635,
    SPELL_ICY_TOUCH1                  = 49723,
    SPELL_THE_LIGHT_OF_DAWN           = 53658,
    SPELL_THE_MIGHT_OF_MOGRAINE       = 53642, // on players when begins
    SPELL_UNHOLY_BLIGHT               = 53640,
    SPELL_ALEXANDROS_MOGRAINE_SPAWN   = 53667,
    SPELL_MOGRAINE_CHARGE             = 53679,
    SPELL_ASHBRINGER                  = 53701,

    // Koltira Deathweaver & Orbaz Bloodbane are using the same abilities
    NPC_KOLTIRA_DEATHWEAVER           = 29199,
    NPC_ORBAZ_BLOODBANE               = 29204, // this guy fleed
    NPC_THASSARIAN                    = 29200, // he also does SPELL_THE_LIGHT_OF_DAWN 53658
    //SPELL_BLOOD_STRIKE                = 52374,
    SPELL_DEATH_GRIP                  = 49576,
    //SPELL_ICY_TOUCH                   = 52372,
    SPELL_PLAGUE_STRIKE1              = 50668,
    // all do SPELL_HERO_AGGRO_AURA    53627

    // Lich King
    NPC_THE_LICH_KING                 = 29183, // show up at end
    SPELL_APOCALYPSE                  = 53210,
    SPELL_TELEPORT_VISUAL             = 52233,
    SPELL_SOUL_FEAST_ALEX             = 53677, // on Alexandros
    SPELL_SOUL_FEAST_TIRION           = 53685, // on Tirion
    SPELL_ICEBOUND_VISAGE             = 53274, // not sure what is it for
    SPELL_REBUKE                      = 53680,

    // others
    NPC_RAMPAGING_ABOMINATION         = 29186,
    SPELL_CLEAVE1                     = 53633,
    SPELL_SCOURGE_HOOK                = 50335,
    SPELL_SCOURGE_AGGRO_AURA          = 53624,

    NPC_FLESH_BEHEMOTH                = 29190, // giant guy
    SPELL_STOMP                       = 53634,
    SPELL_THUNDERCLAP                 = 36706,
    SPELL_HERO_AGGRO_AURA             = 53627,

    NPC_ACHERUS_GHOUL                 = 29219, // just ghoul....
    SPELL_GHOULPLOSION                = 53632,

    NPC_WARRIOR_OF_THE_FROZEN_WASTES  = 29206, // use SPELL_CLEAVE 53631

    NPC_HIGHLORD_ALEXANDROS_MOGRAINE  = 29227, // ghost
    NPC_DARION_MOGRAINE               = 29228, // ghost

    // ---- Dawn npc --------------------
    // Highlord Tirion Fordring
    NPC_HIGHLORD_TIRION_FORDRING      = 29175,
    EQUIP_HIGHLORD_TIRION_FORDRING    = 13262,
    SPELL_LAY_ON_HANDS                = 53778,
    SPELL_REBIRTH_OF_THE_ASHBRINGER   = 53702,
    SPELL_TIRION_CHARGE               = 53705,
    SPELL_TIRION_CHARGE_VISUAL        = 53706,

    // others
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT  = 29176,
    SPELL_CLEAVE                      = 53631,
    SPELL_HEROIC_LEAP                 = 53625,

    NPC_LORD_MAXWELL_TYROSUS          = 29178,
    NPC_LEONID_BARTHALOMEW_THE_REVERED = 29179,
    NPC_DUKE_NICHOLAS_ZVERENHOFF      = 29180,

    NPC_COMMANDER_ELIGOR_DAWNBRINGER  = 29177,
    SPELL_HOLY_LIGHT2                 = 37979,

    NPC_RAYNE                         = 29181,
    SPELL_REJUVENATION                = 20664,
    SPELL_STARFALL                    = 20678,
    SPELL_TRANQUILITY                 = 25817,
    SPELL_WRATH                       = 21807,

    NPC_DEFENDER_OF_THE_LIGHT         = 29174, // also does SPELL_HEROIC_LEAP 53625
    SPELL_HOLY_LIGHT1                 = 29427,
    SPELL_HOLY_STRIKE                 = 53643,
    SPELL_HOLY_WRATH                  = 53638,
    SPELL_UPPERCUT                    = 53629,

    NPC_RIMBLAT_EARTHSHATTER          = 29182,
    SPELL_CHAIN_HEAL                  = 33642,
    SPELL_THUNDER                     = 53630
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

static Locations LightofDawnLoc[]=
{
    {2281.335, -5300.409, 85.170, 0},     // 0 Tirion Fordring loc
    {2283.896, -5287.914, 83.066, 1.55},  // 1 Tirion Fordring loc2
    {2281.461, -5263.014, 81.164, 0},     // 2 Tirion charges
    {2262.277, -5293.477, 82.167, 0},     // 3 Tirion run
    {2270.286, -5287.73, 82.262, 0},      // 4 Tirion relocate
    {2269.511, -5288.289, 82.225, 0},     // 5 Tirion forward
    {2262.277, -5293.477, 82.167, 0},     // 6 Tirion runs to Darion
    {2270.286, -5287.73, 82.262, 0},
    {2269.511, -5288.289, 82.225, 0},
    {2273.205, -5288.848, 82.617, 0},     // 9 Korfax loc1
    {2274.739, -5287.926, 82.684, 0},     // 10 Korfax loc2
    {2253.673, -5318.004, 81.724, 0},     // 11 Korfax kicked
    {2287.028, -5309.644, 87.253, 0},     // 12 Maxwell loc1
    {2286.978, -5308.025, 86.83, 0},      // 13 Maxwell loc2
    {2248.877, -5307.586, 82.166, 0},     // 14 maxwell kicked
    {2278.58, -5316.933, 88.319, 0},      // 15 Eligor loc1
    {2278.535, -5315.479, 88.08, 0},      // 16 Eligor loc2
    {2259.416, -5304.505, 82.149, 0},     // 17 eligor kicked
    {2289.259, -5280.355, 82.112, 0},     // 18 Koltira loc1
    {2289.02, -5281.985, 82.207, 0},      // 19 Koltira loc2
    {2273.289, -5273.675, 81.701, 0},     // 20 Thassarian loc1
    {2273.332, -5275.544, 81.849, 0},     // 21 Thassarian loc2
    {2281.198, -5257.397, 80.224, 4.66},  // 22 Alexandros loc1
    {2281.156, -5259.934, 80.647, 0},     // 23 Alexandros loc2
    {2281.294, -5281.895, 82.445, 1.35},  // 24 Darion loc1
    {2281.093, -5263.013, 81.125, 0},     // 25 Darion loc1
    {2281.313, -5250.282, 79.322, 4.69},  // 26 Lich King spawns
    {2281.523, -5261.058, 80.877, 0},     // 27 Lich king move forwards
    {2272.709, -5255.552, 78.226, 0},     // 28 Lich king kicked
    {2273.972, -5257.676, 78.862, 0}      // 29 Lich king moves forward
};

struct MANGOS_DLL_DECL npc_highlord_darion_mograineAI : public npc_escortAI
{
    npc_highlord_darion_mograineAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    bool bIsBattle;
    uint32 uiStep;
    uint32 uiPhase_timer;
    uint32 uiFight_duration;
    uint32 uiTotal_dawn;
    uint32 uiTotal_scourge;
    uint32 uiSummon_counter;

    // Darion Mograine
    uint32 uiAnti_magic_zone;
    uint32 uiDeath_strike;
    uint32 uiDeath_embrace;
    uint32 uiIcy_touch;
    uint32 uiUnholy_blight;
    uint32 uiFight_speech;
    uint32 uiSpawncheck;
    uint32 uiTargetcheck;

    // Dawn
    uint64 uiTirionGUID;
    uint64 uiAlexandrosGUID;
    uint64 uiDarionGUID;
    uint64 uiKorfaxGUID;
    uint64 uiMaxwellGUID;
    uint64 uiEligorGUID;
    uint64 uiRayneGUID;
    uint64 uiDefenderGUID[ENCOUNTER_DEFENDER_NUMBER];
    uint64 uiEarthshatterGUID[ENCOUNTER_EARTHSHATTER_NUMBER];

    // Death
    uint64 uiKoltiraGUID;
    uint64 uiOrbazGUID;
    uint64 uiThassarianGUID;
    uint64 uiLichKingGUID;
    uint64 uiAbominationGUID[ENCOUNTER_ABOMINATION_NUMBER];
    uint64 uiBehemothGUID[ENCOUNTER_BEHEMOTH_NUMBER];
    uint64 uiGhoulGUID[ENCOUNTER_GHOUL_NUMBER];
    uint64 uiWarriorGUID[ENCOUNTER_WARRIOR_NUMBER];

    // Misc
    uint64 uiDawnofLightGUID;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            bIsBattle = false;
            uiStep = 0;
            uiPhase_timer = 3000;
            uiFight_duration = 300000; // 5 minutes
            uiTotal_dawn = ENCOUNTER_TOTAL_DAWN;
            uiTotal_scourge = ENCOUNTER_TOTAL_SCOURGE;
            uiSummon_counter = 0;

            uiDawnofLightGUID = 0;

            uiAnti_magic_zone = 1000 + rand()%5000;
            uiDeath_strike = 5000 + rand()%5000;
            uiDeath_embrace = 5000 + rand()%5000;
            uiIcy_touch = 5000 + rand()%5000;
            uiUnholy_blight = 5000 + rand()%5000;

            uiFight_speech = 15000;
            uiSpawncheck = 1000;
            uiTargetcheck = 10000;

            ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->Mount(25279);

            //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_REMAINS, 0);
            //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 0);
            //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_EVENT_BEGIN, 0);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiRayneGUID)))
                pTemp->setDeathState(JUST_DIED);

            uiTirionGUID = NULL;
            uiKorfaxGUID = NULL;
            uiMaxwellGUID = NULL;
            uiEligorGUID = NULL;
            uiRayneGUID = NULL;

            for(uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDefenderGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiDefenderGUID[i] = 0;
            }
            for(uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEarthshatterGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiEarthshatterGUID[i] = 0;
            }

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiOrbazGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                pTemp->setDeathState(JUST_DIED);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                pTemp->setDeathState(JUST_DIED);

            uiKoltiraGUID = NULL;
            uiOrbazGUID = NULL;
            uiThassarianGUID = NULL;
            uiLichKingGUID = NULL;
            for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAbominationGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiAbominationGUID[i] = 0;
            }
            for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiBehemothGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiBehemothGUID[i] = 0;
            }
            for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiGhoulGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiGhoulGUID[i] = 0;
            }
            for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
            {
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiWarriorGUID[i])))
                    pTemp->setDeathState(JUST_DIED);
                uiWarriorGUID[i] = 0;
            }
        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (who == m_creature)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->AddThreat(who, 0.0f);
            m_creature->SetInCombatWith(who);
            who->SetInCombatWith(m_creature);
            DoStartMovement(who);
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && m_creature->IsHostileTo(who))
            if (m_creature->IsWithinDistInMap(who, 20) && m_creature->IsWithinLOSInMap(who))
                AttackStart(who);
    }

    void SetHoldState(bool bOnHold)
    {
        SetEscortPaused(bOnHold);
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
            case 0:
                m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                SetHoldState(true);
                break;
            case 1:
                SetHoldState(true);

                if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_LIGHT_OF_DAWN, 100.0f)) // make dawn of light effect off
                {
                    uiDawnofLightGUID = pGo->GetGUID();
                    pGo->SetPhaseMask(0, true);
                }

                SpawnNPC();
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                    DoScriptText(SAY_LIGHT_OF_DAWN07, pTemp);
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                    DoScriptText(SAY_LIGHT_OF_DAWN08, pTemp);

                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    NPCChangeTarget(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    NPCChangeTarget(uiWarriorGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    NPCChangeTarget(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    NPCChangeTarget(uiBehemothGUID[i]);
                NPCChangeTarget(uiKoltiraGUID);
                NPCChangeTarget(uiOrbazGUID);
                NPCChangeTarget(uiThassarianGUID);

                m_creature->Unmount();
                m_creature->CastSpell(m_creature, SPELL_THE_MIGHT_OF_MOGRAINE, true); // need to fix, on player only

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                    pTemp->Unmount();
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                    pTemp->Unmount();

                bIsBattle = true;
                break;
            case 2:
                m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                DoCast(m_creature, SPELL_THE_LIGHT_OF_DAWN);
                break;
            case 3:
            {
                Creature* pTirion = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID));

                DoScriptText(EMOTE_LIGHT_OF_DAWN05, m_creature);
                if (m_creature->HasAura(SPELL_THE_LIGHT_OF_DAWN, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                {
                    if (pTemp->HasAura(SPELL_THE_LIGHT_OF_DAWN, 0))
                        pTemp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[19].x, LightofDawnLoc[19].y, LightofDawnLoc[19].z);
                }
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                {
                    if (pTemp->HasAura(SPELL_THE_LIGHT_OF_DAWN, 0))
                        pTemp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[21].x, LightofDawnLoc[21].y, LightofDawnLoc[21].z);
                }
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                {
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[10].x, LightofDawnLoc[10].y, LightofDawnLoc[10].z);
                }
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                {
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[13].x, LightofDawnLoc[13].y, LightofDawnLoc[13].z);
                }
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                {
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[16].x, LightofDawnLoc[16].y, LightofDawnLoc[16].z);
                }
                JumpToNextStep(10000);
            } break;
            case 4:
                DoScriptText(SAY_LIGHT_OF_DAWN27, m_creature);
                ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                    pTemp->SetStandState(UNIT_STAND_STATE_KNEEL);
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                    pTemp->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetHoldState(true);
                break;
            case 5:
                DoScriptText(SAY_LIGHT_OF_DAWN33, m_creature);
                SetHoldState(true);
                break;
            case 6:
                SetHoldState(true);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPECIALATTACK1H);
                JumpToNextStep(1000);
                break;
            case 7:
                SetHoldState(true);
                JumpToNextStep(2000);
                break;
            case 8:
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_UNEQUIP));
                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                    m_creature->CastSpell(pTemp, SPELL_ASHBRINGER, true);
                DoScriptText(EMOTE_LIGHT_OF_DAWN14, m_creature);
                SetHoldState(true);
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!bIsBattle)
        {
            if (uiPhase_timer < diff)
            {
                // ******* Before battle *****************************************************************
                switch(uiStep)
                {
                    case 0:  // countdown
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 1);
                        break;

                    case 1:  // just delay
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_REMAINS, 1);
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 0);
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_EVENT_BEGIN, 1);
                        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(3000);
                        break;

                    case 2:
                        DoScriptText(SAY_LIGHT_OF_DAWN04, m_creature);
                        if (Creature* pKoltira = GetClosestCreatureWithEntry(m_creature, NPC_KOLTIRA_DEATHWEAVER, 50.0f))
                            uiKoltiraGUID = pKoltira->GetGUID();
                        if (Creature* pOrbaz = GetClosestCreatureWithEntry(m_creature, NPC_ORBAZ_BLOODBANE, 50.0f))
                            uiOrbazGUID = pOrbaz->GetGUID();
                        if (Creature* pThassarian = GetClosestCreatureWithEntry(m_creature, NPC_THASSARIAN, 50.0f))
                            uiThassarianGUID = pThassarian->GetGUID();
                        JumpToNextStep(10000);
                        break;

                    case 3: // rise
                        DoScriptText(SAY_LIGHT_OF_DAWN05, m_creature);
                        JumpToNextStep(3000);
                        break;

                    case 4: // summon ghoul
                        // Dunno whats the summon spell, so workaround
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_GHOUL_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_ACHERUS_GHOUL, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->setFaction(2084);
                                uiGhoulGUID[uiSummon_counter] = pTemp->GetGUID();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 5: // summon abomination
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_ABOMINATION_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_RAMPAGING_ABOMINATION, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->setFaction(2084);
                                uiAbominationGUID[uiSummon_counter] = pTemp->GetGUID();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 6: // summon warrior
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_WARRIOR_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->setFaction(2084);
                                uiWarriorGUID[uiSummon_counter] = pTemp->GetGUID();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 7: // summon warrior
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_BEHEMOTH_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_FLESH_BEHEMOTH, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->setFaction(2084);
                                uiBehemothGUID[uiSummon_counter] = pTemp->GetGUID();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 8: // summon announce
                        DoScriptText(SAY_LIGHT_OF_DAWN06, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 9: // charge begins
                        SetHoldState(false);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                        {
                            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiOrbazGUID)))
                        {
                            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                        {
                            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAbominationGUID[i])))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiBehemothGUID[i])))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiGhoulGUID[i])))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiWarriorGUID[i])))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        JumpToNextStep(5000);
                        break;

                    // ******* After battle *****************************************************************
                    case 11: // Tirion starts to speak
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN28, pTemp);
                        JumpToNextStep(21000);
                        break;

                    case 12:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN29, pTemp);
                        JumpToNextStep(13000);
                        break;

                    case 13:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN30, pTemp);
                        JumpToNextStep(13000);
                        break;

                    case 14:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN31, m_creature);
                        JumpToNextStep(7000);
                        break;

                    case 15: // summon gate
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, LightofDawnLoc[22].x, LightofDawnLoc[22].y, LightofDawnLoc[22].z, LightofDawnLoc[22].o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                        {
                            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->CastSpell(pTemp, SPELL_ALEXANDROS_MOGRAINE_SPAWN, true);
                            DoScriptText(EMOTE_LIGHT_OF_DAWN06, pTemp);
                            uiAlexandrosGUID = pTemp->GetGUID();
                        }
                        JumpToNextStep(4000);
                        break;

                    case 16: // Alexandros out
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                        {
                            pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[23].x, LightofDawnLoc[23].y, LightofDawnLoc[23].z);
                            DoScriptText(SAY_LIGHT_OF_DAWN32, pTemp);
                        }
                        SetHoldState(false); // makes darion turns back
                        JumpToNextStep(5000);
                        break;

                    case 17:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(SAY_LIGHT_OF_DAWN34, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 18: // Darion's spirit out
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_DARION_MOGRAINE, LightofDawnLoc[24].x, LightofDawnLoc[24].y, LightofDawnLoc[24].z, LightofDawnLoc[24].o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                        {
                            DoScriptText(SAY_LIGHT_OF_DAWN35, pTemp);
                            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            uiDarionGUID = pTemp->GetGUID();
                        }
                        JumpToNextStep(4000);
                        break;

                    case 19: // runs to father
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN07, pTemp);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[25].x, LightofDawnLoc[25].y, LightofDawnLoc[25].z);
                        }
                        JumpToNextStep(4000);
                        break;

                    case 20:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN36, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 21:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                            DoScriptText(EMOTE_LIGHT_OF_DAWN08, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 22:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN37, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 23:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN38, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 24:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN39, pTemp);

                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID))) // Tirion moves forward here
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[1].x, LightofDawnLoc[1].y, LightofDawnLoc[1].z);

                        JumpToNextStep(15000);
                        break;

                    case 25:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN40, pTemp);
                        JumpToNextStep(11000);
                        break;

                    case 26:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN41, pTemp);
                        JumpToNextStep(5000);
                        break;

                    case 27:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDarionGUID)))
                            pTemp->setDeathState(JUST_DIED);
                        JumpToNextStep(24000);
                        break;

                    case 28:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN42, pTemp);
                        JumpToNextStep(6000);
                        break;

                    case 29: // lich king spawns
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_THE_LICH_KING, LightofDawnLoc[26].x, LightofDawnLoc[26].y, LightofDawnLoc[26].z, LightofDawnLoc[26].o, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                        {
                            DoScriptText(SAY_LIGHT_OF_DAWN43, pTemp);
                            uiLichKingGUID = pTemp->GetGUID();
                            if (Creature* pAlex = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID)))
                                pTemp->CastSpell(pAlex, SPELL_SOUL_FEAST_ALEX, false);
                        }
                        JumpToNextStep(2000);
                        break;

                    case 30:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAlexandrosGUID))) // remove him
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN09, pTemp);
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                        {
                            pTemp->InterruptNonMeleeSpells(false);
                            DoScriptText(SAY_LIGHT_OF_DAWN45, pTemp);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 31:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(EMOTE_LIGHT_OF_DAWN10, m_creature);
                        DoScriptText(SAY_LIGHT_OF_DAWN44, m_creature);
                        JumpToNextStep(3000);
                        break;

                    case 32:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[27].x, LightofDawnLoc[27].y, LightofDawnLoc[27].z);
                        JumpToNextStep(6000);
                        break;

                    case 33: // Darion supports to jump to lich king here
// disable              if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
//  because mangos          DoCast(m_creature, SPELL_MOGRAINE_CHARGE); // jumping charge
//   doesn't make it looks well, so workarounds, Darion charges, looks better
                        m_creature->SetSpeedRate(MOVE_RUN, 3.0f);
                        m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        SetHoldState(false);
                        JumpToNextStep(0);
                        break;

                    case 35: // Lich king counterattacks
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                        {
                            pTemp->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                            DoScriptText(SAY_LIGHT_OF_DAWN46, pTemp);
                        }
                        m_creature->SetSpeedRate(MOVE_RUN, 6.0f);
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_DEAD);
                        SetHoldState(false); // Darion got kicked by lich king
                        JumpToNextStep(0);
                        break;

                    case 37: // Lich king counterattacks
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        JumpToNextStep(3000);
                        break;

                    case 38:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN47, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 39:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN48, pTemp);
                        JumpToNextStep(15000);
                        break;

                    case 40:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN49, pTemp);
                        JumpToNextStep(17000);
                        break;

                    case 41: // Lich king - Apocalypse
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN11, pTemp);
                            DoScriptText(SAY_LIGHT_OF_DAWN51, pTemp);
                            if (Creature* pTirion = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            {
                                ((Unit*)pTirion)->SetStandState(UNIT_STAND_STATE_KNEEL);
                                //pTemp->CastSpell(pTirion, SPELL_APOCALYPSE, false); // not working
                                pTemp->CastSpell(pTirion, SPELL_SOUL_FEAST_TIRION, false);
                                DoScriptText(EMOTE_LIGHT_OF_DAWN12, pTirion);
                            }
                        }
                        JumpToNextStep(2000);
                        break;

                    case 42: // Maxwell yells for attack
                        {
                            float fLichPositionX, fLichPositionY, fLichPositionZ;
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            {
                                fLichPositionX = pTemp->GetPositionX();
                                fLichPositionY = pTemp->GetPositionY();
                                fLichPositionZ = pTemp->GetPositionZ();
                            }

                            if (fLichPositionX && fLichPositionY)
                            {
                                Creature* pTemp;
                                if (pTemp = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000))
                                {
                                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                    pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                    pTemp->setFaction(m_creature->getFaction());
                                    pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                    uiDefenderGUID[0] = pTemp->GetGUID();
                                }

                                if (pTemp = m_creature->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10000))
                                {
                                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                    pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                    pTemp->setFaction(m_creature->getFaction());
                                    pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                    uiEarthshatterGUID[0] = pTemp->GetGUID();
                                }
                            }
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                DoScriptText(SAY_LIGHT_OF_DAWN50, pTemp);
                            }
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->HandleEmoteCommand(EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                            }
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                            }
                        }
                        JumpToNextStep(4500);
                        break;

                    case 43: // They all got kicked
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(EMOTE_LIGHT_OF_DAWN13, pTemp);

                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[14].x, LightofDawnLoc[14].y, LightofDawnLoc[14].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[11].x, LightofDawnLoc[11].y, LightofDawnLoc[11].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[17].x, LightofDawnLoc[17].y, LightofDawnLoc[17].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDefenderGUID[0])))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEarthshatterGUID[0])))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 44: // make them stand up
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        JumpToNextStep(1000);
                        break;

                    case 45:
                        DoScriptText(SAY_LIGHT_OF_DAWN52, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 46: // Darion stand up, "not today"
                        m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
                        m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN53, m_creature);
                        SetHoldState(false); // Darion throws sword
                        JumpToNextStep(7000);
                        break;

                    case 47: // Ashbringer rebirth
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(EMOTE_LIGHT_OF_DAWN15, m_creature);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                        {
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                            pTemp->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_HIGHLORD_TIRION_FORDRING));
                            pTemp->CastSpell(pTemp, SPELL_REBIRTH_OF_THE_ASHBRINGER, false);
                        }
                        JumpToNextStep(1000);
                        break;

                    case 48: // Show the cleansing effect (dawn of light)
                        if (GameObject* pGo = m_creature->GetMap()->GetGameObject(uiDawnofLightGUID))
                            pGo->SetPhaseMask(128, true);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                        {
                            if (pTemp->HasAura(SPELL_REBIRTH_OF_THE_ASHBRINGER, 0))
                                pTemp->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);
                            pTemp->CastSpell(pTemp, 41542, false); // workarounds, light expoded, makes it cool
                            pTemp->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        }
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            pTemp->InterruptNonMeleeSpells(false);
                        JumpToNextStep(2500);
                        break;

                    case 49:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN54, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 50:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN55, pTemp);
                        JumpToNextStep(5000);
                        break;

                    case 51:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN56, pTemp);
                        JumpToNextStep(1000);
                        break;

                    case 52: // Tiron charges
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN16, pTemp);
                            pTemp->CastSpell(pTemp, SPELL_TIRION_CHARGE, false); // jumping charge
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                            pTemp->SetSpeedRate(MOVE_RUN, 3.0f); // workarounds, make Tirion still running
                            pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[2].x, LightofDawnLoc[2].y, LightofDawnLoc[2].z);
                            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                                pTemp->GetMap()->CreatureRelocation(pTemp, LightofDawnLoc[28].x, LightofDawnLoc[28].y, LightofDawnLoc[28].z, 0.0f); // workarounds, he should kick back by Tirion, but here we relocate him
                        }
                        JumpToNextStep(1500);
                        break;

                    case 53:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN57, pTemp);
                        JumpToNextStep(1000);
                        break;

                    case 54:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 1.0f);
                            m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[29].x, LightofDawnLoc[29].y, LightofDawnLoc[29].z); // 26
                        }
                        JumpToNextStep(4000);
                        break;

                    case 55:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        JumpToNextStep(2000);
                        break;

                    case 56:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        JumpToNextStep(1500);
                        break;

                    case 57:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN58, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 58:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN59, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 59:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
                            pTemp->CastSpell(pTemp, SPELL_TELEPORT_VISUAL, false);
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID))) // Tirion runs to Darion
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 1.0f);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[6].x, LightofDawnLoc[6].y, LightofDawnLoc[6].z);
                        }
                        JumpToNextStep(2500);
                        break;

                    case 60:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID))) // Lich king disappears here
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN17, pTemp);
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        JumpToNextStep(10000);
                        break;

                    case 61:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN60, pTemp);
                        JumpToNextStep(3000);
                        break;

                    case 62:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                        {
                            pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[7].x, LightofDawnLoc[7].y, LightofDawnLoc[7].z);
                        }
                        JumpToNextStep(5500);
                        break;

                    case 63:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                        {
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[8].x, LightofDawnLoc[8].y, LightofDawnLoc[8].z);
                            DoScriptText(SAY_LIGHT_OF_DAWN61, pTemp);
                        }
                        JumpToNextStep(15000);
                        break;

                    case 64:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN62, pTemp);
                        JumpToNextStep(7000);
                        break;

                    case 65:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN63, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 66:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN64, pTemp);
                        JumpToNextStep(11000);
                        break;

                    case 67:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN65, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 68:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN66, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 69:
                        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                            DoScriptText(SAY_LIGHT_OF_DAWN67, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 70:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN68, m_creature);
                        JumpToNextStep(10000);
                        break;

                    case 71:
                        if (GameObject* pGo = m_creature->GetMap()->GetGameObject(uiDawnofLightGUID)) // Turn off dawn of light
                            pGo->SetPhaseMask(0, true);

                        {
                            Map *map = m_creature->GetMap(); // search players with in 50 yards for quest credit
                            Map::PlayerList const &PlayerList = map->GetPlayers();
                            if (!PlayerList.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (i->getSource()->isAlive() && m_creature->IsWithinDistInMap(i->getSource(), 50))
                                        i->getSource()->CastSpell(i->getSource(), SPELL_THE_LIGHT_OF_DAWN_Q, false);
                            }
                        }
                        m_creature->SetVisibility(VISIBILITY_OFF); // respawns another Darion for quest turn in
                        m_creature->SummonCreature(NPC_HIGHLORD_DARION_MOGRAINE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 180000);
                        JumpToNextStep(1000);
                        break;

                    case 72:
                        SetHoldState(false); // Escort ends
                        JumpToNextStep(0);
                        break;
                }

            }else uiPhase_timer -= diff;
        }

        // ******* During battle *****************************************************************
        else
        {
            if (uiAnti_magic_zone < diff)
            {
                DoCast(m_creature, SPELL_ANTI_MAGIC_ZONE1);
                uiAnti_magic_zone = 25000 + rand()%5000;
            }else uiAnti_magic_zone -= diff;

            if (uiDeath_strike < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_DEATH_STRIKE);
                uiDeath_strike = 5000 + rand()%5000;
            }else uiDeath_strike -= diff;

            if (uiDeath_embrace < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_DEATH_EMBRACE);
                uiDeath_embrace = 5000 + rand()%5000;
            }else uiDeath_embrace -= diff;

            if (uiIcy_touch < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_ICY_TOUCH1);
                uiIcy_touch = 5000 + rand()%5000;
            }else uiIcy_touch -= diff;

            if (uiUnholy_blight < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_UNHOLY_BLIGHT);
                uiUnholy_blight = 5000 + rand()%5000;
            }else uiUnholy_blight -= diff;

            if (uiFight_speech < diff)
            {
                switch(rand()%15)
                {
                    case 0: DoScriptText(SAY_LIGHT_OF_DAWN09, m_creature);break;
                    case 1: DoScriptText(SAY_LIGHT_OF_DAWN10, m_creature);break;
                    case 2: DoScriptText(SAY_LIGHT_OF_DAWN11, m_creature);break;
                    case 3: DoScriptText(SAY_LIGHT_OF_DAWN12, m_creature);break;
                    case 4: DoScriptText(SAY_LIGHT_OF_DAWN13, m_creature);break;
                    case 5: DoScriptText(SAY_LIGHT_OF_DAWN14, m_creature);break;
                    case 6: DoScriptText(SAY_LIGHT_OF_DAWN15, m_creature);break;
                    case 7: DoScriptText(SAY_LIGHT_OF_DAWN16, m_creature);break;
                    case 8: DoScriptText(SAY_LIGHT_OF_DAWN17, m_creature);break;
                    case 9: DoScriptText(SAY_LIGHT_OF_DAWN18, m_creature);break;
                    case 10: DoScriptText(SAY_LIGHT_OF_DAWN19, m_creature);break;
                    case 11: DoScriptText(SAY_LIGHT_OF_DAWN20, m_creature);break;
                    case 12: DoScriptText(SAY_LIGHT_OF_DAWN21, m_creature);break;
                    case 13: DoScriptText(SAY_LIGHT_OF_DAWN22, m_creature);break;
                    case 14: DoScriptText(SAY_LIGHT_OF_DAWN23, m_creature);break;
                    case 15: DoScriptText(SAY_LIGHT_OF_DAWN24, m_creature);break;
                }
                uiFight_speech = 15000 + rand()%5000;
            }else uiFight_speech -= diff;

            // Check spawns
            if (uiSpawncheck < diff)
            {
                SpawnNPC();
                uiSpawncheck = 1000;
            }else uiSpawncheck -= diff;

            // Check targets
            if (uiTargetcheck < diff)
            {
                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    NPCChangeTarget(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    NPCChangeTarget(uiWarriorGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    NPCChangeTarget(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    NPCChangeTarget(uiBehemothGUID[i]);
                NPCChangeTarget(uiKoltiraGUID);
                NPCChangeTarget(uiOrbazGUID);
                NPCChangeTarget(uiThassarianGUID);

                uiTargetcheck = 10000;
            }else uiTargetcheck -= diff;

            // Battle end
            if (uiFight_duration < diff + 5000)
            {
                if (!uiTirionGUID)
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, LightofDawnLoc[0].x, LightofDawnLoc[0].y, LightofDawnLoc[0].z, 1.528, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000))
                    {
                        pTemp->setFaction(m_creature->getFaction());
                        pTemp->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_UNEQUIP));
                        DoScriptText(SAY_LIGHT_OF_DAWN25, pTemp);
                        uiTirionGUID = pTemp->GetGUID();
                    }
            }
            if (uiFight_duration < diff)
            {
                bIsBattle = false;
                uiFight_duration = 300000;

                if (m_creature->HasAura(SPELL_THE_MIGHT_OF_MOGRAINE, 0))
                    m_creature->RemoveAurasDueToSpell(SPELL_THE_MIGHT_OF_MOGRAINE);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

                for(uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
                    DespawnNPC(uiDefenderGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
                    DespawnNPC(uiEarthshatterGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    DespawnNPC(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    DespawnNPC(uiBehemothGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    DespawnNPC(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    DespawnNPC(uiWarriorGUID[i]);

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID)))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[9].x, LightofDawnLoc[9].y, LightofDawnLoc[9].z);
                }

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID)))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[12].x, LightofDawnLoc[12].y, LightofDawnLoc[12].z);
                }

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID)))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[15].x, LightofDawnLoc[15].y, LightofDawnLoc[15].z);
                }
                DespawnNPC(uiRayneGUID);

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKoltiraGUID)))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[18].x, LightofDawnLoc[18].y, LightofDawnLoc[18].z);
                    pTemp->CastSpell(pTemp, SPELL_THE_LIGHT_OF_DAWN, false);
                }

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiOrbazGUID)))
                    DoScriptText(EMOTE_LIGHT_OF_DAWN04, pTemp);

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiThassarianGUID)))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[20].x, LightofDawnLoc[20].y, LightofDawnLoc[20].z);
                    pTemp->CastSpell(pTemp, SPELL_THE_LIGHT_OF_DAWN, false);
                }

                if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiTirionGUID)))
                    DoScriptText(SAY_LIGHT_OF_DAWN26, pTemp);

                SetHoldState(false);

            }else uiFight_duration -= diff;

            DoMeleeAttackIfReady();
        }
    }

    void JumpToNextStep(uint32 uiTimer)
    {
        uiPhase_timer = uiTimer;
        uiStep++;
    }

    void NPCChangeTarget(uint64 ui_GUID)
    {
        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), ui_GUID)))
            if (pTemp->isAlive())
                if (Unit* pTarger = SelectUnit(SELECT_TARGET_RANDOM,0))
                    if (pTarger->isAlive())
                    {
                        ((Creature*)pTemp)->AddThreat(pTarger, 0.0f);
                        ((Creature*)pTemp)->AI()->AttackStart(pTarger);
                        ((Creature*)pTemp)->SetInCombatWith(pTarger);
                        pTarger->SetInCombatWith(pTemp);
                    }
    }

    void SpawnNPC()
    {
        Creature* pTemp = NULL;

        // Death
        for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiGhoulGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_ACHERUS_GHOUL, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiGhoulGUID[i] = pTemp->GetGUID();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiAbominationGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiAbominationGUID[i] = pTemp->GetGUID();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiWarriorGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_RAMPAGING_ABOMINATION, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiWarriorGUID[i] = pTemp->GetGUID();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiBehemothGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_FLESH_BEHEMOTH, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiBehemothGUID[i] = pTemp->GetGUID();
            }
        }

        // Dawn
        for(uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiDefenderGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2089);
                m_creature->AddThreat(pTemp, 0.0f);
                uiDefenderGUID[i] = pTemp->GetGUID();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
        {
            if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEarthshatterGUID[i]))))
            {
                pTemp = m_creature->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2089);
                m_creature->AddThreat(pTemp, 0.0f);
                uiEarthshatterGUID[i] = pTemp->GetGUID();
            }
        }
        if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiKorfaxGUID))))
        {
            pTemp = m_creature->SummonCreature(NPC_KORFAX_CHAMPION_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiKorfaxGUID = pTemp->GetGUID();
        }
        if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiMaxwellGUID))))
        {
            pTemp = m_creature->SummonCreature(NPC_LORD_MAXWELL_TYROSUS, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiMaxwellGUID = pTemp->GetGUID();
        }
        if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiEligorGUID))))
        {
            pTemp = m_creature->SummonCreature(NPC_COMMANDER_ELIGOR_DAWNBRINGER, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiEligorGUID = pTemp->GetGUID();
        }
        if (!(pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiRayneGUID))))
        {
            pTemp = m_creature->SummonCreature(NPC_RAYNE, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiRayneGUID = pTemp->GetGUID();
        }
    }

    void DespawnNPC(uint64 pGUID)
    {
        if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), pGUID)))
            if (pTemp->isAlive())
            {
                pTemp->SetVisibility(VISIBILITY_OFF);
                pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
    }
};

bool GossipHello_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu( pCreature->GetGUID() );

    if (pPlayer->GetQuestStatus(12801) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM( 0, "I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            ((npc_highlord_darion_mograineAI*)pCreature->AI())->uiStep = 1;
            ((npc_highlord_darion_mograineAI*)pCreature->AI())->Start(true, false, pPlayer->GetGUID());
            break;
    }
    return true;
}

CreatureAI* GetAI_npc_highlord_darion_mograine(Creature* pCreature)
{
    npc_highlord_darion_mograineAI* tempAI = new npc_highlord_darion_mograineAI(pCreature);

    //tempAI->FillPointMovementListForCreature();

    return (CreatureAI*)tempAI;
}

/*######
## npc the lich king in dawn of light
######*/
struct MANGOS_DLL_DECL npc_the_lich_king_tirion_dawnAI : public ScriptedAI
{
    npc_the_lich_king_tirion_dawnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }
    void Reset() {}
    void AttackStart(Unit *who) { return; } // very sample, just don't make them aggreesive
    void UpdateAI(const uint32 diff) { return; }
    void JustDied(Unit* killer) {}
};

CreatureAI* GetAI_npc_the_lich_king_tirion_dawn(Creature* pCreature)
{
    return new npc_the_lich_king_tirion_dawnAI (pCreature);
}

void AddSC_DK_Quest()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_scarlet_miner";
    newscript->GetAI = &GetAI_npc_scarlet_miner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scarlet_defender";
    newscript->GetAI = &GetAI_npc_scarlet_defender;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mayor_quimby";
    newscript->GetAI = &GetAI_npc_mayor_quimby;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scarlet_crusade_soldier";
    newscript->GetAI = &GetAI_npc_scarlet_crusade_soldier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_citizen_of_new_avalon";
    newscript->GetAI = &GetAI_npc_citizen_of_new_avalon;
    newscript->RegisterSelf();

/* Script in ebon_hold.cpp
    newscript = new Script;
    newscript->Name = "npc_lady_eonys";
    newscript->GetAI = &GetAI_npc_lady_eonys;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_valok_the_righteous";
    newscript->GetAI = &GetAI_npc_valok_the_righteous;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_donovan_pulfrost";
    newscript->GetAI = &GetAI_npc_donovan_pulfrost;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_antoine_brack";
    newscript->GetAI = &GetAI_npc_antoine_brack;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_goby_blastenheimer";
    newscript->GetAI = &GetAI_npc_goby_blastenheimer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ellen_stanbridge";
    newscript->GetAI = &GetAI_npc_ellen_stanbridge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_yazmina_oakenthorn";
    newscript->GetAI = &GetAI_npc_yazmina_oakenthorn;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kug_ironjaw";
    newscript->GetAI = &GetAI_npc_kug_ironjaw;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_malar_bravehorn";
    newscript->GetAI = &GetAI_npc_malar_bravehorn;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_iggy_darktusk";
    newscript->GetAI = &GetAI_npc_iggy_darktusk;
    newscript->RegisterSelf();
*/

    newscript = new Script;
    newscript->Name = "npc_scarlet_soldier";
    newscript->GetAI = &GetAI_npc_scarlet_soldier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scarlet_ballista";
    newscript->GetAI = &GetAI_npc_scarlet_ballista;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_persuade_scarlet";
    newscript->GetAI = &GetAI_npc_persuade_scarlet;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_patchwerk";
    newscript->GetAI = &GetAI_npc_patchwerk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scourge";
    newscript->GetAI = &GetAI_npc_scourge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_plague_cauldron";
    newscript->pGOHello = &GOHello_go_plague_cauldron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_courier_controller";
    newscript->GetAI = &GetAI_mob_scarlet_courier_controller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_courier";
    newscript->GetAI = &GetAI_mob_scarlet_courier;
    newscript->RegisterSelf();

    /*
    newscript = new Script;
    newscript->Name = "npc_koltira_deathweaver";
    newscript->GetAI = &GetAI_npc_koltira_deathweaver;
    newscript->pQuestAccept = &QuestAccept_npc_koltira_deathweaver;
    newscript->RegisterSelf();
    */
    newscript = new Script;
    newscript->Name = "mob_high_inquisitor_valroth";
    newscript->GetAI = &GetAI_mob_high_inquisitor_valroth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_highlord_darion_mograine";
    newscript->GetAI = &GetAI_npc_highlord_darion_mograine;
    newscript->pGossipHello =  &GossipHello_npc_highlord_darion_mograine;
    newscript->pGossipSelect = &GossipSelect_npc_highlord_darion_mograine;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_the_lich_king_tirion_dawn";
    newscript->GetAI = &GetAI_npc_the_lich_king_tirion_dawn;
    newscript->RegisterSelf();
}
