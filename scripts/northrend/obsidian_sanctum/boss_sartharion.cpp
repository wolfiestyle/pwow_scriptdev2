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
SDName: Boss Sartharion
SD%Complete: 95%
SDComment: TODO: Fix flame tsunami (spell-wise)
SDCategory: Obsidian Sanctum
EndScriptData */

#include "precompiled.h"
#include "obsidian_sanctum.h"
#include "TemporarySummon.h"

enum
{
    //Sartharion Yell
    SAY_SARTHARION_AGGRO                        = -1615018,
    SAY_SARTHARION_BERSERK                      = -1615019,
    SAY_SARTHARION_BREATH                       = -1615020,
    SAY_SARTHARION_CALL_SHADRON                 = -1615021,
    SAY_SARTHARION_CALL_TENEBRON                = -1615022,
    SAY_SARTHARION_CALL_VESPERON                = -1615023,
    SAY_SARTHARION_DEATH                        = -1615024,
    SAY_SARTHARION_SPECIAL_1                    = -1615025,
    SAY_SARTHARION_SPECIAL_2                    = -1615026,
    SAY_SARTHARION_SPECIAL_3                    = -1615027,
    SAY_SARTHARION_SPECIAL_4                    = -1615028,
    SAY_SARTHARION_SLAY_1                       = -1615029,
    SAY_SARTHARION_SLAY_2                       = -1615030,
    SAY_SARTHARION_SLAY_3                       = -1615031,

    WHISPER_LAVA_CHURN                          = -1615032,

    WHISPER_SHADRON_DICIPLE                     = -1615008,
    WHISPER_VESPERON_DICIPLE                    = -1615041,
    WHISPER_HATCH_EGGS                          = -1615017,
    WHISPER_OPEN_PORTAL                         = -1615042, // whisper, shared by two dragons

    //Sartharion Spells
    SPELL_BERSERK                               = 61632,    // Increases the caster's attack speed by 150% and all damage it deals by 500% for 5 min.
    SPELL_CLEAVE                                = 56909,    // Inflicts 35% weapon damage to an enemy and its nearest allies, affecting up to 10 targets.
    SPELL_FLAME_BREATH                          = 56908,    // Inflicts 8750 to 11250 Fire damage to enemies in a cone in front of the caster.
    SPELL_FLAME_BREATH_H                        = 58956,    // Inflicts 10938 to 14062 Fire damage to enemies in a cone in front of the caster.
    SPELL_TAIL_LASH                             = 56910,    // A sweeping tail strike hits all enemies behind the caster, inflicting 3063 to 3937 damage and stunning them for 2 sec.
    SPELL_TAIL_LASH_H                           = 58957,    // A sweeping tail strike hits all enemies behind the caster, inflicting 4375 to 5625 damage and stunning them for 2 sec.
    SPELL_WILL_OF_SARTHARION                    = 61254,    // Sartharion's presence bolsters the resolve of the Twilight Drakes, increasing their total health by 25%. This effect also increases Sartharion's health by 25%.
    SPELL_LAVA_STRIKE                           = 57571,    // (Real spell casted should be 57578) 57571 then trigger visual missile, then summon Lava Blaze on impact(spell 57572)
    SPELL_TWILIGHT_REVENGE                      = 60639,

    SPELL_PYROBUFFET                            = 56916,    // currently used for hard enrage after 15 minutes
    SPELL_PYROBUFFET_RANGE                      = 58907,    // possibly used when player get too far away from dummy creatures (2x creature entry 30494)

    SPELL_TWILIGHT_SHIFT_ENTER                  = 57620,    // enter phase. Player get this when click GO
    SPELL_TWILIGHT_SHIFT                        = 57874,    // Twilight Shift Aura
    SPELL_TWILIGHT_SHIFT_REMOVAL                = 61187,    // leave phase
    SPELL_TWILIGHT_SHIFT_REMOVAL_ALL            = 61190,    // leave phase (probably version to make all leave)

    //Mini bosses common spells
    SPELL_TWILIGHT_RESIDUE                      = 61885,    // makes immune to shadow damage, applied when leave phase

    //Miniboses (Vesperon, Shadron, Tenebron)
    SPELL_SHADOW_BREATH_H                       = 59126,    // Inflicts 8788 to 10212 Fire damage to enemies in a cone in front of the caster.
    SPELL_SHADOW_BREATH                         = 57570,    // Inflicts 6938 to 8062 Fire damage to enemies in a cone in front of the caster.

    SPELL_SHADOW_FISSURE_H                      = 59127,    // Deals 9488 to 13512 Shadow damage to any enemy within the Shadow fissure after 5 sec.
    SPELL_SHADOW_FISSURE                        = 57579,    // Deals 6188 to 8812 Shadow damage to any enemy within the Shadow fissure after 5 sec.

    //Vesperon
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_VESPERON                     = 31219,    // Acolyte of Vesperon
    NPC_VESPERON_CONTROLLER                     = 30878,    // controls the spawning of the acolyte + handles debuff removing.
    SPELL_POWER_OF_VESPERON                     = 61251,    // Vesperon's presence decreases the maximum health of all enemies by 25%.
    SPELL_TWILIGHT_TORMENT_VESP                 = 57935,    // (Shadow only) trigger (57948 -> makes boss stop moving, casting, attacking, etc) then 57988
    SPELL_TWILIGHT_TORMENT_VESP_ACO             = 58835,    // (Fire and Shadow) trigger (58853 -> channeled same as above) then 57988

    //Shadron
    //In portal is a disciple, when disciple killed remove Power_of_vesperon, portal open multiple times
    NPC_ACOLYTE_OF_SHADRON                      = 31218,    // Acolyte of Shadron
    NPC_PORTAL_OF_SHADRON                       = 30741,    // controls the debuffs
    SPELL_POWER_OF_SHADRON                      = 58105,    // Shadron's presence increases Fire damage taken by all enemies by 100%.
    SPELL_GIFT_OF_TWILIGHT_SHA                  = 57835,    // TARGET_SCRIPT shadron
    SPELL_GIFT_OF_TWILIGHT_SAR                  = 58766,    // TARGET_SCRIPT sartharion
    SPELL_VOID_BLAST                            = 57581,    // Twilight Fissure
    SPELL_VOID_BLAST_H                          = 59128,

    //Tenebron
    NPC_TENEBRON_EGG_CONTROLLER                 = 31138,    // controlls everything related to the Eggs
    //in the portal spawns 6 eggs, if not killed in time (approx. 20s)  they will hatch,  whelps can cast 60708
    SPELL_POWER_OF_TENEBRON                     = 61248,    // Tenebron's presence increases Shadow damage taken by all enemies by 100%.
    //Tenebron, dummy spell
    SPELL_SUMMON_TWILIGHT_WHELP                 = 58035,    // doesn't work, will spawn NPC_TWILIGHT_WHELP
    SPELL_SUMMON_SARTHARION_TWILIGHT_WHELP      = 58826,    // doesn't work, will spawn NPC_SHARTHARION_TWILIGHT_WHELP

    SPELL_HATCH_EGGS_H                          = 59189,
    SPELL_HATCH_EGGS                            = 58542,
    SPELL_HATCH_EGGS_EFFECT_H                   = 59190,
    SPELL_HATCH_EGGS_EFFECT                     = 58685,

    //Whelps
    NPC_TWILIGHT_EGG                            = 30882,
    NPC_TWILIGHT_WHELP                          = 30890,
    NPC_SHARTHARION_TWILIGHT_WHELP              = 31214,
    SPELL_FADE_ARMOR                            = 60708,    // Reduces the armor of an enemy by 1500 for 15s

    //flame tsunami
    SPELL_FLAME_TSUNAMI                         = 57494,    // the visual dummy
    SPELL_FLAME_TSUNAMI_LEAP                    = 60241,    // SPELL_EFFECT_138 some leap effect, causing caster to move in direction
    SPELL_FLAME_TSUNAMI_DEBUFF                  = 57491,
    SPELL_FLAME_TSUNAMI_DMG_AURA                = 57492,    // periodic damage, npc has this aura
    SPELL_MOLTEN_FURY                           = 60430,    // Spell Applied to the Lava Blazes

    NPC_FLAME_TSUNAMI                           = 30616,    // for the flame waves
    NPC_LAVA_BLAZE                              = 30643,    // adds spawning from flame strike

    //using these custom points for dragons start and end
    POINT_ID_INIT                               = 100,
    POINT_ID_LAND                               = 200,
};

struct Location
{
    float x, y, z;
};

enum Events
{
    EVENT_HARD_ENRAGE = 1,
    EVENT_TAIL_SWEEP,
    EVENT_FLAME_TSUNAMI,
    EVENT_FLAME_BREATH,
    EVENT_CLEAVE,
    EVENT_CALL_TENEBRON,
    EVENT_CALL_SHADRON,
    EVENT_CALL_VESPERON,
    EVENT_LAVA_STRIKE,
};

enum ControllerEvents
{
    EVENT_SUMMON = 1,
    EVENT_DEBUFF,
    EVENT_CLEAR_DEBUFF,
    EVENT_WIPE,
};

//each dragons special points. First where fly to before connect to connon, second where land point is.
static Location const m_aTene[]=
{
    {3212.854f, 575.597f, 109.856f},                        //init
    {3246.425f, 565.367f, 61.249f}                          //end
};

static Location const m_aShad[]=
{
    {3293.238f, 472.223f, 106.968f},
    {3271.669f, 526.907f, 61.931f}
};

static Location const m_aVesp[]=
{
    {3193.310f, 472.861f, 102.697f},
    {3227.268f, 533.238f, 59.995f}
};

#define FACTION_HOSTILE 14
#define MAX_WAYPOINT    6
//points around raid "isle", counter clockwise. should probably be adjusted to be more alike
static Location const m_aDragonCommon[MAX_WAYPOINT]=
{
    {3214.012f, 468.932f, 98.652f},
    {3244.950f, 468.427f, 98.652f},
    {3283.520f, 496.869f, 98.652f},
    {3287.316f, 555.875f, 98.652f},
    {3250.479f, 585.827f, 98.652f},
    {3209.969f, 566.523f, 98.652f}
};

static Location const FlameRight1Spawn =     { 3197.59f, 495.336f, 57.8462f };
static Location const FlameRight1Direction = { 3289.28f, 521.569f, 57.1526f };
static Location const FlameRight2Spawn =     { 3201.94f, 543.324f, 57.7209f };
static Location const FlameRight2Direction = { 3288.98f, 549.291f, 57.1232f };
static Location const FlameLeft1Spawn =      { 3290.24f, 521.725f, 57.1238f };
static Location const FlameLeft1Direction =  { 3199.94f, 516.891f, 57.5112f };
static Location const FlameLeft2Spawn =      { 3290.33f, 564.51f,  57.063f };
static Location const FlameLeft2Direction =  { 3195.03f, 550.135f, 57.6331f };

static Location const AcolyteofShadron =     { 3363.92f, 534.703f, 97.2683f };
static Location const AcolyteofShadron2 =    { 3246.57f, 551.263f, 58.6164f };
static Location const AcolyteofVesperon =    { 3145.68f, 520.71f,  89.7f };
static Location const AcolyteofVesperon2 =   { 3246.57f, 551.263f, 58.6164f };

static Location const TwilightEggs[] =
{
    {3219.28f, 669.121f, 88.5549f},
    {3221.55f, 682.852f, 90.5361f},
    {3239.77f, 685.94f,  90.3168f},
    {3250.33f, 669.749f, 88.7637f},
    {3246.6f,  642.365f, 84.8752f},
    {3233.68f, 653.117f, 85.7051f}
};

static Location const TwilightEggsSarth[] =
{
    {3261.75f, 539.14f,  58.6082f},
    {3257.41f, 512.939f, 58.5432f},
    {3231.04f, 498.281f, 58.6439f}
};

/*######
## Boss Sartharion
######*/

struct MANGOS_DLL_DECL boss_sartharionAI : public ScriptedAI, public ScriptEventInterface
{
    boss_sartharionAI(Creature* pCreature) : ScriptedAI(pCreature), ScriptEventInterface(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode :1;
    bool m_bIsBerserk :1;
    bool m_bIsSoftEnraged :1;
    bool m_bIsHardEnraged :1;

    void Reset()
    {
        m_bIsBerserk = false;
        m_bIsSoftEnraged = false;
        m_bIsHardEnraged = false;
        Events.Reset();

        if (m_creature->HasAura(SPELL_TWILIGHT_REVENGE))
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_REVENGE);

        if (m_pInstance)
            m_pInstance->SetData(DATA_DRAKES, 0);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_SARTHARION_AGGRO, m_creature);

        m_creature->SetInCombatWithZone();

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, IN_PROGRESS);
            FetchDragons();
        }

        Events.ScheduleEvent(EVENT_HARD_ENRAGE, MINUTE*15*IN_MILLISECONDS);
        Events.ScheduleEvent(EVENT_FLAME_TSUNAMI, 30000);
        Events.ScheduleEvent(EVENT_FLAME_BREATH, 20000);
        Events.ScheduleEvent(EVENT_TAIL_SWEEP, 20000);
        Events.ScheduleEvent(EVENT_CLEAVE, 7000);
        Events.ScheduleEvent(EVENT_LAVA_STRIKE, 5000);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_SARTHARION_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SARTHARION_EVENT, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SARTHARION_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SARTHARION_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SARTHARION_SLAY_3, m_creature); break;
        }
    }

    //some loot handling should be applied in order to reward players based on achieve rather than luck
    void AddDrakeLootMode()
    {
    }

    void FetchDragons()
    {
        if (!m_pInstance)
            return;
        Creature *pTene = GET_CREATURE(DATA_TENEBRON);
        Creature *pShad = GET_CREATURE(DATA_SHADRON);
        Creature *pVesp = GET_CREATURE(DATA_VESPERON);

        //can use Will if at least one of the dragons are alive and are being called
        uint32 drakeCount = 0;

        if (pTene && pTene->isAlive() && !pTene->getVictim())
        {
            drakeCount++;
            pTene->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aTene[0].x, m_aTene[0].y, m_aTene[0].z);

            Events.ScheduleEvent(EVENT_CALL_TENEBRON, 30000);
            if (!pTene->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pTene->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (pShad && pShad->isAlive() && !pShad->getVictim())
        {
            drakeCount++;
            pShad->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aShad[0].x, m_aShad[0].y, m_aShad[0].z);

            Events.ScheduleEvent(EVENT_CALL_SHADRON, 75000);
            if (!pShad->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pShad->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (pVesp && pVesp->isAlive() && !pVesp->getVictim())
        {
            drakeCount++;
            pVesp->GetMotionMaster()->MovePoint(POINT_ID_INIT, m_aVesp[0].x, m_aVesp[0].y, m_aVesp[0].z);

            Events.ScheduleEvent(EVENT_CALL_VESPERON, 120000);
            if (!pVesp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                pVesp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        if (m_pInstance)
            m_pInstance->SetData(DATA_DRAKES, drakeCount);

        if (drakeCount)
        {
            if (pTene)
                pTene->CastSpell(pTene, SPELL_WILL_OF_SARTHARION, true);
            if (pVesp)
                pVesp->CastSpell(pVesp, SPELL_WILL_OF_SARTHARION, true);
            if (pShad)
                pShad->CastSpell(pShad, SPELL_WILL_OF_SARTHARION, true);

            //FIXME: this shouldn't be needed to be casted from victim
            if (Unit *victim = m_creature->getVictim())
                victim->CastSpell(m_creature, SPELL_WILL_OF_SARTHARION, true);
            //FIXME: make spell properly increase maximum health from core
            m_creature->SetHealthPercent(100.0f);
        }
    }

    void CallDragon(uint32 uiDataId)
    {
        if (m_pInstance)
        {
            Creature *pTemp = GET_CREATURE(uiDataId);

            if (pTemp && pTemp->isAlive() && !pTemp->getVictim())
            {
                if (pTemp->HasSplineFlag(SPLINEFLAG_WALKMODE))
                    pTemp->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

                if (pTemp->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                int32 iTextId = 0;
                //TODO: Add support for loot handling
                AddDrakeLootMode();

                switch(pTemp->GetEntry())
                {
                    case NPC_TENEBRON:
                        iTextId = SAY_SARTHARION_CALL_TENEBRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aTene[1].x, m_aTene[1].y, m_aTene[1].z);
                        break;
                    case NPC_SHADRON:
                        iTextId = SAY_SARTHARION_CALL_SHADRON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aShad[1].x, m_aShad[1].y, m_aShad[1].z);
                        break;
                    case NPC_VESPERON:
                        iTextId = SAY_SARTHARION_CALL_VESPERON;
                        pTemp->GetMotionMaster()->MovePoint(POINT_ID_LAND, m_aVesp[1].x, m_aVesp[1].y, m_aVesp[1].z);
                        break;
                }

                DoScriptText(iTextId, m_creature);
            }
        }
    }

    void SendFlameTsunami()
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (i->getSource()->isAlive())
                        DoScriptText(WHISPER_LAVA_CHURN, m_creature,i->getSource());
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Creature *pTene = GET_CREATURE(DATA_TENEBRON);
        Creature *pShad = GET_CREATURE(DATA_SHADRON);
        Creature *pVesp = GET_CREATURE(DATA_VESPERON);

        //spell will target dragons, if they are still alive at 35% (soft enrage)
        if (m_creature->GetHealthPercent() < 35.0f && !m_bIsSoftEnraged
            && ((pTene && pTene->isAlive()) || (pShad && pShad->isAlive()) || (pVesp && pVesp->isAlive())))
        {
            DoScriptText(SAY_SARTHARION_BERSERK, m_creature);
            m_creature->InterruptNonMeleeSpells(false);
            if (pTene && pTene->isAlive())
                pTene->CastSpell(pTene, SPELL_BERSERK, false);
            if (pShad && pShad->isAlive())
                pShad->CastSpell(pShad, SPELL_BERSERK, false);
            if (pVesp && pVesp->isAlive())
                pVesp->CastSpell(pVesp, SPELL_BERSERK, false);
            m_bIsSoftEnraged = true;
        }

        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_HARD_ENRAGE:
                    DoCastSpellIfCan(m_creature, SPELL_PYROBUFFET, CAST_TRIGGERED);
                    m_bIsHardEnraged = true;
                    Events.ScheduleEvent(EVENT_HARD_ENRAGE, 5000);
                    break;
                case EVENT_FLAME_TSUNAMI:
                    SendFlameTsunami();
                    switch(urand(0,1))
                    {
                        case 0:
                        {
                            if (Creature *Right1 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, FlameRight1Spawn.x, FlameRight1Spawn.y, FlameRight1Spawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 12000))
                                Right1->GetMotionMaster()->MovePoint(0, FlameRight1Direction.x, FlameRight1Direction.y, FlameRight1Direction.z);
                            if (Creature *Right2 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, FlameRight2Spawn.x, FlameRight2Spawn.y, FlameRight2Spawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 12000))
                                Right2->GetMotionMaster()->MovePoint(0, FlameRight2Direction.x, FlameRight2Direction.y, FlameRight2Direction.z);
                            break;
                        }
                        case 1:
                        {
                            if (Creature *Left1 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, FlameLeft1Spawn.x, FlameLeft1Spawn.y, FlameLeft1Spawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 12000))
                                Left1->GetMotionMaster()->MovePoint(0, FlameLeft1Direction.x, FlameLeft1Direction.y, FlameLeft1Direction.z);
                            if (Creature *Left2 = m_creature->SummonCreature(NPC_FLAME_TSUNAMI, FlameLeft2Spawn.x, FlameLeft2Spawn.y, FlameLeft2Spawn.z, 0, TEMPSUMMON_TIMED_DESPAWN, 12000))
                                Left2->GetMotionMaster()->MovePoint(0, FlameLeft2Direction.x, FlameLeft2Direction.y, FlameLeft2Direction.z);
                            break;
                        }
                    }
                    Events.ScheduleEvent(EVENT_FLAME_TSUNAMI, 30000);
                    break;
                case EVENT_FLAME_BREATH:
                    DoScriptText(SAY_SARTHARION_BREATH, m_creature);
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_FLAME_BREATH));
                    Events.ScheduleEvent(EVENT_FLAME_BREATH, urand(25000, 35000));
                    break;
                case EVENT_TAIL_SWEEP:
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_TAIL_LASH));
                    Events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(15000, 20000));
                    break;
                case EVENT_CLEAVE:
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
                    Events.ScheduleEvent(EVENT_CLEAVE, urand(7000, 10000));
                    break;
                case EVENT_LAVA_STRIKE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                    {
                        DoCastSpellIfCan(pTarget, SPELL_LAVA_STRIKE);

                        switch(urand(0, 15))
                        {
                            case 0: DoScriptText(SAY_SARTHARION_SPECIAL_1, m_creature); break;
                            case 1: DoScriptText(SAY_SARTHARION_SPECIAL_2, m_creature); break;
                            case 2: DoScriptText(SAY_SARTHARION_SPECIAL_3, m_creature); break;
                        }
                    }
                    Events.ScheduleEvent(EVENT_LAVA_STRIKE, urand(5000, 20000));
                    break;
                case EVENT_CALL_TENEBRON:
                    CallDragon(DATA_TENEBRON);
                    break;
                case EVENT_CALL_SHADRON:
                    CallDragon(DATA_SHADRON);
                    break;
                case EVENT_CALL_VESPERON:
                    CallDragon(DATA_VESPERON);
                    break;
            }
        }

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(uiDiff);
    }
};

enum DrakeEvents
{
    EVENT_SHADOW_FISSURE = 1,
    EVENT_SHADOW_BREATH,
    EVENT_DRAKE_SPECIAL,    //for portals each drake's special phase (portal spawns, eggs...)
};

enum TeneText
{
    SAY_TENEBRON_AGGRO                      = -1615009,
    SAY_TENEBRON_SLAY_1                     = -1615010,
    SAY_TENEBRON_SLAY_2                     = -1615011,
    SAY_TENEBRON_DEATH                      = -1615012,
    SAY_TENEBRON_BREATH                     = -1615013,
    SAY_TENEBRON_RESPOND                    = -1615014,
    SAY_TENEBRON_SPECIAL_1                  = -1615015,
    SAY_TENEBRON_SPECIAL_2                  = -1615016
};

enum ShadText
{
    SAY_SHADRON_AGGRO                       = -1615000,
    SAY_SHADRON_SLAY_1                      = -1615001,
    SAY_SHADRON_SLAY_2                      = -1615002,
    SAY_SHADRON_DEATH                       = -1615003,
    SAY_SHADRON_BREATH                      = -1615004,
    SAY_SHADRON_RESPOND                     = -1615005,
    SAY_SHADRON_SPECIAL_1                   = -1615006,
    SAY_SHADRON_SPECIAL_2                   = -1615007
};

enum VespText
{
    SAY_VESPERON_AGGRO                      = -1615033,
    SAY_VESPERON_SLAY_1                     = -1615034,
    SAY_VESPERON_SLAY_2                     = -1615035,
    SAY_VESPERON_DEATH                      = -1615036,
    SAY_VESPERON_BREATH                     = -1615037,
    SAY_VESPERON_RESPOND                    = -1615038,
    SAY_VESPERON_SPECIAL_1                  = -1615039,
    SAY_VESPERON_SPECIAL_2                  = -1615040
};

//to control each dragons common abilities
struct MANGOS_DLL_DECL dummy_dragonAI : public ScriptedAI, public ScriptEventInterface
{
    SummonManager SummonMgr;
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWaypointId;
    uint32 m_uiMoveNextTimer;
    int32 m_iPortalRespawnTime;
    bool m_bCanMoveFree;

    dummy_dragonAI(Creature* pCreature) : ScriptedAI(pCreature), ScriptEventInterface(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    void Reset()
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (GameObject *TwilightPortal = GetClosestGameObjectWithEntry(m_creature, GO_TWILIGHT_PORTAL, 100.0f))
            TwilightPortal->SetGoState(GO_STATE_READY);

        m_uiWaypointId = 0;
        m_uiMoveNextTimer = 500;
        m_iPortalRespawnTime = 30000;
        m_bCanMoveFree = false;
        BroadcastEvent(EVENT_WIPE, 0, 100.0f); // in case we wipe, we remove the controllers
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (!m_pInstance || uiType != POINT_MOTION_TYPE)
            return;

        debug_log("dummy_dragonAI: %s reached point %u", m_creature->GetName(), uiPointId);

        //if healers messed up the raid and we was already initialized
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
        {
            EnterEvadeMode();
            return;
        }

        //this is the end (!)
        if (uiPointId == POINT_ID_LAND)
        {
            m_creature->GetMotionMaster()->Clear();
            m_bCanMoveFree = false;
            m_creature->SetInCombatWithZone();
            return;
        }

        //get amount of common points
        uint32 uiCommonWPCount = sizeof(m_aDragonCommon)/sizeof(Location);

        //increase
        m_uiWaypointId = uiPointId+1;

        //if we have reached a point bigger or equal to count, it mean we must reset to point 0
        if (m_uiWaypointId >= uiCommonWPCount)
        {
            if (!m_bCanMoveFree)
                m_bCanMoveFree = true;

            m_uiWaypointId = 0;
        }

        m_uiMoveNextTimer = 500;
    }

    //used when open portal and spawn mobs in phase
    void DoRaidWhisper(int32 iTextId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    DoScriptText(iTextId, m_creature, i->getSource());
            }
        }
    }

    //"opens" the portal and does the "opening" whisper
    void OpenPortal()
    {
        int32 iTextId = 0;

        //there are 4 portal spawn locations, each are expected to be spawned with negative spawntimesecs in database

        //using a grid search here seem to be more efficient than caching all four guids
        //in instance script and calculate range to each.
        GameObject* pPortal = GetClosestGameObjectWithEntry(m_creature,GO_TWILIGHT_PORTAL,50.0f);
        DoRaidWhisper(WHISPER_OPEN_PORTAL);
        switch (m_creature->GetEntry())
        {
            case NPC_TENEBRON:
                SummonMgr.SummonCreature(NPC_TENEBRON_EGG_CONTROLLER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+3, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30*IN_MILLISECONDS);
                break;
            case NPC_SHADRON:
                SummonMgr.SummonCreature(NPC_PORTAL_OF_SHADRON, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+3, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 6*MINUTE*IN_MILLISECONDS);
                break;
            case NPC_VESPERON:
                SummonMgr.SummonCreature(NPC_VESPERON_CONTROLLER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+3, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 6*MINUTE*IN_MILLISECONDS);
                break;
        }

        //By using SetRespawnTime() we will actually "spawn" the object with our defined time.
        //Once time is up, portal will disappear again.
        if (pPortal && !pPortal->isSpawned())
            pPortal->SetRespawnTime(m_iPortalRespawnTime);

        //Unclear what are expected to happen if one drake has a portal open already
        //Refresh respawnTime so time again are set to 30secs?
    }

    //Removes each drakes unique debuff from players
    void RemoveDebuff(uint32 uiSpellId)
    {
        Map* pMap = m_creature->GetMap();

        if (pMap && pMap->IsDungeon())
        {
            Map::PlayerList const &PlayerList = pMap->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && i->getSource()->HasAura(uiSpellId))
                    i->getSource()->RemoveAurasDueToSpell(uiSpellId);
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        int32 iTextId = 0;
        uint32 uiSpellId = 0;

        switch(m_creature->GetEntry())
        {
            case NPC_TENEBRON:
                iTextId = SAY_TENEBRON_DEATH;
                uiSpellId = SPELL_POWER_OF_TENEBRON;
                break;
            case NPC_SHADRON:
                iTextId = SAY_SHADRON_DEATH;
                uiSpellId = SPELL_POWER_OF_SHADRON;
                break;
            case NPC_VESPERON:
                iTextId = SAY_VESPERON_DEATH;
                uiSpellId = SPELL_POWER_OF_VESPERON;
                break;
        }

        DoScriptText(iTextId, m_creature);

        RemoveDebuff(uiSpellId);

        if (m_pInstance)
        {
            // not if solo mini-boss fight
            if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                return;

            // Twilight Revenge to main boss
            if (Creature *pSartharion = GET_CREATURE(DATA_SARTHARION))
                if (pSartharion->isAlive())
                    m_creature->CastSpell(pSartharion, SPELL_TWILIGHT_REVENGE, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bCanMoveFree && m_uiMoveNextTimer)
        {
            if (m_uiMoveNextTimer <= uiDiff)
            {
                if (m_uiWaypointId < MAX_WAYPOINT)
                m_creature->GetMotionMaster()->MovePoint(m_uiWaypointId,
                    m_aDragonCommon[m_uiWaypointId].x, m_aDragonCommon[m_uiWaypointId].y, m_aDragonCommon[m_uiWaypointId].z);

                debug_log("dummy_dragonAI: %s moving to point %u", m_creature->GetName(), m_uiWaypointId);
                m_uiMoveNextTimer = 0;
            }
            else
                m_uiMoveNextTimer -= uiDiff;
        }
    }
};

/*######
## Mob Tenebron
######*/

struct MANGOS_DLL_DECL mob_tenebronAI : public dummy_dragonAI
{
    mob_tenebronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    void Reset()
    {
        Events.Reset();
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {        
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
            DoScriptText(SAY_TENEBRON_AGGRO, m_creature);
        else
            DoScriptText(SAY_TENEBRON_RESPOND, m_creature);
        Events.ScheduleEvent(EVENT_SHADOW_FISSURE, 5000);
        Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, 30000);
        Events.ScheduleEvent(EVENT_SHADOW_BREATH, 20000);
        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_TENEBRON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_TENEBRON_SLAY_1 : SAY_TENEBRON_SLAY_2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }
        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SHADOW_FISSURE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCastSpellIfCan(pTarget, DIFFICULTY(SPELL_SHADOW_FISSURE));
                    Events.ScheduleEvent(EVENT_SHADOW_FISSURE, urand(15000, 20000));
                    break;
                case EVENT_DRAKE_SPECIAL:
                    OpenPortal();
                    Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, urand(30000, 45000));
                    break;
                case EVENT_SHADOW_BREATH:
                    DoScriptText(SAY_TENEBRON_BREATH, m_creature);
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SHADOW_BREATH));
                    Events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(20000, 25000));
                    break;
            }

        DoMeleeAttackIfReady();
    }
};


/*######
## Mob Shadron
######*/

struct MANGOS_DLL_DECL mob_shadronAI : public dummy_dragonAI
{
    mob_shadronAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    void Reset()
    {
        Events.Reset();
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_creature->HasAura(SPELL_TWILIGHT_TORMENT_VESP))
            m_creature->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP);

        if (m_creature->HasAura(SPELL_GIFT_OF_TWILIGHT_SHA))
            m_creature->RemoveAurasDueToSpell(SPELL_GIFT_OF_TWILIGHT_SHA);
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEvent(EVENT_SHADOW_FISSURE, 5000);
        Events.ScheduleEvent(EVENT_SHADOW_BREATH, 20000);
        Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, 60000);
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
            DoScriptText(SAY_SHADRON_AGGRO, m_creature);
        else
            DoScriptText(SAY_SHADRON_RESPOND, m_creature);
        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_SHADRON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SHADRON_SLAY_1 : SAY_SHADRON_SLAY_2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SHADOW_FISSURE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCastSpellIfCan(pTarget, DIFFICULTY(SPELL_SHADOW_FISSURE));
                    Events.ScheduleEvent(EVENT_SHADOW_FISSURE, urand(15000, 20000));
                    break;
                case EVENT_DRAKE_SPECIAL:
                    OpenPortal();
                    Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, urand(60000, 65000));
                    break;
                case EVENT_SHADOW_BREATH:
                    DoScriptText(SAY_SHADRON_BREATH, m_creature);
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SHADOW_BREATH));
                    Events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(20000, 25000));
                    break;
                }

        DoMeleeAttackIfReady();
    }
};


/*######
## Mob Vesperon
######*/

struct MANGOS_DLL_DECL mob_vesperonAI : public dummy_dragonAI
{
    mob_vesperonAI(Creature* pCreature) : dummy_dragonAI(pCreature) { Reset(); }

    void Reset()
    {
        Events.Reset();
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {
        Events.ScheduleEvent(EVENT_SHADOW_BREATH, 20000);
        Events.ScheduleEvent(EVENT_SHADOW_FISSURE, 5000);
        Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, 60000);

        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
            DoScriptText(SAY_VESPERON_AGGRO, m_creature);
        else
            DoScriptText(SAY_VESPERON_RESPOND, m_creature);

        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_POWER_OF_VESPERON);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_VESPERON_SLAY_1 : SAY_VESPERON_SLAY_2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //if no target, update dummy and return
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            dummy_dragonAI::UpdateAI(uiDiff);
            return;
        }

        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SHADOW_FISSURE:
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                        DoCastSpellIfCan(pTarget, DIFFICULTY(SPELL_SHADOW_FISSURE));
                    Events.ScheduleEvent(EVENT_SHADOW_FISSURE, urand(15000, 20000));
                    break;
                }
                case EVENT_DRAKE_SPECIAL:
                    OpenPortal();
                    Events.ScheduleEvent(EVENT_DRAKE_SPECIAL, urand(60000, 70000));
                    break;
                case EVENT_SHADOW_BREATH:
                    DoScriptText(SAY_VESPERON_BREATH, m_creature);
                    DoCastSpellIfCan(m_creature->getVictim(), DIFFICULTY(SPELL_SHADOW_BREATH));
                    Events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(20000, 25000));
                    break;
            }

        DoMeleeAttackIfReady();
    }
};
/*######
## Mob Portal of Shadron 
######*/

struct MANGOS_DLL_DECL mob_portal_of_shadronAI : public ScriptedAI, public ScriptEventInterface
{
    SummonManager SummonMgr;
    mob_portal_of_shadronAI(Creature* pCreature) : ScriptedAI(pCreature), ScriptEventInterface(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;

    void Reset()
    {
        SummonMgr.UnsummonAll();
        Events.Reset();
        m_creature->SetDisplayId(11686);
        Events.ScheduleEvent(EVENT_SUMMON, 1000);
        Events.ScheduleEvent(EVENT_DEBUFF, 1000);
        //Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 58000); //this is not required, on wipe it will be removed automatically (controller despawn)
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (pSummon)
        {
            SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
            Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 2000);
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SUMMON:
                {
                    m_creature->MonsterTextEmote(WHISPER_SHADRON_DICIPLE, 0, true);
                    if (m_pInstance && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                        m_creature->SummonCreature(NPC_ACOLYTE_OF_SHADRON, AcolyteofShadron.x, AcolyteofShadron.y , AcolyteofShadron.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    else
                        m_creature->SummonCreature(NPC_ACOLYTE_OF_SHADRON, AcolyteofShadron2.x, AcolyteofShadron2.y , AcolyteofShadron2.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    break;
                }
                case EVENT_DEBUFF:
                {
                    m_creature->CastSpell(NULL, m_pInstance->GetData(TYPE_SARTHARION_EVENT) == IN_PROGRESS ? SPELL_GIFT_OF_TWILIGHT_SAR : SPELL_GIFT_OF_TWILIGHT_SHA, true);
                    break;
                }
                case EVENT_WIPE:
                    SummonMgr.UnsummonAll();
                    // no break;
                case EVENT_CLEAR_DEBUFF:
                {     
                    Map *map = m_creature->GetMap();
                    if (map->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = map->GetPlayers();

                        if (PlayerList.isEmpty())
                            return;

                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            Unit* pDebuffTarget = i->getSource();
                            if (pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_SHIFT) && !pDebuffTarget->getVictim())
                            {
                                pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, true);
                                pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_RESIDUE, true);
                                pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT);
                                pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
                            }
                        }
                    }
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    break;
                }
            }
    }
};

/*######
## Mob Acolyte of Shadron
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_shadronAI : public ScriptedAI
{
    mob_acolyte_of_shadronAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        m_creature->SetPhaseMask(16, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Vesperon Controller
######*/

struct MANGOS_DLL_DECL mob_vesperon_controllerAI : public ScriptedAI, public ScriptEventInterface
{
    SummonManager SummonMgr;
    ScriptedInstance* m_pInstance;

    mob_vesperon_controllerAI(Creature* pCreature) : ScriptedAI(pCreature), ScriptEventInterface(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        Events.Reset();
        m_creature->SetDisplayId(11686);
        Events.ScheduleEvent(EVENT_SUMMON, 1000);
        Events.ScheduleEvent(EVENT_DEBUFF, 1000);
        Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 59000);
    }

    void SummonedCreatureJustDied(Creature *pSummon)
    {
        if (pSummon)
        {
            SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
            Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 2000);
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SUMMON:
                {
                    m_creature->MonsterTextEmote(WHISPER_VESPERON_DICIPLE, 0, true);
                    if (m_pInstance && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                        SummonMgr.SummonCreature(NPC_ACOLYTE_OF_VESPERON, AcolyteofVesperon.x, AcolyteofVesperon.y, AcolyteofVesperon.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    else
                        SummonMgr.SummonCreature(NPC_ACOLYTE_OF_VESPERON, AcolyteofVesperon2.x, AcolyteofVesperon2.y, AcolyteofVesperon2.z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    break;
                }
                case EVENT_DEBUFF:
                {
                    Creature* pVesp = GET_CREATURE(DATA_VESPERON);
                    if (pVesp)
                         pVesp->CastSpell(pVesp, SPELL_TWILIGHT_TORMENT_VESP, true);
                    break;
                }
                case EVENT_WIPE:
                    SummonMgr.UnsummonAll();
                    // no break;
                case EVENT_CLEAR_DEBUFF:
                {
                    Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

                    if (PlayerList.isEmpty())
                        break;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Unit* pDebuffTarget = i->getSource();
                        if (pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_SHIFT) && !pDebuffTarget->getVictim()) 
                        {
                            pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, true);
                            pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_RESIDUE, true);
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT);
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
                        }
                        if (pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_TORMENT_VESP))
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP);

                        if (pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_TORMENT_VESP_ACO))
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_TORMENT_VESP_ACO);
                    }
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    break;
                }
            }
    }
};

/*######
## Mob Acolyte of Vesperon
######*/

struct MANGOS_DLL_DECL mob_acolyte_of_vesperonAI : public ScriptedAI
{
    mob_acolyte_of_vesperonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_creature->SetPhaseMask(16, false);
        m_creature->SetInCombatWithZone();
    }

    void Aggro(Unit* pWho)
    {
        m_creature->CastSpell(m_creature, SPELL_TWILIGHT_TORMENT_VESP_ACO, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Tenebron Egg Controller
######*/

struct MANGOS_DLL_DECL mob_tenebron_egg_controllerAI : public ScriptedAI, public ScriptEventInterface
{
    ScriptedInstance* m_pInstance;
    bool done, summoned;
    SummonManager SummonMgr;

    mob_tenebron_egg_controllerAI(Creature* pCreature) : ScriptedAI(pCreature), ScriptEventInterface(pCreature), SummonMgr(pCreature)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    void Reset()
    {
        SummonMgr.UnsummonAll();
        Events.Reset();
        m_creature->SetDisplayId(11686);
        done = false;
        summoned = false;
        Events.ScheduleEvent(EVENT_SUMMON, 100);
        Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 20100);
    }

    void SummonedCreatureJustDied(Creature* pSummon)
    {
        if (pSummon)
            SummonMgr.RemoveSummonFromList(pSummon->GetObjectGuid());
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!SummonMgr.GetSummonCount() && summoned && !done)
        {
            Events.ScheduleEvent(EVENT_CLEAR_DEBUFF, 2000);
            done = true;
        }

        Events.Update(uiDiff);

        while (uint32 eventId = Events.ExecuteEvent())
            switch (eventId)
            {
                case EVENT_SUMMON:
                {
                    if (m_pInstance && m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
                    {
                        for (int i = 0 ; i < 6; i++)
                            SummonMgr.SummonCreature(NPC_TWILIGHT_EGG, TwilightEggs[0].x+(rand()%5-2.5f), TwilightEggs[0].y+(rand()%5-2.5f), TwilightEggs[0].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    }
                    else
                    {
                        for (int i = 0 ; i < 6; i++)
                            SummonMgr.SummonCreature(NPC_TWILIGHT_EGG, TwilightEggsSarth[0].x+(rand()%5-2.5f), TwilightEggsSarth[0].y+(rand()%5-2.5f), TwilightEggsSarth[0].z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                    }
                    summoned = true;
                    if (Creature *pTene = GET_CREATURE(DATA_TENEBRON))
                        pTene->MonsterTextEmote(WHISPER_HATCH_EGGS, 0, true);
                    break;
                }
                case EVENT_CLEAR_DEBUFF:
                {
                    //send players back to the normal realm (eggs failed to be destroyed so whelps spawned in the normal realm)
                    Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                        Unit* pDebuffTarget = i->getSource();
                        if (pDebuffTarget->isAlive() && pDebuffTarget->HasAura(SPELL_TWILIGHT_SHIFT) && !pDebuffTarget->getVictim())
                        {
                            pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_SHIFT_REMOVAL_ALL, true);
                            pDebuffTarget->CastSpell(pDebuffTarget, SPELL_TWILIGHT_RESIDUE, true);
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT);
                            pDebuffTarget->RemoveAurasDueToSpell(SPELL_TWILIGHT_SHIFT_ENTER);
                        }
                    }
                    m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    break;
                }
            }
    }
};

/*######
## Mob Twilight Eggs
######*/

struct MANGOS_DLL_DECL mob_twilight_eggsAI : public ScriptedAI
{
    mob_twilight_eggsAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint32 m_uiHatchEggTimer;

    void Reset()
    {
        m_uiHatchEggTimer = 20000;
        m_creature->SetPhaseMask(16, false);
    }    

    void SpawnWhelps()
    {
        if (m_pInstance->GetData(TYPE_SARTHARION_EVENT) != IN_PROGRESS)
            m_creature->SummonCreature(NPC_TWILIGHT_WHELP, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
        else
            m_creature->SummonCreature(NPC_SHARTHARION_TWILIGHT_WHELP, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
        m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void UpdateAI(const uint32 uiDiff) 
    {
        // hatch eggs
        if (m_uiHatchEggTimer <= uiDiff)
            SpawnWhelps();
        else
            m_uiHatchEggTimer -= uiDiff;
    }

    void AttackStart(Unit* pWho) { }
    void MoveInLineOfSight(Unit* pWho) { }
};

/*######
## Flame Tsunami
######*/

struct mob_flame_tsunamiAI : public ScriptedAI
{
    mob_flame_tsunamiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 damageAuraTimer;

    void Reset()
    {
        m_creature->SetLevel(83);
        m_creature->setFaction(14);
        m_creature->SetDisplayId(11686);
        m_creature->CastSpell(m_creature, SPELL_FLAME_TSUNAMI, true);
        m_creature->SetSpeedRate(MOVE_WALK, 3.0f, true);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        damageAuraTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (damageAuraTimer < uiDiff)
        {
            Map* pMap = m_creature->GetMap();

            if (pMap && pMap->IsDungeon())
            {
                m_creature->CastSpell(m_creature, SPELL_FLAME_TSUNAMI_DEBUFF, true); //TODO : Check why it doesnt apply the DoT on players

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    Unit* pTarget = i->getSource();

                    if (pTarget->IsWithinDistInMap(m_creature, 7.0f))
                    {
                        pTarget->CastSpell(pTarget, SPELL_FLAME_TSUNAMI_LEAP, true);
                    }
                }
            }

            if (Unit* pBlaze = m_creature->SelectRandomUnfriendlyTarget(NULL, 10.0f))
                pBlaze->CastSpell(pBlaze, SPELL_MOLTEN_FURY, true);

            damageAuraTimer = 1000;
        }
        else
            damageAuraTimer -= uiDiff;
    }
};

/*######
## Twilight Fissure
######*/

struct mob_twilight_fissureAI : public Scripted_NoMovementAI
{
    mob_twilight_fissureAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        pCreature->setFaction(FACTION_HOSTILE);
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 VoidBlast_Timer;

    void Reset()
    {
        VoidBlast_Timer = 5000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (VoidBlast_Timer <= diff)
        {
            DoCast(m_creature->getVictim(), DIFFICULTY(SPELL_VOID_BLAST), true);
            VoidBlast_Timer = 9000;
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, SELF_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        }
        else
            VoidBlast_Timer -= diff;
    }
};

/*######
## Mob Twilight Whelps
######*/

struct MANGOS_DLL_DECL mob_twilight_whelpAI : public ScriptedAI
{
    mob_twilight_whelpAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiFadeArmorTimer;

    void Reset()
    {
        m_uiFadeArmorTimer = 1000;
        m_creature->SetPhaseMask(1, false);
        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->isTemporarySummon())
                static_cast<TemporarySummon*>(m_creature)->UnSummon();
            return;
        }

        // twilight torment
        if (m_uiFadeArmorTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FADE_ARMOR);
            m_uiFadeArmorTimer = urand(5000, 10000);
        }
        else
            m_uiFadeArmorTimer -= uiDiff;

        m_creature->SetInCombatWithZone();
        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_sartharion()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_sartharion);
    REGISTER_SCRIPT(mob_vesperon);
    REGISTER_SCRIPT(mob_shadron);
    REGISTER_SCRIPT(mob_tenebron);
    REGISTER_SCRIPT(mob_portal_of_shadron);
    REGISTER_SCRIPT(mob_acolyte_of_shadron);
    REGISTER_SCRIPT(mob_vesperon_controller);
    REGISTER_SCRIPT(mob_acolyte_of_vesperon);
    REGISTER_SCRIPT(mob_tenebron_egg_controller);
    REGISTER_SCRIPT(mob_twilight_eggs);
    REGISTER_SCRIPT(mob_twilight_whelp);
    REGISTER_SCRIPT(mob_flame_tsunami);
    REGISTER_SCRIPT(mob_twilight_fissure);
}
