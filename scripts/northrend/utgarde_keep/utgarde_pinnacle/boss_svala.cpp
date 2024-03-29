/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Svala
SD%Complete: 30%
SDComment: TODO: abilities. The way spells for intro works could use more research.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum Says
{
    // Intro
    SAY_INTRO_1                 = -1575000,
    SAY_INTRO_2_ARTHAS          = -1575001,
    SAY_INTRO_3                 = -1575002,
    SAY_INTRO_4_ARTHAS          = -1575003,
    SAY_INTRO_5                 = -1575004,

    // In-fight
    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,
};

enum NPCs
{
    NPC_SVALA_SORROW            = 26668,
    NPC_ARTHAS_IMAGE            = 29280,
    NPC_RITUAL_CHANNELER        = 27281,
    NPC_SCOURGE_HULK            = 26555,
    NPC_RITUAL_TARGET           = 27327,
};

enum Spells
{
    SPELL_ARTHAS_VISUAL         = 54134,

    // don't know how these should work in relation to each other
    SPELL_TRANSFORMING          = 54205,
    SPELL_TRANSFORMING_FLOATING = 54140,
    SPELL_TRANSFORMING_CHANNEL  = 54142,

    // Svala's spells
    SPELL_RITUAL_OF_SWORD       = 48276,
    SPELL_RITUAL_STRIKE         = 48331, // careful with this spell.. dummy creature can aggro (?)
    SPELL_CALL_FLAMES           = 48258, // spell sends script event (needs to be added to Database)
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,
    SPELL_RITUAL_PREPARATION    = 48267,
    SPELL_SUMMON_CHANNELER_1    = 48271,
    SPELL_SUMMON_CHANNELER_2    = 48274,
    SPELL_SUMMON_CHANNELER_3    = 48275,

    // Summon's spells
    SPELL_PARALIZE              = 48278,
    SPELL_SHADOWS_IN_THE_DARK   = 59407,

    // Scourge Hulk spells
    SPELL_MIGHTY_BLOW           = 48697,
    SPELL_VOLATILE_INFECTION    = 56785,
};

enum Events
{
    EVENT_RITUAL = 1,
    EVENT_SINISTER_STRIKE,
    EVENT_CALL_FLAMES,
    EVENT_SUMMON_CHANNELERS,

    EVENT_KNOCK = 1,
    EVENT_DISEASE,
};

/*######
## boss_svala
######*/

#define TIMER_SINISTER_STRIKE       5*IN_MILLISECONDS, 10*IN_MILLISECONDS
#define TIMER_RITUAL_OF_THE_SWORD   20*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_FLAMES                10*IN_MILLISECONDS, 20*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_svalaAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode :1;
    bool m_bIsIntroDone :1;
    EventManager Events;
    SummonManager SummonMgr;
    ObjectGuid p_Sacrifice;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroCount;

    boss_svalaAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData())),
        m_bIsRegularMode(pCreature->GetMap()->IsRegularDifficulty()),
        m_bIsIntroDone(false),
        SummonMgr(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        Events.Reset();
        SummonMgr.UnsummonAll();

        m_uiIntroTimer = 2500;
        m_uiIntroCount = 0;
        p_Sacrifice.Clear();

        if (m_pInstance)
            m_pInstance->SetData(DATA_ACHIEVEMENT_SVALA, 0);

        if (m_creature->isAlive() && m_pInstance && m_pInstance->GetData(TYPE_SVALA) > IN_PROGRESS)
        {
            if (m_creature->GetEntry() != NPC_SVALA_SORROW)
                m_creature->UpdateEntry(NPC_SVALA_SORROW);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            m_bIsIntroDone = true;
        }
    }

    void JustReachedHome()
    {
        DoMoveToPosition();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bIsIntroDone && m_pInstance && m_pInstance->GetData(TYPE_SVALA) == IN_PROGRESS)
        {
            m_pInstance->SetData(TYPE_SVALA, SPECIAL);

            float fX, fY, fZ;
            m_creature->GetClosePoint(fX, fY, fZ, m_creature->GetObjectBoundingRadius(), 16.0f, 0.0f);

            // we assume m_creature is spawned in proper location
            SummonMgr.SummonCreature(NPC_ARTHAS_IMAGE, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        if (m_creature->HasSplineFlag(SPLINEFLAG_FLYING)) // really needed?
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);

        Events.ScheduleEventInRange(EVENT_SINISTER_STRIKE, TIMER_SINISTER_STRIKE, TIMER_SINISTER_STRIKE);
        Events.ScheduleEvent(EVENT_RITUAL, TIMER_RITUAL_OF_THE_SWORD);
        Events.ScheduleEventInRange(EVENT_CALL_FLAMES, TIMER_FLAMES, TIMER_FLAMES);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ARTHAS_IMAGE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ARTHAS_VISUAL, true);
            pSummoned->SetFacingToObject(m_creature);
        }
        else if (pSummoned->GetEntry() == NPC_RITUAL_CHANNELER)
        {
            SummonMgr.AddSummonToList(pSummoned->GetObjectGuid());
            if (!m_bIsRegularMode)
                pSummoned->CastSpell(pSummoned, SPELL_SHADOWS_IN_THE_DARK, true);
            if (!p_Sacrifice.IsEmpty())
                if (Unit* pSacrificed = pSummoned->GetMap()->GetUnit(p_Sacrifice))
                    pSummoned->CastSpell(pSacrificed, SPELL_PARALIZE, false);
        }
    }

    void SpellHit(Unit* pCaster, SpellEntry const* pSpell)
    {
        if (pSpell->Id == SPELL_TRANSFORMING)
        {
            if (Unit* pArthas = SummonMgr.GetFirstFoundSummonWithId(NPC_ARTHAS_IMAGE))
                pArthas->InterruptNonMeleeSpells(true);

            m_creature->UpdateEntry(NPC_SVALA_SORROW);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        SummonMgr.UnsummonAll();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SVALA, DONE);
    }

    void DoMoveToPosition()
    {
        float fX, fZ, fY;
        m_creature->GetRespawnCoord(fX, fY, fZ);

        m_creature->AddSplineFlag(SPLINEFLAG_FLYING);

        m_creature->SendMonsterMoveWithSpeed(fX, fY, fZ + 5.0f, m_uiIntroTimer);
        m_creature->GetMap()->CreatureRelocation(m_creature, fX, fY, fZ + 5.0f, m_creature->GetOrientation());
    }

    void UpdateIntro(const uint32 uiDiff)
    {
        if (m_bIsIntroDone)
            return;
        Unit* pArthas = SummonMgr.GetFirstFoundSummonWithId(NPC_ARTHAS_IMAGE);

        if (pArthas && pArthas->isAlive())
        {
            if (m_uiIntroTimer < uiDiff)
            {
                m_uiIntroTimer = 10000;

                switch(m_uiIntroCount)
                {
                    case 0:
                        DoScriptText(SAY_INTRO_1, m_creature);
                        break;
                    case 1:
                        DoScriptText(SAY_INTRO_2_ARTHAS, pArthas);
                        break;
                    case 2:
                        pArthas->CastSpell(m_creature, SPELL_TRANSFORMING_CHANNEL, false);
                        m_creature->CastSpell(m_creature, SPELL_TRANSFORMING_FLOATING, false);
                        DoMoveToPosition();
                        break;
                    case 3:
                        m_creature->CastSpell(m_creature, SPELL_TRANSFORMING, false);
                        DoScriptText(SAY_INTRO_3, m_creature);
                        break;
                    case 4:
                        DoScriptText(SAY_INTRO_4_ARTHAS, pArthas);
                        break;
                    case 5:
                        DoScriptText(SAY_INTRO_5, m_creature);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_bIsIntroDone = true;
                        break;
                }

                ++m_uiIntroCount;
            }
            else
                m_uiIntroTimer -= uiDiff;
        }

    }

    void UpdateAI(const uint32 uiDiff)
    {
        UpdateIntro(uiDiff);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->getVictim()->GetEntry() == NPC_RITUAL_TARGET)
        {
            EnterEvadeMode();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_RITUAL: // handle teleports
                    if (Unit* pSacrifice = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        switch(urand(0, 4))
                        {
                            case 0: DoScriptText(SAY_SACRIFICE_1, m_creature); break;
                            case 1: DoScriptText(SAY_SACRIFICE_2, m_creature); break;
                            case 2: DoScriptText(SAY_SACRIFICE_3, m_creature); break;
                            case 3: DoScriptText(SAY_SACRIFICE_4, m_creature); break;
                            case 4: DoScriptText(SAY_SACRIFICE_5, m_creature); break;
                        }
                        p_Sacrifice = pSacrifice->GetObjectGuid();
                        m_creature->CastSpell(pSacrifice, SPELL_RITUAL_PREPARATION, true);
                        m_creature->CastSpell(m_creature, SPELL_RITUAL_OF_SWORD, false);
                        Events.DelayEventsWithId(EVENT_SINISTER_STRIKE, 27*IN_MILLISECONDS);
                        Events.DelayEventsWithId(EVENT_CALL_FLAMES, 27*IN_MILLISECONDS);
                        Events.ScheduleEvent(EVENT_SUMMON_CHANNELERS, 1500); // enough time to get teleported before casting
                    }
                    break;
                case EVENT_SUMMON_CHANNELERS:
                    {
                        if (!p_Sacrifice.IsEmpty())
                            if (Unit* pSacrificed = m_creature->GetMap()->GetUnit(p_Sacrifice))
                                m_creature->CastSpell(pSacrificed, SPELL_RITUAL_STRIKE, true);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_CHANNELER_1, true);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_CHANNELER_2, true);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_CHANNELER_3, true);
                    }
                    break;
                case EVENT_SINISTER_STRIKE:
                    if (m_creature->getVictim())
                        m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode? SPELL_SINISTER_STRIKE : SPELL_SINISTER_STRIKE_H, false);
                    break;
                case EVENT_CALL_FLAMES: // haven't seen this spell used enough
                    m_creature->CastSpell(m_creature, SPELL_CALL_FLAMES, true);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

#define TIMER_KNOCK 15*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_DISEASE 10*IN_MILLISECONDS, 25*IN_MILLISECONDS

struct MANGOS_DLL_DECL npc_scourge_hulkAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    EventManager Events;

    npc_scourge_hulkAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_pInstance(dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData()))
    {
    }

    void Reset()
    {
        Events.Reset();
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEventInRange(EVENT_KNOCK, TIMER_KNOCK, TIMER_KNOCK);
        Events.ScheduleEventInRange(EVENT_DISEASE, TIMER_DISEASE, TIMER_DISEASE);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() && pDoneBy->GetEntry() == NPC_SVALA_SORROW)
            m_pInstance->SetData(DATA_ACHIEVEMENT_SVALA, 1);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->getVictim()->GetEntry() == NPC_SVALA_SORROW)
        {
            EnterEvadeMode();
            return;
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_KNOCK:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_MIGHTY_BLOW, false);
                    break;
                case EVENT_DISEASE:
                    m_creature->CastSpell(m_creature->getVictim(), SPELL_VOLATILE_INFECTION, false);
                    break;
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

bool AreaTrigger_at_svala_intro(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (ScriptedInstance* pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_SVALA) == NOT_STARTED)
            pInstance->SetData(TYPE_SVALA, IN_PROGRESS);
    }
    return false;
}

CreatureAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI(pCreature);
}

CreatureAI* GetAI_npc_scourge_hulk(Creature* pCreature)
{
    return new npc_scourge_hulkAI(pCreature);
}

void AddSC_boss_svala()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_svala";
    newscript->GetAI = &GetAI_boss_svala;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scourge_hulk";
    newscript->GetAI = &GetAI_npc_scourge_hulk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_svala_intro";
    newscript->pAreaTrigger = &AreaTrigger_at_svala_intro;
    newscript->RegisterSelf();
}
