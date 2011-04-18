/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Mage-Lord Urom
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"

enum Says
{
    SAY_SUMMON_1    = -1578012,
    SAY_SUMMON_2    = -1578013,
    SAY_SUMMON_3    = -1578014,
    SAY_AGGRO       = -1578015,
    SAY_EXPLOSION_1 = -1578016,
    SAY_EXPLOSION_2 = -1578017,
    SAY_DEATH       = -1578018,
    SAY_KILL_1      = -1578019,
    SAY_KILL_2      = -1578020,
    SAY_KILL_3      = -1578021,
};

enum Spells
{
    SPELL_ARCANE_BARRIER                = 53813, //Dummy --> Channeled, shields the caster from damage.
    SPELL_EVOCATION_VISUAL              = 51602,

    SPELL_EMPOWERED_ARCANE_EXPLOSION_N  = 51110,
    SPELL_EMPOWERED_ARCANE_EXPLOSION_H  = 59377,

    SPELL_FROSTBOMB = 51103, //Urom throws a bomb, hitting its target with the highest aggro which inflict directly 650 frost damage and drops a frost zone on the ground. This zone deals 650 frost damage per second and reduce the movement speed by 35%. Lasts 1 minute.

    SPELL_SUMMON_MENAGERIE_1    = 50476, //Summons an assortment of creatures and teleports the caster to safety.
    SPELL_SUMMON_MENAGERIE_2    = 50495,
    SPELL_SUMMON_MENAGERIE_3    = 50496,

    SPELL_TELEPORT = 51112,     //Teleports to the center of Oculus

    SPELL_TIME_BOMB_N = 51121,  // Deals arcane damage to a random player, and after 6 seconds,
                                // deals zone damage to nearby equal to the health missing of
                                // the target afflicted by the debuff.
    SPELL_TIME_BOMB_H = 59376,
};

enum NPCs
{
    NPC_PHANTASMAL_MAMMOTH = 27642,
    NPC_PHANTASMAL_WOLF = 27644,
    NPC_PHANTASMAL_CLOUDSCRAPER = 27645,
    NPC_PHANTASMAL_OGRE = 27647,
    NPC_PHANTASMAL_NAGA = 27648,
    NPC_PHANTASMAL_MURLOC = 27649,
    NPC_PHANTASMAL_AIR = 27650,
    NPC_PHANTASMAL_FIRE = 27651,
    NPC_PHANTASMAL_WATER = 27653
};

enum Events
{
    EVENT_TELEPORT = 1,
    EVENT_EXPLOSION,
    EVENT_RETURN,
    EVENT_FROST_BOMB,
    EVENT_TIME_BOMB,
    EVENT_CHEAT_CHECK,
};

#define EXPLOSION_EMOTE "Mage-Lord Urom begins to cast Empowered Arcane Explosion!"

#define TIMER_TELEPORT      40*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_FROST_BOMB    15*IN_MILLISECONDS, 15*IN_MILLISECONDS
#define TIMER_TIME_BOMB     30*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_CHEAT_CHECK   2*IN_MILLISECONDS,  2*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_uromAI : public ScriptedAI
{

    ScriptedInstance* m_pInstance;
    SummonManager SummonMgr;
    EventManager Events;

    bool m_bIsRegularMode;
    uint32 m_uiPhase;
    bool m_bIsIntro;

    boss_uromAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->GetMotionMaster()->MoveIdle();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE);
        pCreature->SetActiveObjectState(true);
        pCreature->StopMoving();
        Reset();
    }

    void Reset()
    {
        if (m_pInstance && m_creature->isAlive())
            m_pInstance->SetData(TYPE_UROM, NOT_STARTED);
        SummonMgr.UnsummonAll();
        SetCombatMovement(false);
        m_uiPhase = 0;
        m_bIsIntro = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE);
        DoCast(m_creature, SPELL_EVOCATION_VISUAL, true);
        DoCast(m_creature, SPELL_ARCANE_BARRIER, true);
        Events.Reset();
    }

    bool VehicleCheck()
    {
        Map *map = m_creature->GetMap();
        if(map->IsDungeon())
        {
           Map::PlayerList const &PlayerList = map->GetPlayers();

           for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
           {
              if(i->getSource()->isAlive() && i->getSource()->GetVehicle())
                  return true;
           }
        }
        return false;
    }

    void JustSummoned(Creature* pSumm)
    {
        pSumm->SetInCombatWithZone();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_pInstance->GetData(TYPE_VAROS) == DONE)
            if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack()
                && pWho->IsWithinDist(m_creature, 30.0f) && !VehicleCheck() && !SummonMgr.GetSummonCount())
            {
                switch(m_uiPhase)
                {
                    case 0:
                        m_bIsIntro = true;
                        m_uiPhase = 1;
                        SetCombatMovement(false);
                        m_pInstance->SetData(TYPE_UROM, IN_PROGRESS);
                        DoScriptText(SAY_SUMMON_1, m_creature);
                        DoSummon(NPC_PHANTASMAL_FIRE, NPC_PHANTASMAL_FIRE, NPC_PHANTASMAL_AIR, NPC_PHANTASMAL_WATER);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_MENAGERIE_1, false);
                        break;
                    case 1:
                        DoScriptText(SAY_SUMMON_2, m_creature);
                        DoSummon(NPC_PHANTASMAL_OGRE, NPC_PHANTASMAL_OGRE, NPC_PHANTASMAL_NAGA, NPC_PHANTASMAL_MURLOC);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_MENAGERIE_2, false);
                        m_uiPhase = 2;
                        break;
                    case 2:
                        m_creature->RemoveAllAuras();
                        DoScriptText(SAY_SUMMON_3, m_creature);
                        DoSummon(NPC_PHANTASMAL_MAMMOTH, NPC_PHANTASMAL_WOLF, NPC_PHANTASMAL_WOLF, NPC_PHANTASMAL_CLOUDSCRAPER, NPC_PHANTASMAL_CLOUDSCRAPER);
                        m_creature->CastSpell(m_creature, SPELL_SUMMON_MENAGERIE_3, false);
                        m_uiPhase = 3;
                        break;
                    case 3:
                        SetCombatMovement(true);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PASSIVE);
                        m_creature->SetInCombatWithZone();
                        ScriptedAI::MoveInLineOfSight(pWho);
                        m_bIsIntro = false;
                        m_uiPhase = 4;
                        break;
                }
            }
    }

    void SummonedCreatureJustDied(Creature* pSumm)
    {
        if (pSumm->GetTypeId() == TYPEID_UNIT)
            SummonMgr.RemoveSummonFromList(pSumm->GetObjectGuid());
    }

    void EnterEvadeMode()
    {
        if (m_uiPhase < 3)
            return;
        float x, y, z;
        m_creature->GetRespawnCoord(x, y, z);
        m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(); // overrun ScriptedAI::EnterEvadeMode call

        Reset();
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_SUMMON_MENAGERIE_1 || pSpell->Id == SPELL_SUMMON_MENAGERIE_2 || pSpell->Id == SPELL_SUMMON_MENAGERIE_3)
        {
            m_creature->GetMotionMaster()->MoveIdle();
        }
    }

    void AttackStart(Unit* pWho)
    {
        if(m_uiPhase < 3)
            return;
        ScriptedAI::AttackStart(pWho);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_TELEPORT, TIMER_TELEPORT);
        Events.ScheduleEvent(EVENT_FROST_BOMB, TIMER_FROST_BOMB);
        Events.ScheduleEvent(EVENT_TIME_BOMB, TIMER_TIME_BOMB);
        Events.ScheduleEvent(EVENT_CHEAT_CHECK, TIMER_CHEAT_CHECK);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_UROM, DONE);

        if (Creature* pSumm = m_creature->SummonCreature(NPC_BALGAR_IMAGE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 2*MINUTE*IN_MILLISECONDS))
            SendScriptMessageTo(pSumm, killer, MESSAGE_UROM, 0);
    }

    void KilledUnit(Unit *victim)
    {
        uint8 uiText = urand(0, 2);
        switch (uiText)
        {
           case 0: DoScriptText(SAY_KILL_1, m_creature); break;
           case 1: DoScriptText(SAY_KILL_2, m_creature); break;
           case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void DoSummon(uint32 entry = 0, uint32 entry1 = 0, uint32 entry2 = 0, uint32 entry3 = 0, uint32 entry4 = 0)
    {
        if (entry)
            SummonMgr.SummonCreature(entry, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (entry1)
            SummonMgr.SummonCreature(entry1, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (entry2)
            SummonMgr.SummonCreature(entry2, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (entry3)
            SummonMgr.SummonCreature(entry3, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (entry4)
            SummonMgr.SummonCreature(entry4, m_creature->GetPositionX() - 5 + urand(0, 10), m_creature->GetPositionY() - 5 + urand(0, 10),
                m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // dont update in teleports
        if (m_bIsIntro)
            return;

        if ((!m_creature->SelectHostileTarget() || !m_creature->getVictim()) && !m_bIsIntro)
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_TELEPORT:
                    m_creature->AddSplineFlag(SPLINEFLAG_FLYING);
                    m_creature->StopMoving();
                    DoStartNoMovement(m_creature->getVictim());
                    DoCast(m_creature,SPELL_TELEPORT);
                    Events.DelayEvents(9*IN_MILLISECONDS);
                    Events.ScheduleEvent(EVENT_EXPLOSION, 1*IN_MILLISECONDS);
                    break;
                case EVENT_FROST_BOMB:
                    DoCast(m_creature->getVictim(), SPELL_FROSTBOMB);
                    break;
                case EVENT_TIME_BOMB:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_TIME_BOMB_N : SPELL_TIME_BOMB_H);
                    break;
                case EVENT_EXPLOSION:
                    Events.ScheduleEvent(EVENT_RETURN, m_bIsRegularMode ? 8*IN_MILLISECONDS + 500 : 6*IN_MILLISECONDS + 500);
                    m_creature->MonsterTextEmote(EXPLOSION_EMOTE, m_creature, true);
                    DoScriptText(urand(0,1) ? SAY_EXPLOSION_1 : SAY_EXPLOSION_2, m_creature);
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_EMPOWERED_ARCANE_EXPLOSION_N : SPELL_EMPOWERED_ARCANE_EXPLOSION_H);
                    break;
                case EVENT_RETURN:
                    SetCombatMovement(true);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        DoStartMovement(m_creature->getVictim());
                        m_creature->NearTeleportTo(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, false);
                        AttackStart(pTarget);
                    }
                    break;
                case EVENT_CHEAT_CHECK:
                    if (VehicleCheck())
                        EnterEvadeMode();
                    break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_urom(Creature* pCreature)
{
    return new boss_uromAI(pCreature);
}

void AddSC_boss_urom()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_urom";
    newscript->GetAI = &GetAI_boss_urom;
    newscript->RegisterSelf();
}
