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
SDName: Boss_Four_Horsemen
SD%Complete: 75
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    //all horsemen
    SPELL_SHIELDWALL        = 29061,
    SPELL_BESERK            = 26662,

    //lady blaumeux
    SAY_BLAU_AGGRO          = -1533044,
    SAY_BLAU_TAUNT1         = -1533045,
    SAY_BLAU_TAUNT2         = -1533046,
    SAY_BLAU_TAUNT3         = -1533047,
    SAY_BLAU_SPECIAL        = -1533048,
    SAY_BLAU_SLAY           = -1533049,
    SAY_BLAU_DEATH          = -1533050,

    SPELL_MARK_OF_BLAUMEUX  = 28833,
    SPELL_UNYILDING_PAIN    = 57381,
    SPELL_VOIDZONE          = 28863,
    H_SPELL_VOIDZONE        = 57463,
    SPELL_SHADOW_BOLT       = 57374,
    H_SPELL_SHADOW_BOLT     = 57464,

    //baron rivendare
    SAY_RIVE_AGGRO1         = -1533065,
    SAY_RIVE_AGGRO2         = -1533066,
    SAY_RIVE_AGGRO3         = -1533067,
    SAY_RIVE_SLAY1          = -1533068,
    SAY_RIVE_SLAY2          = -1533069,
    SAY_RIVE_SPECIAL        = -1533070,
    SAY_RIVE_TAUNT1         = -1533071,
    SAY_RIVE_TAUNT2         = -1533072,
    SAY_RIVE_TAUNT3         = -1533073,
    SAY_RIVE_DEATH          = -1533074,

    SPELL_MARK_OF_RIVENDARE = 28834,
    SPELL_UNHOLY_SHADOW     = 28882,
    H_SPELL_UNHOLY_SHADOW   = 57369,

    //thane korthazz
    SAY_KORT_AGGRO          = -1533051,
    SAY_KORT_TAUNT1         = -1533052,
    SAY_KORT_TAUNT2         = -1533053,
    SAY_KORT_TAUNT3         = -1533054,
    SAY_KORT_SPECIAL        = -1533055,
    SAY_KORT_SLAY           = -1533056,
    SAY_KORT_DEATH          = -1533057,

    SPELL_MARK_OF_KORTHAZZ  = 28832,
    SPELL_METEOR            = 26558,                        // m_creature->getVictim() auto-area spell but with a core problem

    //sir zeliek
    SAY_ZELI_AGGRO          = -1533058,
    SAY_ZELI_TAUNT1         = -1533059,
    SAY_ZELI_TAUNT2         = -1533060,
    SAY_ZELI_TAUNT3         = -1533061,
    SAY_ZELI_SPECIAL        = -1533062,
    SAY_ZELI_SLAY           = -1533063,
    SAY_ZELI_DEATH          = -1533064,

    SPELL_MARK_OF_ZELIEK    = 28835,
    SPELL_HOLY_WRATH        = 28883,
    H_SPELL_HOLY_WRATH      = 57466,
    SPELL_HOLY_BOLT         = 57376,
    H_SPELL_HOLY_BOLT       = 57465,
    SPELL_CONDEMNATION      = 57377,

    // horseman spirits
    NPC_SPIRIT_OF_BLAUMEUX    = 16776,
    NPC_SPIRIT_OF_RIVENDARE   = 0,                          //creature entry not known yet
    NPC_SPIRIT_OF_KORTHAZZ    = 16778,
    NPC_SPIRIT_OF_ZELIREK     = 16777
};

struct MANGOS_DLL_DECL boss_lady_blaumeuxAI : public ScriptedAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 VoidZone_Timer;
    uint32 UnyieldingPain_Timer;
    uint32 Shadowbolt_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint64 KorthazzGUID;
    uint64 RivendareGUID;
    uint64 ZeliekGUID;
    uint64 LootChestGUID;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        VoidZone_Timer = 15000;
        UnyieldingPain_Timer = 5000;
        Shadowbolt_Timer = 2000 + rand()%500;
        ShieldWall1 = true;
        ShieldWall2 = true;

        KorthazzGUID = 0;
        RivendareGUID = 0;
        ZeliekGUID = 0;
        LootChestGUID = 0;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_BLAU_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_BLAU_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_BLAU_DEATH, m_creature);

        if (m_pInstance)
        {
            KorthazzGUID = m_pInstance->GetData64(NPC_THANE);
            RivendareGUID = m_pInstance->GetData64(NPC_RIVENDARE);
            ZeliekGUID = m_pInstance->GetData64(NPC_ZELIEK);
            LootChestGUID = m_pInstance->GetData64(GO_CHEST_HORSEMEN_NORM);

            if (RivendareGUID && KorthazzGUID && ZeliekGUID && LootChestGUID)
            {
                Creature* Rivendare = ((Creature*)Unit::GetUnit((*m_creature), RivendareGUID));
                Creature* Korthazz = ((Creature*)Unit::GetUnit((*m_creature), KorthazzGUID));
                Creature* Zeliek = ((Creature*)Unit::GetUnit((*m_creature), ZeliekGUID));

                if (Rivendare && Korthazz && Zeliek && !Rivendare->isAlive() && !Korthazz->isAlive() && !Zeliek->isAlive())
                {
                    m_pInstance->DoRespawnGameObject(LootChestGUID);
                    m_pInstance->SetData(TYPE_FOUR_HORSEMEN, DONE);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Blaumeux
        if (Mark_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_BLAUMEUX);
            Mark_Timer = 12000;
        }else Mark_Timer -= uiDiff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Void Zone
        if (VoidZone_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),m_bIsRegularMode ? SPELL_VOIDZONE : H_SPELL_VOIDZONE);
            VoidZone_Timer = 15000;
        }else VoidZone_Timer -= uiDiff;

        // Unyielding Pain
        if (UnyieldingPain_Timer < uiDiff)
        {
            if (!m_creature->IsWithinDist(m_creature->getVictim(),55.0f))
                DoCast(m_creature->getVictim(),SPELL_UNYILDING_PAIN);
            UnyieldingPain_Timer = 5000;
        }else UnyieldingPain_Timer -= uiDiff;

        //Uses Shadow Bolt instead of Melee
        if (Shadowbolt_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHADOW_BOLT : H_SPELL_SHADOW_BOLT);
            Shadowbolt_Timer = 2000 + rand()%500;
        }else Shadowbolt_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}

struct MANGOS_DLL_DECL boss_rivendare_naxxAI : public ScriptedAI
{
    boss_rivendare_naxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 UnholyShadow_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint64 KorthazzGUID;
    uint64 BlaumeuxGUID;
    uint64 ZeliekGUID;
    uint64 LootChestGUID;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        UnholyShadow_Timer = 15000;
        ShieldWall1 = true;
        ShieldWall2 = true;

        KorthazzGUID = 0;
        BlaumeuxGUID = 0;
        ZeliekGUID = 0;
        LootChestGUID = 0;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_RIVE_AGGRO1, m_creature); break;
            case 1: DoScriptText(SAY_RIVE_AGGRO2, m_creature); break;
            case 2: DoScriptText(SAY_RIVE_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(urand(0, 1) ? SAY_RIVE_SLAY1 : SAY_RIVE_SLAY2, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_RIVE_DEATH, m_creature);

        if (m_pInstance)
        {
            KorthazzGUID = m_pInstance->GetData64(NPC_THANE);
            BlaumeuxGUID = m_pInstance->GetData64(NPC_BLAUMEUX);
            ZeliekGUID = m_pInstance->GetData64(NPC_ZELIEK);
            LootChestGUID = m_pInstance->GetData64(GO_CHEST_HORSEMEN_NORM);

            if (KorthazzGUID && BlaumeuxGUID && ZeliekGUID && LootChestGUID)
            {
                Creature* Korthazz = ((Creature*)Unit::GetUnit((*m_creature), KorthazzGUID));
                Creature* Blaumeux = ((Creature*)Unit::GetUnit((*m_creature), BlaumeuxGUID));
                Creature* Zeliek = ((Creature*)Unit::GetUnit((*m_creature), ZeliekGUID));

                if (Korthazz && Blaumeux && Zeliek && !Korthazz->isAlive() && !Blaumeux->isAlive() && !Zeliek->isAlive())
                {
                    m_pInstance->DoRespawnGameObject(LootChestGUID);
                    m_pInstance->SetData(TYPE_FOUR_HORSEMEN, DONE);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Rivendare
        if (Mark_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_RIVENDARE);
            Mark_Timer = 12000;
        }else Mark_Timer -= uiDiff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Unholy Shadow
        if (UnholyShadow_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_UNHOLY_SHADOW : H_SPELL_UNHOLY_SHADOW);
            UnholyShadow_Timer = 15000;
        }else UnholyShadow_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI(pCreature);
}

struct MANGOS_DLL_DECL boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 Meteor_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint64 RivendareGUID;
    uint64 BlaumeuxGUID;
    uint64 ZeliekGUID;
    uint64 LootChestGUID;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        Meteor_Timer = 15000;
        ShieldWall1 = true;
        ShieldWall2 = true;

        RivendareGUID = 0;
        BlaumeuxGUID = 0;
        ZeliekGUID = 0;
        LootChestGUID = 0;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_KORT_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_KORT_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_KORT_DEATH, m_creature);

        if (m_pInstance)
        {
            RivendareGUID = m_pInstance->GetData64(NPC_RIVENDARE);
            BlaumeuxGUID = m_pInstance->GetData64(NPC_BLAUMEUX);
            ZeliekGUID = m_pInstance->GetData64(NPC_ZELIEK);
            LootChestGUID = m_pInstance->GetData64(GO_CHEST_HORSEMEN_NORM);

            if (RivendareGUID && BlaumeuxGUID && ZeliekGUID && LootChestGUID)
            {
                Creature* Rivendare = ((Creature*)Unit::GetUnit((*m_creature), RivendareGUID));
                Creature* Blaumeux = ((Creature*)Unit::GetUnit((*m_creature), BlaumeuxGUID));
                Creature* Zeliek = ((Creature*)Unit::GetUnit((*m_creature), ZeliekGUID));

                if (Rivendare && Blaumeux && Zeliek && !Rivendare->isAlive() && !Blaumeux->isAlive() && !Zeliek->isAlive())
                {
                    m_pInstance->DoRespawnGameObject(LootChestGUID);
                    m_pInstance->SetData(TYPE_FOUR_HORSEMEN, DONE);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Korthazz
        if (Mark_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_KORTHAZZ);
            Mark_Timer = 12000;
        }else Mark_Timer -= uiDiff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Meteor
        if (Meteor_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_METEOR);
            Meteor_Timer = 15000;
        }else Meteor_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}

struct MANGOS_DLL_DECL boss_sir_zeliekAI : public ScriptedAI
{
    boss_sir_zeliekAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 HolyWrath_Timer;
    uint32 Condemnation_Timer;
    uint32 HolyBolt_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    uint64 KorthazzGUID;
    uint64 RivendareGUID;
    uint64 BlaumeuxGUID;
    uint64 LootChestGUID;

    void Reset()
    {
        Mark_Timer = 20000;                                 // First Horsemen Mark is applied at 20 sec.
        HolyWrath_Timer = 15000;
        Condemnation_Timer = 5000;
        HolyBolt_Timer = 2000 + rand()%500;
        ShieldWall1 = true;
        ShieldWall2 = true;

        KorthazzGUID = 0;
        RivendareGUID = 0;
        BlaumeuxGUID = 0;
        LootChestGUID = 0;

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_ZELI_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FOUR_HORSEMEN, IN_PROGRESS);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_ZELI_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_ZELI_DEATH, m_creature);

        if (m_pInstance)
        {
            RivendareGUID = m_pInstance->GetData64(NPC_RIVENDARE);
            BlaumeuxGUID = m_pInstance->GetData64(NPC_BLAUMEUX);
            KorthazzGUID = m_pInstance->GetData64(NPC_THANE);
            LootChestGUID = m_pInstance->GetData64(GO_CHEST_HORSEMEN_NORM);

            if (RivendareGUID && BlaumeuxGUID && KorthazzGUID && LootChestGUID)
            {
                Creature* Rivendare = ((Creature*)Unit::GetUnit((*m_creature), RivendareGUID));
                Creature* Blaumeux = ((Creature*)Unit::GetUnit((*m_creature), BlaumeuxGUID));
                Creature* Korthazz = ((Creature*)Unit::GetUnit((*m_creature), KorthazzGUID));

                if (Rivendare && Blaumeux && Korthazz && !Rivendare->isAlive() && !Blaumeux->isAlive() && !Korthazz->isAlive())
                {
                    m_pInstance->DoRespawnGameObject(LootChestGUID);
                    m_pInstance->SetData(TYPE_FOUR_HORSEMEN, DONE);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Zeliek
        if (Mark_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_ZELIEK);
            Mark_Timer = 12000;
        }else Mark_Timer -= uiDiff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(m_creature,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Holy Wrath
        if (HolyWrath_Timer < uiDiff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_HOLY_WRATH : H_SPELL_HOLY_WRATH);
            HolyWrath_Timer = 15000;
        }else HolyWrath_Timer -= uiDiff;

        // Condemnation
        if (Condemnation_Timer < uiDiff)
        {
            if (!m_creature->IsWithinDist(m_creature->getVictim(),55.0f))
                DoCast(m_creature->getVictim(),SPELL_CONDEMNATION);
            Condemnation_Timer = 5000;
        }else Condemnation_Timer -= uiDiff;

        //Uses Holy Bolt instead of Melee
        if (HolyBolt_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_HOLY_BOLT : H_SPELL_HOLY_BOLT);
            HolyBolt_Timer = 2000 + rand()%500;
        }else HolyBolt_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}

void AddSC_boss_four_horsemen()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_lady_blaumeux";
    NewScript->GetAI = &GetAI_boss_lady_blaumeux;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_rivendare_naxx";
    NewScript->GetAI = &GetAI_boss_rivendare_naxx;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_thane_korthazz";
    NewScript->GetAI = &GetAI_boss_thane_korthazz;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_sir_zeliek";
    NewScript->GetAI = &GetAI_boss_sir_zeliek;
    NewScript->RegisterSelf();
}
