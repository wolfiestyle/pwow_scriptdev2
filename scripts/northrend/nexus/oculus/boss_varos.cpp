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
SDName: Boss Varos Cloudstrider
SD%Complete: 0
SDComment:
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "def_oculus.h"

enum Says
{
    SAY_AGGRO       = -1578023,
    SAY_KILL_1      = -1578028,
    SAY_KILL_2      = -1578029,
    SAY_DEATH       = -1578027,
    SAY_STRIKE_1    = -1578024,
    SAY_STRIKE_2    = -1578025,
    SAY_STRIKE_3    = -1578026,
};

enum Spells
{
    SPELL_ARCANE_SHIELD             = 50053,
    SPELL_ENERGIZE_CORES_N          = 50785,
    SPELL_ENERGIZE_CORES_H          = 59372,
    SPELL_ENERGIZE_CORES_TRIGGER_1  = 54069,
    SPELL_ENERGIZE_CORES_TRIGGER_2  = 56251,

    SPELL_CALL_AZURE_RING_CAPTAIN   = 51002,    //Effect Send Event (12229)
    SPELL_CALL_AZURE_RING_CAPTAIN_2 = 51006,    //Effect Send Event (10665)
    SPELL_CALL_AZURE_RING_CAPTAIN_3 = 51007,    //Effect Send Event (18454)
    SPELL_CALL_AZURE_RING_CAPTAIN_4 = 51008,    //Effect Send Event (18455)

    SPELL_CALL_AMPLIFY_MAGIC_N      = 51054,
    SPELL_CALL_AMPLIFY_MAGIC_H      = 59371,

    SPELL_CORE_VISUAL               = 50798,
    SPELL_CORE_MISSILE              = 61407,    //need core fix max target 4, and spell script on 28183

    SPELL_BEAM                      = 51024,
    SPELL_BEAM_DMG_AURA             = 51019,

    SPELL_BEAM_VISUAL_SOUND         = 51022,
    SPELL_SUMMON_BEAM               = 51017,
};

struct Locations
{
    float x1, y1, x2, y2;
};

struct Locations Regions[]=
{
    {0, 0, 0},
    {1323.0f, 1056.0f, 1333.0f, 1066.0f}, //first orb 1
    {1319.3f, 1084.0f, 1329.5f, 1094.0f}, //second orb 2
    {1288.3f, 1108.8f, 1298.3f, 1118.8f}, //third orb 3
    {1260.3f, 1104.2f, 1270.3f, 1114.2f}, //four orb 4
    {1237.7f, 1074.5f, 1247.7f, 1084.5f}, //fifth orb 5
    {1241.8f, 1046.1f, 1251.8f, 1056.1f}, // 6
    {1272.0f, 1022.1f, 1282.0f, 1032.1f}, // 7
    {1300.5f, 1026.2f, 1310.5f, 1036.2f}, // 8
};
enum NPCs
{
    NPC_AZURE_CAPTAIN   = 28236,
    NPC_BEAM            = 28239,
    NPC_VAROS_CORE      = 28183,
};

enum Events
{
    EVENT_CHEAT_CHECK = 1,
    EVENT_SUMMON_CAPTAIN,
    EVENT_SELECT_ORBS,
    EVENT_ENERGIZE_CORES,
    EVENT_AMPLIFY_MAGIC,
    EVENT_MOVE_CAPTAIN,
};

#define TIMER_AMPLIFY_MAGIC     40*IN_MILLISECONDS, 40*IN_MILLISECONDS
#define TIMER_SUMMON_CAPTAIN    30*IN_MILLISECONDS, 30*IN_MILLISECONDS
#define TIMER_SELECT_ORBS       1*IN_MILLISECONDS
#define TIMER_CAST_CHAIN        5*IN_MILLISECONDS

#define EMOTE_SUMMON_CAPTAIN    "Varos Cloudstrider calls an Azure Ring Captain!"

struct MANGOS_DLL_DECL boss_varosAI : public ScriptedAI
{
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventManager Events;

    SummonManager SummonMgr;
    bool m_bStarted;
    uint8 MinOrb;
    uint8 MaxOrb;
    float angle01;
    float angle02;

    boss_varosAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if(m_pInstance)
           m_pInstance->SetData(TYPE_VAROS, IN_PROGRESS);

        if(Creature* Dragon = SummonMgr.SummonCreature(NPC_AZURE_CAPTAIN, (m_creature->GetPositionX()-45)+rand()%90, (m_creature->GetPositionY()-45)+rand()%90, m_creature->GetPositionZ() + 30.0f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 100))
        {
           Dragon->AddSplineFlag(SPLINEFLAG_FLYING);
           Dragon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

        Events.ScheduleEvent(EVENT_CHEAT_CHECK,     2*IN_MILLISECONDS, 2*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SUMMON_CAPTAIN,  TIMER_SUMMON_CAPTAIN);
        Events.ScheduleEvent(EVENT_SELECT_ORBS,     10*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_AMPLIFY_MAGIC,   TIMER_AMPLIFY_MAGIC);
    }

    void SelectOrbs()
    {
        MinOrb = MinOrb+2;
        MaxOrb = MaxOrb+2;
        if(MinOrb > 8)
           MinOrb = 1;
        if(MaxOrb > 8)
           MaxOrb = 2;

        std::list<Creature*> m_pSpheres;
        GetCreatureListWithEntryInGrid(m_pSpheres, m_creature, NPC_VAROS_CORE, DEFAULT_VISIBILITY_INSTANCE);

        if(!m_pSpheres.empty())
           for(std::list<Creature*>::iterator iter = m_pSpheres.begin(); iter != m_pSpheres.end(); ++iter)
           {
              for(uint8 i = 1; i < 9; i++)
                 if((i <= MaxOrb && i >= MinOrb) || (MinOrb == 7 && (i <= MaxOrb || i >= MinOrb)))
                    if((*iter)->GetPositionX() > Regions[i].x1 && (*iter)->GetPositionX() < Regions[i].x2)
                       if((*iter)->GetPositionY() > Regions[i].y1 && (*iter)->GetPositionY() < Regions[i].y2)
                          if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_CORE_MISSILE))
                          {
                             pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                             pTempSpell->EffectImplicitTargetB[0] = 0;
                             pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                             pTempSpell->EffectImplicitTargetB[1] = 0;
                             pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                             pTempSpell->EffectImplicitTargetB[2] = 0;
                             (*iter)->CastSpell(m_creature, pTempSpell, true);
                          }
           }
    }

    void CastEnergy()
    {

        std::list<Creature*> m_pSpheres;
        GetCreatureListWithEntryInGrid(m_pSpheres, m_creature, NPC_VAROS_CORE, DEFAULT_VISIBILITY_INSTANCE);

        if(!m_pSpheres.empty())
            for(std::list<Creature*>::iterator iter = m_pSpheres.begin(); iter != m_pSpheres.end(); ++iter)
            {
                for(uint8 i = 1; i < 9; i++)
                    if((i <= MaxOrb && i >= MinOrb) || (MinOrb == 7 && (i <= MaxOrb || i >= MinOrb)))
                        if((*iter)->GetPositionX() > Regions[i].x1 && (*iter)->GetPositionX() < Regions[i].x2)
                            if((*iter)->GetPositionY() > Regions[i].y1 && (*iter)->GetPositionY() < Regions[i].y2)
                            {
                                if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_ENERGIZE_CORES_TRIGGER_1))
                                {
                                    pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
                                    pTempSpell->EffectImplicitTargetB[0] = 0;
                                    pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
                                    pTempSpell->EffectImplicitTargetB[1] = 0;
                                    pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
                                    pTempSpell->EffectImplicitTargetB[2] = 0;
                                    (*iter)->CastSpell(m_creature, pTempSpell, true);
                                }

                                if(i == MinOrb)
                                    angle01 = m_creature->GetAngle((*iter));
                                if(i == MaxOrb)
                                    angle02 = m_creature->GetAngle((*iter));

                                Map *map = m_creature->GetMap();
                                if(map->IsDungeon())
                                {
                                    Map::PlayerList const &PlayerList = map->GetPlayers();

                                    if(PlayerList.isEmpty())
                                        return;

                                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    {
                                        if (i->getSource()->isAlive())
                                        {
                                            float pAngle = m_creature->GetAngle(i->getSource());
                                            if(angle01 < angle02)
                                                if(pAngle < angle02 && pAngle > angle01)
                                                    DoEnergy(i->getSource());
                                            if(angle01 > angle02)
                                                if(pAngle < angle02 || pAngle > angle01)
                                                    DoEnergy(i->getSource());
                                        }
                                    }
                                }
                            }
            }
    }

    void DoEnergy(Unit* pTarget)
    {
        if(SpellEntry* pTempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(m_bIsRegularMode ? SPELL_ENERGIZE_CORES_N : SPELL_ENERGIZE_CORES_H))
        {
            pTempSpell->EffectImplicitTargetA[0] = TARGET_EFFECT_SELECT;
            pTempSpell->EffectImplicitTargetB[0] = 0;
            pTempSpell->EffectImplicitTargetA[1] = TARGET_EFFECT_SELECT;
            pTempSpell->EffectImplicitTargetB[1] = 0;
            pTempSpell->EffectImplicitTargetA[2] = TARGET_EFFECT_SELECT;
            pTempSpell->EffectImplicitTargetB[2] = 0;
            m_creature->CastSpell(pTarget, pTempSpell, true);
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_DEATH, m_creature);
        SummonMgr.UnsummonAll();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_VAROS, DONE);

        Creature* pSumm = m_creature->SummonCreature(28012, m_creature->GetPositionX(), m_creature->GetPositionY(),
            m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 2*MINUTE*IN_MILLISECONDS, true);

        if (pSumm)
            SendScriptMessageTo(pSumm, killer, MESSAGE_VAROS, 0);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void Reset() 
    {
        if (m_bStarted != true)
            m_bStarted = false;

        MinOrb = 1;
        MaxOrb = 3;
        angle01 = 0.0f;
        angle02 = 0.0f;
        SummonMgr.UnsummonAll();
        Events.Reset();

        if(m_pInstance)
        {
            if (m_creature->isAlive())
                m_pInstance->SetData(TYPE_VAROS, NOT_STARTED);

            if(m_pInstance->GetData(TYPE_ROBOTS) == 0)
            {
                m_creature->RemoveAllAuras();
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
            else
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoCast(m_creature, SPELL_ARCANE_SHIELD);
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_BEAM)
        {
            pSummoned->setFaction(14);
            pSummoned->SetDisplayId(11686);
            pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
            pSummoned->CastSpell(pSummoned, SPELL_BEAM_VISUAL_SOUND, false);
            pSummoned->CastSpell(pSummoned, SPELL_BEAM_DMG_AURA, false);
            pSummoned->SetInCombatWithZone();

            if(Unit* pTarget = pSummoned->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
            {
                pSummoned->AddThreat(pTarget, 100000.0f);
                pSummoned->AI()->AttackStart(pTarget);
            }

            if(m_pInstance)
                if(Creature* Dragon = SummonMgr.GetFirstFoundSummonWithId(NPC_AZURE_CAPTAIN))
                {
                    Dragon->GetMotionMaster()->MovementExpired(false);
                    Dragon->GetMotionMaster()->Clear(false);
                    Dragon->CastSpell(pSummoned, SPELL_BEAM, true);
                }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bStarted && m_pInstance && m_pInstance->GetData(TYPE_ROBOTS) == 0 && m_pInstance->GetData(TYPE_DRAKOS) == DONE)
        {
            m_bStarted = true;
            m_creature->RemoveAllAuras();
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            EnterEvadeMode();
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CHEAT_CHECK:
                {
                    Map* map = m_creature->GetMap();
                    if(map && map->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = map->GetPlayers();

                        if(PlayerList.isEmpty())
                            return;

                        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if(i->getSource()->isAlive() && i->getSource()->GetVehicle())
                            {
                                EnterEvadeMode();
                                break;
                            }
                        }
                    }
                    break;
                }
                case EVENT_MOVE_CAPTAIN:
                    if(Creature* pCaptain = SummonMgr.GetFirstFoundSummonWithId(NPC_AZURE_CAPTAIN))
                    {
                        pCaptain->GetMotionMaster()->MovementExpired(false);
                        pCaptain->GetMotionMaster()->Clear(false);
                        pCaptain->GetMotionMaster()->MovePoint(0, (m_creature->GetPositionX()-45)+rand()%90, (m_creature->GetPositionY()-45)+rand()%90, m_creature->GetPositionZ() + 30.0f);
                    }
                    break;
                case EVENT_SUMMON_CAPTAIN:
                {           
                    DoCast(m_creature, SPELL_SUMMON_BEAM, true);
                    uint8 uiText = urand(0, 2);
                    Events.ScheduleEvent(EVENT_MOVE_CAPTAIN, 15*IN_MILLISECONDS);
                    switch (uiText)
                    {
                        case 0:
                            DoScriptText(SAY_STRIKE_1, m_creature);
                            break;
                        case 1:
                            DoScriptText(SAY_STRIKE_2, m_creature);
                            break;
                        case 2:
                            DoScriptText(SAY_STRIKE_3, m_creature);
                            break;
                    }
                    m_creature->MonsterTextEmote(EMOTE_SUMMON_CAPTAIN, m_creature, true);
                    break;
                }
                case EVENT_SELECT_ORBS:
                {
                    Events.ScheduleEvent(EVENT_ENERGIZE_CORES, TIMER_CAST_CHAIN);
                    SelectOrbs();
                    break;
                }
                case EVENT_ENERGIZE_CORES:
                {
                    Events.ScheduleEvent(EVENT_SELECT_ORBS, TIMER_SELECT_ORBS);
                    CastEnergy();
                    break;
                }
                case EVENT_AMPLIFY_MAGIC:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, m_bIsRegularMode ? SPELL_CALL_AMPLIFY_MAGIC_N : SPELL_CALL_AMPLIFY_MAGIC_H);
                    break;
            }

        DoMeleeAttackIfReady();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_creature->HasAura(SPELL_ARCANE_SHIELD))
            uiDamage = 0;            
    }
};

struct MANGOS_DLL_DECL npc_varos_orbAI : public ScriptedAI
{
    npc_varos_orbAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }

    void Reset()
    {
        m_creature->setFaction(14);
        DoCast(m_creature, SPELL_CORE_VISUAL, true);
    }

    void AttackStart(Unit* pWho)
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct MANGOS_DLL_DECL npc_varos_beam_targetAI : public ScriptedAI
{
    npc_varos_beam_targetAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }
    uint32 uiDeathTimer;

    void Reset()
    {
        m_creature->SetSpeedRate(MOVE_RUN, 0.5f, true);
        uiDeathTimer = 15000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiDeathTimer < diff)
            DespawnCreature(m_creature);
        else uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_boss_varos(Creature* pCreature)
{
    return new boss_varosAI(pCreature);
}

CreatureAI* GetAI_npc_varos_orb(Creature* pCreature)
{
    return new npc_varos_orbAI (pCreature);
}

CreatureAI* GetAI_npc_varos_beam_target(Creature* pCreature)
{
    return new npc_varos_beam_targetAI (pCreature);
}

void AddSC_boss_varos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_varos";
    newscript->GetAI = &GetAI_boss_varos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_varos_orb";
    newscript->GetAI = &GetAI_npc_varos_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_varos_beam_target";
    newscript->GetAI = &GetAI_npc_varos_beam_target;
    newscript->RegisterSelf();
}
