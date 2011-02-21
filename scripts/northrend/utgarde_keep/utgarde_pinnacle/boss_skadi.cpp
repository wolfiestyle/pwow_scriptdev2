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
SDName: Boss_Skadi
SD%Complete: 20%
SDComment: starts at trigger 4991
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "Vehicle.h"
#include "utgarde_pinnacle.h"

enum Says
{
    SAY_AGGRO                       = -1575019,
    SAY_DRAKEBREATH_1               = -1575020,
    SAY_DRAKEBREATH_2               = -1575021,
    SAY_DRAKEBREATH_3               = -1575022,
    SAY_DRAKE_HARPOON_1             = -1575023,
    SAY_DRAKE_HARPOON_2             = -1575024,
    SAY_KILL_1                      = -1575025,
    SAY_KILL_2                      = -1575026,
    SAY_KILL_3                      = -1575027,
    SAY_DEATH                       = -1575028,
    SAY_DRAKE_DEATH                 = -1575029,
    EMOTE_HARPOON_RANGE             = -1575030,
};

enum Spells
{
    SPELL_CRUSH                     = 50234,
    SPELL_CRUSH_H                   = 59330,

    SPELL_WHIRLWIND                 = 50228,
    SPELL_WHIRLWIND_H               = 59322,

    SPELL_POISONED_SPEAR            = 50255,
    SPELL_POISONED_SPEAR_H          = 59331,

    // casted with base of creature 22515 (World Trigger), so we must make sure
    // to use the close one by the door leading further in to instance.
    SPELL_SUMMON_GAUNTLET_MOBS      = 48630,                // tick every 30 sec
    SPELL_SUMMON_GAUNTLET_MOBS_H    = 59275,                // tick every 25 sec

    SPELL_GAUNTLET_PERIODIC         = 47546,                // what is this? Unknown use/effect, but probably related
    SPELL_LAUNCH_HARPOON            = 48642,                // this spell hit drake to reduce HP (force triggered from 48641)
    SPELL_FREEZING_CLOUD            = 47579,

    NPC_YMIRJAR_WARRIOR             = 26690,
    NPC_YMIRJAR_WITCH_DOCTOR        = 26691,
    NPC_YMIRJAR_HARPOONER           = 26692,
    NPC_TRIGGER                     = 28351,
};

enum Events
{
    EVENT_MOUNT_GRAUF = 1,
    EVENT_MOVE,
    EVENT_DESPAWN,
    EVENT_DESPAWN_2,
    EVENT_WIPE_CHECK,
    EVENT_SUMMON_WAVE,

    EVENT_CRUSH,
    EVENT_POISONED_SPEAR,
    EVENT_WHIRLWIND,
    EVENT_WHIRLWIND_2,

    MESSAGE_PHASE_2,

    PHASE_MOUNTED = 1,
    PHASE_LANDED,
    PMASK_MOUNTED = bit_mask<PHASE_MOUNTED>::value,
    PMASK_LANDED = bit_mask<PHASE_LANDED>::value,
};

#define EMOTE_BREATH "Grauf takes a deep breath."

static float SpawnLoc[3] = {468.931f, -513.555f, 104.723f};

static float Location[72][3] =
{
    // Boss
    {341.740997f, -516.955017f, 104.66900f}, // 0
    {293.299f, -505.95f, 140.03f},           // 1
    {301.664f, -535.164f, 135.03f},          // 2
    {341.740997f, -535.164f, 128.03f},       // 3
    {477.311981f, -535.164f, 128.03f},       // 4
    {517.031006f, -535.164f, 128.03f},       // 5
    {341.740997f, -516.955017f, 104.66900f}, // 6
    {341.740997f, -516.955017f, 104.66900f}, // 7
    // Triggers Left
    {469.661f, -484.546f, 104.712f},         // 8
    {483.315f, -485.028f, 104.718f},         // 9
    {476.87f, -487.994f, 104.735f},          //10
    {477.512f, -497.772f, 104.728f},         //11
    {486.287f, -500.759f, 104.722f},         //12
    {480.1f, -503.895f, 104.722f},           //13
    {472.391f, -505.103f, 104.723f},         //14
    {478.885f, -510.803f, 104.723f},         //15
    {489.529f, -508.615f, 104.723f},         //16
    {484.272f, -508.589f, 104.723f},         //17
    {465.328f, -506.495f, 104.427f},         //18
    {456.885f, -508.104f, 104.447f},         //19
    {450.177f, -507.989f, 105.247f},         //20
    {442.273f, -508.029f, 104.813f},         //21
    {434.225f, -508.19f, 104.787f},          //22
    {423.902f, -508.525f, 104.274f},         //23
    {414.551f, -508.645f, 105.136f},         //24
    {405.787f, -508.755f, 104.988f},         //25
    {398.812f, -507.224f, 104.82f},          //26
    {389.702f, -506.846f, 104.729f},         //27
    {381.856f, -506.76f, 104.756f},          //28
    {372.881f, -507.254f, 104.779f},         //29
    {364.978f, -508.182f, 104.673f},         //30
    {357.633f, -508.075f, 104.647f},         //31
    {350.008f, -506.826f, 104.588f},         //32
    {341.69f, -506.77f, 104.499f},           //33
    {335.31f, -505.745f, 105.18f},           //34
    {471.178f, -510.74f, 104.723f},          //35
    {461.759f, -510.365f, 104.199f},         //36
    {424.07287f, -510.082916f, 104.711082f}, //37
    // Triggers Right
    {489.46f, -513.297f, 105.413f},          //38
    {485.706f, -517.175f, 104.724f},         //39
    {480.98f, -519.313f, 104.724f},          //40
    {475.05f, -520.52f, 104.724f},           //41
    {482.97f, -512.099f, 104.724f},          //42
    {477.082f, -514.172f, 104.724f},         //43
    {468.991f, -516.691f, 104.724f},         //44
    {461.722f, -517.063f, 104.627f},         //45
    {455.88f, -517.681f, 104.707f},          //46
    {450.499f, -519.099f, 104.701f},         //47
    {444.889f, -518.963f, 104.82f},          //48
    {440.181f, -518.893f, 104.861f},         //49
    {434.393f, -518.758f, 104.891f},         //50
    {429.328f, -518.583f, 104.904f},         //51
    {423.844f, -518.394f, 105.004f},         //52
    {418.707f, -518.266f, 105.135f},         //53
    {413.377f, -518.085f, 105.153f},         //54
    {407.277f, -517.844f, 104.893f},         //55
    {401.082f, -517.443f, 104.723f},         //56
    {394.933f, -514.64f, 104.724f},          //57
    {388.917f, -514.688f, 104.734f},         //58
    {383.814f, -515.834f, 104.73f},          //59
    {377.887f, -518.653f, 104.777f},         //60
    {371.376f, -518.289f, 104.781f},         //61
    {365.669f, -517.822f, 104.758f},         //62
    {359.572f, -517.314f, 104.706f},         //63
    {353.632f, -517.146f, 104.647f},         //64
    {347.998f, -517.038f, 104.538f},         //65
    {341.803f, -516.98f, 104.584f},          //66
    {335.879f, -516.674f, 104.628f},         //67
    {329.871f, -515.92f, 104.711f},          //68
    // Breach Zone
    {485.4577f, -511.2515f, 115.3011f},      //69
    {435.1892f, -514.5232f, 118.6719f},      //70
    {413.9327f, -540.9407f, 138.2614f},      //71
};

struct MANGOS_DLL_DECL npc_graufAI : public ScriptedAI, public ScriptMessageInterface
{    
    npc_graufAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    EventManager Events;
    ObjectGuid m_GraufGuid;
    SummonManager SummonMgr;

    uint32 m_uiPassHitCount;
    uint32 m_uiTotalHitCount;
    uint32 m_uiWaypointId;

    bool m_bIsRegularMode;
    bool m_bSaidEmote;
    bool m_bStarted;

    // TODO: core support for out of LoS casting... this spell doesnt hit if out of LoS + visual not showing up
    void SpellHit(Unit* pDoneBy, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LAUNCH_HARPOON && Events.GetPhase() != PHASE_LANDED)
        {
            m_uiPassHitCount++;
            m_uiTotalHitCount++;
            if (m_uiTotalHitCount >= 3)
            {
                Events.SetPhase(PHASE_LANDED);
                if (m_uiPassHitCount >= 3)
                    // update achievement progress
                    if (m_pInstance)
                        m_pInstance->SetData(DATA_ACHIEVEMENT_SKADI_MY_GIRL, 1);
            }
        }
    }

    void JustSummoned(Creature* pSummon)
    {
        if (pSummon->GetEntry() == NPC_TRIGGER)
        {
            pSummon->setFaction(14);
            pSummon->CastSpell(pSummon, SPELL_FREEZING_CLOUD, true);
        }
    }

    void Reset()
    {
        m_uiWaypointId = 0;
        m_uiPassHitCount = 0;
        m_uiTotalHitCount = 0;

        m_bStarted = false;
        m_bSaidEmote = false;
        Events.Reset();

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        Events.SetPhase(PHASE_MOUNTED);

        if (m_pInstance) // Reset Progress
            m_pInstance->SetData(DATA_ACHIEVEMENT_SKADI_MY_GIRL, 0);
    }

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (pSender->GetTypeId() == TYPEID_PLAYER && Events.GetPhase() != PHASE_LANDED)
        {
            // TODO : this should be handled on spellhit
            m_uiPassHitCount++;
            m_uiTotalHitCount++;
            if (m_uiTotalHitCount >= 3 )
            {
                Events.SetPhase(PHASE_LANDED);
                if (m_uiPassHitCount >= 3)
                {
                    if (m_pInstance)
                        m_pInstance->SetData(DATA_ACHIEVEMENT_SKADI_MY_GIRL, 1);
                }
                Events.ScheduleEvent(EVENT_DESPAWN, 1*IN_MILLISECONDS, 0, 0, 0, PMASK_LANDED);
                Events.ScheduleEvent(EVENT_DESPAWN_2, 2*IN_MILLISECONDS, 0, 0, 0, PMASK_LANDED);
            }
        }
        else // we just work like an "echo"
            SendScriptMessageTo((Creature*)pSender, m_creature, data1, data2);

    }
    void PassengerBoarded(Unit* pWho, int8 seat, bool apply)
    {
        if (pWho->GetEntry() == NPC_SKADI && apply) // skadi mounts
        {
            m_bStarted = true;
            Events.SetPhase(PHASE_MOUNTED);

            m_creature->HandleEmote(448); // nice emote (ONESHOT_FLY_SIT_GROUND_UP)
            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2); // make it flap its wings :D
            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            m_creature->AddSplineFlag(SPLINEFLAG_FLYING);

            Events.ScheduleEvent(EVENT_WIPE_CHECK, 5*IN_MILLISECONDS, 5*IN_MILLISECONDS);
            Events.ScheduleEvent(EVENT_MOVE, 3*IN_MILLISECONDS, 0,0,0, PMASK_MOUNTED);

            if (m_pInstance) // Reset Progress
                m_pInstance->SetData(DATA_ACHIEVEMENT_SKADI_MY_GIRL, 0);
        }
    }
    void SpawnTrigger() // this summons the "frost breath"
    {
        uint8 iStart = 0, iEnd = 0;
        switch (urand(0,1)) // left or right
        {
            case 0:
                iStart = 8;
                iEnd = 37;
                break;
            case 1:
                iStart = 38;
                iEnd = 68;
                break;
            default:
                break;
        }
        for (uint32 i = iStart; i < iEnd; ++i)
            SummonMgr.SummonCreature(NPC_TRIGGER, Location[i][0], Location[i][1], Location[i][2], 0, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS);
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_bStarted)
            return;
        if (Events.GetPhase() == PHASE_MOUNTED)
        {
            // Update passenger position
            if (m_creature->GetVehicleKit())
                if (Unit* pSkadi = m_creature->GetVehicleKit()->GetPassenger(0))
                    pSkadi->Relocate(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());

            if (m_creature->GetPositionX() >= 505.0f)
            {
                if (!m_bSaidEmote)
                {
                    m_uiPassHitCount = 0;
                    DoScriptText(EMOTE_HARPOON_RANGE, m_creature);
                    m_bSaidEmote = true;
                }
            }
            else
            {
                m_bSaidEmote = false;
            }
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_WIPE_CHECK:
                {
                    Map* pMap = m_creature->GetMap();

                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();

                        bool wipe = true;
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (i->getSource()->isGameMaster())
                                continue;
                            if (Unit* pPlayer = i->getSource())
                                if (pPlayer->isAlive())
                                    if (pPlayer->IsWithinDist2d(397.017f, -511.496f, 150.0f))
                                    {
                                        wipe = false;
                                        break;
                                    }
                        }
                        if (wipe)
                        {
                            m_bStarted = false;
                            m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0);
                            EnterEvadeMode();
                        }
                    }
                }
                break;
                case EVENT_DESPAWN:
                    if (m_creature->GetVehicleKit())
                        if (Unit* pSkadi = m_creature->GetVehicleKit()->GetPassenger(0))
                            if (pSkadi)
                                SendScriptMessageTo((Creature*)pSkadi, m_creature, MESSAGE_PHASE_2, 0);
                    m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_FLYDEATH);
                    break;
                case EVENT_DESPAWN_2:
                    m_creature->DealDamage(m_creature,m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    break;
                case EVENT_MOVE:
                    {
                        uint32 m_uiMovementTimer;
                        switch(m_uiWaypointId)
                        {
                            case 0:
                                m_creature->MonsterMoveWithSpeed(Location[1][0], Location[1][1], Location[1][2], 5*IN_MILLISECONDS);
                                m_uiMovementTimer = 5000;
                                break;
                            case 1:
                                 m_creature->MonsterMoveWithSpeed(Location[2][0], Location[2][1], Location[2][2], 2*IN_MILLISECONDS);
                                m_uiMovementTimer = 2000;
                                break;
                            case 2:
                                m_creature->MonsterMoveWithSpeed(Location[3][0], Location[3][1], Location[3][2], 5*IN_MILLISECONDS);
                                m_uiMovementTimer = 5000;
                                break;
                            case 3:
                                m_creature->MonsterMoveWithSpeed(Location[4][0], Location[4][1], Location[4][2], 5*IN_MILLISECONDS);
                                m_uiMovementTimer = 5000;
                                break;
                            case 4:
                                m_creature->MonsterMoveWithSpeed(Location[5][0], Location[5][1], Location[5][2], 5*IN_MILLISECONDS);
                                m_uiMovementTimer = 5000;
                                break;
                            case 5:
                                m_creature->MonsterMoveWithSpeed(Location[69][0], Location[69][1], Location[69][2], 2500);
                                if (Unit* pSkadi = m_creature->GetVehicleKit()->GetPassenger(0))
                                switch(uint32 i = urand(0,2))
                                {
                                    case 0:
                                        DoScriptText(SAY_DRAKEBREATH_1, pSkadi);
                                        break;
                                    case 1:
                                        DoScriptText(SAY_DRAKEBREATH_2, pSkadi);
                                        break;
                                    case 2:
                                        DoScriptText(SAY_DRAKEBREATH_3, pSkadi);
                                        break;
                                }
                                m_creature->MonsterTextEmote(EMOTE_BREATH, m_creature, true);
                                m_uiMovementTimer = 2500;
                                break;
                            case 6:
                                m_creature->MonsterMoveWithSpeed(Location[70][0], Location[70][1], Location[70][2], 2*IN_MILLISECONDS);
                                m_uiMovementTimer = 2000;
                                SpawnTrigger();
                                break;
                            case 7:
                                m_creature->MonsterMoveWithSpeed(Location[71][0], Location[71][1], Location[71][2], 3*IN_MILLISECONDS);
                                m_uiMovementTimer = 3000;
                                break;
                            case 8:
                                m_creature->MonsterMoveWithSpeed(Location[71][0], Location[71][1], Location[71][2], 10*IN_MILLISECONDS);
                                m_uiMovementTimer = 10*IN_MILLISECONDS;
                                break;
                            case 9:
                                m_creature->MonsterMoveWithSpeed(Location[4][0], Location[4][1], Location[4][2], 5*IN_MILLISECONDS);
                                m_uiWaypointId = 3;
                                m_uiMovementTimer = 5000;
                                break;
                        }
                        m_uiWaypointId++;
                        if (m_uiMovementTimer)
                        {
                            Events.ScheduleEvent(EVENT_MOVE, m_uiMovementTimer, 0, 0, 0, PMASK_MOUNTED);
                        }
                    break;
                    }
                default:
                    break;
        }
    }
};

/*######
## boss_skadi
######*/

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI, public ScriptMessageInterface
{
    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    EventManager Events;
    SummonManager SummonMgr;
    ObjectGuid m_GraufGuid;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bStarted;

    void ScriptMessage(WorldObject* pSender, uint32 data1, uint32 data2)
    {
        if (pSender->GetEntry() == NPC_GRAUF)
            m_GraufGuid = pSender->GetObjectGuid();
        if (data1 == MESSAGE_PHASE_2)
        {
            Events.SetPhase(PHASE_LANDED);
            Events.ScheduleEvent(EVENT_DESPAWN, 4*IN_MILLISECONDS);
            DoScriptText(SAY_DRAKE_DEATH, m_creature);
            m_creature->ExitVehicle();
            m_creature->MonsterJump(475.98f, -509.623f, 104.72f, 1*IN_MILLISECONDS, 50);
            Events.ScheduleEvent(EVENT_CRUSH, 8*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            Events.ScheduleEvent(EVENT_POISONED_SPEAR, 7*IN_MILLISECONDS, 11*IN_MILLISECONDS);
            Events.ScheduleEvent(EVENT_WHIRLWIND, 20*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            m_creature->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0));
            m_creature->SetInCombatWithZone();
        }
    }

    void Reset()
    {
        m_creature->SetActiveObjectState(true);

        if (!m_GraufGuid.IsEmpty())
            if (Creature* pGrauf = m_creature->GetMap()->GetCreature(m_GraufGuid))
                if (pGrauf->isDead())
                    pGrauf->Respawn();

        SummonMgr.UnsummonAll();
        Events.Reset();
        Events.SetPhase(PHASE_MOUNTED);

        m_bStarted = false;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, NOT_STARTED);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (pSummon)
            pSummon->SetInCombatWithZone();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() && !m_bStarted)
        {
            m_bStarted = true;

            if (m_GraufGuid.IsEmpty())
                BroadcastScriptMessageToEntry(m_creature, NPC_GRAUF, 20.0f);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (Events.GetPhase() == PHASE_LANDED)
            return;

        DoScriptText(SAY_AGGRO, m_creature);
        Events.ScheduleEvent(EVENT_MOUNT_GRAUF, 3*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_WIPE_CHECK, 5*IN_MILLISECONDS, 5*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_SUMMON_WAVE, 5*IN_MILLISECONDS, 25*IN_MILLISECONDS, 0, 0, PMASK_MOUNTED);

    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void SummonNextMobWave()
    {
        for (uint32 i = (m_bIsRegularMode? 5 : 6); i; --i)
        {
            switch (urand(0,2))
            {
                case 0: SummonMgr.SummonCreature(NPC_YMIRJAR_WARRIOR, SpawnLoc[0]+rand()%5, SpawnLoc[1]+rand()%5, SpawnLoc[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
                case 1: SummonMgr.SummonCreature(NPC_YMIRJAR_WITCH_DOCTOR, SpawnLoc[0]+rand()%5, SpawnLoc[1]+rand()%5, SpawnLoc[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
                case 2: SummonMgr.SummonCreature(NPC_YMIRJAR_HARPOONER, SpawnLoc[0]+rand()%5, SpawnLoc[1]+rand()%5, SpawnLoc[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000); break;
            }
        }
    }
    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        SummonMgr.UnsummonAll();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_WIPE_CHECK:
                {
                    Map* pMap = m_creature->GetMap();

                    if (pMap && pMap->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = pMap->GetPlayers();

                        bool wipe = true;
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (i->getSource()->isGameMaster())
                                continue;
                            if (Unit* pPlayer = i->getSource())
                                if (pPlayer->isAlive())
                                    if (pPlayer->IsWithinDist2d(397.017f, -511.496f, 150.0f))
                                    {
                                        wipe = false;
                                        break;
                                    }
                        }
                        if (wipe)
                        {
                            m_creature->ExitVehicle();
                            EnterEvadeMode();
                        }
                    }
                }
                break;
                case EVENT_MOUNT_GRAUF:
                    if (!m_GraufGuid.IsEmpty())
                        if (Unit* pGrauf = m_creature->GetMap()->GetUnit(m_GraufGuid))
                        {
                            m_creature->EnterVehicle(pGrauf->GetVehicleKit(), 0);
                        }
                break;
                case EVENT_SUMMON_WAVE:
                    SummonNextMobWave();
                    break;
                case EVENT_CRUSH:
                    if (m_creature->getVictim())
                        m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H, false);
                    break;
                case EVENT_POISONED_SPEAR:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 1))
                        m_creature->CastSpell(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR : SPELL_POISONED_SPEAR_H, false);
                    break;
                case EVENT_WHIRLWIND:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        DoStartNoMovement(m_creature->getVictim());
                        m_creature->CastSpell(m_creature, m_bIsRegularMode? SPELL_WHIRLWIND : SPELL_WHIRLWIND_H, false);
                        m_creature->GetMotionMaster()->MoveChase(pTarget);
                        Events.ScheduleEvent(EVENT_WHIRLWIND_2, 10*IN_MILLISECONDS);
                    }
                    break;
                case EVENT_WHIRLWIND_2:
                    if (m_creature->getVictim())
                    {
                        m_creature->GetMotionMaster()->Clear();
                        DoStartMovement(m_creature->getVictim());
                    }
                    break;
            }

        if (Events.GetPhase() == PHASE_LANDED)
            DoMeleeAttackIfReady();
    }
};


bool GossipHello_go_harpoon_launcher(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* m_pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData());
    if (!m_pInstance)
        return false;
    if (Creature* pGrauf = pPlayer->GetMap()->GetCreature(m_pInstance->GetData64(DATA_GRAUF)))
    {
        if (pGrauf->GetPositionX() > 500.0f && pGrauf->GetPositionY() < -490.0f)
        {
            npc_graufAI* graufAI = (npc_graufAI*)(pGrauf->AI());
            if (graufAI)
            {
                graufAI->ScriptMessage(pPlayer, 1, 0);
            }
        }
        // TODO - spell should ignore LoS and visual doesnt work unless target is close (in LoS)
        return false;
    }
    return false;
}



CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

CreatureAI* GetAI_npc_grauf(Creature* pCreature)
{
    return new npc_graufAI(pCreature);
}

bool AreaTrigger_at_skadi(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (ScriptedInstance* pInstance = dynamic_cast<ScriptedInstance*>(pPlayer->GetInstanceData()))
    {
        if (pInstance->GetData(TYPE_SKADI) == NOT_STARTED)
            pInstance->SetData(TYPE_SKADI, SPECIAL);
    }

    return false;
}

void AddSC_boss_skadi()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_skadi";
    newscript->GetAI = &GetAI_boss_skadi;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_grauf";
    newscript->GetAI = &GetAI_npc_grauf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_skadi";
    newscript->pAreaTrigger = &AreaTrigger_at_skadi;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_harpoon_launcher";
    newscript->pGOUse = &GossipHello_go_harpoon_launcher;
    newscript->RegisterSelf();
}
