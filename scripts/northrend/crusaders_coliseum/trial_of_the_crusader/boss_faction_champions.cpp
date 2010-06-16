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
SDName: Bosses Faction Champions
SD%Complete: 0
SDComment: 28 champions, divided equally between horde/alliance, each pair sharing AIs. The warrior's Overpower is not implemented correctly. Not sure how to do so.
SDCategory: Trial of the Crusader
EndScriptData */

/* ContentData
boss_toc_deathknight
boss_toc_caster_druid
boss_toc_heal_druid
boss_toc_hunter
boss_toc_mage
boss_toc_heal_paladin
boss_toc_ret_paladin
boss_toc_disc_priest
boss_toc_shadow_priest
boss_toc_rogue
boss_toc_magic_shaman
boss_toc_melee_shaman
boss_toc_warlock
boss_toc_warrior
EndContentData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "SpellMgr.h"

//there is not enough information about the encounter to accurately reconstruct each individual AI
//so it is scripted according to how (I think) a 'player' would do it
//It is not nessessarily blizzlike. Feel free to tune up the AI's

#define RETURN_SPELL_IF_COOLED(SP)  do { if (!SpellIsOnCooldown(SP)) return SP; } while(0)
#define TIMER_CAST                  1.5*IN_MILLISECONDS //simulate GCD
#define TIMER_SWITCH_TARGET         9*IN_MILLISECONDS
#define MAX_CASTER_RANGE            30.0f
#define NORMAL_CASTER_RANGE         20.0f

enum Events
{
    EVENT_CAST = 1,
    EVENT_SWITCH_TARGET,
    EVENT_REGAIN_MANA,
    EVENT_REGAIN_ENERGY,
};

enum Says
{
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED1 = -1300328,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED2 = -1300329,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED3 = -1300330,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED4 = -1300331,
    SAY_VARIAN_HORDE_PLAYER_KILLED1     = -1300332,
    SAY_VARIAN_HORDE_PLAYER_KILLED2     = -1300333,
    SAY_VARIAN_HORDE_PLAYER_KILLED3     = -1300334,
    SAY_VARIAN_HORDE_PLAYER_KILLED4     = -1300335,
};

static int32 const HordeSays[] =
{
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED1,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED2,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED3,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED4
};

static int32 const AllianceSays[] =
{
    SAY_VARIAN_HORDE_PLAYER_KILLED1,
    SAY_VARIAN_HORDE_PLAYER_KILLED2,
    SAY_VARIAN_HORDE_PLAYER_KILLED3,
    SAY_VARIAN_HORDE_PLAYER_KILLED4
};

enum Spells
{
    //all
    SPELL_PVP_TRINKET       = 65547, //heroic only

    //death knight
    SPELL_CHAINS_OF_ICE     = 66020,
    SPELL_DEATH_COIL        = 66019,
    SPELL_DEATH_GRIP        = 66017,
    SPELL_FROST_STRIKE      = 66047,
    SPELL_ICEBOUND_FORTITUDE= 66023,
    SPELL_ICY_TOUCH         = 66021,
    SPELL_STRANGULATE       = 66018,

    //caster druid
    SPELL_BARKSKIN          = 65860,
    SPELL_CYCLONE           = 65859,
    SPELL_ENTANGLING_ROOTS  = 65857,
    SPELL_FAERIE_FIRE       = 65863,
    SPELL_FORCE_OF_NATURE   = 65861,
    SPELL_INSECT_SWARM      = 65855,
    SPELL_MOONFIRE          = 65856,
    SPELL_STARFIRE          = 65854,
    SPELL_WRATH             = 65862,

    //heal druid
    //SPELL_BARKSKIN        = 65860,
    SPELL_LIFEBLOOM         = 66093,
    SPELL_NATURES_GRASP     = 66071,
    SPELL_NOURISH           = 66066,
    SPELL_REGROWTH          = 66067,
    SPELL_REJUVINATION      = 66065,
    SPELL_THORNS            = 66068,
    SPELL_TRANQUILITY       = 66086,

    //hunter
    SPELL_AIMED_SHOT        = 65883,
    SPELL_CALL_PET          = 67777,
    SPELL_DETERRENCE        = 65871,
    SPELL_DISENGAGE         = 65869,
    SPELL_EXPLOSIVE_SHOT    = 65866,
    SPELL_FROST_TRAP        = 65880,
    SPELL_SHOOT             = 65868,
    SPELL_STEADY_SHOT       = 65867,
    SPELL_WING_CLIP         = 66207,
    SPELL_WYVERN_STING      = 65877,

    //mage
    SPELL_ARCANE_BARRAGE    = 65799,
    SPELL_ARCANE_BLAST      = 65791,
    SPELL_ARCANE_BLAST_EFFECT = 36032,
    SPELL_ARCANE_EXPLOSION  = 65800,
    SPELL_BLINK             = 65793,
    SPELL_COUNTERSPELL      = 65790,
    SPELL_FROST_NOVA        = 65792,
    SPELL_FROSTBOLT         = 65807,
    SPELL_ICE_BLOCK         = 65802,
    SPELL_POLYMORPH         = 65801,

    //heal pally
    SPELL_CLENSE            = 66116,
    SPELL_DIVINE_SHEILD     = 66010,
    SPELL_FLASH_OF_LIGHT    = 66113,
    SPELL_HAMMER_OF_JUSTICE = 66613,
    SPELL_HAND_OF_FREEDOM   = 66115,
    SPELL_HAND_OF_PROTECTION = 66009,
    SPELL_HOLY_LIGHT        = 66112,
    SPELL_HOLY_SHOCK        = 66114,

    //retri pally
    SPELL_AVENGING_WRATH    = 66011,
    SPELL_CRUSADER_STRIKE   = 66003,
    SPELL_DIVINE_SHIELD     = 66010,
    SPELL_DIVINE_STORM      = 66006,
    SPELL_HAMMER_OF_JUSTICE2= 66997,
    //SPELL_HAND_OF_PROTECTION = 66009,
    SPELL_JUDGEMENT_OF_COMMAND = 66005,
    SPELL_REPENTANCE        = 66008,
    SPELL_SEAL_OF_COMMAND   = 66004,

    //disipline priest
    SPELL_DISPEL_MAGIC      = 65546,
    SPELL_FLASH_HEAL        = 66104,
    SPELL_MANA_BURN         = 66100,
    SPELL_PENANCE           = 66098,
    SPELL_POWER_WORD_SHIELD = 66099,
    SPELL_PSYCHIC_SCREAM    = 65543,
    SPELL_RENEW             = 66177,

    //shadow priest
    //SPELL_DISPEL_MAGIC    = 65546,
    SPELL_DISPERSION        = 65544,
    SPELL_MIND_BLAST        = 65492,
    SPELL_MIND_FLAY         = 65488,
    SPELL_PSYCHIC_HORROR    = 65545,
    //SPELL_PSYCHIC_SCREAM  = 65543,
    SPELL_SHADOW_WORD_PAIN  = 65541,
    SPELL_SILENCE           = 65542,
    SPELL_VAMPIRIC_TOUCH    = 65490,

    //rogue
    SPELL_BLADE_FLURRY      = 65956,
    SPELL_BLIND             = 65960,
    SPELL_CLOAK_OF_SHADOWS  = 65961,
    SPELL_EVISCERATE        = 65957,
    SPELL_FAN_OF_KNIVES     = 65955,
    SPELL_HEMORRHAGE        = 65954,
    SPELL_SHADOWSTEP        = 66178,

    //melee shaman
    SPELL_EARTH_SHOCK       = 65973,
    SPELL_HEROISM           = 65983,
    SPELL_BLOODLUST         = 65980,
    SPELL_LAVA_LASH         = 65974,
    SPELL_STORMSTRIKE       = 65970,

    //magic shaman
    SPELL_CLEANSE_SPIRIT    = 66056,
    SPELL_EARTH_SHIELD      = 66063,
    //SPELL_EARTH_SHOCK     = 65973,
    //SPELL_HEROISM         = 65983,
    //SPELL_BLOODLUST       = 65980,
    SPELL_HEX               = 66054,
    SPELL_LESSER_HEALING_WAVE = 66055,
    SPELL_RIPTIDE           = 66053,

    //warlock
    SPELL_CORRUPTION        = 65810,
    SPELL_CURSE_OF_AGONY    = 65814,
    SPELL_CURSE_OF_EXHAUSTION = 65815,
    SPELL_DEATH_COIL_WARLOCK= 68139,
    SPELL_FEAR              = 65809,
    SPELL_HELLFIRE          = 65816,
    SPELL_SEARING_PAIN      = 65819,
    SPELL_SHADOW_BOLT       = 65821,
    SPELL_SUMMON_FELHUNTER  = 67514,
    SPELL_UNSTABLE_AFFLICTION = 65812,

    //warrior
    SPELL_BLADESTORM        = 65947,
    SPELL_CHARGE            = 65927,
    SPELL_DISARM            = 65935,
    SPELL_INTIMIDATING_SHOUT = 65930,
    SPELL_MORTAL_STRIKE     = 65946,
    SPELL_OVERPOWER         = 65924,
    SPELL_RETALIATION       = 65932,
    SPELL_SHATTERING_THROW  = 65940,
    SPELL_SUNDER_ARMOR      = 65936,
};

// common parts for all faction champions - a generalized AI
struct MANGOS_DLL_DECL boss_faction_championAI: public boss_trial_of_the_crusaderAI
{
    bool m_bIsHorde;
    InstanceVar<uint64> const m_FriendlyLeader;
    InstanceVar<uint64> const m_EnemyLeader;
    int32 const *m_FriendlySays;
    int32 const *m_EnemySays;

    Unit *NextPossibleHealTarget;
    Unit *CurrHostileTarget;
    CanCastResult LastSpellResult;

    bool IsDPS :1;
    bool IsHealer :1;
    bool IsMelee :1;

    boss_faction_championAI(Creature *pCreature):
        boss_trial_of_the_crusaderAI(pCreature),
        m_bIsHorde(IS_HORDE),
        m_FriendlyLeader(m_bIsHorde ? TYPE_GARROSH_HELLSCREAM : TYPE_VARIAN_WYRM, m_pInstance),
        m_EnemyLeader(m_bIsHorde ? TYPE_VARIAN_WYRM : TYPE_GARROSH_HELLSCREAM, m_pInstance),
        m_FriendlySays(m_bIsHorde ? HordeSays : AllianceSays),
        m_EnemySays(m_bIsHorde ? AllianceSays : HordeSays),
        NextPossibleHealTarget(NULL),
        CurrHostileTarget(NULL),
        LastSpellResult(CAST_OK),
        IsDPS(false), IsHealer(false), IsMelee(false)
    {
    }

    virtual uint32 ChooseDamageSpell() = 0;
    virtual uint32 ChooseCCSpell() = 0;
    virtual uint32 ChooseHealSpell() = 0;
    virtual uint32 ChooseBuff() = 0;

    void Aggro(Unit *who)
    {
        RESCHEDULE_EVENT(CAST);
        Events.RescheduleEvent(EVENT_SWITCH_TARGET, 0); //force acquire target
        if (m_creature->getPowerType() == POWER_MANA)
            Events.RescheduleEvent(EVENT_REGAIN_MANA, 1000);
        if (m_creature->getPowerType() == POWER_ENERGY)
            Events.RescheduleEvent(EVENT_REGAIN_ENERGY, 1000);
        m_BossEncounter = IN_PROGRESS;
    }

    // boss kills player
    void KilledUnit(Unit *who)
    {
        boss_trial_of_the_crusaderAI::KilledUnit(who);
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        if (Creature *source = m_EnemyLeader.getObject<Creature>())
            DoScriptText(m_EnemySays[urand(0, 3)], source);
    }

    // player kills boss
    void JustDied(Unit *killer)
    {
        if (Creature *source = m_FriendlyLeader.getObject<Creature>())
            DoScriptText(m_FriendlySays[urand(0, 3)], source);
    }

    Unit* ChooseNewHealTarget()
    {
        //I think they should try to heal the lowest hp npc.
        return DoSelectLowestHpFriendly(m_bIsHeroic ? 60 : 40, 10000);
    }

    Unit* ChooseNewAttackTarget() const
    {
        //"based on proximity,health and damage"
        ThreatList const& tlist = m_creature->getThreatManager().getThreatList();
        if (tlist.empty())
            return NULL;

        std::vector<Unit*> PossibleTargets;
        PossibleTargets.reserve(tlist.size());
        // FIXME: this looks ugly, improve selection algorithm
        for (ThreatList::const_iterator i = tlist.begin(); i != tlist.end(); ++i)
        {
            if (Unit *target = m_creature->GetUnit(*m_creature, (*i)->getUnitGuid()))
            {
                //make it more likely too attack if very little hp
                if (target->GetHealthPercent() < 50.0f)
                    PossibleTargets.push_back(target);
                if (target->GetHealthPercent() < 35.0f)
                    PossibleTargets.push_back(target);
                if (target->GetHealthPercent() < 10.0f)
                    PossibleTargets.push_back(target);
                //more likely if close
                if (target->IsWithinDistInMap(m_creature, 30.0f))
                    PossibleTargets.push_back(target);
                if (target->IsWithinDistInMap(m_creature, 10.0f))
                    PossibleTargets.push_back(target);
                if (target->IsWithinDistInMap(m_creature, 5.0f))
                    PossibleTargets.push_back(target);
            }
        }

        if (PossibleTargets.empty())
            return m_creature->GetMap()->GetPlayers().begin()->getSource();

        return PossibleTargets[urand(0, PossibleTargets.size()-1)];
    }

    bool SpellIsOnCooldown(uint32 uiSpell) const
    {
        return m_creature->HasSpellCooldown(uiSpell);
    }

    void DoCastWithCooldown(Unit *pVictim, uint32 uiSpellId, bool bTriggered = false)
    {
        LastSpellResult = CanCastSpell(pVictim, GetSpellStore()->LookupEntry(uiSpellId), bTriggered);
        if (LastSpellResult == CAST_OK)
        {
            DoCast(pVictim, uiSpellId, bTriggered);
            m_creature->AddCreatureSpellCooldown(uiSpellId);
        }
    }

    bool IsSpellInRange(uint32 SpellId) const
    {
        SpellEntry const *pSpell = GetSpellStore()->LookupEntry(SpellId);
        if (!pSpell)
            return false;
        SpellRangeEntry const *pRange = GetSpellRangeStore()->LookupEntry(pSpell->rangeIndex);
        return pRange ? m_creature->IsWithinDistInMap(CurrHostileTarget, pRange->maxRange) : false;
    }

    uint32 GetNumberOfPlayersInRange(float Range) const
    {
        uint32 PlayersInRange = 0;
        Map::PlayerList const &Plyrs = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = Plyrs.begin(); i!= Plyrs.end(); ++i)
            if (i->getSource()->IsWithinDistInMap(m_creature, Range))
                PlayersInRange++;
        return PlayersInRange;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsHeroic && (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED) || 
            m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) || 
            m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING) ||
            m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)) &&
            !SpellIsOnCooldown(SPELL_PVP_TRINKET) && !m_creature->IsNonMeleeSpellCasted(false))
            DoCastWithCooldown(m_creature, SPELL_PVP_TRINKET, true);

        if (!IsMelee && CurrHostileTarget)
        {
            if (LastSpellResult == CAST_FAIL_POWER)
                DoStartMovement(CurrHostileTarget);
            else
            {
                float dist = m_creature->GetDistance(CurrHostileTarget);
                if (dist > MAX_CASTER_RANGE)
                    DoStartMovement(CurrHostileTarget);
                else if (dist <= NORMAL_CASTER_RANGE)
                    DoStartNoMovement(CurrHostileTarget);
            }
        }

        Events.Update(uiDiff);
        while (uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                case EVENT_CAST:
                {
                    if (!m_creature->IsNonMeleeSpellCasted(false))
                    {
                        uint32 CCSpellToCast = 0, SpellToCast = 0, HealSpellToCast = 0, DamageSpellToCast = 0, BuffToCast = 0;
                        BuffToCast = ChooseBuff();
                        if (BuffToCast)
                            DoCastWithCooldown(m_creature, BuffToCast);
                        else
                        {
                            if (IsDPS && CurrHostileTarget)
                                DamageSpellToCast = ChooseDamageSpell();
                            if (IsHealer)
                            {
                                NextPossibleHealTarget = ChooseNewHealTarget();
                                if (NextPossibleHealTarget)
                                    HealSpellToCast = ChooseHealSpell();
                            }
                            if (CCSpellToCast = ChooseCCSpell())
                            {
                                if (GetNumberOfPlayersInRange(14.0f) < 5)
                                    CCSpellToCast = 0;
                            }
                            if (CCSpellToCast && urand(4, 8) == 4)
                                SpellToCast = CCSpellToCast;
                            if (DamageSpellToCast && HealSpellToCast && !SpellToCast)
                            {
                                if (IsHealer && urand(1, 5) != 4)
                                    SpellToCast = HealSpellToCast;
                                else
                                    SpellToCast = DamageSpellToCast;
                            }

                            if (!SpellToCast && IsDPS)
                                SpellToCast = DamageSpellToCast;
                            if (!SpellToCast && IsHealer)
                                SpellToCast = HealSpellToCast;

                            if (SpellToCast)
                            {
                                if (SpellToCast != HealSpellToCast)
                                    DoCastWithCooldown(CurrHostileTarget, SpellToCast);
                                else
                                    DoCastWithCooldown(NextPossibleHealTarget, SpellToCast);
                            }
                        }
                    }
                    RESCHEDULE_EVENT(CAST);
                    break;
                }
                case EVENT_SWITCH_TARGET:
                    if (!CurrHostileTarget || CurrHostileTarget->GetHealthPercent() > 20.0f)
                    {
                        if (m_creature->IsNonMeleeSpellCasted(false))
                        {
                            Events.RescheduleEvent(EVENT_SWITCH_TARGET, 1000);
                            break;
                        }
                        else
                        {
                            CurrHostileTarget = ChooseNewAttackTarget();
                            DoResetThreat();
                            if (IsMelee)
                                m_creature->Attack(CurrHostileTarget, IsMelee);
                        }
                    }
                    RESCHEDULE_EVENT(SWITCH_TARGET);
                    break;
                case EVENT_REGAIN_MANA:
                {
                    uint32 mana = m_creature->GetMaxPower(POWER_MANA)*0.05;
                    m_creature->SetPower(POWER_MANA, m_creature->GetPower(POWER_MANA) + mana);
                    Events.RescheduleEvent(EVENT_REGAIN_MANA, urand(1,2)*IN_MILLISECONDS);
                    break;
                }
                case EVENT_REGAIN_ENERGY:
                {
                    m_creature->SetPower(POWER_ENERGY, m_creature->GetPower(POWER_ENERGY) + 10);
                    Events.RescheduleEvent(EVENT_REGAIN_ENERGY, 1000);
                    break;
                }
                default:
                    break;
            }

        if (IsMelee || LastSpellResult == CAST_FAIL_POWER)
            DoMeleeAttackIfReady();
    }

    uint32 GetAuraCountByDispelType(bool PositiveSpells, Unit *pTarget, DispelType dispelType) const
    {
        // Create dispel mask by dispel type
        uint32 dispelMask = GetDispellMask(dispelType);
        uint32 count = 0;

        Unit::AuraMap const& auras = pTarget->GetAuras();
        for (Unit::AuraMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            SpellEntry const* spell = itr->second->GetSpellProto();
            if ((1 << spell->Dispel) & dispelMask && itr->second->IsPositive() == PositiveSpells)
                ++count;
        }

        return count;
    }
};

struct MANGOS_DLL_DECL boss_toc_deathknightAI: public boss_faction_championAI
{
    boss_toc_deathknightAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = false;
    }

    // only should be called on wipe, creature WILL BE DESTROYED upon wipe, definition in boss_trial_of_the_crusaderAI
    // if defining a reset function, remember to call boss_trial_of_the_crusaderAI::Reset() after all of it
    //void Reset()
    //{
    //    boss_trial_of_the_crusaderAI::Reset();
    //}

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        float Dist = CurrHostileTarget->GetDistance(m_creature);
        if (Dist > 25.0f && Dist < 30.0f)
        {
            if (CurrHostileTarget->IsNonMeleeSpellCasted(false) && !SpellIsOnCooldown(SPELL_STRANGULATE))
                return SPELL_STRANGULATE;
            RETURN_SPELL_IF_COOLED(SPELL_DEATH_GRIP);
            RETURN_SPELL_IF_COOLED(SPELL_DEATH_COIL);
        }
        if (Dist > 5.0f && Dist < 25.0f)
        {
            if (CurrHostileTarget->IsNonMeleeSpellCasted(false) && !SpellIsOnCooldown(SPELL_STRANGULATE))
                return SPELL_STRANGULATE;
            RETURN_SPELL_IF_COOLED(SPELL_CHAINS_OF_ICE);
            RETURN_SPELL_IF_COOLED(SPELL_ICY_TOUCH);
            RETURN_SPELL_IF_COOLED(SPELL_DEATH_GRIP);
        }
        if (Dist < 5.0f)
            RETURN_SPELL_IF_COOLED(SPELL_FROST_STRIKE);
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (CurrHostileTarget && CurrHostileTarget->IsWithinDistInMap(m_creature, 25.0f))
            RETURN_SPELL_IF_COOLED(SPELL_CHAINS_OF_ICE);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        RETURN_SPELL_IF_COOLED(SPELL_ICEBOUND_FORTITUDE);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_caster_druidAI: public boss_faction_championAI
{
    boss_toc_caster_druidAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (IsSpellInRange(SPELL_FAERIE_FIRE) && !CurrHostileTarget->HasAuraByDifficulty(SPELL_FAERIE_FIRE))
            RETURN_SPELL_IF_COOLED(SPELL_FAERIE_FIRE);
        if (IsSpellInRange(SPELL_INSECT_SWARM) && !CurrHostileTarget->HasAuraByDifficulty(SPELL_INSECT_SWARM))
            RETURN_SPELL_IF_COOLED(SPELL_INSECT_SWARM);
        if (IsSpellInRange(SPELL_MOONFIRE) && !CurrHostileTarget->HasAuraByDifficulty(SPELL_MOONFIRE))
            RETURN_SPELL_IF_COOLED(SPELL_MOONFIRE);
        if (IsSpellInRange(SPELL_STARFIRE))
            if (urand(0,10) < 8)
                return SPELL_STARFIRE;
            else
                return SPELL_FORCE_OF_NATURE;

        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, 30.0f))
            RETURN_SPELL_IF_COOLED(SPELL_ENTANGLING_ROOTS);
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, 20.0f))
            RETURN_SPELL_IF_COOLED(SPELL_CYCLONE);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        RETURN_SPELL_IF_COOLED(SPELL_BARKSKIN);
        RETURN_SPELL_IF_COOLED(SPELL_FORCE_OF_NATURE);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_heal_druidAI: public boss_faction_championAI
{
    boss_toc_heal_druidAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = false;
        IsHealer = true;
    }

    uint32 ChooseDamageSpell()
    {
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        if (!NextPossibleHealTarget)
            return 0;
        uint32 HealthPercent = NextPossibleHealTarget->GetHealthPercent();
        if (HealthPercent > 98.0f)
            return 0;
        if (HealthPercent > 85.0f)
            if (urand(1, 5) < 3)
                return SPELL_LIFEBLOOM;
        if (HealthPercent > 75.0f)
            return SPELL_REGROWTH;
        if (HealthPercent > 50.0f)
            return SPELL_NOURISH;
        RETURN_SPELL_IF_COOLED(SPELL_TRANQUILITY);
        return SPELL_REJUVINATION;
    }

    uint32 ChooseBuff()
    {
        if (!m_creature->HasAuraByDifficulty(SPELL_BARKSKIN))
            RETURN_SPELL_IF_COOLED(SPELL_BARKSKIN);
        if (!m_creature->HasAuraByDifficulty(SPELL_NATURES_GRASP))
            RETURN_SPELL_IF_COOLED(SPELL_NATURES_GRASP);
        if (!m_creature->HasAuraByDifficulty(SPELL_THORNS))
            RETURN_SPELL_IF_COOLED(SPELL_THORNS);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_hunterAI: public boss_faction_championAI
{
    boss_toc_hunterAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, 5.0f))
        {
            if (!SpellIsOnCooldown(SPELL_DISENGAGE))
                return SPELL_DISENGAGE;
            else if (CurrHostileTarget->IsNonMeleeSpellCasted(false))
                RETURN_SPELL_IF_COOLED(SPELL_DETERRENCE);
        }
        else
        {
            if (IsSpellInRange(SPELL_AIMED_SHOT))
            {
                RETURN_SPELL_IF_COOLED(SPELL_AIMED_SHOT);
                RETURN_SPELL_IF_COOLED(SPELL_EXPLOSIVE_SHOT);
                RETURN_SPELL_IF_COOLED(SPELL_STEADY_SHOT);
            }
        }
        if (IsSpellInRange(SPELL_SHOOT))
            return SPELL_SHOOT;
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (m_creature->IsWithinDistInMap(CurrHostileTarget, ATTACK_DISTANCE))
            RETURN_SPELL_IF_COOLED(SPELL_WING_CLIP);
        if (IsSpellInRange(SPELL_WYVERN_STING) && m_creature->GetDistance(CurrHostileTarget) > ATTACK_DISTANCE)
            RETURN_SPELL_IF_COOLED(SPELL_WYVERN_STING);
        RETURN_SPELL_IF_COOLED(SPELL_FROST_TRAP);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (!m_creature->GetPet())
            return SPELL_CALL_PET;
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_mageAI: public boss_faction_championAI
{
    boss_toc_mageAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, 5.0f) || m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED))
            RETURN_SPELL_IF_COOLED(SPELL_BLINK);
        uint32 ArcaneStack = 0;
        if (Aura *aur = CurrHostileTarget->GetAura(SPELL_ARCANE_BLAST_EFFECT, EFFECT_INDEX_0))
            ArcaneStack = aur->GetStackAmount();
        if (IsSpellInRange(SPELL_ARCANE_BLAST) && ArcaneStack < 4)
            return SPELL_ARCANE_BLAST;
        if (IsSpellInRange(SPELL_COUNTERSPELL) && CurrHostileTarget->IsNonMeleeSpellCasted(false))
            RETURN_SPELL_IF_COOLED(SPELL_COUNTERSPELL);
        if (IsSpellInRange(SPELL_ARCANE_BARRAGE) && ArcaneStack == 4)
            RETURN_SPELL_IF_COOLED(SPELL_ARCANE_BARRAGE);
        return SPELL_FROSTBOLT;
    }

    uint32 ChooseCCSpell()
    {
        RETURN_SPELL_IF_COOLED(SPELL_FROST_NOVA);
        RETURN_SPELL_IF_COOLED(SPELL_POLYMORPH);
        RETURN_SPELL_IF_COOLED(SPELL_ICE_BLOCK);
        return SPELL_ARCANE_EXPLOSION;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_heal_paladinAI: public boss_faction_championAI
{
    boss_toc_heal_paladinAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;   //very small though
        IsHealer = true;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (urand(0, 6) == 4)
        {
            if (CurrHostileTarget->IsWithinDistInMap(m_creature, 10.0f))
                RETURN_SPELL_IF_COOLED(SPELL_HAMMER_OF_JUSTICE);
            if (IsSpellInRange(SPELL_HOLY_SHOCK))
                return SPELL_HOLY_SHOCK;
        }
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        if (!NextPossibleHealTarget)
            return 0;
        Creature *heal_target = dynamic_cast<Creature*>(NextPossibleHealTarget);
        if ((NextPossibleHealTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED) || 
            NextPossibleHealTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) || 
            NextPossibleHealTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FLEEING) ||
            NextPossibleHealTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)) &&
            (m_bIsHeroic && heal_target ? heal_target->HasSpellCooldown(SPELL_PVP_TRINKET) : true))
            RETURN_SPELL_IF_COOLED(SPELL_HAND_OF_FREEDOM);
        if ((GetAuraCountByDispelType(false, NextPossibleHealTarget, DISPEL_MAGIC) + 
            GetAuraCountByDispelType(false, NextPossibleHealTarget, DISPEL_DISEASE) + 
            GetAuraCountByDispelType(false, NextPossibleHealTarget, DISPEL_POISON)) > 0)
            RETURN_SPELL_IF_COOLED(SPELL_CLENSE);
        if (NextPossibleHealTarget->GetHealthPercent() < 40.0f)
            RETURN_SPELL_IF_COOLED(SPELL_HAND_OF_PROTECTION);
        if (NextPossibleHealTarget->GetHealthPercent() < 70.0f)
            return SPELL_HOLY_LIGHT;
        if (NextPossibleHealTarget->GetHealthPercent() < 90.0f)
            return SPELL_FLASH_OF_LIGHT;
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (m_creature->GetHealthPercent() < 30.0f)
            RETURN_SPELL_IF_COOLED(SPELL_DIVINE_SHEILD);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_ret_paladinAI: public boss_faction_championAI
{
    boss_toc_ret_paladinAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = true;    //only hand of protection
    }

    uint32 ChooseDamageSpell()
    {
        if (GetNumberOfPlayersInRange(8.0f) > 3)
            RETURN_SPELL_IF_COOLED(SPELL_DIVINE_STORM);
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->GetDistance(m_creature) > ATTACK_DISTANCE)
            RETURN_SPELL_IF_COOLED(SPELL_JUDGEMENT_OF_COMMAND);
        if (IsSpellInRange(SPELL_CRUSADER_STRIKE))
            RETURN_SPELL_IF_COOLED(SPELL_CRUSADER_STRIKE);
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (IsSpellInRange(SPELL_HAMMER_OF_JUSTICE2))
            RETURN_SPELL_IF_COOLED(SPELL_HAMMER_OF_JUSTICE2);
        if (IsSpellInRange(SPELL_REPENTANCE))
            RETURN_SPELL_IF_COOLED(SPELL_REPENTANCE);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        if (!NextPossibleHealTarget)
            return 0;
        if (NextPossibleHealTarget->GetHealthPercent() < 20.0f)
            RETURN_SPELL_IF_COOLED(SPELL_HAND_OF_PROTECTION);
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (!m_creature->HasAuraByDifficulty(SPELL_SEAL_OF_COMMAND))
            return SPELL_SEAL_OF_COMMAND;
        if (m_creature->GetHealthPercent() < 30.0f)
            RETURN_SPELL_IF_COOLED(SPELL_DIVINE_SHIELD);
        if (m_creature->GetHealthPercent() < 15.0f)
            RETURN_SPELL_IF_COOLED(SPELL_HAND_OF_PROTECTION);
        RETURN_SPELL_IF_COOLED(SPELL_AVENGING_WRATH);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_disc_priestAI: public boss_faction_championAI
{
    boss_toc_disc_priestAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;   // only SPELL_DISPEL_MAGIC, SPELL_MANA_BURN
        IsHealer = true;
    }

    uint32 ChooseDamageSpell()
    {
        if (IsSpellInRange(SPELL_MANA_BURN) && urand(3, 8) == 4)
            RETURN_SPELL_IF_COOLED(SPELL_MANA_BURN);
        if (!CurrHostileTarget)
            return 0;
        if (IsSpellInRange(SPELL_DISPEL_MAGIC) && GetAuraCountByDispelType(true, CurrHostileTarget, DISPEL_MAGIC) >= 2)
            return SPELL_DISPEL_MAGIC;
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        RETURN_SPELL_IF_COOLED(SPELL_PSYCHIC_SCREAM);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        if (!NextPossibleHealTarget)
            return 0;
        if (GetAuraCountByDispelType(false, NextPossibleHealTarget, DISPEL_MAGIC) >= 2 && urand(3, 6) == 4)
            return SPELL_DISPEL_MAGIC;
        float percent = NextPossibleHealTarget->GetHealthPercent();
        if (percent > 80.0f)
            return SPELL_PENANCE;
        if (percent > 70.0f)
            return SPELL_FLASH_HEAL;
        if (percent < 60.0f)
        {
            if (SpellIsOnCooldown(SPELL_POWER_WORD_SHIELD))
                return SPELL_RENEW;
            else
                return SPELL_POWER_WORD_SHIELD;
        }
        return 0;
    }

    uint32 ChooseBuff()
    {
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_shadow_priestAI: public boss_faction_championAI
{
    boss_toc_shadow_priestAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (IsSpellInRange(SPELL_DISPEL_MAGIC) && m_creature->GetDistance(CurrHostileTarget) > 30.0f &&
            GetAuraCountByDispelType(true, CurrHostileTarget, DISPEL_MAGIC) >= 2)
            return SPELL_DISPEL_MAGIC;
        if (IsSpellInRange(SPELL_SILENCE) && CurrHostileTarget->IsNonMeleeSpellCasted(false) && urand(3, 6) == 4)
            RETURN_SPELL_IF_COOLED(SPELL_SILENCE);
        if (IsSpellInRange(SPELL_SHADOW_WORD_PAIN) && !CurrHostileTarget->HasAuraByDifficulty(SPELL_SHADOW_WORD_PAIN))
            return SPELL_SHADOW_WORD_PAIN;
        if (IsSpellInRange(SPELL_VAMPIRIC_TOUCH) && !CurrHostileTarget->HasAuraByDifficulty(SPELL_VAMPIRIC_TOUCH))
            return SPELL_VAMPIRIC_TOUCH;
        if (IsSpellInRange(SPELL_MIND_FLAY))
            return SPELL_MIND_FLAY;
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->IsNonMeleeSpellCasted(false))
            RETURN_SPELL_IF_COOLED(SPELL_SILENCE);
        RETURN_SPELL_IF_COOLED(SPELL_PSYCHIC_HORROR);
        RETURN_SPELL_IF_COOLED(SPELL_PSYCHIC_SCREAM);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (m_creature->GetHealthPercent() < 20.0f || m_creature->GetPower(POWER_MANA)*100 / m_creature->GetMaxPower(POWER_MANA) < 20)
            RETURN_SPELL_IF_COOLED(SPELL_DISPERSION);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_rogueAI: public boss_faction_championAI
{
    boss_toc_rogueAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = false;
        m_creature->setPowerType(POWER_ENERGY);
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (GetNumberOfPlayersInRange(10.0f) > 5)
            RETURN_SPELL_IF_COOLED(SPELL_FAN_OF_KNIVES);
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, 10.0f))
            RETURN_SPELL_IF_COOLED(SPELL_SHADOWSTEP);
        if(urand(4, 5) == 4)
            RETURN_SPELL_IF_COOLED(SPELL_EVISCERATE);
        else
            return SPELL_HEMORRHAGE;
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        RETURN_SPELL_IF_COOLED(SPELL_BLIND);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (!CurrHostileTarget)
            return 0;
        if (m_creature->IsWithinDistInMap(CurrHostileTarget, 5.0f))
            RETURN_SPELL_IF_COOLED(SPELL_BLADE_FLURRY);
        if (GetAuraCountByDispelType(false, m_creature, DISPEL_ALL) > 6)
            RETURN_SPELL_IF_COOLED(SPELL_CLOAK_OF_SHADOWS);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_magic_shamanAI: public boss_faction_championAI
{
    boss_toc_magic_shamanAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;
        IsHealer = true;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (IsSpellInRange(SPELL_EARTH_SHOCK) && CurrHostileTarget->IsNonMeleeSpellCasted(false))
            RETURN_SPELL_IF_COOLED(SPELL_EARTH_SHOCK);
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (m_creature->IsWithinDistInMap(CurrHostileTarget, 20.0f))
            RETURN_SPELL_IF_COOLED(SPELL_HEX);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        if (!NextPossibleHealTarget)
            return 0;
        //heal target guaranteed to be in range.. i think (40 yards normal, 60 yrds heroic)
        if ((GetAuraCountByDispelType(false, NextPossibleHealTarget,DISPEL_CURSE) + 
            GetAuraCountByDispelType(false, NextPossibleHealTarget,DISPEL_DISEASE) + 
            GetAuraCountByDispelType(false, NextPossibleHealTarget,DISPEL_POISON)) > 0)
            RETURN_SPELL_IF_COOLED(SPELL_CLEANSE_SPIRIT);
        if (NextPossibleHealTarget->GetHealthPercent() < 70.0f)
            RETURN_SPELL_IF_COOLED(SPELL_RIPTIDE);
        return SPELL_LESSER_HEALING_WAVE;
    }

    uint32 ChooseBuff()
    {
        if (!m_creature->HasAuraByDifficulty(SPELL_EARTH_SHIELD))
            return SPELL_EARTH_SHIELD;
        uint32 spell = m_bIsHorde ? SPELL_HEROISM : SPELL_BLOODLUST;
        if (!m_creature->HasAuraByDifficulty(spell))
            RETURN_SPELL_IF_COOLED(spell);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_melee_shamanAI: public boss_faction_championAI
{
    boss_toc_melee_shamanAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (IsSpellInRange(SPELL_EARTH_SHOCK) && CurrHostileTarget->IsNonMeleeSpellCasted(false))
            RETURN_SPELL_IF_COOLED(SPELL_EARTH_SHOCK);
        if (IsSpellInRange(SPELL_STORMSTRIKE))
        {
            RETURN_SPELL_IF_COOLED(SPELL_STORMSTRIKE);
            RETURN_SPELL_IF_COOLED(SPELL_LAVA_LASH);
        }
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (!m_creature->HasAuraByDifficulty(SPELL_EARTH_SHIELD))
            return SPELL_EARTH_SHIELD;
        uint32 spell = m_bIsHorde ? SPELL_HEROISM : SPELL_BLOODLUST;
        if (!m_creature->HasAuraByDifficulty(spell))
            RETURN_SPELL_IF_COOLED(spell);
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_warlockAI: public boss_faction_championAI
{
    boss_toc_warlockAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = false;
        IsDPS = true;
        IsHealer = false;
        DoCast(m_creature, SPELL_SUMMON_FELHUNTER, true);   //need a triggered cast (soul shard needed)
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (!IsSpellInRange(SPELL_CORRUPTION))
            return 0;

        if (!CurrHostileTarget->HasAuraByDifficulty(SPELL_CURSE_OF_EXHAUSTION))
            return SPELL_CURSE_OF_EXHAUSTION;

        if (!CurrHostileTarget->HasAuraByDifficulty(SPELL_CORRUPTION))
            return SPELL_CORRUPTION;

        if (!CurrHostileTarget->HasAuraByDifficulty(SPELL_UNSTABLE_AFFLICTION))
            RETURN_SPELL_IF_COOLED(SPELL_UNSTABLE_AFFLICTION);

        if (!CurrHostileTarget->HasAuraByDifficulty(SPELL_CURSE_OF_AGONY))
            return SPELL_CURSE_OF_AGONY;

        if (GetNumberOfPlayersInRange(10.0f) > 5 && m_creature->GetHealthPercent() > 40.0f)
            return SPELL_HELLFIRE;

        return urand(4,5)==4 ? SPELL_SHADOW_BOLT : SPELL_SEARING_PAIN;
    }

    uint32 ChooseCCSpell()
    {
        if (m_creature->GetHealthPercent() < 70.0f)
            RETURN_SPELL_IF_COOLED(SPELL_DEATH_COIL_WARLOCK);
        RETURN_SPELL_IF_COOLED(SPELL_FEAR);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        return 0;
    }
};

struct MANGOS_DLL_DECL boss_toc_warriorAI: public boss_faction_championAI
{
    boss_toc_warriorAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        IsMelee = true;
        IsDPS = true;
        IsHealer = false;
    }

    uint32 ChooseDamageSpell()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->GetDistance(m_creature) > 20 && IsSpellInRange(SPELL_SHATTERING_THROW))
            RETURN_SPELL_IF_COOLED(SPELL_SHATTERING_THROW);
        if (CurrHostileTarget->GetDistance(m_creature) > 10 && IsSpellInRange(SPELL_CHARGE))
            RETURN_SPELL_IF_COOLED(SPELL_CHARGE);
        if (GetNumberOfPlayersInRange(8.0f) > 5)
            RETURN_SPELL_IF_COOLED(SPELL_BLADESTORM);
        if (IsSpellInRange(SPELL_DISARM))
        {
            Aura *SunderAura = CurrHostileTarget->GetAura(SPELL_SUNDER_ARMOR, EFFECT_INDEX_0);
            if (SunderAura && SunderAura->GetStackAmount() < 5 && urand (4,6) == 4)
                return SPELL_SUNDER_ARMOR;
            RETURN_SPELL_IF_COOLED(SPELL_DISARM);
            RETURN_SPELL_IF_COOLED(SPELL_MORTAL_STRIKE);
        }
        return 0;
    }

    uint32 ChooseCCSpell()
    {
        RETURN_SPELL_IF_COOLED(SPELL_INTIMIDATING_SHOUT);
        return 0;
    }

    uint32 ChooseHealSpell()
    {
        return 0;
    }

    uint32 ChooseBuff()
    {
        if (!CurrHostileTarget)
            return 0;
        if (CurrHostileTarget->IsWithinDistInMap(m_creature, ATTACK_DISTANCE))
        {
            RETURN_SPELL_IF_COOLED(SPELL_RETALIATION);
            RETURN_SPELL_IF_COOLED(SPELL_OVERPOWER);    //should only be useable after a dodge. Not sure how to implement.
        }

        return 0;
    }
};

void AddSC_boss_faction_champions()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_deathknight);
    REGISTER_SCRIPT(boss_toc_caster_druid);
    REGISTER_SCRIPT(boss_toc_heal_druid);
    REGISTER_SCRIPT(boss_toc_hunter);
    REGISTER_SCRIPT(boss_toc_mage);
    REGISTER_SCRIPT(boss_toc_heal_paladin);
    REGISTER_SCRIPT(boss_toc_ret_paladin);
    REGISTER_SCRIPT(boss_toc_disc_priest);
    REGISTER_SCRIPT(boss_toc_shadow_priest);
    REGISTER_SCRIPT(boss_toc_rogue);
    REGISTER_SCRIPT(boss_toc_magic_shaman);
    REGISTER_SCRIPT(boss_toc_melee_shaman);
    REGISTER_SCRIPT(boss_toc_warlock);
    REGISTER_SCRIPT(boss_toc_warrior);
}
