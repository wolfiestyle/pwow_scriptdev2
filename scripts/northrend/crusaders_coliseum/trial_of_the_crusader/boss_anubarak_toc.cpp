#include "precompiled.h"
#include "trial_of_the_crusader.h"

//evidently, azjol-nerub was merely a set-back

enum Spells
{
    SPELL_BERSERK               = 26662,
    SPELL_FREEZING_SLASH        = 66012,
    SPELL_PENETRATING_COLD_N10  = 66013,
    SPELL_PENETRATING_COLD_N25  = 67700,
    SPELL_PENETRATING_COLD_H10  = 68509,
    SPELL_PENETRATING_COLD_H25  = 68510,
    SPELL_LEECHING_SWARM_N10    = 66118,
    SPELL_LEECHING_SWARM_N25    = 67630,
    SPELL_LEECHING_SWARM_H10    = 68646,
    SPELL_LEECHING_SWARM_H25    = 68647,
    SPELL_PURSUING_SPIKES       = 65922,  // only part of the effect, probably used by dummy creature
};

enum AddIds
{
    NPC_FROST_SPHERE            = 34606,
    NPC_NERUBIAN_BURROWER       = 34607,
    NPC_SWARM_SCARAB            = 34605,
};

struct MANGOS_DLL_DECL boss_toc_anubarakAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_anubarakAI(Creature* pCreature):
        boss_trial_of_the_crusaderAI(pCreature)
    {
    }

    void Aggro(Unit *pWho)
    {
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

    void JustDied(Unit *pKiller)
    {
        //only non-summoned boss, so to streamline things its easier to do this
        if (Creature *barrett = GET_CREATURE(TYPE_BARRETT_RAMSAY))
            if(barrett->AI())
                barrett->AI()->SummonedCreatureJustDied(m_creature);
    }
};

void AddSC_boss_anubarak_toc()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_anubarak);
}
