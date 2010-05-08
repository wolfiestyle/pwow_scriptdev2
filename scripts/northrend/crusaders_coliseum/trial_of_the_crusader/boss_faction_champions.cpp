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
SDComment:
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

//28 champions, divided equally between horde/alliance, each pair sharing AIs

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

static int32 KilledPlayerSays[8] = {
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED1,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED2,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED3,
    SAY_GARROSH_ALLIANCE_PLAYER_KILLED4,
    SAY_VARIAN_HORDE_PLAYER_KILLED1,
    SAY_VARIAN_HORDE_PLAYER_KILLED2,
    SAY_VARIAN_HORDE_PLAYER_KILLED3,
    SAY_VARIAN_HORDE_PLAYER_KILLED4
};

// common parts for all faction champions
struct MANGOS_DLL_DECL boss_faction_championAI: public boss_trial_of_the_crusaderAI
{
    boss_faction_championAI(Creature *pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void KilledUnit(Unit *who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;
        bool isHorde = IS_HORDE;
        Creature *source = GET_CREATURE(isHorde ? TYPE_VARIAN_WYRM : TYPE_GARROSH_HELLSCREAM);
        if (!source)
            return;
        DoScriptText(KilledPlayerSays[urand(0,3) + (isHorde ? 4 : 0)], source);
    }

    void JustDied(Unit *killer)
    {
        bool isHorde = IS_HORDE;
        Creature *source = GET_CREATURE(isHorde ? TYPE_GARROSH_HELLSCREAM : TYPE_VARIAN_WYRM);
        if (!source)
            return;
        DoScriptText(KilledPlayerSays[urand(0,3) + (isHorde ? 0 : 4)], source);
    }
};

struct MANGOS_DLL_DECL boss_toc_deathknightAI: public boss_faction_championAI
{
    boss_toc_deathknightAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
        //do any init here, instance stuff in parent class
        ////DO NOT CALL Reset() or creature will despawn
    }

    // only should be called on wipe, creature WILL BE DESTROYED upon wipe, definition in boss_trial_of_the_crusaderAI
    // if defining a reset function, remember to call boss_trial_of_the_crusaderAI::Reset() after all of it
    /*void Reset()
    {
        boss_trial_of_the_crusaderAI::Reset();
    }*/

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_caster_druidAI: public boss_faction_championAI
{
    boss_toc_caster_druidAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_heal_druidAI: public boss_faction_championAI
{
    boss_toc_heal_druidAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_hunterAI: public boss_faction_championAI
{
    boss_toc_hunterAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_mageAI: public boss_faction_championAI
{
    boss_toc_mageAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_heal_paladinAI: public boss_faction_championAI
{
    boss_toc_heal_paladinAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_ret_paladinAI: public boss_faction_championAI
{
    boss_toc_ret_paladinAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_disc_priestAI: public boss_faction_championAI
{
    boss_toc_disc_priestAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_shadow_priestAI: public boss_faction_championAI
{
    boss_toc_shadow_priestAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_rogueAI: public boss_faction_championAI
{
    boss_toc_rogueAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_magic_shamanAI: public boss_faction_championAI
{
    boss_toc_magic_shamanAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_melee_shamanAI: public boss_faction_championAI
{
    boss_toc_melee_shamanAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_warlockAI: public boss_faction_championAI
{
    boss_toc_warlockAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_toc_warriorAI: public boss_faction_championAI
{
    boss_toc_warriorAI(Creature *pCreature):
        boss_faction_championAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
        DoCast(m_creature, SPELL_SUNWELL_RADIANCE, true); //TODO: remove this when script is complete
        if (m_pInstance)
            m_pInstance->SetData(m_uiBossEncounterId, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Events.Update(uiDiff);
        while(uint32 uiEventId = Events.ExecuteEvent())
            switch (uiEventId)
            {
                default:
                    break;
            }

        DoMeleeAttackIfReady();
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
