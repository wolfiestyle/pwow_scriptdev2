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
SDName: boss_valithria
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_CORRUPTION            = 70904,
    SPELL_DREAMWALKER_RAGE      = 71189,
    SPELL_EMERALD_VIGOR         = 70873,
    SPELL_DREAM_SLIP            = 71196,
    SPELL_DREAM_PORTAL_PRE      = 71304,
    SPELL_NIGHTMARE_PORTAL_PRE  = 71986,
    SPELL_NIGHTMARE_PORTAL_VIS  = 71994,
    SPELL_DREAM_PORTAL_VIS      = 70763,
    SPELL_ROT_WORM_SPAWNER      = 70675,
    SPELL_DREAM_STATE           = 70766,
    SPELL_DREAM_CLOUD_VIS       = 70876,
    SPELL_NIGHTMARE_CLOUD_VIS   = 71939,
    SPELL_TWISTING_NIGHTMARES   = 71941,
    SPELL_SUMMON_DREAM_PORTAL   = 71301,
    SPELL_SUMMON_NIGHTMARE_PORT = 72480,
    //SPELL_NIGHTMARES            = 71946, // no reports of this being used actually
};

enum Adds
{
    NPC_DREAM_CLOUD             = 37985,
    NPC_NIGHTMARE_CLOUD         = 38421,
    NPC_DREAM_PORTAL_PRE        = 38186,
    NPC_NIGHTMARE_PORTAL_PRE    = 38429,
    NPC_DREAM_PORTAL            = 37945,
    NPC_NIGHTMARE_PORTAL        = 38430,
    NPC_BLAZING_SKELETON        = 36791,
    NPC_BLISTERING_ZOMBIE       = 37934,
    NPC_RISEN_ARCHMAGE          = 37868,
    NPC_GLUTTONOUS_ABOMINATION  = 37886,
    NPC_ROT_WORM                = 37907,
    NPC_SUPPRESSER              = 37863,
    NPC_LICH_KING_VOICE         = 28765,
    NPC_GREEN_DRAGON_COMBAT_TRIGGER = 38752,
};

enum Events
{
    EVENT_SUMMON_PORTALS = 1,
    EVENT_BEGIN_FIGHT,
    EVENT_INIT_SCRIPT,
    EVENT_BERSERK,
    EVENT_DESPAWN,
    EVENT_SUMMON_RISEN,
    EVENT_SUMMON_BLAZING,
    EVENT_SUMMON_SUPPRESSERS,
    EVENT_SUMMON_ABOMS,
    EVENT_SUMMON_ZOMBIE,
};

enum Says
{
    SAY_VALITHRIA_AGGRO         = -1301000,
    SAY_VALITHRIA_PORTALS       = -1301001,
    SAY_VALITHRIA_75            = -1301002,
    SAY_VALITHRIA_25            = -1301003,
    SAY_VALITHRIA_FAIL          = -1301004,
    SAY_VALITHRIA_SLAY_GOOD     = -1301005,
    SAY_VALITHRIA_BERSERK       = -1301006,
    SAY_VALITHRIA_SLAY_BAD      = -1301007,
    SAY_VALITHRIA_WIN           = -1301008,
    SAY_LICH_KING_GREEN_DRAGON  = -1301009,

};

// Following timers are gotten from video-research
static const uint32 blazingtimers[] = {50, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5, 5, 5, 5, 5};
static const uint32 abomtimers[] = {20, 60, 55, 50, 50, 50, 45, 45, 45, 40, 40, 40, 35, 35, 35, 30, 30, 30, 25, 25, 25};
static const float locations[4][2] =
{
    {4241.56f, 2424.29f}, //L1
    {4241.56f, 2546.83f}, //R1
    {4165.97f, 2424.29f}, //L2
    {4165.97f, 2546.83f}, //R2
};

#define ZOMBIE_SUMMON_TIMER 30*IN_MILLISECONDS
#define RISEN_SUMMON_TIMER 30*IN_MILLISECONDS
#define SUPPRESSER_SUMMON_TIMER 60*IN_MILLISECONDS
#define PORTAL_TIMER 55*IN_MILLISECONDS
#define BERSERK_TIMER 7*MINUTE*IN_MILLISECONDS
#define FACTION_HOSTILE 14

struct MANGOS_DLL_DECL boss_valithriaAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    uint32 m_auiSays;

    boss_valithriaAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        m_auiSays(0)
    {
        pCreature->SetHealthPercent(50.0f);
        pCreature->CastSpell(pCreature, SPELL_CORRUPTION, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        DoStartNoMovement(pCreature);
        SetCombatMovement(false);
    }

    
    void Reset()
    {
        m_auiSays = 0;
        SummonMgr.UnsummonAll();

        m_creature->SetHealthPercent(50.0f);
        m_creature->CastSpell(m_creature, SPELL_CORRUPTION, false);

        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_ENTRANCE))
            Door->SetGoState(GO_STATE_ACTIVE);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_1))
            Door->SetGoState(GO_STATE_READY);
        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_1))
            Door->SetGoState(GO_STATE_READY);
        if (!m_bIs10Man)
        {
            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_2))
                Door->SetGoState(GO_STATE_READY);
            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_2))
                Door->SetGoState(GO_STATE_READY);
        }

        m_BossEncounter = NOT_STARTED;
        boss_icecrown_citadelAI::Reset();
    }
    

    void Aggro(Unit* pWho)
    {
        //m_BossEncounter = IN_PROGRESS;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if ( pWho && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 65.0f) && !(m_auiSays & 0x00001))
        {
            Unit* LK = m_creature->SummonCreature(NPC_LICH_KING_VOICE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() , 0, TEMPSUMMON_TIMED_DESPAWN, 5000);
            LK->SetDisplayId(11686);
            DoScriptText(SAY_LICH_KING_GREEN_DRAGON, LK);
            m_auiSays |= 0x00001;

            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_ENTRANCE)) //LK talk -> Doors Close and players enter in combat
                Door->SetGoState(GO_STATE_READY);

            Events.RescheduleEvent(EVENT_BEGIN_FIGHT, 12500);
            Events.RescheduleEvent(EVENT_INIT_SCRIPT, 3500);
            m_BossEncounter = IN_PROGRESS;
        }
        else
            return;

    }
    void KilledUnit(Unit* pWho)
    {
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        if (m_BossEncounter != DONE)
        {
            m_BossEncounter = NOT_STARTED;
            m_creature->Respawn();
        }
    }       
    void JustSummoned(Creature* pSummon)
    {
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());
        if (pSummon->GetEntry() == NPC_DREAM_PORTAL || pSummon->GetEntry() == NPC_NIGHTMARE_PORTAL || pSummon->GetEntry() == NPC_DREAM_PORTAL_PRE || pSummon->GetEntry() == NPC_NIGHTMARE_PORTAL_PRE)
            return;
        pSummon->SetInCombatWithZone();
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
        if (urand(0,100) < 15)
            DoScriptText(SAY_VALITHRIA_SLAY_BAD, m_creature);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_BossEncounter != IN_PROGRESS)
            return;

        m_creature->SetTargetGUID(0); 
        /*if(!m_creature->HasAura(SPELL_NIGHTMARES))        // Undocumented boss-related Spell
            DoCast(m_creature, SPELL_NIGHTMARES, false);*/
        if (m_creature->GetHealthPercent() <= 25.0f && !(m_auiSays & 0x00100))
        {
            DoScriptText(SAY_VALITHRIA_25, m_creature);
            m_auiSays |= 0x00100;
        }
        if (m_creature->GetHealthPercent() >= 75.0f && !(m_auiSays & 0x01000))
        {
            DoScriptText(SAY_VALITHRIA_75, m_creature);
            m_auiSays |= 0x01000;
        }
        if (m_creature->GetHealthPercent() >= 99.7f && !(m_auiSays & 0x10000))
        {
            if (m_creature->HasAura(SPELL_CORRUPTION))
                m_creature->RemoveAurasDueToSpell(SPELL_CORRUPTION);

            DoScriptText(SAY_VALITHRIA_WIN, m_creature);
            m_creature->CastSpell(m_creature, SPELL_DREAMWALKER_RAGE, false);
            SummonMgr.UnsummonAllWithId(NPC_GREEN_DRAGON_COMBAT_TRIGGER);
            m_auiSays |= 0x10000;
            Events.Reset();
            Events.ScheduleEvent(EVENT_DESPAWN,5*IN_MILLISECONDS);
        }
        if (Unit* CombatTrigger = SummonMgr.GetFirstFoundSummonWithId(NPC_GREEN_DRAGON_COMBAT_TRIGGER))
            if (CombatTrigger->getThreatManager().getThreatList().size() == 0)
                Reset();

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_INIT_SCRIPT:
                    {
                        Creature* CT = SummonMgr.SummonCreature(NPC_GREEN_DRAGON_COMBAT_TRIGGER, 4268.0f, 2484.0f, 365.02f , 3.175f, TEMPSUMMON_MANUAL_DESPAWN, 20000); //spawn a combat trigger to keep track of players

                        Events.RescheduleEvent(EVENT_SUMMON_PORTALS, 41.5f*IN_MILLISECONDS); // first portal timer (taken from DBM database)
                        Events.RescheduleEvent(EVENT_BERSERK, BERSERK_TIMER);

                        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_1))
                            Door->SetGoState(GO_STATE_ACTIVE);
                        if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_1))
                            Door->SetGoState(GO_STATE_ACTIVE);
                        if (!m_bIs10Man)
                        {
                            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_LEFT_2))
                                Door->SetGoState(GO_STATE_ACTIVE);
                            if (GameObject* Door = GET_GAMEOBJECT(DATA_VALITHRIA_DOOR_RIGHT_2))
                                Door->SetGoState(GO_STATE_ACTIVE);
                        }
                        break;
                    }
                case EVENT_BEGIN_FIGHT:
                    {
                        if (!(m_auiSays & 0x00010))
                        {
                            DoScriptText(SAY_VALITHRIA_AGGRO, m_creature);
                            m_auiSays |= 0x00010;
                        }
                    break;
                    }
                case EVENT_SUMMON_PORTALS:
                    if (!m_bIsHeroic)
                        DoScriptText(SAY_VALITHRIA_PORTALS, m_creature);

                    for (uint8 i = 0 ; i < (m_bIs10Man ? 3  : 6 ); i++)
                        DoCast(m_creature, m_bIsHeroic ? SPELL_SUMMON_NIGHTMARE_PORT : SPELL_SUMMON_DREAM_PORTAL, true);
                    Events.RescheduleEvent(EVENT_SUMMON_PORTALS, PORTAL_TIMER);
                    break;
                case EVENT_BERSERK: // either you die cause of add overwhelming or she dies (berserk spell is not documented) perhaps she turns unfriendly and casts the same rage she casts at win, but noone lasts long enough to see that
                    DoScriptText(SAY_VALITHRIA_BERSERK, m_creature);
                    m_creature->RemoveAllAttackers();
                    Reset();
                    break;
                case EVENT_DESPAWN:
                    m_creature->SetPhaseMask(16, true);
                    m_BossEncounter = DONE; 
                    m_creature->DealDamage(m_creature,m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                default:
                    break;
            }
    }
};

bool GossipHello_mob_valithria_portal(Player *player, Creature* pCreature)
{
    player->CastSpell(player, SPELL_DREAM_STATE, false);
    if (pCreature->isTemporarySummon())
        pCreature->ForcedDespawn();
    return true;
};

struct MANGOS_DLL_DECL mob_green_dragon_combat_triggerAI: public Scripted_NoMovementAI
{
    EventManager Events;
    SummonManager SummonMgr;
    uint32 abomWaveCount;
    uint32 blazingWaveCount;
    ScriptedInstance* m_pInstance;
    bool m_bIs10man :1;
    bool started :1;

    mob_green_dragon_combat_triggerAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        abomWaveCount(0),
        blazingWaveCount(0),
        started(0),
        SummonMgr(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIs10man(m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || m_pInstance->instance->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
    {
        pCreature->setFaction(FACTION_HOSTILE);
        pCreature->SetInCombatWithZone();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        Events.RescheduleEvent(EVENT_SUMMON_RISEN, RISEN_SUMMON_TIMER+urand(5,6)*IN_MILLISECONDS);
        Events.RescheduleEvent(EVENT_SUMMON_BLAZING, blazingtimers[blazingWaveCount++]*IN_MILLISECONDS);
        Events.RescheduleEvent(EVENT_SUMMON_SUPPRESSERS, 23*IN_MILLISECONDS);
        Events.RescheduleEvent(EVENT_SUMMON_ABOMS, abomtimers[abomWaveCount++]*IN_MILLISECONDS);
        Events.RescheduleEvent(EVENT_SUMMON_ZOMBIE, ZOMBIE_SUMMON_TIMER);
    }

    void Reset()
    {
        Events.Reset();
        SummonMgr.UnsummonAll();
        abomWaveCount = 0;
        blazingWaveCount = 0;
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
        if (Creature* Valithria = GET_CREATURE(TYPE_VALITHRIA))
            if (urand(0,100) < 10) //10% chance of having her say stuff for killing adds
                DoScriptText(SAY_VALITHRIA_SLAY_BAD, Valithria);
    }

    void JustSummoned(Creature* pSummon)
    {
        SummonMgr.AddSummonToList(pSummon->GetObjectGuid());
        pSummon->SetInCombatWithZone();        
    }

    void SummonAdd(uint32 entry)
    {
        uint32 loc = urand(0, m_bIs10man? 1:3) ;
        SummonMgr.SummonCreature(entry, locations[loc][0], locations[loc][1], m_creature->GetPositionZ()+1.0f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 6*IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() ||  !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SUMMON_BLAZING:
                    SummonAdd(NPC_BLAZING_SKELETON);
                    Events.RescheduleEvent(EVENT_SUMMON_BLAZING, blazingtimers[blazingWaveCount++]*IN_MILLISECONDS);
                    break;
                case EVENT_SUMMON_ZOMBIE:
                    SummonAdd(NPC_BLISTERING_ZOMBIE);
                    Events.RescheduleEvent(EVENT_SUMMON_ZOMBIE, ZOMBIE_SUMMON_TIMER);
                    break;
                case EVENT_SUMMON_SUPPRESSERS:
                    for (int32 count = 0; count < (m_bIs10man ? 5 : 8); count++)
                        SummonAdd(NPC_SUPPRESSER);
                    Events.RescheduleEvent(EVENT_SUMMON_SUPPRESSERS, SUPPRESSER_SUMMON_TIMER);
                    break;
                case EVENT_SUMMON_RISEN:
                    SummonAdd(NPC_RISEN_ARCHMAGE);
                    Events.RescheduleEvent(EVENT_SUMMON_RISEN, RISEN_SUMMON_TIMER);
                    break;
                case EVENT_SUMMON_ABOMS:
                    SummonAdd(NPC_GLUTTONOUS_ABOMINATION);
                    Events.RescheduleEvent(EVENT_SUMMON_ABOMS, abomtimers[abomWaveCount++]*IN_MILLISECONDS);
                    break;
                default:
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_valithria_pre_portalAI: public ScriptedAI
{
    uint32 timer;

    mob_valithria_pre_portalAI(Creature *pCreature):
        ScriptedAI(pCreature),
        timer(15*IN_MILLISECONDS)
    {
        pCreature->CastSpell(pCreature, pCreature->GetEntry() == NPC_DREAM_PORTAL_PRE ? SPELL_DREAM_PORTAL_PRE : SPELL_NIGHTMARE_PORTAL_PRE, false);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Reset(){}

    void JustSummoned(Creature* pSummon)
    {
        pSummon->CastSpell(pSummon, pSummon->GetEntry() == NPC_DREAM_PORTAL?SPELL_DREAM_PORTAL_VIS:SPELL_NIGHTMARE_PORTAL_VIS, false);
        pSummon->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (timer <= uiDiff)
        {
            m_creature->SummonCreature( m_creature->GetEntry() == NPC_DREAM_PORTAL_PRE ? NPC_DREAM_PORTAL : NPC_NIGHTMARE_PORTAL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS);
            if (m_creature->isTemporarySummon())
                m_creature->ForcedDespawn();
        } else timer -=uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_valithria_cloudAI: public ScriptedAI
{
    bool m_bIsUsed;

    mob_valithria_cloudAI(Creature *pCreature):
        ScriptedAI(pCreature),
        m_bIsUsed(false)
    {
        Reset();
    }
    
    void Reset()
    {
        m_creature->setFaction(14);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        m_bIsUsed = false;
        m_creature->CastSpell(m_creature, m_creature->GetEntry()== NPC_DREAM_CLOUD? SPELL_DREAM_CLOUD_VIS :SPELL_NIGHTMARE_CLOUD_VIS, false);
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_bIsUsed || !pWho || pWho->GetTypeId() != TYPEID_PLAYER || !m_creature->IsWithinDistInMap(pWho, 2.0f))
            return;

        switch (m_creature->GetEntry())
        {
            case NPC_DREAM_CLOUD:
                DoCast(pWho, SPELL_EMERALD_VIGOR, false);
                m_bIsUsed = true;
                break;
            case NPC_NIGHTMARE_CLOUD:
                DoCast(pWho, SPELL_TWISTING_NIGHTMARES, false);
                m_bIsUsed = true;
                break;
            default:
                return;
        }
        m_creature->ForcedDespawn(1500);
    }

    void UpdateAI(uint32 const uiDiff)
    {
    }
};

void AddSC_boss_valithria()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_valithria);
    REGISTER_SCRIPT(mob_valithria_cloud);
    REGISTER_SCRIPT(mob_valithria_pre_portal);
    REGISTER_SCRIPT(mob_green_dragon_combat_trigger);

    newscript = new Script;
    newscript->Name = "mob_valithria_portal";
    newscript->pGossipHello = &GossipHello_mob_valithria_portal;
    newscript->RegisterSelf();

}
