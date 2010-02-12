  // ************************************//
 // Phoenix Development Team May, 2009  //
//*************************************//

//Purpose: To allow the DK's quest chains to happen. There is a mess of crap in this 1 file... 
//but it's overall purpose is to make the DK starting area work right.

#include "precompiled.h"

#define QUEST_GRAND_THEFT_PALOMINO 12680
#define	QUEST_INTO_THE_REALM_OF_SHADOWS 12687

#define SPELL_REALM_OF_SHADOWS 52275

//Quest: Death Comes On High
#define EYE_OF_ACHERUS 28511

struct MANGOS_DLL_DECL npc_eye_of_acherusAI : public ScriptedAI
{

    npc_eye_of_acherusAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        stage=0;
        pOwner=NULL;
    }

    void JustRespawned() { //called on spawn
        m_creature->SetMonsterMoveFlags(MONSTER_MOVE_SPLINE_FLY);
        // allow fly
        DoCast(m_creature,51923,true);
    }

    Unit* pOwner;
    uint8 stage;
    //0 = start
    //1 = flying twoards the place
    //2 = player controled doing quest

    int Permissible(const Creature *creature)
    {
        if( creature->GetCharmerGUID())
            return PERMIT_BASE_SPECIAL;

        return PERMIT_BASE_NO;
    }
    
    void MovementInform(uint32 MovementType, uint32 Data)
    {
        if (!pOwner)
            return;
        
        if(MovementType=POINT_MOTION_TYPE && Data==0)
        {
            m_creature->MonsterWhisper("The Eye of Acherus is now in your control",pOwner->GetGUID(),true);
            m_creature->CastSpell(m_creature,51890,true);
            ((Player*)pOwner)->SetClientControl(m_creature,true);
            m_creature->SetSpeedRate(MOVE_WALK,4,true);
            stage=2;
        }
    }

    void MoveInLineOfSight(Unit* u)
    { //if it's visible to the possessed unit, make it visible to the player
        if (pOwner)
            ((Player*)pOwner)->UpdateVisibilityOf(m_creature, u);
    }

    void JustDied(Unit *)
    {
        if (!pOwner)
            return;

        pOwner->InterruptSpell(CURRENT_CHANNELED_SPELL);
        pOwner->RemoveAurasDueToSpell(m_creature->GetUInt32Value(UNIT_CREATED_BY_SPELL));

    }
    
    void UpdateAI(const uint32 diff)
    {
        if(!pOwner)
        {
            pOwner = m_creature->GetCharmerOrOwner();
        }

        if(!pOwner || pOwner->GetTypeId()!=TYPEID_PLAYER)
            return;

        switch(stage)
        {
            case 0:
                m_creature->MonsterWhisper("The Eye of Acherus launches towards its destination",pOwner->GetGUID(),true);
                ((Player*)pOwner)->SetClientControl(m_creature,false);
                m_creature->SetSpeedRate(MOVE_WALK,8,true);
                m_creature->GetMotionMaster()->MovePoint(0, 1780,-5882,149); //(real destination)
                //m_creature->GetMotionMaster()->MovePoint(0, 2309,-5695,437); //test destination
                stage=1;
                break;
            case 2: //make sure they're not misbehaving with the toy
                switch (m_creature->GetAreaId())
                {
                    case 4343: //New Avalon
                    case 4351: //Scarlet Hold
                    case 4377: //New Avalon Forge
                    case 4346: //New Avalon Town Hall
                    case 4352: //Chapel of the Crimson Flame
                    case 4360: //Scarlet Overlook
                    case 4345: //New Avalon Orchard
                    case 4358: //Tyr's Hand
                        break;
                    default:
                        JustDied(NULL);
                        break;
                }
                
                break;

        }
    }

};

CreatureAI* GetAI_npc_eye_of_acherus(Creature* pCreature)
{
    return new npc_eye_of_acherusAI(pCreature);
}

struct MANGOS_DLL_DECL npc_citizen_of_havenshireAI : public ScriptedAI
{
	npc_citizen_of_havenshireAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset(){	}

	void JustDied(Unit *killer)
    {
		if(killer->GetTypeId()==TYPEID_PLAYER){
			Player* pKiller = (Player*)killer;

			if(pKiller->GetReqKillOrCastCurrentCount(12678,28764) < 10){
				pKiller->KilledMonsterCredit(28764,m_creature->GetGUID());
			}
		}
    }
};
struct MANGOS_DLL_DECL npc_scarlet_crusaderAI : public ScriptedAI
{
	npc_scarlet_crusaderAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

	void Reset(){	}

	void JustDied(Unit *killer)
    {
		if(killer->GetTypeId()==TYPEID_PLAYER){
			Player* pKiller = (Player*)killer;

			if(pKiller->GetReqKillOrCastCurrentCount(12678,28763) < 10){
				pKiller->KilledMonsterCredit(28763,m_creature->GetGUID());
			}
		}
    }
};
/*
struct MANGOS_DLL_DECL npc_death_knight_initiateAI : public ScriptedAI
{
	npc_death_knight_initiateAI(Creature* pCreature) : ScriptedAI(pCreature){ Reset(); }

	void Reset()
	{
		m_creature->setFaction(2084); //covers wrong faction in DB
		m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
	}

    void EnterEvadeMode()
    {
		m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
		m_creature->AttackStop();

		if (m_creature->isAlive())
			m_creature->GetMotionMaster()->MoveTargetedHome();
		Reset();
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

	void DamageDeal(Unit *done_to, uint32 &damage)
	{
		if(damage > done_to->GetHealth())
		{
			damage = 0;
			done_to->SetHealth(1);
			done_to->CombatStop(true);
			if(done_to->GetTypeId()==TYPEID_PLAYER)
				((Player*)done_to)->AttackStop();
			
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            EnterEvadeMode();
		}
	}
    void DamageTaken(Unit *done_by, uint32 &damage)
    {
		if(done_by != m_creature->getVictim())//only the dueler can inflict dmg
			damage = 0;

        if (damage > m_creature->GetHealth())
        {
			damage = 0;

			if(done_by->GetTypeId()==TYPEID_PLAYER){
				Player* pKiller = (Player*)done_by;
				pKiller->CombatStop();
				if(pKiller->GetReqKillOrCastCurrentCount(12733,29025) < 5){
					pKiller->KilledMonster(29025,m_creature->GetGUID());
				}
			}
			done_by->CombatStop();
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            EnterEvadeMode();

        }
    }
};
*/

CreatureAI* GetAI_npc_citizen_of_havenshire(Creature* pCreature)
{
    return new npc_citizen_of_havenshireAI(pCreature);
}

CreatureAI* GetAI_npc_scarlet_crusader(Creature* pCreature)
{
    return new npc_scarlet_crusaderAI(pCreature);
}
/*
CreatureAI* GetAI_npc_death_knight_initiate(Creature* pCreature)
{
    return new npc_death_knight_initiateAI(pCreature);
}
//
//Gossip
//
bool GossipHello_npc_death_knight_initiate(Player *player, Creature *_Creature)
{
	if (player->GetQuestStatus(12733) == QUEST_STATUS_INCOMPLETE)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I challenge you to a duel!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(98,_Creature->GetGUID());
	}else{
		player->SEND_GOSSIP_MENU(98,_Creature->GetGUID());
	}
    return true;
}
bool GossipSelect_npc_death_knight_initiate(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
		//I know... we checked quest before... but doesn't hurt to double check.
		if (player->GetQuestStatus(12733) == QUEST_STATUS_INCOMPLETE){

			player->CLOSE_GOSSIP_MENU();

			_Creature->setFaction(7);
			_Creature->AI()->AttackStart(player);
		}
    }
    return true;
}
*/
bool GossipHello_npc_havenshire_horse(Player *player, Creature *_Creature)
{
	if (player->GetQuestStatus(12680) == QUEST_STATUS_INCOMPLETE)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "[Steal Me!]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(68,_Creature->GetGUID());
	}else
	{
		player->SEND_GOSSIP_MENU(68,_Creature->GetGUID());
	}
    return true;
}

bool GossipSelect_npc_havenshire_horse(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
		if (player->GetQuestStatus(QUEST_GRAND_THEFT_PALOMINO) == QUEST_STATUS_INCOMPLETE)
		{
			player->CLOSE_GOSSIP_MENU();

			player->SetUInt32Value( UNIT_FIELD_FLAGS , 0x001000 );
			player->Mount(10670);
			_Creature->setDeathState(DEAD);

			player->SetSpeedRate(MOVE_RUN,1.2);
			player->SetSpeedRate(MOVE_SWIM,1.2);

			if(!player->GetReqKillOrCastCurrentCount(QUEST_GRAND_THEFT_PALOMINO,28767))
				player->KilledMonsterCredit(28767,_Creature->GetGUID());
		}
    }
    return true;
}


bool GossipHello_npc_salanar_the_horseman(Player *player, Creature *_Creature)
{
	if (!player->GetReqKillOrCastCurrentCount(QUEST_INTO_THE_REALM_OF_SHADOWS,28768) && !player->HasAura(SPELL_REALM_OF_SHADOWS))
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I would like to enter the realm of shadows", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(68,_Creature->GetGUID());

		return true;
	}/*else
	{
		player->SEND_GOSSIP_MENU(68,_Creature->GetGUID());
	}
*/
    return false;
}

bool GossipSelect_npc_salanar_the_horseman(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
		if (player->GetQuestStatus(QUEST_INTO_THE_REALM_OF_SHADOWS) == QUEST_STATUS_INCOMPLETE)
		{
			player->CLOSE_GOSSIP_MENU();

			if(!player->HasAura(SPELL_REALM_OF_SHADOWS))
				player->CastSpell(player,SPELL_REALM_OF_SHADOWS,true);

		}
    }
    return true;
}


//
//Quest
//
bool QuestChooseReward_npc_salanar_the_horseman(Player* pPlayer, Creature* pCreature, const Quest* pQuest, uint32 slot)
{
	if (pQuest->GetQuestId() == QUEST_GRAND_THEFT_PALOMINO || pQuest->GetQuestId() == QUEST_INTO_THE_REALM_OF_SHADOWS)
	{
		pPlayer->Unmount();
		pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
		pPlayer->SetSpeedRate(MOVE_RUN,1);
		pPlayer->SetSpeedRate(MOVE_SWIM,1);
	}

	if (pQuest->GetQuestId() == QUEST_INTO_THE_REALM_OF_SHADOWS)
	{
		if(pPlayer->HasAura(SPELL_REALM_OF_SHADOWS))
			pPlayer->RemoveAurasDueToSpell(SPELL_REALM_OF_SHADOWS);
	}
	return true;
}

bool QuestAccept_npc_salanar_the_horseman(Player* pPlayer, Creature* pCreature, const Quest* pQuest)

{
	if (pQuest->GetQuestId() == QUEST_INTO_THE_REALM_OF_SHADOWS)
	{
		pPlayer->CastSpell(pPlayer,SPELL_REALM_OF_SHADOWS,true);
	}
	return true;
}

/*######
## Mob Dark Rider of Acherus
######*/
enum darkrider
{
    SPELL_ICY_TOUCH             = 52372,
    SPELL_BLOOD_STRIKE          = 52374,
    SPELL_PLAGUE_STRIKE2        = 50688,
    SPELL_THROW                 = 52356,
    SPELL_DEATH_RACE_COMPLETE   = 52361
};
// 52693
struct MANGOS_DLL_DECL mob_dark_rider_of_acherusAI : public ScriptedAI
{
    mob_dark_rider_of_acherusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiBlood_strike_timer;
    uint32 uiIcy_touch_timer;
    uint32 uiPlague_strike_timer;
    uint32 uiThrow_timer;
    uint64 uiPlayerGUID;

    void Reset()
    {
        uiBlood_strike_timer = 3000;
        uiIcy_touch_timer = 4000;
        uiPlague_strike_timer = 5000;
        uiThrow_timer = 10000;
        uiPlayerGUID = 0;
    }

    void Aggro(Unit* who)
    {
        uiPlayerGUID = who->GetGUID();
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiBlood_strike_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_BLOOD_STRIKE);
            uiBlood_strike_timer = 5000 + rand()%1000;
        }else uiBlood_strike_timer -= diff;

        if (uiIcy_touch_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ICY_TOUCH);
            uiIcy_touch_timer = 6000 + rand()%1000;
        }else uiIcy_touch_timer -= diff;

        if (uiPlague_strike_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_PLAGUE_STRIKE2);
            uiPlague_strike_timer = 12000 + rand()%1000;
        }else uiPlague_strike_timer -= diff;

        if (uiThrow_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_THROW);
            uiThrow_timer = 10000 + rand()%1000;
        }else uiThrow_timer -= diff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer)
    {
        if (Unit* pPlayer = Unit::GetUnit(*m_creature, uiPlayerGUID))
            pPlayer->CastSpell(pPlayer, SPELL_DEATH_RACE_COMPLETE, true);
    }
};

CreatureAI* GetAI_mob_dark_rider_of_acherus(Creature* pCreature)
{
    return new mob_dark_rider_of_acherusAI (pCreature);
}


//
//Script Defines
//
void AddSC_quest_DK_StartingArea()
{
    Script *newscript;

        newscript = new Script;
    newscript->Name = "npc_eye_of_acherus";
    newscript->GetAI = &GetAI_npc_eye_of_acherus;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_citizen_of_havenshire";
    newscript->GetAI = &GetAI_npc_citizen_of_havenshire;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_scarlet_crusader";
    newscript->GetAI = &GetAI_npc_scarlet_crusader;
    newscript->RegisterSelf();
/*
	newscript = new Script;
    newscript->Name = "npc_death_knight_initiate";
    newscript->GetAI = &GetAI_npc_death_knight_initiate;
	newscript->pGossipHello = &GossipHello_npc_death_knight_initiate;
    newscript->pGossipSelect = &GossipSelect_npc_death_knight_initiate;
    newscript->RegisterSelf();
*/
	newscript = new Script;
    newscript->Name = "npc_havenshire_horse";
	newscript->pGossipHello = &GossipHello_npc_havenshire_horse;
    newscript->pGossipSelect = &GossipSelect_npc_havenshire_horse;
    newscript->RegisterSelf();

	newscript = new Script;
    newscript->Name = "npc_salanar_the_horseman";
	newscript->pChooseReward = &QuestChooseReward_npc_salanar_the_horseman;
	newscript->pQuestAccept = &QuestAccept_npc_salanar_the_horseman;
	//newscript->pGossipHello = &GossipHello_npc_salanar_the_horseman;
    //newscript->pGossipSelect = &GossipSelect_npc_salanar_the_horseman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rider_of_acherus";
    newscript->GetAI = &GetAI_mob_dark_rider_of_acherus;
    newscript->RegisterSelf();
}
