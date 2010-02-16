#include "precompiled.h"
#include "trial_of_the_crusader.h"

//evidently, azjol-nerub was merely a set-back

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
