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
SDName: boss_blood_queen_lanathel
SD%Complete: 0%
SDComment:
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_MIND_CONTROL              = 70923,
    SPELL_VAMPIRIC_BITE_BOSS        = 71726,
    SPELL_BLOODBOLT                 = 71446,
    SPELL_SHROUD_OF_SORROW          = 72981,
    SPELL_PRESENCEOFTHEDARKFALLEN   = 70995,
    SPELL_DELIRIOUS_SLASH           = 71623,
    SPELL_BLOOD_MIRROR_MASTER       = 70821,
    SPELL_BLOOD_MIRROR_SLAVE        = 71510,
    SPELL_PACT_OF_THE_DARKFALLEN    = 71340,
    SPELL_SWARMING_SHADOWS          = 71264,
    SPELL_INCITE_TERROR             = 73070,
    SPELL_SUMMON_BLOODBOLT_WHIRL    = 71772,
    SPELL_BLOODBOLT_WHIRL_ATTACK    = 71899,
    SPELL_BERSERK                   = 26662,
    SPELL_ESSENCE_OF_THE_BLOODQUEEN = 70867,
    SPELL_FRENZIED_BLOODTHIRST      = 70877,
    SPELL_UNCONTROLLABLE_FRENZY     = 70923,
    SPELL_SWARMING_SHADOWS_DMG_AURA = 71267,
};

enum Events
{
    EVENT_BERSERK = 20,
    EVENT_BITE,         // Happens only once @ 15 seconds of starting the fight
    EVENT_BLOODBOLT,
    EVENT_DELIRIOUS_SLASH,
    EVENT_BLOOD_MIRROR,
    EVENT_PACT_OF_THE_DARKFALLEN,
    EVENT_SWARMING_SHADOWS,
    EVENT_INCITE_TERROR,
    EVENT_INCITE_TERROR2,
    EVENT_MINDCONTROL_CHECK,
    EVENT_TAKEOFF,
    EVENT_START_ATTACKING,
    EVENT_LANDING,
    EVENT_BLOODWHIRL,
};

enum Says
{
    SAY_BLOODQUEEN_AGGRO        = -1301100,
    SAY_BLOODQUEEN_BITE01       = -1301101,
    SAY_BLOODQUEEN_BITE02       = -1301102,
    SAY_BLOODQUEEN_ADD01        = -1301103,
    SAY_BLOODQUEEN_SPECIAL01    = -1301104,
    SAY_BLOODQUEEN_SPECIAL02    = -1301105,
    SAY_BLOODQUEEN_SPECIAL03    = -1301106,
    SAY_BLOODQUEEN_RESET        = -1301107,
    SAY_BLOODQUEEN_MINDCONTROL  = -1301108,
    SAY_BLOODQUEEN_SLAY01       = -1301109,
    SAY_BLOODQUEEN_SLAY02       = -1301110,
    SAY_BLOODQUEEN_BERSERK      = -1301111,
    SAY_BLOODQUEEN_DEATH        = -1301112,
};

enum Adds
{
    NPC_SWARMING_SHADOWS    = 38163,
};

#define SWARMING_SHADOWS_EMOTE  "Shadows amass and swarm around $N!"

enum Phases
{
    PHASE_GROUND    = 1,
    PHASE_AIR,
    PMASK_GROUND    = bit_mask<PHASE_GROUND>::value,
    PMASK_AIR       = bit_mask<PHASE_AIR>::value,
    PMASK_ALL       = bit_mask<PHASE_GROUND, PHASE_AIR>::value,
};

#define TIMER_BERSERK               320*IN_MILLISECONDS // 5mins 20 secs
#define TIMER_BITE                  15*IN_MILLISECONDS
#define TIMER_BLOODBOLT             10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_BLOODWHIRL            10*IN_MILLISECONDS, 20*IN_MILLISECONDS
#define TIMER_DELIRIOUS_SLASH       10*IN_MILLISECONDS, 25*IN_MILLISECONDS
#define TIMER_PACT_OF_THE_DARKFALLEN 20*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_DARKFALLEN_CHECK      2*IN_MILLISECONDS
#define TIMER_SWARMING_SHADOWS      30500
#define TIMER_PACTOFTHEDARKFALLEN   30500
#define TIMER_INCITE_TERROR         100*IN_MILLISECONDS

#define ROOM_CENTER_X               4595.298f
#define ROOM_CENTER_Y               2769.011f
#define ROOM_FLOOR_HEIGHT           401.748f

struct MANGOS_DLL_DECL boss_blood_queen_lanathelAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    std::set<uint64> m_Vampires;

    boss_blood_queen_lanathelAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature)
    {
    }

    void Reset()
    {
        m_Vampires.clear();
        if (m_creature->HasSplineFlag(SPLINEFLAG_FLYING))
        {
            m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
        }
        RemoveAuras();  // debuff clearing on wipe (bloodmirror and mindcontrol)
        DoScriptText(SAY_BLOODQUEEN_RESET, m_creature);
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        if (InstanceProgressionCheck())
            return;
        Events.ScheduleEvent(EVENT_BERSERK, TIMER_BERSERK);
        Events.ScheduleEvent(EVENT_BLOOD_MIRROR, 15*IN_MILLISECONDS, 0, 0, 0, PMASK_ALL);
        Events.ScheduleEvent(EVENT_MINDCONTROL_CHECK, 1*MINUTE*IN_MILLISECONDS, 5*IN_MILLISECONDS, 0, 0, PMASK_ALL);
        SCHEDULE_EVENT_R(DELIRIOUS_SLASH, 0, 0, PMASK_GROUND);
        SCHEDULE_EVENT_R(BLOODBOLT, 0, 0, PMASK_GROUND);
        Events.ScheduleEventInRange(EVENT_PACT_OF_THE_DARKFALLEN, TIMER_PACT_OF_THE_DARKFALLEN, TIMER_PACT_OF_THE_DARKFALLEN, 0, 0, PMASK_GROUND);
        Events.ScheduleEvent(EVENT_BITE, TIMER_BITE, 0, 0, 0, PMASK_GROUND);
        Events.ScheduleEvent(EVENT_INCITE_TERROR, TIMER_INCITE_TERROR, m_bIs10Man ? 120*IN_MILLISECONDS : 100*IN_MILLISECONDS, 0, 0, PMASK_GROUND);
        Events.ScheduleEvent(EVENT_SWARMING_SHADOWS, TIMER_SWARMING_SHADOWS, TIMER_SWARMING_SHADOWS, 0, 0, PMASK_GROUND);
        DoCast(m_creature, SPELL_SHROUD_OF_SORROW, true);
        Events.SetPhase(PHASE_GROUND);
        if (m_bIsHeroic)
            DoCast(m_creature, SPELL_PRESENCEOFTHEDARKFALLEN, true);
        DoScriptText(SAY_BLOODQUEEN_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (roll_chance_i(60)) // just to prevent spam
                DoScriptText(urand(0,1) ? SAY_BLOODQUEEN_SLAY01 : SAY_BLOODQUEEN_SLAY02, m_creature);
        }
    }

    void JustDied(Unit* pKiller)
    {
        RemoveAuras();
        DoScriptText(SAY_BLOODQUEEN_DEATH, m_creature);
        m_BossEncounter = DONE;
    }

    void RemoveAuras()
    {
        RemoveEncounterAuras(-SPELL_BLOOD_MIRROR_MASTER, -SPELL_BLOOD_MIRROR_SLAVE);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BLOODQUEEN_BERSERK, m_creature);
                    break;
                case EVENT_MINDCONTROL_CHECK:
                    {
                        ThreatList const &m_tList = m_creature->getThreatManager().getThreatList();
                        for (ThreatList::const_iterator i = m_tList.begin(); i != m_tList.end(); ++i)
                        {
                            Unit* pPlayer = m_creature->GetMap()->GetUnit((*i)->getUnitGuid());
                            if (pPlayer->GetTypeId() != TYPEID_PLAYER)
                                continue;
                            if ((pPlayer->HasAuraByDifficulty(SPELL_ESSENCE_OF_THE_BLOODQUEEN) 
                                || pPlayer->HasAuraByDifficulty(SPELL_FRENZIED_BLOODTHIRST))
                                && !m_Vampires.count(pPlayer->GetGUID()))
                                m_Vampires.insert(pPlayer->GetGUID());
                            else if (m_Vampires.count(pPlayer->GetGUID())
                                && !pPlayer->HasAuraByDifficulty(SPELL_ESSENCE_OF_THE_BLOODQUEEN) 
                                && !pPlayer->HasAuraByDifficulty(SPELL_FRENZIED_BLOODTHIRST)
                                && !pPlayer->HasAuraByDifficulty(SPELL_UNCONTROLLABLE_FRENZY))
                            {
                                DoCast(pPlayer, SPELL_UNCONTROLLABLE_FRENZY, true);
                                DoScriptText(SAY_BLOODQUEEN_MINDCONTROL, m_creature);
                            }
                        }
                    }
                    break;
                case EVENT_BITE:
                    DoScriptText(urand(0,1) ? SAY_BLOODQUEEN_BITE01 : SAY_BLOODQUEEN_BITE02, m_creature);

                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO_PLAYER, 2))
                        DoCast(target, SPELL_VAMPIRIC_BITE_BOSS, false); // g'luck ppl
                    break;
                case EVENT_PACT_OF_THE_DARKFALLEN:
                    DoScriptText(SAY_BLOODQUEEN_SPECIAL02, m_creature);
                    // TODO: Spell max targets = 2 (10 man) 5 (25 man)
                    // TODO: Spell doesnt trigger the "damage" part
                    // TODO: Spell doesnt remove once all "chained" targets are @ 5 yards of each other
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 2))
                        DoCast(target, SPELL_PACT_OF_THE_DARKFALLEN, false);
                    break;
                case EVENT_DELIRIOUS_SLASH:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO_PLAYER, 1))
                        DoCast(target, SPELL_DELIRIOUS_SLASH);
                    break;
                case EVENT_BLOOD_MIRROR: 
                    // we apply this only once throughout the fight
                    if (Unit* target = m_creature->getVictim())
                        if (!target->HasAura(SPELL_BLOOD_MIRROR_MASTER))
                        {
                            Map* pMap = m_creature->GetMap();
                            if (pMap && pMap->IsDungeon())
                            {
                                Map::PlayerList const &PlayerList = pMap->GetPlayers();

                                Player* slave = NULL;
                                float Distance = 50.0f; //we give it a default value just to prevent grabbing players "too far away"

                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                {
                                    Player* pPlayer = i->getSource();
                                    if (!pPlayer->isAlive())
                                        continue;
                                    if (pPlayer->GetObjectGuid() == target->GetObjectGuid())
                                        continue;
                                    float dist = pPlayer->GetDistance2d(target);
                                    if (dist < Distance)
                                    {
                                        slave = pPlayer;
                                        Distance = dist;
                                    }
                                }
                                if (slave)
                                {
                                    target->CastSpell(slave, SPELL_BLOOD_MIRROR_SLAVE, true);
                                    slave->CastSpell(target, SPELL_BLOOD_MIRROR_MASTER, true);
                                }
                            }
                        }
                    break;
                case EVENT_SWARMING_SHADOWS:
                    DoScriptText(SAY_BLOODQUEEN_SPECIAL01, m_creature);
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 2)) //dont target tanks
                    {
                        m_creature->MonsterTextEmote(SWARMING_SHADOWS_EMOTE, target->GetGUID(), true);
                        DoCast(target, SPELL_SWARMING_SHADOWS);
                    }                    
                    break;
                case EVENT_INCITE_TERROR:
                    DoStartNoMovement(m_creature->getVictim());
                    m_creature->GetMotionMaster()->MovePoint(0, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_FLOOR_HEIGHT);
                    Events.ScheduleEvent(EVENT_INCITE_TERROR2, 3*IN_MILLISECONDS);
                    break;
                case EVENT_INCITE_TERROR2:
                    m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                    m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
                    DoScriptText(SAY_BLOODQUEEN_SPECIAL03, m_creature);
                    DoCast(m_creature, SPELL_INCITE_TERROR);
                    Events.ScheduleEvent(EVENT_TAKEOFF, 2*IN_MILLISECONDS);
                    break;
                case EVENT_TAKEOFF:
                    Events.SetPhase(PHASE_AIR);
                    m_creature->GetMotionMaster()->MovePoint(0, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_FLOOR_HEIGHT + 20.0f); // sloppy movement...
                    Events.ScheduleEvent(EVENT_LANDING, 10*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_BLOODWHIRL, 2*IN_MILLISECONDS, 0, 0, 0, PMASK_ALL);
                    break;
                case EVENT_LANDING:
                    m_creature->GetMotionMaster()->MovePoint(0, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_FLOOR_HEIGHT);
                    Events.SetPhase(PHASE_GROUND);
                    m_creature->SetSplineFlags(SPLINEFLAG_WALKMODE);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                    Events.ScheduleEvent(EVENT_START_ATTACKING, 3*IN_MILLISECONDS, 0, 0, 0, PMASK_GROUND);
                    break;
                case EVENT_START_ATTACKING:
                    DoStartMovement(m_creature->getVictim());
                    break;
                case EVENT_BLOODWHIRL:
                    // TODO: spell doesnt work, it should shoot lots of bloodbolts to all targets (which triggers splash damage (6 yards) on hit)
                    DoCast(m_creature, SPELL_SUMMON_BLOODBOLT_WHIRL);
                    // Bloodbolt whirl triggers every 2 seconds 
                    break;
                case EVENT_BLOODBOLT:          
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 2)) // dont target tanks
                        DoCast(target, SPELL_BLOODBOLT);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

#define SWARMING_SHADOWS_DESPAWN_TIMER  2*MINUTE*IN_MILLISECONDS

struct MANGOS_DLL_DECL mob_swarming_shadowsAI: public Scripted_NoMovementAI
{
    ScriptedInstance *m_pInstance;
    uint32 m_uiDespawnTimer;

    mob_swarming_shadowsAI(Creature* pCreature):
        Scripted_NoMovementAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_uiDespawnTimer(0)
    {
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetDisplayId(11686);
        pCreature->setFaction(FACTION_HOSTILE);
        DoCast(pCreature, SPELL_SWARMING_SHADOWS_DMG_AURA);
    }

    void Aggro(Unit* pWho) {}    

    void Reset() {}

    void UpdateAI(uint32 const uiDiff)
    {
        m_uiDespawnTimer += uiDiff;
        if (m_uiDespawnTimer > SWARMING_SHADOWS_DESPAWN_TIMER || !m_pInstance->IsEncounterInProgress())
            DespawnCreature(m_creature);
    }
};

void AddSC_boss_blood_queen_lanathel()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_blood_queen_lanathel);
    REGISTER_SCRIPT(mob_swarming_shadows);
}
