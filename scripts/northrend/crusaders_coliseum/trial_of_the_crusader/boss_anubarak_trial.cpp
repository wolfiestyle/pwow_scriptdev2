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
SDName: Boss Anub'arak
SD%Complete: 0
SDComment: most add timers need verification, pursuing spikes not working (no idea about how it works)
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_anubarak_trial
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "TemporarySummon.h"

//evidently, azjol-nerub was merely a set-back

enum Spells
{
    // Anub'arak
    SPELL_BERSERK               = 26662,
    SPELL_FREEZING_SLASH        = 66012,
    SPELL_PENETRATING_COLD_N10  = 66013,
    SPELL_PENETRATING_COLD_N25  = 67700,
    SPELL_PENETRATING_COLD_H10  = 68509,
    SPELL_PENETRATING_COLD_H25  = 68510,
    SPELL_LEECHING_SWARM_N10    = 66118,
    SPELL_LEECHING_SWARM_N25    = 67630,
    SPELL_LEECHING_SWARM_H10    = 68646,
    SPELL_LEECHING_SWARM_H25    = 68647,
    SPELL_LEECHING_SWARM_DAMAGE = 66240,
    SPELL_LEECHING_SWARM_HEAL   = 66125,
    SPELL_PURSUING_SPIKES       = 65922,  // only part of the effect, probably used by dummy creature
    SPELL_SUBMERGE_ANUBARAK     = 65981,
    SPELL_UNDERGROUND_VISUAL    = 65921,
    SPELL_PURSUED_BY_ANUBARK    = 67574,

    // Frost Sphere
    SPELL_PERMAFROST_N10        = 66193,
    SPELL_PERMAFROST_N25        = 67855,
    SPELL_PERMAFROST_H10        = 67856,
    SPELL_PERMAFROST_H25        = 67857,
    // Nerubian Burrower
    SPELL_SUBMERGE_BURROWER     = 68394,
    SPELL_SHADOW_STRIKE         = 66134, //only heroic
    SPELL_SUBMERGE_ATTEMPT      = 67322,
    SPELL_EXPOSE_WEAKNESS       = 67720,
    SPELL_SPIDER_FRENZY         = 66128,
    // Swarm Scarab
    SPELL_ACID_DRENCHED_MANDIBLES_N10 = 65775,
    SPELL_ACID_DRENCHED_MANDIBLES_N25 = 67861,
    SPELL_ACID_DRENCHED_MANDIBLES_H10 = 67862,
    SPELL_ACID_DRENCHED_MANDIBLES_H25 = 67863,
    SPELL_DETERMINATION         = 66092,
};

enum AddIds
{
    NPC_FROST_SPHERE            = 34606,
    NPC_NERUBIAN_BURROWER       = 34607,
    NPC_SWARM_SCARAB            = 34605,
};

enum Says
{
    SAY_ANUBARAK_ANUBARAK_INTRO6 = -1300354,

    SAY_AGGRO                   = -1300355,
    SAY_SUBMERGE                = -1300356,
    SAY_LEECHING_SWARM          = -1300357,
    SAY_KILLED_PLAYER1          = -1300358,
    SAY_KILLED_PLAYER2          = -1300359,
    SAY_DEATH                   = -1300360,
    SAY_BERSERK                 = -1300361,
};

enum Events
{
    // Anub'arak
    EVENT_BERSERK = 1,
    EVENT_FREEZING_SLASH,
    EVENT_PENETRATING_COLD,
    EVENT_SUBMERGE,
    EVENT_SUMMON_BURROWER,
    EVENT_SUMMON_SWARM_SCARAB,
    EVENT_PURSUING_SPIKES,
    EVENT_UNSUBMERGE,
    EVENT_LEECHING_SWARM_DAMAGE,
    // Nerubian Burrower
    EVENT_ATTEMPT_SUBMERGE,
    EVENT_EXPOSE_WEAKNESS,
    EVENT_SHADOW_STRIKE,
    // Swarm Scarab
    EVENT_ACID_DRENCHED_MANDIBLES,
    EVENT_DETERMINATION,
};

enum Phases
{
    PHASE_NOTSTARTED,
    PHASE_ABOVEGROUND,
    PHASE_BELOWGROUND,
    PHASE_THREE,
    PHASE_DONE
};

typedef std::multimap<uint32 /*entry*/, uint64 /*guid*/> GuidMap;
typedef std::pair<GuidMap::iterator, GuidMap::iterator> GuidMapRange;
typedef std::list<uint64> GuidList;

// Anub'arak
#define TIMER_BERSERK               10*MINUTE*IN_MILLISECONDS
#define TIMER_SUMMON_SWARM_SCARAB   4*IN_MILLISECONDS
#define TIMER_SUMMON_BURROWER       45*IN_MILLISECONDS
#define TIMER_PENETRATING_COLD      18*IN_MILLISECONDS           //not sure about this one
#define TIMER_FREEZING_SLASH        urand(19,21)*IN_MILLISECONDS
#define TIMER_SUBMERGE              75*IN_MILLISECONDS
#define TIMER_UNSUBMERGE            65*IN_MILLISECONDS
#define TIMER_LEECHING_SWARM_DAMAGE 1*IN_MILLISECONDS
// Nerubian Burrower
#define TIMER_ATTEMPT_SUBMERGE      50*IN_MILLISECONDS           //not sure
#define TIMER_EXPOSE_WEAKNESS       urand(3,10)*IN_MILLISECONDS  //unsure
#define TIMER_SHADOW_STRIKE         30.5*IN_MILLISECONDS
// Swarm Scarab
#define TIMER_ACID_DRENCHED_MANDIBLES 2*IN_MILLISECONDS
#define TIMER_DETERMINATION         urand(30,400)*IN_MILLISECONDS    //unsure (wowwiki says "random chance")

#define MAX_ALIVE_FROST_SPHERES     6
#define FLOOR_HEIGHT                143

static const float SummonPositions [4][2] = 
{
    {724.0f, 191.0f},
    {688.5f, 189.0f},
    {716.0f, 81.6f},
    {767.0f, 82.0f}
};

struct MANGOS_DLL_DECL boss_anubarak_trialAI: public boss_trial_of_the_crusaderAI
{
    uint32 CurrPhase;
    GuidMap m_Summons;
    GuidList m_AliveFrostSpheres;
    bool m_bSaidBeginningStuff;

    boss_anubarak_trialAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        CurrPhase(PHASE_NOTSTARTED),
        m_bSaidBeginningStuff(false)
    {
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bSaidBeginningStuff)
        {
            m_bSaidBeginningStuff = true;
            DoScriptText(SAY_ANUBARAK_ANUBARAK_INTRO6, m_creature);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Reset()
    {
        CurrPhase = PHASE_NOTSTARTED;
        UnSummonAllCreatures();
        m_creature->RemoveAllAuras();
        boss_trial_of_the_crusaderAI::Reset();
    }

    void Aggro(Unit *pWho)
    {
        Events.Reset();
        CurrPhase = PHASE_ABOVEGROUND;

        RESCHEDULE_EVENT(SUMMON_BURROWER);
        RESCHEDULE_EVENT(BERSERK);
        RESCHEDULE_EVENT(FREEZING_SLASH);
        RESCHEDULE_EVENT(PENETRATING_COLD);
        RESCHEDULE_EVENT(SUBMERGE);
        RESCHEDULE_EVENT(SUMMON_SWARM_SCARAB);

        SummonAdds(NPC_FROST_SPHERE, 6);

        DoScriptText(SAY_AGGRO, m_creature);
        m_BossEncounter = IN_PROGRESS;
    }

    void UnSummonAllCreatures()
    {
        for (GuidMap::const_iterator i = m_Summons.begin(); i != m_Summons.end(); ++i)
            if (Creature *pSummon = m_creature->GetMap()->GetCreature(i->second))
            {
                if (pSummon->GetEntry() == NPC_FROST_SPHERE)
                    static_cast<TemporarySummon*>(pSummon)->UnSummon();
                else
                    pSummon->ForcedDespawn();
            }
        m_Summons.clear();
    }

    void JustSummoned(Creature *pSumm)
    {
        if (!pSumm)
            return;

        m_Summons.insert(std::make_pair(pSumm->GetEntry(), pSumm->GetGUID()));

        switch (pSumm->GetEntry())
        {
            case NPC_FROST_SPHERE:
                pSumm->GetMotionMaster()->MoveIdle();
                pSumm->SetSplineFlags(SPLINEFLAG_UNKNOWN7); //Fly
                m_AliveFrostSpheres.push_back(pSumm->GetGUID());
                break;
            case NPC_SWARM_SCARAB:
                if (CurrPhase == PHASE_ABOVEGROUND)
                    pSumm->setFaction(7);   //neutral
                else
                    pSumm->SetInCombatWithZone();
                break;
            case NPC_NERUBIAN_BURROWER:
                pSumm->SetInCombatWithZone();
                break;
        }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (!pSumm)
            return;

        if (pSumm->GetEntry() == NPC_FROST_SPHERE)
        {
            pSumm->Relocate(pSumm->GetPositionX(), pSumm->GetPositionY(), FLOOR_HEIGHT);  //move to ground
            m_AliveFrostSpheres.remove(pSumm->GetGUID());
            pSumm->CastSpell(pSumm, DIFFICULTY(SPELL_PERMAFROST), true);
        }
        else
        {
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
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void GetRandomPointInCircle(float max_rad, float &x, float &y)
    {
        float ang = 2*M_PI * rand_norm();
        float rad = max_rad * sqrt(rand_norm());
        x = rad * cos(ang);
        y = rad * sin(ang);
    }

    void SummonAdds(uint32 entry, uint32 num = 1)
    {
        if (entry == NPC_FROST_SPHERE)
        {
            for (int i = 0; i < num; i++)
            {
                float x, y;
                GetRandomPointInCircle(54.0f, x, y);
                x += 741.0f;
                y += 133.6f;
                m_creature->SummonCreature(NPC_FROST_SPHERE, x, y, 163, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            }
        }
        else
        {
            for(int i = 0; i < num; i++)
            {
                uint32 RandPos = urand(0,3);
                m_creature->SummonCreature(entry, SummonPositions[RandPos][0], SummonPositions[RandPos][1], FLOOR_HEIGHT, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_AliveFrostSpheres.size() < MAX_ALIVE_FROST_SPHERES && !m_bIsHeroic)
            SummonAdds(NPC_FROST_SPHERE, MAX_ALIVE_FROST_SPHERES - m_AliveFrostSpheres.size());

        if (CurrPhase != PHASE_THREE && m_creature->GetHealthPercent() < 30.0f)
        {
            m_creature->InterruptNonMeleeSpells(true);
            CurrPhase = PHASE_THREE;
            RESCHEDULE_EVENT(PENETRATING_COLD);
            RESCHEDULE_EVENT(FREEZING_SLASH);
            RESCHEDULE_EVENT(LEECHING_SWARM_DAMAGE);
            Events.CancelEvent(EVENT_SUMMON_SWARM_SCARAB);
            Events.CancelEvent(EVENT_SUBMERGE);
            Events.CancelEvent(EVENT_UNSUBMERGE);
            if (!m_bIsHeroic)
                Events.CancelEvent(EVENT_SUMMON_BURROWER);
            DoScriptText(SAY_LEECHING_SWARM, m_creature);
            DoCast(m_creature, DIFFICULTY(SPELL_LEECHING_SWARM));
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(false);
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    break;
                case EVENT_FREEZING_SLASH:
                    DoCast(m_creature->getVictim(), SPELL_FREEZING_SLASH);
                    RESCHEDULE_EVENT(FREEZING_SLASH);
                    break;
                case EVENT_PENETRATING_COLD:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, DIFFICULTY(SPELL_PENETRATING_COLD));
                    RESCHEDULE_EVENT(PENETRATING_COLD);
                    break;
                case EVENT_SUMMON_BURROWER:
                    /* 1 will spawn on 10man normal
                     * 2 will spawn on 25man normal
                     * 2 will spawn on 10man heroic (and keep spawning even during phase 3)
                     * 4 will spawn on 25man heroic (and keep spawning even during phase 3)
                     */
                    SummonAdds(NPC_NERUBIAN_BURROWER, DIFF_SELECT(1, 2, 2, 4));
                    RESCHEDULE_EVENT(SUMMON_BURROWER);
                    break;
                case EVENT_SUMMON_SWARM_SCARAB:
                    SummonAdds(NPC_SWARM_SCARAB);
                    RESCHEDULE_EVENT(SUMMON_SWARM_SCARAB);
                    break;
                case EVENT_LEECHING_SWARM_DAMAGE:
                {
                    ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
                    GuidList TargetGuids;   //to prevent crashes from players dieing while being damaged
                    for (ThreatList::const_iterator i = tlist.begin(); i != tlist.end(); ++i)
                        if (*i)
                            TargetGuids.push_back((*i)->getUnitGuid());

                    for (GuidList::const_iterator i = TargetGuids.begin(); i != TargetGuids.end(); ++i)
                        if (Unit *Target = m_creature->GetUnit(*m_creature, *i))
                            if (Target->isAlive() && Target->IsInMap(m_creature))
                            {
                                int32 Damage = Target->GetHealth() * DIFF_SELECT(0.1f, 0.1f, 0.2f, 0.3f);
                                if (Damage < 250)
                                    Damage = 250;
                                m_creature->CastCustomSpell(Target, SPELL_LEECHING_SWARM_DAMAGE, &Damage, NULL, NULL, true);
                                m_creature->CastCustomSpell(m_creature, SPELL_LEECHING_SWARM_HEAL, &Damage, NULL, NULL, true);
                            }

                    RESCHEDULE_EVENT(LEECHING_SWARM_DAMAGE);
                    break;
                }
                case EVENT_SUBMERGE:
                {
                    CurrPhase = PHASE_BELOWGROUND;
                    DoScriptText(SAY_SUBMERGE, m_creature);
                    m_creature->InterruptNonMeleeSpells(true);
                    DoCast(m_creature, SPELL_SUBMERGE_ANUBARAK, true);
                    //set scarabs to hostile
                    GuidMapRange range = m_Summons.equal_range(NPC_SWARM_SCARAB);
                    if (range.first == range.second)
                        return;
                    for (GuidMap::const_iterator i = range.first; i != range.second; ++i)
                        if (Creature *scarab = m_creature->GetMap()->GetCreature(i->second))
                        {
                            scarab->setFaction(14);//hostile
                            scarab->SetInCombatWithZone();
                        }
                    //DoCast(m_creature, SPELL_UNDERGROUND_VISUAL, true); //doesn't show
                    DoResetThreat();

                    Events.CancelEvent(EVENT_PENETRATING_COLD);
                    Events.CancelEvent(EVENT_FREEZING_SLASH);
                    RESCHEDULE_EVENT(UNSUBMERGE);
                    break;
                }
                case EVENT_UNSUBMERGE:
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_ANUBARAK);
                    CurrPhase = PHASE_ABOVEGROUND;
                    RESCHEDULE_EVENT(FREEZING_SLASH);
                    RESCHEDULE_EVENT(PENETRATING_COLD);
                    RESCHEDULE_EVENT(SUBMERGE);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit *pKiller)
    {
        UnSummonAllCreatures();
        Events.Reset();
        DoScriptText(SAY_DEATH, m_creature);
        //only non-summoned boss, so to streamline things its easier to do this
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
            if (barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

struct MANGOS_DLL_DECL mob_toc_nerubian_burrowerAI: public ScriptedAI
{
    bool m_bIsHeroic;
    bool m_bIs10Man;
    EventMap Events;

    mob_toc_nerubian_burrowerAI(Creature* pCreature): ScriptedAI(pCreature) 
    {
        Difficulty diff = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
        m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
        Reset();
    }

    void SpellHit(Unit *who, const SpellEntry *Spell) 
    {
        if (Spell->Id == SPELL_SUBMERGE_ATTEMPT)
        {
            Creature *FrostSphere = GetClosestCreatureWithEntry(m_creature, NPC_FROST_SPHERE, 8.0f);
            if (!FrostSphere)
            {
                DoCast(m_creature, SPELL_SUBMERGE_BURROWER);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                Events.RescheduleEvent(EVENT_UNSUBMERGE, 1*IN_MILLISECONDS);
            }
        }
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        RESCHEDULE_EVENT(ATTEMPT_SUBMERGE);
        RESCHEDULE_EVENT(EXPOSE_WEAKNESS);
        DoCast(m_creature, SPELL_SPIDER_FRENZY, true);
        if (m_bIsHeroic)
            RESCHEDULE_EVENT(SHADOW_STRIKE);
    }

    void Reset()
    {
        Events.Reset();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ATTEMPT_SUBMERGE:
                    DoCast(m_creature, SPELL_SUBMERGE_ATTEMPT);
                    RESCHEDULE_EVENT(UNSUBMERGE);
                    RESCHEDULE_EVENT(ATTEMPT_SUBMERGE);
                    break;
                case EVENT_EXPOSE_WEAKNESS:
                    DoCast(m_creature->getVictim(), SPELL_EXPOSE_WEAKNESS);
                    RESCHEDULE_EVENT(EXPOSE_WEAKNESS);
                    break;
                case EVENT_SHADOW_STRIKE:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_EXPOSE_WEAKNESS);
                    RESCHEDULE_EVENT(SHADOW_STRIKE);
                    break;
                case EVENT_UNSUBMERGE:
                    m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_BURROWER);
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_toc_swarm_scarabAI: public ScriptedAI
{
    bool m_bIsHeroic;
    bool m_bIs10Man;
    EventMap Events;

    mob_toc_swarm_scarabAI(Creature* pCreature): ScriptedAI(pCreature) 
    {
        Difficulty diff = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = diff == RAID_DIFFICULTY_10MAN_HEROIC || diff == RAID_DIFFICULTY_25MAN_HEROIC;
        m_bIs10Man = diff == RAID_DIFFICULTY_10MAN_NORMAL || diff == RAID_DIFFICULTY_10MAN_HEROIC;
        Reset();
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit *who)
    {
        Events.Reset();
        RESCHEDULE_EVENT(ACID_DRENCHED_MANDIBLES);
        RESCHEDULE_EVENT(DETERMINATION);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_ACID_DRENCHED_MANDIBLES:
                    DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_ACID_DRENCHED_MANDIBLES));
                    RESCHEDULE_EVENT(ACID_DRENCHED_MANDIBLES);
                    break;
                case EVENT_DETERMINATION:
                    DoCast(m_creature, SPELL_DETERMINATION);
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_anubarak_trial()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_anubarak_trial);
    REGISTER_SCRIPT(mob_toc_nerubian_burrower);
    REGISTER_SCRIPT(mob_toc_swarm_scarab);
}
