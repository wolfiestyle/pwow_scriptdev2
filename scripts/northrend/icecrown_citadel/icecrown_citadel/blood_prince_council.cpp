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
SDName: blood_prince_council
SD%Complete: 90%
SDComment: Kinetic Bomb movement choppy, Ball of flames does not shrink over time.
SDCategory: Icecrown Citadel
EndScriptData */

/* ContentData
boss_valanar_ICC
boss_keleseth_ICC
boss_taldaram_ICC
EndContentData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                       = 26662,
    SPELL_SHADOW_PRISON                 = 73001,

    // Valanar
    SPELL_SHOCK_VORTEX                  = 72037,
    SPELL_SHOCK_VORTEX_AURA             = 71945,
    SPELL_SHOCK_VORTEX_VISUAL           = 72633,
    SPELL_EMPOWERED_SHOCK_VORTEX        = 72039,
    SPELL_UNSTABLE                      = 72059,
    SPELL_KINETIC_BOMB                  = 72053,
    SPELL_KINETIC_BOMB_SUMMON           = 72080,
    SPELL_KINETIC_BOMB_VISUAL           = 72054,
    SPELL_KINETIC_BOMB_EXPLOSION        = 72052,
    SPELL_INVOCATION_OF_BLOOD_VALANAR   = 70952,

    // Keleseth
    SPELL_SHADOW_LANCE                  = 71405,
    SPELL_EMPOWERED_SHADOW_LANCE        = 71815,
    SPELL_SHADOW_RESONANCE_NPC_SUMMON   = 71943,
    SPELL_INVOCATION_OF_BLOOD_KELESETH  = 70981,
    // Dark Nucleus
    SPELL_SHADOW_RESONANCE              = 71822,

    // Taldaram
    SPELL_GLITTERING_SPARKS             = 71807,
    SPELL_FLAMES_END_DAMAGE             = 71393,
    SPELL_CONJURE_FLAMES                = 71718,
    SPELL_CONJURE_EMPOWERED_FLAMES      = 72040,
    SPELL_EMPOWERED_FLAMES_BUFF         = 71709,
    SPELL_FLAMES_VISUAL                 = 71706,
    SPELL_INVOCATION_OF_BLOOD_TALDARAM  = 70982,
};

enum Npcs
{
    // Valanar
    NPC_SHOCK_VORTEX                    = 38422,
    NPC_KINETIC_BOMB_TARGET             = 38458,
    NPC_KINETIC_BOMB                    = 38454,

    // Keleseth
    NPC_DARK_NUCLEUS                    = 38369,

    // Taldaram
    NPC_BALL_OF_FLAMES                  = 38332,
    NPC_BALL_OF_INFERNO_FLAMES          = 38451,
};

enum Says
{    
    SAY_LANATHEL_INTRO1                 = -1300503,
    SAY_LANATHEL_INTRO2                 = -1300504,
    SAY_KELESETH_INVOCATION             = -1300505,
    SAY_KELESETH_DARK_NUCLEUS           = -1300506,
    SAY_KELESETH_KILLED_PLAYER1         = -1300507,
    SAY_KELESETH_KILLED_PLAYER2         = -1300508,
    SAY_KELESETH_BERSERK                = -1300509,
    SAY_KELESETH_DEATH                  = -1300510,
    SAY_TALDARAM_INVOCATION             = -1300511,
    SAY_TALDARAM_CONJURE_FLAMES         = -1300512,
    SAY_TALDARAM_KILLED_PLAYER1         = -1300513,
    SAY_TALDARAM_KILLED_PLAYER2         = -1300514,
    SAY_TALDARAM_BERSERK                = -1300515,
    SAY_TALDARAM_DEATH                  = -1300516,
    SAY_VALANAR_INVOCATION              = -1300517,
    SAY_VALANAR_SHOCK_VORTEX            = -1300518,
    SAY_VALANAR_KILLED_PLAYER1          = -1300519,
    SAY_VALANAR_KILLED_PLAYER2          = -1300520,
    SAY_VALANAR_BERSERK                 = -1300521,
    SAY_VALANAR_DEATH                   = -1300522,
    BOSSEMOTE_INVOCATION_TO_KELESETH    = -1300523,
    BOSSEMOTE_INVOCATION_TO_VALANAR     = -1300524,
    BOSSEMOTE_INVOCATION_TO_TALDARAM    = -1300525,
    BOSSEMOTE_VALANAR_EMPOWERED_SHOCK_VORTEX = -1300526,
};

enum Events
{
    EVENT_BERSERK = 1,
    EVENT_CHANGE_INVOCATION,

    // Valanar
    EVENT_START_VORTEX,
    EVENT_SHOCK_VORTEX,
    EVENT_KINETIC_BOMB,

    // Keleseth
    EVENT_DARK_NUCLEUS,
    EVENT_SHADOW_LANCE,

    // Taldaram
    EVENT_GLITTERING_SPARKS,
    EVENT_BALL_OF_FLAMES,

    // Messages
    MESSAGE_AGGRO,
    MESSAGE_DIE,
};

static const float CenterSummonPoint[3] = { 4658.3f, 2770.0f, 365.0f };
static const float BossEntries[3]       = { NPC_VALANAR, NPC_KELESETH, NPC_TALDARAM};

#define TIMER_BERSERK           10*MINUTE*IN_MILLISECONDS
#define TIMER_CHANGE_INVOCATION 47*IN_MILLISECONDS

// Valanar
#define TIMER_START_VORTEX      3*IN_MILLISECONDS
#define TIMER_SHOCK_VORTEX      16.5*IN_MILLISECONDS, 21.5*IN_MILLISECONDS
#define TIMER_KINETIC_BOMB      24*IN_MILLISECONDS          // 2/3 bombs always up [wowwiki] so 60/2.5 sec timer.
#define TIMER_VORTEX_DECAY      3*IN_MILLISECONDS
#define TIMER_MOVE_KINETIC_BOMB 1*IN_MILLISECONDS

// Keleseth
#define TIMER_DARK_NUCLEUS      14*IN_MILLISECONDS
#define TIMER_SHADOW_LANCE      1.7*IN_MILLISECONDS

// Taldaram
#define TIMER_GLITTERING_SPARKS 15*IN_MILLISECONDS          // guessed, cannot find any timer references
#define TIMER_BALL_OF_FLAMES    20*IN_MILLISECONDS, 30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_valanar_ICCAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    uint32 CurrInvocationBoss;
    uint32 IntroTimer;
    bool HasDoneIntro;

    boss_valanar_ICCAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        CurrInvocationBoss(0),
        IntroTimer(0),
        HasDoneIntro(false)
    {
        DoCast(pCreature, SPELL_INVOCATION_OF_BLOOD_VALANAR, true);
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!HasDoneIntro && !IntroTimer && pWho && pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack())
            if (Creature *Lanathel = GET_CREATURE(TYPE_LANATHEL))    
            {
                DoScriptText(SAY_LANATHEL_INTRO1, Lanathel);
                IntroTimer = 14*IN_MILLISECONDS;
            }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_AGGRO);
        BroadcastScriptMessage(m_creature, 200.0f, NPC_VALANAR, m_creature->GetHealth(), true);
        CurrInvocationBoss = 0;
        m_BossEncounter = IN_PROGRESS;
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(CHANGE_INVOCATION);
        SCHEDULE_EVENT_R(SHOCK_VORTEX);
        SCHEDULE_EVENT(KINETIC_BOMB);
        if (m_bIsHeroic && pWho)
            DoCast(pWho, SPELL_SHADOW_PRISON);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_VALANAR_KILLED_PLAYER1 : SAY_VALANAR_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_DIE);
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_VALANAR_DEATH, m_creature);
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm)
        {
            switch (pSumm->GetEntry())
            {
                case NPC_SHOCK_VORTEX:
                    pSumm->setFaction(m_creature->getFaction());
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    pSumm->CastSpell(pSumm, SPELL_SHOCK_VORTEX_VISUAL, true);
                    pSumm->ForcedDespawn(30*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_START_VORTEX, TIMER_START_VORTEX);
                    break;
                case NPC_KINETIC_BOMB_TARGET:
                    pSumm->setFaction(m_creature->getFaction());
                    pSumm->CastSpell(pSumm, SPELL_UNSTABLE, true);
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    if (Creature *Bomb = SummonMgr.SummonCreatureAt(pSumm, NPC_KINETIC_BOMB, TEMPSUMMON_TIMED_DESPAWN, MINUTE*IN_MILLISECONDS, 0.0f, 0.0f, 20.0f))
                    {
                        Bomb->SetOwnerGUID(pSumm->GetGUID());
                        Bomb->CastSpell(pSumm, SPELL_UNSTABLE, true);
                        Bomb->CastSpell(pSumm, SPELL_KINETIC_BOMB_VISUAL, true);
                    }
                    break;
            }
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
        }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) 
    {
        if (!m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_VALANAR))
            m_creature->SetHealth(uiDamage+1);
    }

    void ScriptMessage(WorldObject *pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_AGGRO:
                m_creature->SetInCombatWithZone();
                return;
            case MESSAGE_DIE:
                m_creature->SetHealth(0);
                m_creature->setDeathState(JUST_DIED);
                return;
        }
        if (pSender && pSender->GetEntry() == NPC_VALANAR)
        {
            if (data1 == m_creature->GetEntry())
            {
                m_creature->SetHealth(data2);
                DoCast(m_creature, SPELL_INVOCATION_OF_BLOOD_VALANAR, true);
                DoScriptText(SAY_VALANAR_INVOCATION, m_creature);
                DoScriptText(BOSSEMOTE_INVOCATION_TO_VALANAR, m_creature);
            }
            else
            {
                m_creature->SetHealth(1);
                m_creature->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_VALANAR);
            }
            return;
        }
        ScriptEventInterface::ScriptMessage(pSender, data1, data2);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (IntroTimer)
            if (IntroTimer <= uiDiff)
            {
                if (Creature *Lanathel = GET_CREATURE(TYPE_LANATHEL))
                {
                    DoScriptText(SAY_LANATHEL_INTRO2, Lanathel);
                    HasDoneIntro = true;
                    IntroTimer = 0;
                }
            }
            else
                IntroTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_VALANAR_BERSERK, m_creature);
                    break;
                case EVENT_CHANGE_INVOCATION:
                {
                    uint32 NextInvocationBoss;
                    do
                        NextInvocationBoss = urand(0, 2);
                    while (NextInvocationBoss == CurrInvocationBoss);
                    uint32 NextHp = 1;
                    if (Creature *NextBoss = GET_CREATURE(TYPE_VALANAR + CurrInvocationBoss))
                        NextHp = NextBoss->GetHealth();
                    BroadcastScriptMessage(m_creature, 200, BossEntries[NextInvocationBoss], NextHp, true);
                    CurrInvocationBoss = NextInvocationBoss;
                    break;
                }
                case EVENT_SHOCK_VORTEX:
                    if (m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_VALANAR))
                        DoCast(m_creature->getVictim(), SPELL_EMPOWERED_SHOCK_VORTEX);
                    else if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        DoCast(Target, SPELL_SHOCK_VORTEX);
                        DoScriptText(BOSSEMOTE_VALANAR_EMPOWERED_SHOCK_VORTEX, m_creature);
                    }
                    DoScriptText(SAY_VALANAR_SHOCK_VORTEX, m_creature);
                    break;
                case EVENT_START_VORTEX:
                {
                    std::list<Creature*> Vortexes;
                    SummonMgr.GetAllSummonsWithId(Vortexes, NPC_SHOCK_VORTEX);
                    for (std::list<Creature*>::const_iterator i = Vortexes.begin(); i!= Vortexes.end(); ++i)
                        if (!(*i)->HasAura(SPELL_SHOCK_VORTEX_AURA))
                            (*i)->CastSpell(*i, SPELL_SHOCK_VORTEX_AURA, true);
                    break;
                }
                case EVENT_KINETIC_BOMB:
                    m_creature->CastSpell(CenterSummonPoint[0], CenterSummonPoint[1], CenterSummonPoint[2], SPELL_KINETIC_BOMB, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_kinetic_bombAI: public ScriptedAI
{
    uint32 MoveTimer;
    bool m_bIsHeroic;

    mob_kinetic_bombAI(Creature *pCreature): 
        ScriptedAI(pCreature),
        MoveTimer(TIMER_MOVE_KINETIC_BOMB),
        m_bIsHeroic(m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC || m_creature->GetMap()->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
    {
        DoCast(m_creature, SPELL_KINETIC_BOMB_VISUAL, true);
        m_creature->SetSplineFlags(SPLINEFLAG_UNKNOWN7); //Fly? - tried with different flags, all same (Movement in Z axis choppy)
    }

    void Reset() {}

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) 
    {
        DoCast(m_creature, SPELL_KINETIC_BOMB_VISUAL, true);
        uiDamage = 0;
        m_creature->StopMoving();
        m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 7.0f);
    }

    void AttackedBy(Unit *pAttacker) {}

    void UpdateAI(uint32 const uiDiff)
    {
        if (MoveTimer < uiDiff)
            m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() - (m_bIsHeroic ? 1.0f : 0.5f));
        else
            MoveTimer -= uiDiff;

        if (Unit* Owner = m_creature->GetOwner())
            if (Owner->GetPositionZ() + 2.0f >= m_creature->GetPositionZ())
            {
                Owner->CastSpell(Owner, SPELL_KINETIC_BOMB_EXPLOSION, true);
                m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 9.0f);
            }
    }
};

struct MANGOS_DLL_DECL boss_keleseth_ICCAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;

    boss_keleseth_ICCAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature)
    {
        SetCombatMovement(false);
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
    }

    void Aggro(Unit* pWho)
    {
        m_BossEncounter = IN_PROGRESS;
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_AGGRO);
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(DARK_NUCLEUS);
        SCHEDULE_EVENT(SHADOW_LANCE);
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KELESETH_KILLED_PLAYER1 : SAY_KELESETH_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_DIE);
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_KELESETH_DEATH, m_creature);
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm && pSumm->GetEntry() == NPC_DARK_NUCLEUS)
            SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) 
    {
        if (!m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_KELESETH))
            m_creature->SetHealth(uiDamage+1);
    }

    void ScriptMessage(WorldObject *pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_AGGRO:
                m_creature->SetInCombatWithZone();
                return;
            case MESSAGE_DIE:
                m_creature->SetHealth(0);
                m_creature->setDeathState(JUST_DIED);
                return;
        }
        if (pSender && pSender->GetEntry() == NPC_VALANAR)
        {
            if (data1 == m_creature->GetEntry())
            {
                m_creature->SetHealth(data2);
                DoCast(m_creature, SPELL_INVOCATION_OF_BLOOD_KELESETH, true);
                DoScriptText(SAY_KELESETH_INVOCATION, m_creature);
                DoScriptText(BOSSEMOTE_INVOCATION_TO_KELESETH, m_creature);
            }
            else
            {
                m_creature->SetHealth(1);
                m_creature->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_KELESETH);
            }
            return;
        }
        ScriptEventInterface::ScriptMessage(pSender, data1, data2);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || OutOfCombatAreaCheck())
            return;

        if (m_creature->IsWithinDist(m_creature->getVictim(), 30.0f))
            DoStartNoMovement(m_creature->getVictim());
        else
            DoStartMovement(m_creature->getVictim());

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_KELESETH_BERSERK, m_creature);
                    break;
                case EVENT_DARK_NUCLEUS:
                    m_creature->CastSpell(CenterSummonPoint[0], CenterSummonPoint[1], CenterSummonPoint[2], SPELL_SHADOW_RESONANCE_NPC_SUMMON, false);
                    DoScriptText(SAY_KELESETH_DARK_NUCLEUS, m_creature);
                    break;
                case EVENT_SHADOW_LANCE:
                    DoCast(m_creature->getVictim(), m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_KELESETH) ? SPELL_EMPOWERED_SHADOW_LANCE : SPELL_SHADOW_LANCE);
                    break;
                default:
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_dark_nucleusAI: public ScriptedAI
{
    uint32 DecayTimer;

    mob_dark_nucleusAI(Creature *pCreature): 
        ScriptedAI(pCreature),
        DecayTimer(TIMER_VORTEX_DECAY)
    {
    }

    void Reset() {}

    void MoveInLineOfSight(Unit *pWho)
    {
        if (pWho && m_creature->getVictim() == pWho)
            if (m_creature->IsWithinDist(pWho, 15.0f))
            {
                if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                {
                    DoStartNoMovement(pWho);
                    SetCombatMovement(false);
                    DoCast(pWho, SPELL_SHADOW_RESONANCE);
                    return;
                }
            }
            else
            {
                m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);
                DoStartMovement(pWho);
                return;
            }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustDied(Unit *pKiller)
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (DecayTimer < uiDiff)
        {
            if (m_creature->GetHealthPercent() < 5.0f)
            {
                m_creature->ForcedDespawn();
                return;
            }
            m_creature->SetHealthPercent(m_creature->GetHealthPercent() - 5.0f);
            DecayTimer = TIMER_VORTEX_DECAY;
        }
        else DecayTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL boss_taldaram_ICCAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;

    boss_taldaram_ICCAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature)
    {
    }

    void Reset()
    {
        boss_icecrown_citadelAI::Reset();
        SummonMgr.UnsummonAll();
    }

    void Aggro(Unit* pWho)
    {
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_AGGRO);
        m_BossEncounter = IN_PROGRESS;
        SCHEDULE_EVENT(BERSERK);
        SCHEDULE_EVENT(GLITTERING_SPARKS);
        SCHEDULE_EVENT_R(BALL_OF_FLAMES);
    }

    void JustSummoned(Creature *pSumm)
    {
        if (pSumm)
            switch (pSumm->GetEntry())
            {
                case NPC_BALL_OF_INFERNO_FLAMES:
                    pSumm->CastSpell(pSumm, SPELL_EMPOWERED_FLAMES_BUFF, true);
                case NPC_BALL_OF_FLAMES:
                    pSumm->CastSpell(pSumm, SPELL_FLAMES_VISUAL, true);
                    pSumm->SetInCombatWithZone();
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        pSumm->AddThreat(Target, 50000.0f);
                        pSumm->Attack(Target, true);
                    }
                    pSumm->SetSpeedRate(MOVE_RUN, 2.0f);
                    pSumm->setFaction(m_creature->getFaction());
                    SummonMgr.AddSummonToList(pSumm->GetObjectGuid());
                    break;
                default:
                    break;
            }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        if (pSumm)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_TALDARAM_KILLED_PLAYER1 : SAY_TALDARAM_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        BroadcastScriptMessage(m_creature, 200.0f, MESSAGE_DIE);
        SummonMgr.UnsummonAll();
        m_BossEncounter = DONE;
        DoScriptText(SAY_TALDARAM_DEATH, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage) 
    {
        if (!m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_TALDARAM))
            m_creature->SetHealth(uiDamage+1);
    }

    void ScriptMessage(WorldObject *pSender, uint32 data1, uint32 data2)
    {
        switch (data1)
        {
            case MESSAGE_AGGRO:
                m_creature->SetInCombatWithZone();
                return;
            case MESSAGE_DIE:
                m_creature->SetHealth(0);
                m_creature->setDeathState(JUST_DIED);
                return;
        }
        if (pSender && pSender->GetEntry() == NPC_VALANAR)
        {
            if (data1 == m_creature->GetEntry())
            {
                m_creature->SetHealth(data2);
                DoCast(m_creature, SPELL_INVOCATION_OF_BLOOD_TALDARAM, true);
                DoScriptText(SAY_TALDARAM_INVOCATION, m_creature);
                DoScriptText(BOSSEMOTE_INVOCATION_TO_TALDARAM, m_creature);
            }
            else
            {
                m_creature->SetHealth(1);
                m_creature->RemoveAurasDueToSpell(SPELL_INVOCATION_OF_BLOOD_TALDARAM);
            }
            return;
        }
        ScriptEventInterface::ScriptMessage(pSender, data1, data2);
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
                    DoScriptText(SAY_TALDARAM_BERSERK, m_creature);
                    break;
                case EVENT_GLITTERING_SPARKS:
                    if (Unit *Target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCast(Target, SPELL_GLITTERING_SPARKS);
                    break;
                case EVENT_BALL_OF_FLAMES:
                    DoCast(m_creature, m_creature->HasAura(SPELL_INVOCATION_OF_BLOOD_TALDARAM) ? SPELL_CONJURE_EMPOWERED_FLAMES : SPELL_CONJURE_FLAMES);
                    DoScriptText(SAY_TALDARAM_CONJURE_FLAMES, m_creature);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_ball_of_flamesAI: public ScriptedAI
{
    bool m_bIsUsed;

    mob_ball_of_flamesAI(Creature *pCreature):
        ScriptedAI(pCreature),
        m_bIsUsed(false)
    {
    }

    void Reset() {}

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bIsUsed)
            return;

        if (m_creature->getVictim())
        {
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            if (m_creature->IsWithinDist(m_creature->getVictim(), 6.0f))
            {
                m_bIsUsed = true;
                DoCast(m_creature->getVictim(), SPELL_FLAMES_END_DAMAGE, true);
                m_creature->ForcedDespawn(100);
            }
        }

        DoMeleeAttackIfReady();
    }
};

void AddSC_blood_prince_council()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_valanar_ICC);
    REGISTER_SCRIPT(boss_keleseth_ICC);
    REGISTER_SCRIPT(boss_taldaram_ICC);
    REGISTER_SCRIPT(mob_kinetic_bomb);
    REGISTER_SCRIPT(mob_dark_nucleus);
    REGISTER_SCRIPT(mob_ball_of_flames);
}
