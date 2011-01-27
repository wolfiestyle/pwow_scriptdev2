/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1649056,
    SAY_BERSERK                         = -1649057,
    SAY_COLORSWITCH                     = -1649058,
    SAY_DEATH                           = -1649059,
    SAY_SLAY_1                          = -1649060,
    SAY_SLAY_2                          = -1649061,
    SAY_TO_BLACK                        = -1649062,
    SAY_TO_WHITE                        = -1649063,
};
*/

enum Spells
{
    // Eydis Darkbane
    SPELL_EYDIS_TWIN_SPIKE          = 66069,
    SPELL_SURGE_OF_DARKNESS         = 65768,
    SPELL_SHIELD_OF_DARKNESS        = 65874,
    SPELL_TWINS_PACT_EYDIS          = 67303,
    SPELL_DARK_VORTEX               = 66058,
    SPELL_TOUCH_OF_DARKNESS         = 67282, //only heroic

    // Fjola Lightbane
    SPELL_FJOLA_TWIN_SPIKE          = 66075,
    SPELL_SURGE_OF_LIGHT            = 65766,
    SPELL_SHIELD_OF_LIGHTS          = 65858,
    SPELL_TWINS_PACT_FJOLA          = 65876,
    SPELL_LIGHT_VORTEX              = 66046,
    SPELL_TOUCH_OF_LIGHT            = 67296, //only heroic

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
    EVENT_LIGHT_VORTEX,
    EVENT_LIGHT_PACT,
    EVENT_DARK_PACT,
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

#define ORB_NUMBER          urand(25,30)

#define TIMER_BERSERK       m_bIsHeroic ? 6*MINUTE*IN_MILLISECONDS : 8*MINUTE*IN_MILLISECONDS
#define TIMER_SPAWN_ORBS    30*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_TWIN_SPIKE    20*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_SPECIAL       45*IN_MILLISECONDS
#define TIMER_TOUCH         15*IN_MILLISECONDS, 20*IN_MILLISECONDS

// Used for Light/Dark essence damage effect (aura 303)
// core should do this on apply/remove aura, but don't know that info
#define AURA_STATE_DARK     AuraState(19)
#define AURA_STATE_LIGHT    AuraState(22)

/*######
## boss_fjola
######*/

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

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SURGE_OF_LIGHT);
        m_creature->ModifyAuraState(AURA_STATE_LIGHT, true);
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT_R(TWIN_SPIKE);
        if (m_bIsHeroic)
            SCHEDULE_EVENT_R(TOUCH);
        DoScriptText(SAY_TWIN_VALKYR_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit *who)
    {
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
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(SAY_TWIN_VALKYR_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_TWIN_SPIKE:
                    DoCast(m_creature->getVictim(), SPELL_EYDIS_TWIN_SPIKE);
                    break;
                case EVENT_TOUCH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_TOUCH_OF_LIGHT);
                    break;
                case EVENT_LIGHT_PACT:
                    DoScriptText(SAY_TWIN_VALKYR_TWIN_PACT, m_creature);
                    DoCast(m_creature, SPELL_SHIELD_OF_LIGHTS, true);
                    DoCast(m_creature, SPELL_TWINS_PACT_FJOLA);
                    break;
                case EVENT_LIGHT_VORTEX:
                    DoScriptText(SAY_TWIN_VALKYR_LIGHT_VORTEX, m_creature);
                    DoCast(m_creature, SPELL_LIGHT_VORTEX);
                    break;
                case EVENT_DARK_PACT:
                    DoCast(m_creature, SPELL_POWER_OF_THE_TWINS);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_TWIN_VALKYR_DEATH, m_creature);
        if (Creature *Darkbane = GET_CREATURE(TYPE_EYDIS_DARKBANE))
        {
            Darkbane->DealDamage(Darkbane, Darkbane->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        // for some odd reason, SummonCreatureJustDied() is not called when using SetDeathState() + setHealth()...
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSEY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

/*######
## boss_eydis
######*/

//eydis is the 'master'
struct MANGOS_DLL_DECL boss_eydisAI: public boss_trial_of_the_crusaderAI
{
    SummonManager SummonMgr;
    std::bitset<4> SpecialsUsed; //0=light vortex, 1= dark vortex, 2=light pact, 3=dark pact

    boss_eydisAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        SummonMgr(pCreature)
    {
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (!pSumm)
            return;
        SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void RemoveEncounterAuras()
    {
        Map::PlayerList const &Players = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
        {
            Unit *pPlayer = itr->getSource();
            if (!pPlayer)
                continue;
            pPlayer->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
            pPlayer->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
            pPlayer->RemoveAurasDueToSpell(SPELL_POWERING_UP);
        }
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        RemoveEncounterAuras();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        bool IsDark = false;
        for (int i=1; i<8; i+=2)
        {
            float x, y;
            GetPointOnCircle(x, y, 38.0f, i*M_PI/4, CENTER_X, CENTER_Y);
            SummonMgr.SummonCreature(IsDark ? NPC_DARK_ESSENCE : NPC_LIGHT_ESSENCE, x, y, FLOOR_HEIGHT+2, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000);
            IsDark = !IsDark;
        }

        DoCast(m_creature, SPELL_SURGE_OF_DARKNESS);
        m_creature->ModifyAuraState(AURA_STATE_DARK, true);

        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT_R(SPAWN_ORBS);
        SCHEDULE_EVENT_R(TWIN_SPIKE);
        SCHEDULE_EVENT(SPECIAL);
        if (m_bIsHeroic)
            SCHEDULE_EVENT_R(TOUCH);

        DoScriptText(SAY_TWIN_VALKYR_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (Creature *Lightbane = GET_CREATURE(TYPE_FJOLA_LIGHTBANE))
            Lightbane->SetHealth(m_creature->GetHealth());
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_TWIN_VALKYR_KILLED_PLAYER1 : SAY_TWIN_VALKYR_KILLED_PLAYER2, m_creature);
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
                    int NumOrbs = ORB_NUMBER - (SummonMgr.GetSummonCount(NPC_CONCENTRATED_DARKNESS) + SummonMgr.GetSummonCount(NPC_CONCENTRATED_LIGHT));
                    if (m_bIsHeroic)
                        NumOrbs += 15; //"More Concentrated Light and Concentrated Darkness spawn" (wowhead). Not quite sure how many more
                    for (int i = 0; i <= NumOrbs; i++)
                    {
                        float x, y;
                        GetPointOnCircle(x, y, 48.5f, 2*M_PI*rand_norm(), CENTER_X, CENTER_Y);
                        SummonMgr.SummonCreature(urand(0,1) ? NPC_CONCENTRATED_DARKNESS : NPC_CONCENTRATED_LIGHT, x, y, FLOOR_HEIGHT+1.0f, 0, TEMPSUMMON_CORPSE_DESPAWN, 1000);
                    }
                    break;
                }
                case EVENT_TWIN_SPIKE:
                    DoCast(m_creature->getVictim(), SPELL_EYDIS_TWIN_SPIKE);
                    break;
                case EVENT_SPECIAL:
                {
                    if (SpecialsUsed[0] && SpecialsUsed[1] && SpecialsUsed[2] && SpecialsUsed[3])
                        SpecialsUsed.reset();

                    int SpecialNum;
                    do
                    {
                        SpecialNum = urand(0, 3);
                    }
                    while (SpecialsUsed[SpecialNum]);
                    SpecialsUsed[SpecialNum] = true;

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
                            BroadcastEventToEntry(NPC_FJOLA_LIGHTBANE, EVENT_DARK_PACT, 0, 150.0f);
                            break;
                        case SPECIAL_LIGHT_VORTEX:
                            BroadcastEventToEntry(NPC_FJOLA_LIGHTBANE, EVENT_LIGHT_VORTEX, 0, 150.0f);
                            break;
                        case SPECIAL_LIGHT_PACT:
                            BroadcastEventToEntry(NPC_FJOLA_LIGHTBANE, EVENT_LIGHT_PACT, 0, 150.0f);
                            DoCast(m_creature, SPELL_POWER_OF_THE_TWINS);
                            break;
                    }
                    break;
                }
                case EVENT_TOUCH:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(target, SPELL_TOUCH_OF_DARKNESS);
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
            Lightbane->DealDamage(Lightbane, Lightbane->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        // for some odd reason, SummonCreatureJustDied() is not called when using SetDeathState() + setHealth()...
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSEY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);

        SummonMgr.UnsummonAll();
        RemoveEncounterAuras();
    }
};

#define RANDOM_MOVE_TIMER   5*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_concentrated_orbAI: public ScriptedAI
{
    uint32 m_uiMoveTimer;
    uint32 m_uiDieTimer;
    bool m_bIsUsed;
    float tar_x, tar_y;

    mob_concentrated_orbAI(Creature *pCreature):
        ScriptedAI(pCreature),
        m_uiMoveTimer(0),
        m_bIsUsed(false),
        m_uiDieTimer(500)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetSpeedRate(MOVE_WALK, 1.0f, true);
        pCreature->SetSpeedRate(MOVE_RUN, 1.0f, true);
        InitMovement();
    }

    void Reset()
    {
        m_uiMoveTimer = 0;
        m_bIsUsed = false;
    }

    void InitMovement()
    {
        GetRandomPointInCircle(tar_x, tar_y, 48.5f, CENTER_X, CENTER_Y);
        m_creature->GetMotionMaster()->MovePoint(0, tar_x, tar_y, m_creature->GetPositionZ());
    }

    void CastPowerUp(Unit* pTarget, bool IsLight)
    {
        pTarget->CastSpell(pTarget, SPELL_POWERING_UP, true);
        Aura *aur = pTarget->GetAura(SPELL_POWERING_UP, EFFECT_INDEX_0);
        if (aur)
        {
            uint8 powerstack = aur->GetStackAmount() + urand(5, 8);
            if (powerstack >= 100)
            {
                pTarget->RemoveAurasDueToSpell(SPELL_POWERING_UP);
                pTarget->CastSpell(pTarget, IsLight ? SPELL_EMPOWERED_LIGHT : SPELL_EMPOWERED_DARKNESS, true);
            }
            else
                aur->GetHolder()->SetStackAmount(powerstack);
        }
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (m_bIsUsed || !pWho || pWho->GetTypeId() != TYPEID_PLAYER || !m_creature->IsWithinDistInMap(pWho, 2.0f))
            return;
        switch (m_creature->GetEntry())
        {
            case NPC_CONCENTRATED_LIGHT:
                if (pWho->HasAura(SPELL_LIGHT_ESSENCE))
                    CastPowerUp(pWho, true);
                else
                {
                    m_creature->setFaction(FACTION_HOSTILE);
                    DoCast(pWho, SPELL_UNLEASHED_LIGHT, true);
                }
                m_bIsUsed = true;
                break;
            case NPC_CONCENTRATED_DARKNESS:
                if (pWho->HasAura(SPELL_DARK_ESSENCE))
                    CastPowerUp(pWho, false);
                else
                {
                    m_creature->setFaction(FACTION_HOSTILE);
                    DoCast(pWho, SPELL_UNLEASHED_DARK, true);
                }
                m_bIsUsed = true;
                break;
            default:
                return;
        }
    }

    void UpdateMovement(float cur_x, float cur_y)
    {
        // 0.2 is a random value (less than 1y will make it look "smooth" when re-initting movement
        if (abs(tar_x - cur_x) < 0.2f && abs(tar_y - cur_y) < 0.2f)
            InitMovement();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_uiMoveTimer < uiDiff)
        {
            UpdateMovement(m_creature->GetPositionX(), m_creature->GetPositionY());
            m_uiMoveTimer = RANDOM_MOVE_TIMER;
        }
        else
            m_uiMoveTimer -= uiDiff;

        if (m_bIsUsed)
        {
            // if creature is "despawned" it never enters the "SummonedCreatureJustDied".
            if (m_uiDieTimer < uiDiff)
            {
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->ForcedDespawn(100);
            }
            else
                m_uiDieTimer -= uiDiff;
        }
    }
};

bool GossipHello_mob_light_essence(Player *player, Creature* pCreature)
{
    player->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
    if (pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_DARKNESS);
    player->CastSpell(player, SPELL_LIGHT_ESSENCE, false);
    return true;
};

bool GossipHello_mob_dark_essence(Player *player, Creature* pCreature)
{
    player->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
    if (pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || pCreature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
        player->RemoveAurasDueToSpell(SPELL_TOUCH_OF_LIGHT);
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
