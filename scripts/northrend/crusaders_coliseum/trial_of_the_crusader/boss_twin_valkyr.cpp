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
SDName: Boss Valkyr Twins
SD%Complete: 95
SDComment: timers need verification
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_fjola
boss_eydis
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "SpellMgr.h"

enum Spells
{
    // Eydis Darkbane
    SPELL_EYDIS_TWIN_SPIKE          = 66069,
    SPELL_SURGE_OF_DARKNESS         = 65768,
    SPELL_SHIELD_OF_DARKNESS        = 65874,
    SPELL_TWINS_PACT_EYDIS          = 67303,
    SPELL_DARK_VORTEX               = 66058,
    SPELL_TOUCH_OF_DARKNESS         = 67282, //only heroic
    SPELL_TOUCH_OF_DARKNESS_25      = 67283,

    // Fjola Lightbane
    SPELL_FJOLA_TWIN_SPIKE          = 66075,
    SPELL_SURGE_OF_LIGHT            = 65766,
    SPELL_SHIELD_OF_LIGHTS          = 65858,
    SPELL_TWINS_PACT_FJOLA          = 65876,
    SPELL_LIGHT_VORTEX              = 66046,
    SPELL_TOUCH_OF_LIGHT            = 67296, //only heroic
    SPELL_TOUCH_OF_LIGHT_25         = 67298,

    // both
    SPELL_POWER_OF_THE_TWINS        = 67246,

    // orbs
    SPELL_POWERING_UP               = 67604,
    SPELL_EMPOWERED_DARKNESS        = 65724,
    SPELL_EMPOWERED_LIGHT           = 67215,
    SPELL_UNLEASHED_DARK            = 65808,
    SPELL_UNLEASHED_LIGHT           = 65795,

    //portals
    SPELL_LIGHT_ESSENCE             = 65686,
    SPELL_DARK_ESSENCE              = 65684,

    SPELL_BERSERK                   = 64238,
};

enum Adds
{
    NPC_DARK_ESSENCE                = 34567,
    NPC_LIGHT_ESSENCE               = 34568,
    NPC_CONCENTRATED_LIGHT          = 34630,
    NPC_CONCENTRATED_DARKNESS       = 34628,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_SPAWN_ORBS,
    EVENT_TWIN_SPIKE,
    EVENT_SPECIAL,
    EVENT_TOUCH,
};

enum Specials
{
    SPECIAL_DARK_VORTEX,
    SPECIAL_LIGHT_VORTEX,
    SPECIAL_LIGHT_PACT,
    SPECIAL_DARK_PACT,
};

enum Says
{
    SAY_TWIN_VALKYR_AGGRO           = -1300340,
    SAY_TWIN_VALKYR_BERSERK         = -1300341,
    SAY_TWIN_VALKYR_TWIN_PACT       = -1300342,
    SAY_TWIN_VALKYR_DEATH           = -1300343,
    SAY_TWIN_VALKYR_KILLED_PLAYER1  = -1300344,
    SAY_TWIN_VALKYR_KILLED_PLAYER2  = -1300345,
    SAY_TWIN_VALKYR_DARK_VORTEX     = -1300346,
    SAY_TWIN_VALKYR_LIGHT_VORTEX    = -1300347,
};

#define FLOOR_HEIGHT    394
#define CENTER_X        563.5
#define CENTER_Y        140

typedef std::multimap<uint32 /*entry*/, uint64 /*guid*/> GuidMap;
typedef std::pair<GuidMap::iterator, GuidMap::iterator> GuidMapRange;

#define ORB_NUMBER          urand(25,30)

#define TIMER_BERSERK       6*MINUTE*IN_MILLISECONDS
#define TIMER_SPAWN_ORBS    urand(30,40)*IN_MILLISECONDS
#define TIMER_TWIN_SPIKE    20*IN_MILLISECONDS
#define TIMER_SPECIAL       45*IN_MILLISECONDS
#define TIMER_TOUCH         urand(15,20)*IN_MILLISECONDS

// Used for Light/Dark essence damage effect (aura 303)
// core should do this on apply/remove aura, but don't know that info
#define AURA_STATE_DARK     AuraState(19)
#define AURA_STATE_LIGHT    AuraState(22)

//fjola is the 'slave'
struct MANGOS_DLL_DECL boss_fjolaAI: public boss_trial_of_the_crusaderAI
{
    boss_fjolaAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Reset()
    {
        if (Creature *Darkbane = GET_CREATURE(TYPE_EYDIS_DARKBANE))
            if (ScriptedAI *DarkAI = dynamic_cast<ScriptedAI*>(Darkbane->AI()))
                DarkAI->Reset();
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (Creature *Darkbane = GET_CREATURE(TYPE_EYDIS_DARKBANE))
            Darkbane->SetHealth(m_creature->GetHealth());
    }

    void DoSpecial(Specials special)
    {
        switch (special)
        {
            case SPECIAL_LIGHT_PACT:
                DoScriptText(SAY_TWIN_VALKYR_TWIN_PACT, m_creature);
                DoCast(m_creature, SPELL_SHIELD_OF_LIGHTS, true);
                DoCast(m_creature, SPELL_TWINS_PACT_FJOLA);
                break;
            case SPECIAL_LIGHT_VORTEX:
                DoScriptText(SAY_TWIN_VALKYR_LIGHT_VORTEX, m_creature);
                DoCast(m_creature, SPELL_LIGHT_VORTEX);
                break;
            case SPECIAL_DARK_PACT:
                DoCast(m_creature, SPELL_POWER_OF_THE_TWINS);
                break;
            default:
                break;
        }
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SURGE_OF_LIGHT);
        m_creature->ModifyAuraState(AURA_STATE_LIGHT, true);
        RESCHEDULE_EVENT(BERSERK);
        RESCHEDULE_EVENT(TWIN_SPIKE);
        if (m_bIsHeroic)
            RESCHEDULE_EVENT(TOUCH);
        DoScriptText(SAY_TWIN_VALKYR_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit *who)
    {
        boss_trial_of_the_crusaderAI::KilledUnit(who);
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_TWIN_VALKYR_KILLED_PLAYER1 : SAY_TWIN_VALKYR_KILLED_PLAYER2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Creature *Darkbane = GET_CREATURE(TYPE_EYDIS_DARKBANE))
        {
            uint32 eydis_health = Darkbane->GetHealth();
            if (eydis_health < m_creature->GetHealth())
                m_creature->SetHealth(eydis_health);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(true);
                    DoScriptText(SAY_TWIN_VALKYR_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_TWIN_SPIKE:
                    DoCast(m_creature->getVictim(), SPELL_EYDIS_TWIN_SPIKE);
                    RESCHEDULE_EVENT(TWIN_SPIKE);
                    break;
                case EVENT_TOUCH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_TOUCH_OF_LIGHT);
                    RESCHEDULE_EVENT(TOUCH);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_TWIN_VALKYR_DEATH, m_creature);
        if(Creature *Darkbane = GET_CREATURE(TYPE_EYDIS_DARKBANE))
        {
            Darkbane->SetHealth(0);
            Darkbane->setDeathState(JUST_DIED);
            if (killer)
                Darkbane->AI()->JustDied(NULL);
        }
        // for some odd reason, SummonCreatureJustDied() is not called when using setDeathState() + setHealth()...
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

//eydis is the 'master'
struct MANGOS_DLL_DECL boss_eydisAI: public boss_trial_of_the_crusaderAI
{
    GuidMap m_Summons;
    bool m_bIsClearingSummons;
    std::bitset<4> SpecialsUsed; //0=light vortex, 1= dark vortex, 2=light pact, 3=dark pact

    boss_eydisAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        m_bIsClearingSummons(false)
    {
    }

    void SummonedCreatureDespawn(Creature *pSumm)
    {
        SummonedCreatureJustDied(pSumm);
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if(m_bIsClearingSummons)
            return;

        if (!pSumm)
            return;

        GuidMapRange range = m_Summons.equal_range(pSumm->GetEntry());
        if (range.first == range.second)
            return;

        uint64 guid = pSumm->GetGUID();
        for (GuidMap::iterator i = range.first; i != range.second; ++i)
        {
            if (i->second == guid)
            {
                m_Summons.erase(i);
                break;
            }
        }
    }

    void UnSummonAllCreatures()
    {
        m_bIsClearingSummons = true;
        for (GuidMap::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(i->second))
                    pSummon->ForcedDespawn();

        m_Summons.clear();
        m_bIsClearingSummons = false;
    }

    void Reset()
    {
        UnSummonAllCreatures();
        Map *pMap = m_creature->GetMap();
        Map::PlayerList const &Players = pMap->GetPlayers();
        for(Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Unit* pPlayer = itr->getSource();
            if (!pPlayer)
                continue;
            pPlayer->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
            pPlayer->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
        }
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        bool IsDark = true;
        for (int i=1; i<8; i+=2)
        {
            float x, y;
            GetPointOnCircle(38, i*M_PI/4, x, y);
            m_creature->SummonCreature(IsDark ? NPC_DARK_ESSENCE : NPC_LIGHT_ESSENCE, CENTER_X+x, CENTER_Y+y, FLOOR_HEIGHT+2, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);
            IsDark = !IsDark;
        }

        DoCast(m_creature, SPELL_SURGE_OF_DARKNESS);
        m_creature->ModifyAuraState(AURA_STATE_DARK, true);

        RESCHEDULE_EVENT(BERSERK);
        RESCHEDULE_EVENT(SPAWN_ORBS);
        RESCHEDULE_EVENT(TWIN_SPIKE);
        RESCHEDULE_EVENT(SPECIAL);
        if(m_bIsHeroic)
            RESCHEDULE_EVENT(TOUCH);

        DoScriptText(SAY_TWIN_VALKYR_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void JustSummoned(Creature *pSumm)
    {
        if (!pSumm)
            return;

        m_Summons.insert(std::make_pair(pSumm->GetEntry(), pSumm->GetGUID()));
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (Creature *Lightbane = GET_CREATURE(TYPE_FJOLA_LIGHTBANE))
            Lightbane->SetHealth(m_creature->GetHealth());
    }

    void KilledUnit(Unit *who)
    {
        boss_trial_of_the_crusaderAI::KilledUnit(who);
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_TWIN_VALKYR_KILLED_PLAYER1 : SAY_TWIN_VALKYR_KILLED_PLAYER2, m_creature);
    }

    void GetPointOnCircle(float rad, float ang, float &x, float &y)
    {
        x = rad * cos(ang);
        y = rad * sin(ang);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Creature *Lightbane = GET_CREATURE(TYPE_FJOLA_LIGHTBANE))
        {
            uint32 fjola_health = Lightbane->GetHealth();
            if (fjola_health < m_creature->GetHealth())
                m_creature->SetHealth(fjola_health);
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(true);
                    DoScriptText(SAY_TWIN_VALKYR_BERSERK,m_creature);
                    DoCast(m_creature,SPELL_BERSERK);
                    break;
                case EVENT_SPAWN_ORBS:
                {
                    int NumOrbs = ORB_NUMBER - (m_Summons.count(NPC_CONCENTRATED_DARKNESS) + m_Summons.count(NPC_CONCENTRATED_LIGHT));
                    if (m_bIsHeroic)
                        NumOrbs += 15; //"More Concentrated Light and Concentrated Darkness spawn" (wowhead). Not quite sure how many more
                    for(int i = 0; i <= NumOrbs; i++)
                    {
                        float x, y;
                        GetPointOnCircle(48.5, rand_norm()*2*M_PI, x, y);
                        m_creature->SummonCreature(urand(0,1) ? NPC_CONCENTRATED_DARKNESS : NPC_CONCENTRATED_LIGHT, CENTER_X+x, CENTER_Y+y, FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);
                    }
                    RESCHEDULE_EVENT(SPAWN_ORBS);
                    break;
                }
                case EVENT_TWIN_SPIKE:
                    DoCast(m_creature->getVictim(), SPELL_EYDIS_TWIN_SPIKE);
                    RESCHEDULE_EVENT(TWIN_SPIKE);
                    break;
                case EVENT_SPECIAL:
                {
                    if (SpecialsUsed[0] && SpecialsUsed[1] && SpecialsUsed[2] && SpecialsUsed[3])
                        SpecialsUsed.reset();

                    int SpecialNum;
                    do
                        SpecialNum = urand(0, 3);
                    while (SpecialsUsed[SpecialNum]);
                    SpecialsUsed[SpecialNum] = true;

                    Creature *Lightbane = GET_CREATURE(TYPE_FJOLA_LIGHTBANE);
                    boss_fjolaAI *LightbaneAI = NULL;
                    if (Lightbane)
                        boss_fjolaAI *LightbaneAI = dynamic_cast<boss_fjolaAI*>(Lightbane->AI());

                    switch (SpecialNum)
                    {
                        case SPECIAL_DARK_VORTEX:
                            DoScriptText(SAY_TWIN_VALKYR_DARK_VORTEX, m_creature);
                            DoCast(m_creature, SPELL_DARK_VORTEX);
                            break;
                        case SPECIAL_DARK_PACT:
                            DoScriptText(SAY_TWIN_VALKYR_TWIN_PACT, m_creature);
                            DoCast(m_creature, SPELL_SHIELD_OF_DARKNESS, true);
                            DoCast(m_creature, SPELL_TWINS_PACT_EYDIS);
                            if (LightbaneAI)
                                LightbaneAI->DoSpecial(SPECIAL_DARK_PACT);
                            break;
                        case SPECIAL_LIGHT_VORTEX:
                            if (LightbaneAI)
                                LightbaneAI->DoSpecial(SPECIAL_LIGHT_VORTEX);
                            break;
                        case SPECIAL_LIGHT_PACT:
                            if (LightbaneAI)
                                LightbaneAI->DoSpecial(SPECIAL_LIGHT_PACT);
                            DoCast(m_creature, SPELL_POWER_OF_THE_TWINS);
                            break;
                    }

                    RESCHEDULE_EVENT(SPECIAL);
                    break;
                }
                case EVENT_TOUCH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_TOUCH_OF_DARKNESS);
                    RESCHEDULE_EVENT(TOUCH);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_TWIN_VALKYR_DEATH, m_creature);
        if (Creature *Lightbane = GET_CREATURE(TYPE_FJOLA_LIGHTBANE))
        {
            Lightbane->setDeathState(JUST_DIED);
            Lightbane->SetHealth(0);
            if (killer)
                Lightbane->AI()->JustDied(NULL);
        }
        // for some odd reason, SummonCreatureJustDied() is not called when using setDeathState() + setHealth()...
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);

        UnSummonAllCreatures();
    }
};

struct MANGOS_DLL_DECL mob_concentrated_orbAI : public ScriptedAI
{
    mob_concentrated_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        float x,y;
        GetRandomPointInCircle(48.5, x, y);
        m_creature->GetMotionMaster()->MovePoint(5, x+CENTER_X, y+CENTER_Y, FLOOR_HEIGHT);
    }

    void Reset()
    {
    }

    void GetRandomPointInCircle(float max_rad, float &x, float &y)
    {
        float ang = 2 * M_PI * rand_norm();
        float rad = max_rad * sqrt(rand_norm());
        x = rad * cos(ang);
        y = rad * sin(ang);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(type != POINT_MOTION_TYPE)
            return;
        //simulate random movement
        float x, y;
        GetRandomPointInCircle(48.5, x, y);
        m_creature->GetMotionMaster()->MovePoint(5, x+CENTER_X, y+CENTER_Y, FLOOR_HEIGHT);
    }

    void CastPowerUp(Unit* pTarget, bool IsLight)
    {
        pTarget->CastSpell(pTarget, SPELL_POWERING_UP, true);
        Aura *aur = pTarget->GetAura(SPELL_POWERING_UP, EFFECT_INDEX_0);
        if (aur && aur->GetStackAmount() >= 100)
        {
            pTarget->RemoveAurasDueToSpell(SPELL_POWERING_UP);
            if (IsLight)
                pTarget->CastSpell(pTarget, SPELL_EMPOWERED_LIGHT, true);
            else
                pTarget->CastSpell(pTarget, SPELL_EMPOWERED_DARKNESS, true);
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->GetDistance(m_creature) < 1.7)
        {
            if (m_creature->GetEntry() == NPC_CONCENTRATED_LIGHT)
            {
                if (pWho->HasAura(SPELL_LIGHT_ESSENCE))
                    CastPowerUp(pWho,true);
                else
                {
                    m_creature->setFaction(14);//hostile
                    DoCast(pWho, SPELL_UNLEASHED_LIGHT, true);
                }
            }
            else
            {
                if (pWho->HasAura(SPELL_DARK_ESSENCE))
                    CastPowerUp(pWho, false);
                else
                {
                    m_creature->setFaction(14);//hostile
                    DoCast(pWho, SPELL_UNLEASHED_DARK, true);
                }
            }
            m_creature->ForcedDespawn();
        }
    }
};

bool GossipHello_mob_light_essence(Player *player, Creature* pCreature)
{
    player->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
    if (pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
    {
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_DARKNESS);
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_DARKNESS_25);
    }
    player->CastSpell(player, SPELL_LIGHT_ESSENCE, false);
    return true;
};

bool GossipHello_mob_dark_essence(Player *player, Creature* pCreature)
{
    player->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
    if (pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
    {
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_LIGHT);
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_LIGHT_25);
    }
    player->CastSpell(player, SPELL_DARK_ESSENCE, false);
    return true;
}

void AddSC_twin_valkyr()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_fjola);
    REGISTER_SCRIPT(boss_eydis);
    REGISTER_SCRIPT(mob_concentrated_orb);

    newscript = new Script;
    newscript->Name = "mob_light_essence";
    newscript->pGossipHello = &GossipHello_mob_light_essence;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_essence";
    newscript->pGossipHello = &GossipHello_mob_dark_essence;
    newscript->RegisterSelf();
}
