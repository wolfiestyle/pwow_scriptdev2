#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct MANGOS_DLL_DECL boss_toc_jaraxxusAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_jaraxxusAI(Creature* pCreature):
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
                
            }

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_jaraxxus()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_jaraxxus);
}
