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
SDName: Boss Razorscale
SD%Complete: 0
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum Spells
{
    SPELL_BERSERK           = 47008,
    SPELL_FIREBALL          = 62796,
    SPELL_FIREBALL_H        = 63815,
    SPELL_WING_BUFFET       = 62666,
    SPELL_FLAME_BREATH      = 63317,
    SPELL_FLAME_BREATH_H    = 64021,
    SPELL_FLAME_BUFFET      = 64016,
    SPELL_FLAME_BUFFET_H    = 64023,
    SPELL_DEVOURING_FLAME   = 64704,
    SPELL_DEVOURING_FLAME_H = 64733,
    SPELL_FUSE_ARMOR        = 64771
};

enum Says
{
    //commanders
    SAY_INTRO1              = -1300032,
    //engineer
    SAY_AGGRO1              = -1300033,
    //commander
    SAY_AGGRO2              = -1300034,
    ///engineer
    SAY_AGGRO3              = -1300035,
    //commander
    SAY_GROUND_PHASE        = -1300036,

    EMOTE_FLAME_BREATH      = -1300037,
    SAY_FIRE_EXTINGUISHED   = -1300038
};

struct MANGOS_DLL_DECL boss_razorscaleAI : public ScriptedAI
{
    boss_razorscaleAI(Creature* pCreature):
        ScriptedAI(pCreature),
        m_uiBossEncounterId(TYPE_RAZORSCALE)
    {
        m_pInstance = dynamic_cast<ScriptedInstance*>(pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    uint32 const m_uiBossEncounterId;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

	uint8 Phase;
	uint32 DevouringFlame_Timer;
    uint32 Fireball_Timer;
	uint32 FlameBreath_Timer;
	uint32 WingBuffet_Timer;
	uint32 FlameBuffet_Timer;
	uint32 FuseArmor_Timer;
	uint32 Berserk_Timer;

    void Reset() 
    {
		Phase = 1;
		DevouringFlame_Timer = 8000;    //8 seconds
		Fireball_Timer = 15000;         //15 seconds
		FlameBreath_Timer = 30000;      //30 seconds
		WingBuffet_Timer = 40000;       //40 seconds
		FlameBuffet_Timer = 8000;       //8 seconds
		FuseArmor_Timer = 30000;        //30 seconds
		Berserk_Timer = 600000;         //10 minutes

        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetHover(true);

        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, NOT_STARTED);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		//Phase 1
		if (Phase == 1)
		{
			if ((int) (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() +0.5) <= 50)
			{
                m_creature->SetHover(false);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

				Phase = 2;
                return;
			}

			//Wing Buffet timer
			if (WingBuffet_Timer < diff)
			{
			    DoCast(m_creature,SPELL_WING_BUFFET);

			    WingBuffet_Timer = 40000;
			}else WingBuffet_Timer -= diff;

			//Flame Breath timer
			if (FlameBreath_Timer < diff)
			{
                DoScriptText(EMOTE_FLAME_BREATH, m_creature);
			    DoCast(m_creature, HEROIC(SPELL_FLAME_BREATH, SPELL_FLAME_BREATH_H));

			    FlameBreath_Timer = 30000;
			}else FlameBreath_Timer -= diff;

			//Fireball timer
			if (Fireball_Timer < diff)
			{
			    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			        DoCast(target, HEROIC(SPELL_FIREBALL, SPELL_FIREBALL_H));

			    Fireball_Timer = 15000;
			}else Fireball_Timer -= diff;

			//Devouring Flame timer
			if (DevouringFlame_Timer < diff)
			{
			    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			        DoCast(target, HEROIC(SPELL_DEVOURING_FLAME, SPELL_DEVOURING_FLAME_H));

			    DevouringFlame_Timer = 8000;
			}else DevouringFlame_Timer -= diff;
		}

		//Phase 2
		if (Phase == 2)
		{
			//Flame Breath timer
			if (FlameBreath_Timer < diff)
			{
                DoScriptText(EMOTE_FLAME_BREATH, m_creature);
				DoCast(m_creature, HEROIC(SPELL_FLAME_BREATH, SPELL_FLAME_BREATH_H));

			    FlameBreath_Timer = 40000;
			}else FlameBreath_Timer -= diff;

			//Fuse Armor timer
			if (FuseArmor_Timer < diff)
			{
			    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					DoCast(target,SPELL_FUSE_ARMOR);

			    FuseArmor_Timer = 30000;
			}else FuseArmor_Timer -= diff;

			//Devouring Flame timer
			if (DevouringFlame_Timer < diff)
			{
				DoCast(m_creature->getVictim(), HEROIC(SPELL_DEVOURING_FLAME, SPELL_DEVOURING_FLAME_H));

			    DevouringFlame_Timer = 15000;
			}else DevouringFlame_Timer -= diff;

			//Flame Buffet timer
			if (FlameBuffet_Timer < diff)
			{
			    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
			        DoCast(target, HEROIC(SPELL_FLAME_BUFFET, SPELL_FLAME_BUFFET_H));

			    FlameBuffet_Timer = 8000;
			}else FlameBuffet_Timer -= diff;

			DoMeleeAttackIfReady();
		}

		//Berserk timer
		if (Berserk_Timer < diff)
		{
			DoCast(m_creature,SPELL_BERSERK);

			Berserk_Timer = 300000;
		}else Berserk_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI(pCreature);
}

void AddSC_boss_razorscale()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_razorscale";
    newscript->GetAI = &GetAI_boss_razorscale;
    newscript->RegisterSelf();
}
