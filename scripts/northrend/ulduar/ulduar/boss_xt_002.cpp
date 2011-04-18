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
SDName: Boss XT-002 Deconstructor
SD%Complete: 10%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

/* TODO: merge this
enum
{
    SAY_AGGRO                           = -1603045,
    SAY_SLAY_1                          = -1603046,
    SAY_SLAY_2                          = -1603047,
    SAY_BERSERK                         = -1603048,
    SAY_ADDS                            = -1603049,
    SAY_DEATH                           = -1603050,
    SAY_HEART_OPEN                      = -1603051,
    SAY_HEART_CLOSE                     = -1603052,
    SAY_TANCTRUM                        = -1603053,

    EMOTE_HEART                         = -1603054,
    EMOTE_REPAIR                        = -1603055,
};
*/

enum Spells
{
    SPELL_BERSERK               = 47008,
    SPELL_TYMPANIC_TANTRUM      = 62776,
    SPELL_SEARING_LIGHT         = 63018,
    SPELL_SEARING_LIGHT_H       = 65121,
    SPELL_GRAVITY_BOMB          = 63024,
    SPELL_GRAVITY_BOMB_H        = 64234,
    //SPELL_GRAVITY_BOMB_EFFECT   = 63025,
    //SPELL_GRAVITY_BOMB_EFFECT_H = 64233,
};

enum Adds
{
    NPC_HEARTH_OF_DECONST       = 33329
};

enum Says
{
    SAY_AGGRO           = -1300039,
    SAY_TANTRUM         = -1300040,
    SAY_KILLED_PLAYER1  = -1300041,
    SAY_KILLED_PLAYER2  = -1300042,
    SAY_REST            = -1300043,
    SAY_GETUP           = -1300044,
    SAY_SUMMON_ADDS     = -1300045,
    SAY_BERSERK         = -1300046,
    SAY_DEATH           = -1300047
};

#define BERSERK_TIMER           10*MINUTE*IN_MILLISECONDS
#define TIMPANIC_TANTRUM_TIMER  60*IN_MILLISECONDS
#define SEARING_LIGHT_TIMER     urand(25, 30)*IN_MILLISECONDS
#define GRAVITY_BOMB_TIMER      urand(35, 40)*IN_MILLISECONDS
#define HEARTH_PHASE_TIMER      30*IN_MILLISECONDS

struct MANGOS_DLL_DECL boss_deconstructorAI : public ScriptedAI
{
    boss_deconstructorAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_XT002)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 const m_uiBossEncounterId;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Berserk_Timer;
    uint32 TympanicTantrum_Timer;
	uint32 SearingLight_Timer;
	uint32 GravityBomb_Timer;
    uint32 HP_Counter;
    bool HearthPhase;
    uint32 HearthPhase_Timer;

    void Reset() 
    {
        Berserk_Timer = BERSERK_TIMER;
        TympanicTantrum_Timer = TIMPANIC_TANTRUM_TIMER;
        SearingLight_Timer = SEARING_LIGHT_TIMER;
        GravityBomb_Timer = GRAVITY_BOMB_TIMER;
        HP_Counter = 3;
        HearthPhase = false;
        HearthPhase_Timer = HEARTH_PHASE_TIMER;

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    /*void SpellHitTarget(Unit *target, const SpellEntry *spell)
    {
		if(spell->Id == HEROIC(SPELL_GRAVITY_BOMB, SPELL_GRAVITY_BOMB_H))
			DoCast(target, HEROIC(SPELL_GRAVITY_BOMB_EFFECT, SPELL_GRAVITY_BOMB_EFFECT_H));
    }*/

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0,1) ? SAY_KILLED_PLAYER1 : SAY_KILLED_PLAYER2, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (IsOutOfCombatArea(m_creature))
        {
            EnterEvadeMode();
            return;
        }

        //While in hearth phase
        if (HearthPhase)
        {
            if (HearthPhase_Timer < diff)
            {
                HearthPhase = false;
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoStartMovement(m_creature->getVictim());
                DoScriptText(SAY_GETUP, m_creature);
            }
            else
            {
                HearthPhase_Timer -= diff;
                DoStartNoMovement(m_creature->getVictim());
                return;
            }
        }

        //HP at 75%, 50%, 25%
        if (HP_Counter > 0 && m_creature->GetHealthPercent() <= float(25 * HP_Counter))
        {
            if (Creature *Hearth = m_creature->SummonCreature(NPC_HEARTH_OF_DECONST,
                    m_creature->GetPositionX()+rand()%11-5,
                    m_creature->GetPositionY()+rand()%11-5,
                    m_creature->GetPositionZ(), 0,
                    TEMPSUMMON_TIMED_DESPAWN, HEARTH_PHASE_TIMER))
                Hearth->SetInCombatWithZone();
			HP_Counter--;
            // make boss idle and unattackable
            HearthPhase = true;
            HearthPhase_Timer = HEARTH_PHASE_TIMER;
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoScriptText(SAY_REST, m_creature);
            return;
        }

        //Berserk Timer
        if (Berserk_Timer < diff)
        {
            DoScriptText(SAY_BERSERK, m_creature);
            DoCast(m_creature, SPELL_BERSERK);
            Berserk_Timer = 5*MINUTE*IN_MILLISECONDS;
        }
        else
            Berserk_Timer -= diff;

        //Gravity Bomb timer
        if (GravityBomb_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target, HEROIC(SPELL_GRAVITY_BOMB, SPELL_GRAVITY_BOMB_H));

            GravityBomb_Timer = GRAVITY_BOMB_TIMER;
        }else GravityBomb_Timer -= diff;

	    //Searing Light timer
        if (SearingLight_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target, HEROIC(SPELL_SEARING_LIGHT, SPELL_SEARING_LIGHT_H));

            SearingLight_Timer = SEARING_LIGHT_TIMER;
        }else SearingLight_Timer -= diff;

        //Tympanic Tantrum timer
        if (TympanicTantrum_Timer < diff)
        {
            DoScriptText(SAY_TANTRUM, m_creature);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM_PLAYER, 0))
                DoCast(target,SPELL_TYMPANIC_TANTRUM);

            TympanicTantrum_Timer = TIMPANIC_TANTRUM_TIMER;
        }else TympanicTantrum_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_deconstructor(Creature* pCreature)
{
    return new boss_deconstructorAI(pCreature);
}

void AddSC_boss_xt_002()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_deconstructor";
    newscript->GetAI = &GetAI_boss_deconstructor;
    newscript->RegisterSelf();
}
