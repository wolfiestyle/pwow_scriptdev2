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
SDName: boss_lady_deathwhisper
SD%Complete: 0%
SDComment: curse of torpor not working, dominate mind not working
SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum Spells
{
    SPELL_BERSERK                   = 26662,
    SPELL_DEATH_AND_DECAY           = 71001,
    SPELL_MANA_BARRIER              = 70842,
    SPELL_SHADOWBOLT                = 71254,
    SPELL_DOMINATE_MIND             = 71289, //every difficulty except N10
    SPELL_FROSTBOLT                 = 71420,
    SPELL_FROST_VOLLEY              = 72905,
    SPELL_TOUCH_OF_INSIGNIFICANCE   = 71204,

    SPELL_DARK_TRANSFORMATION       = 70900,
    SPELL_DARK_EMPOWERMENT          = 70901,
    SPELL_DARK_MARTYRDOM            = 70903,

    //Adherants
    SPELL_FROST_FEVER               = 71129,
    SPELL_SHROUD_OF_THE_OCCULT      = 70768,
    SPELL_DEATHCHILL_BOLT           = 70594,
    SPELL_DEATHCHILL_BLAST          = 70906,
    SPELL_CURSE_OF_TORPOR           = 71237,
    SPELL_ADHERENTS_DETERMINATION   = 71234,

    //Fanatics
    SPELL_NECROTIC_STRIKE           = 70659,
    SPELL_VAMPIRIC_MIGHT            = 70674,
    SPELL_SHADOW_CLEAVE             = 70670,
    SPELL_FANATICS_DETERMINATION    = 71235,

    //Vengeful shade
    SPELL_VENGEFUL_BLAST            = 72011,
};

enum NpcIds
{
    NPC_CULT_ADHERENT               = 37949,
    NPC_EMPOWERED_ADHERANT          = 38136,
    NPC_REANIMATED_ADHERANT         = 38010,

    NPC_CULT_FANATIC                = 37890,
    NPC_DEFORMED_FANATIC            = 38135,
    NPC_REANIMATED_FANATIC          = 38009,

    NPC_VENGEFUL_SHADE              = 38222,
};

enum Says
{
    SAY_INTRO1                      = -1300410,
    SAY_INTRO2                      = -1300411,
    SAY_INTRO3                      = -1300412,
    SAY_INTRO4                      = -1300413,
    SAY_INTRO5                      = -1300414,
    SAY_INTRO6                      = -1300415,
    SAY_INTRO7                      = -1300416,
    SAY_AGGRO                       = -1300417,
    SAY_PHASE2                      = -1300418,
    SAY_DOMINATE_MIND               = -1300419,
    SAY_DARK_EMPOWERMENT            = -1300420,
    SAY_DARK_TRANSFORMATION         = -1300421,
    SAY_ANIMATE_DEAD                = -1300422,
    SAY_KILLED_PLAYER1              = -1300423,
    SAY_KILLED_PLAYER2              = -1300424,
    SAY_BERSERK                     = -1300425,
    SAY_DEATH                       = -1300426,
};

enum Events
{
    EVENT_BERSERK = 1, 
    EVENT_SHADOWBOLT, 
    EVENT_DEATH_AND_DECAY,
    EVENT_DOMINATE_MIND,
    EVENT_FROSTBOLT,
    EVENT_FROST_VOLLEY,
    EVENT_TOUCH_OF_INSIGNIFICANCE,
    EVENT_SUMMON_SHADE,

    EVENT_SUMMON_ADDS,
    EVENT_BUFF_ADD,

    //fanatics
    EVENT_SHADOW_CLEAVE,
    EVENT_NECROTIC_STRIKE,
    EVENT_VAMPIRIC_MIGHT,

    //adherents
    EVENT_CURSE_OF_TORPOR,
    EVENT_DEATHCHILL_CAST,
    EVENT_FROST_FEVER,
    EVENT_SHROUD_OF_THE_OCCULT,
};

static const float XSummonPositions[3] = {-618.7f, -599.4f, -578.2f};

#define LEFT_SUMMON_Y                   2155.5f
#define RIGHT_SUMMON_Y                  2267.1f

#define SUMMON_FLOOR_HEIGHT             50.85f

static const float EntranceSummonPosition[3] = {-519.7f, 2210.8f, 62.8f};

#define TIMER_BERSERK                   10*MINUTE*IN_MILLISECONDS
#define TIMER_SHADOWBOLT                2500
#define TIMER_DEATH_AND_DECAY           urand(19,22)*IN_MILLISECONDS
#define TIMER_DOMINATE_MIND             40*IN_MILLISECONDS
#define TIMER_SUMMON_ADDS               60*IN_MILLISECONDS
#define TIMER_BUFF_ADD                  urand(3,80)*IN_MILLISECONDS //'randomly' buffs adds
#define TIMER_FROSTBOLT                 urand(9,11)*IN_MILLISECONDS
#define TIMER_FROST_VOLLEY              urand(18,23)*IN_MILLISECONDS
#define TIMER_TOUCH_OF_INSIGNIFICANCE   urand(6,7)*IN_MILLISECONDS
#define TIMER_SUMMON_SHADE              urand(7,10)*IN_MILLISECONDS
#define TIMER_SHADOW_CLEAVE             6*IN_MILLISECONDS
#define TIMER_NECROTIC_STRIKE           20*IN_MILLISECONDS
#define TIMER_VAMPIRIC_MIGHT            urand(20,100)*IN_MILLISECONDS
#define TIMER_CURSE_OF_TORPOR           urand(20,100)*IN_MILLISECONDS
#define TIMER_FROST_FEVER               urand(30,35)*IN_MILLISECONDS
#define TIMER_DEATHCHILL_CAST           3*IN_MILLISECONDS
#define TIMER_SHROUD_OF_THE_OCCULT      urand(25,70)*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_lady_deathwhisperAI: public boss_icecrown_citadelAI
{
    SummonManager SummonMgr;
    bool HasDoneIntro :1;
    bool IsPhase1 :1;
    bool LastSummonSideIsLeft :1;
    uint32 IntroPhase;
    uint32 IntroTimer;

    boss_lady_deathwhisperAI(Creature* pCreature):
        boss_icecrown_citadelAI(pCreature),
        SummonMgr(pCreature),
        HasDoneIntro(false),
        IsPhase1(true),
        LastSummonSideIsLeft(false),
        IntroPhase(0),
        IntroTimer(0)
    {
    }

    void Reset()
    {
        IsPhase1 = true;
        LastSummonSideIsLeft = false;
        SummonMgr.UnsummonAll();
        boss_icecrown_citadelAI::Reset();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!HasDoneIntro && pWho && pWho->GetTypeId() == TYPEID_PLAYER && !IntroPhase)
        {
            IntroPhase = 1;
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        IsPhase1 = true;
        DoScriptText(SAY_AGGRO, m_creature);
        //if (!m_bIs10Man || m_bIsHeroic)
        //    RESCHEDULE_EVENT(DOMINATE_MIND); // spell doesn't work!
        RESCHEDULE_EVENT(BERSERK);
        RESCHEDULE_EVENT(SHADOWBOLT);
        RESCHEDULE_EVENT(DEATH_AND_DECAY);
        RESCHEDULE_EVENT(BUFF_ADD);
        Events.ScheduleEvent(EVENT_SUMMON_ADDS, 5*IN_MILLISECONDS);
        DoCast(m_creature, SPELL_MANA_BARRIER);
        m_BossEncounter = IN_PROGRESS;
    }

    void DamageTaken(Unit* pAttacker, uint32 &damage)
    {
        if (!m_creature->HasAura(SPELL_MANA_BARRIER))
            return;
        if (m_creature->GetPower(POWER_MANA) < damage)
        {
            damage -= m_creature->GetPower(POWER_MANA);
            m_creature->SetPower(POWER_MANA, 0);
            m_creature->RemoveAurasDueToSpell(SPELL_MANA_BARRIER);
            IsPhase1 = false;
            DoStartMovement(m_creature->getVictim());
            DoScriptText(SAY_PHASE2, m_creature);
            Events.CancelEvent(EVENT_SHADOWBOLT);
            if (!m_bIsHeroic)
            {
                Events.CancelEvent(EVENT_SUMMON_ADDS);
                Events.CancelEvent(EVENT_BUFF_ADD);
            }
            RESCHEDULE_EVENT(FROST_VOLLEY);
            RESCHEDULE_EVENT(FROSTBOLT);
            RESCHEDULE_EVENT(TOUCH_OF_INSIGNIFICANCE);
            RESCHEDULE_EVENT(SUMMON_SHADE);
            m_creature->MonsterTextEmote("Lady Deathwhisper's Mana Barrier shimmers and fades away!", NULL, true);
        }
        else
        {
            m_creature->SetPower(POWER_MANA, m_creature->GetPower(POWER_MANA) - damage);
            damage = 0;
        }
    }

    void ChangeSummonTo(Creature *OriginalSummon, uint32 NextSummonId)
    {
        if (!OriginalSummon)
            return;
        if (Creature *pSummon = SummonMgr.SummonCreatureAt(OriginalSummon, NextSummonId, TEMPSUMMON_CORPSE_DESPAWN))
        {
            SummonMgr.UnsummonCreature(OriginalSummon);
            pSummon->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature *pSumm)
    {
        SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        SummonMgr.UnsummonAll();
        DoScriptText(SAY_DEATH, m_creature);
        m_BossEncounter = DONE;
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (IntroPhase && !HasDoneIntro)
        {
            if (IntroTimer < uiDiff)
            {
                switch (IntroPhase)
                {
                    case 1:
                        DoScriptText(SAY_INTRO1, m_creature);
                        IntroTimer = 11*IN_MILLISECONDS;
                        break;
                    case 2:
                        DoScriptText(SAY_INTRO2, m_creature);
                        IntroTimer = 10*IN_MILLISECONDS;
                        break;
                    case 3:
                        DoScriptText(SAY_INTRO3, m_creature);
                        IntroTimer = 10*IN_MILLISECONDS;
                        break;
                    case 4:
                        DoScriptText(SAY_INTRO4, m_creature);
                        IntroTimer = 8*IN_MILLISECONDS;
                        break;
                    case 5:
                        DoScriptText(SAY_INTRO5, m_creature);
                        IntroTimer = 8*IN_MILLISECONDS;
                        break;
                    case 6:
                        DoScriptText(SAY_INTRO6, m_creature);
                        IntroTimer = 10*IN_MILLISECONDS;
                        break;
                    case 7:
                        DoScriptText(SAY_INTRO7, m_creature);
                        IntroTimer = 16*IN_MILLISECONDS;
                        break;
                    case 8:
                        HasDoneIntro = true;
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        break;
                }
                IntroPhase++;
            }
            else
                IntroTimer -= uiDiff;
            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsPhase1)
            DoStartNoMovement(m_creature->getVictim());

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_BERSERK:
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature, SPELL_BERSERK);
                    DoScriptText(SAY_BERSERK, m_creature);
                    break;
                case EVENT_SHADOWBOLT:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_SHADOWBOLT);
                    RESCHEDULE_EVENT(SHADOWBOLT);
                    break;
                case EVENT_DEATH_AND_DECAY:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_DEATH_AND_DECAY);
                    RESCHEDULE_EVENT(DEATH_AND_DECAY);
                    break;
                case EVENT_DOMINATE_MIND:
                    for (uint32 count = !m_bIs10Man && m_bIsHeroic ? 3 : 1; count; --count)
                        if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_DOMINATE_MIND);
                    DoScriptText(SAY_DOMINATE_MIND, m_creature);
                    RESCHEDULE_EVENT(DOMINATE_MIND);
                    break;
                case EVENT_SUMMON_ADDS:
                    if (m_bIs10Man)
                    {
                        bool IsFanatic = LastSummonSideIsLeft;
                        for (uint32 i = 0; i < 3; i++)
                        {
                            Creature *pSumm = SummonMgr.SummonCreature(IsFanatic ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT,
                                    XSummonPositions[i], LastSummonSideIsLeft ? RIGHT_SUMMON_Y : LEFT_SUMMON_Y, SUMMON_FLOOR_HEIGHT, 0,
                                    TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000);
                            if (pSumm)
                            {
                                pSumm->SetInCombatWithZone();
                                pSumm->SetOwnerGUID(m_creature->GetGUID());
                            }
                            IsFanatic = !IsFanatic;
                        }
                        LastSummonSideIsLeft = !LastSummonSideIsLeft;
                    }
                    else
                    {
                        bool IsFanatic = true;
                        //two sides
                        for (uint32 j = 0; j <= 1; j++)
                            for (uint32 i = 0; i < 3; i++)
                            {
                                Creature *pSumm = SummonMgr.SummonCreature(IsFanatic ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT,
                                        XSummonPositions[i], j ? RIGHT_SUMMON_Y : LEFT_SUMMON_Y, SUMMON_FLOOR_HEIGHT, 0,
                                        TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000);
                                if (pSumm)
                                {
                                    pSumm->SetInCombatWithZone();
                                    pSumm->SetOwnerGUID(m_creature->GetGUID());
                                }
                                IsFanatic = !IsFanatic;
                            }
                        //entrance
                        Creature *pSumm = SummonMgr.SummonCreature(urand(0,1) ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT,
                                EntranceSummonPosition[0], EntranceSummonPosition[1], EntranceSummonPosition[2], 0,
                                TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000);
                        if (pSumm)
                        {
                            pSumm->SetInCombatWithZone();
                            pSumm->SetOwnerGUID(m_creature->GetGUID());
                        }
                    }
                    RESCHEDULE_EVENT(SUMMON_ADDS);
                    break;
                case EVENT_BUFF_ADD:
                {
                    RESCHEDULE_EVENT(BUFF_ADD);

                    std::vector<ObjectGuid> ApplicableSummons;
                    ApplicableSummons.reserve(SummonMgr.GetSummonCount());
                    SummonMgr.GetAllSummonsWithId(ApplicableSummons, NPC_CULT_FANATIC);
                    SummonMgr.GetAllSummonsWithId(ApplicableSummons, NPC_CULT_ADHERENT);
                    if (ApplicableSummons.empty())
                        break;

                    //select a random list element
                    uint32 RandomPlace = urand(0, ApplicableSummons.size()-1);
                    Creature *pTarget = m_creature->GetMap()->GetCreature(ApplicableSummons[RandomPlace]);
                    if (!pTarget)
                        break;

                    if (urand(0, 2))
                    {
                        if (pTarget->GetEntry() == NPC_CULT_FANATIC)
                        {
                            DoScriptText(SAY_DARK_TRANSFORMATION, m_creature);
                            pTarget->CastSpell(pTarget, SPELL_DARK_TRANSFORMATION, false);
                        }
                        else
                        {
                            DoScriptText(SAY_DARK_EMPOWERMENT, m_creature);
                            pTarget->CastSpell(pTarget, SPELL_DARK_EMPOWERMENT, false);
                        }
                    }
                    else
                    {
                        DoScriptText(SAY_ANIMATE_DEAD, m_creature);
                        pTarget->CastSpell(pTarget, SPELL_DARK_MARTYRDOM, false);
                    }
                    break;
                }
                case EVENT_FROSTBOLT:
                    if (Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_FROSTBOLT);
                    RESCHEDULE_EVENT(FROSTBOLT);
                    break;
                case EVENT_TOUCH_OF_INSIGNIFICANCE:
                    DoCast(m_creature->getVictim(), SPELL_TOUCH_OF_INSIGNIFICANCE);
                    RESCHEDULE_EVENT(TOUCH_OF_INSIGNIFICANCE);
                    break;
                case EVENT_FROST_VOLLEY:
                    DoCast(m_creature->getVictim(), SPELL_FROST_VOLLEY);
                    RESCHEDULE_EVENT(FROST_VOLLEY);
                    break;
                case EVENT_SUMMON_SHADE:
                {
                    float x, y;
                    GetRandomPointInCircle(x, y, 40.0f, m_creature->GetPositionX(), m_creature->GetPositionY());
                    Creature *pSumm = SummonMgr.SummonCreature(NPC_VENGEFUL_SHADE, x, y, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN);
                    if (pSumm)
                    {
                        pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pSumm->SetInCombatWithZone();
                    }
                    RESCHEDULE_EVENT(SUMMON_SHADE);
                    break;
                }
                default:
                    break;
            }

        if (!IsPhase1)
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_deathwhisper_fanaticAI: public ScriptedAI
{
    EventMap Events;

    mob_deathwhisper_fanaticAI(Creature *pCreature):
        ScriptedAI(pCreature)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_REANIMATED_FANATIC:
                DoCast(m_creature, SPELL_FANATICS_DETERMINATION);
                break;
            case NPC_DEFORMED_FANATIC:
                DoCast(m_creature, SPELL_DARK_TRANSFORMATION, true);
                break;
        }
    }

    void Reset()
    {
        Events.Reset();
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (!m_creature->GetOwner())
            return;
        switch (pSpell->Id)
        {
            case SPELL_DARK_TRANSFORMATION:
            {
                Creature *pOwner = dynamic_cast<Creature*>(m_creature->GetOwner());
                boss_lady_deathwhisperAI *OwnerAI;
                if (pOwner && (OwnerAI = dynamic_cast<boss_lady_deathwhisperAI*>(pOwner->AI())))
                    OwnerAI->ChangeSummonTo(m_creature, NPC_DEFORMED_FANATIC);
                break;
            }
            case SPELL_DARK_MARTYRDOM:
            {
                Creature *pOwner = dynamic_cast<Creature*>(m_creature->GetOwner());
                boss_lady_deathwhisperAI *OwnerAI;
                if (pOwner && (OwnerAI = dynamic_cast<boss_lady_deathwhisperAI*>(pOwner->AI())))
                    OwnerAI->ChangeSummonTo(m_creature, NPC_REANIMATED_FANATIC);
                break;
            }
        }
    }

    void Aggro(Unit *pWho)
    {
        RESCHEDULE_EVENT(SHADOW_CLEAVE);
        RESCHEDULE_EVENT(NECROTIC_STRIKE);
        RESCHEDULE_EVENT(VAMPIRIC_MIGHT);
    }

    void DamageDeal(Unit *pVictim, uint32& uiDamage)
    {
        if (m_creature->HasAura(SPELL_VAMPIRIC_MIGHT))
            m_creature->SetHealth(m_creature->GetHealth() + uiDamage * 3);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_SHADOW_CLEAVE:
                    DoCast(m_creature->getVictim(), SPELL_SHADOW_CLEAVE);
                    RESCHEDULE_EVENT(SHADOW_CLEAVE);
                    break;
                case EVENT_NECROTIC_STRIKE:
                    DoCast(m_creature->getVictim(), SPELL_NECROTIC_STRIKE);
                    RESCHEDULE_EVENT(NECROTIC_STRIKE);
                    break;
                case EVENT_VAMPIRIC_MIGHT:
                    DoCast(m_creature->getVictim(), SPELL_VAMPIRIC_MIGHT);
                    RESCHEDULE_EVENT(VAMPIRIC_MIGHT);
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_deathwhisper_adherentAI: public ScriptedAI
{
    EventMap Events;

    mob_deathwhisper_adherentAI(Creature *pCreature):
        ScriptedAI(pCreature)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_REANIMATED_ADHERANT:
                DoCast(m_creature, SPELL_ADHERENTS_DETERMINATION, true);
                break;
            case NPC_EMPOWERED_ADHERANT:
                DoCast(m_creature, SPELL_DARK_EMPOWERMENT, true);
                break;
        }
    }

    void Reset() 
    {
        Events.Reset();
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (!m_creature->GetOwner())
            return;
        switch (pSpell->Id)
        {
            case SPELL_DARK_EMPOWERMENT:
            {
                Creature *pOwner = dynamic_cast<Creature*>(m_creature->GetOwner());
                boss_lady_deathwhisperAI *OwnerAI;
                if (pOwner && (OwnerAI = dynamic_cast<boss_lady_deathwhisperAI*>(pOwner->AI())))
                    OwnerAI->ChangeSummonTo(m_creature, NPC_EMPOWERED_ADHERANT);
                break;
            }
            case SPELL_DARK_MARTYRDOM:
            {
                Creature *pOwner = dynamic_cast<Creature*>(m_creature->GetOwner());
                boss_lady_deathwhisperAI *OwnerAI;
                if (pOwner && (OwnerAI = dynamic_cast<boss_lady_deathwhisperAI*>(pOwner->AI())))
                    OwnerAI->ChangeSummonTo(m_creature, NPC_REANIMATED_ADHERANT);
                break;
            }
        }
    }

    void Aggro(Unit *pWho)
    {
        //RESCHEDULE_EVENT(CURSE_OF_TORPOR); //not working at all
        RESCHEDULE_EVENT(DEATHCHILL_CAST);
        RESCHEDULE_EVENT(FROST_FEVER);
        RESCHEDULE_EVENT(SHROUD_OF_THE_OCCULT);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance(m_creature->getVictim()) > 30.0f)
            DoStartMovement(m_creature->getVictim());
        else
            DoStartNoMovement(m_creature->getVictim());

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CURSE_OF_TORPOR:
                    DoCast(m_creature->getVictim(), SPELL_CURSE_OF_TORPOR);
                    RESCHEDULE_EVENT(CURSE_OF_TORPOR);
                    break;
                case EVENT_DEATHCHILL_CAST:
                    DoCast(m_creature->getVictim(), m_creature->GetEntry() == NPC_EMPOWERED_ADHERANT ? SPELL_DEATHCHILL_BLAST : SPELL_DEATHCHILL_BOLT);
                    RESCHEDULE_EVENT(DEATHCHILL_CAST);
                    break;
                case EVENT_FROST_FEVER:
                    DoCast(m_creature->getVictim(), SPELL_FROST_FEVER);
                    RESCHEDULE_EVENT(FROST_FEVER);
                    break;
                case EVENT_SHROUD_OF_THE_OCCULT:
                    DoCast(m_creature, SPELL_SHROUD_OF_THE_OCCULT);
                    RESCHEDULE_EVENT(SHROUD_OF_THE_OCCULT);
                    break;
            }
    }
};

struct MANGOS_DLL_DECL mob_vengeful_shadeAI: public ScriptedAI
{
    uint32 BlastTimer;
    bool IsExploded;

    mob_vengeful_shadeAI (Creature *pCreature):
        ScriptedAI(pCreature),
        BlastTimer(5000),
        IsExploded(false)
    {
    }

    void Reset() {}

    void Explode()
    {
        if (IsExploded)
            return;
        DoCast(m_creature->getVictim(), SPELL_VENGEFUL_BLAST);
        m_creature->ForcedDespawn(500);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (BlastTimer < uiDiff)
        {
            Explode();
            return;
        }
        else
            BlastTimer -= uiDiff;

        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 1.0f))
        {
            Explode();
            return;
        }
    }
};

void AddSC_boss_lady_deathwhisper()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_lady_deathwhisper);
    REGISTER_SCRIPT(mob_deathwhisper_adherent);
    REGISTER_SCRIPT(mob_deathwhisper_fanatic);
    REGISTER_SCRIPT(mob_vengeful_shade);
}
