#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Spells
{
    // Gormok
    SPELL_IMPALE_N10            = 66331,    // "Requires Polearms"
    SPELL_IMPALE_N25            = 67477,
    SPELL_IMPALE_H10            = 67478,
    SPELL_IMPALE_H25            = 67479,
    SPELL_STAGGERING_STOMP_N10  = 66330,
    SPELL_STAGGERING_STOMP_N25  = 67647,
    SPELL_STAGGERING_STOMP_H10  = 67648,
    SPELL_STAGGERING_STOMP_H25  = 67649,
    SPELL_RISING_ANGER          = 66636,
    SPELL_SNOBOLLED             = 66406,    // dummy effect
    // Snobold Vassal
    SPELL_BATTER                = 66408,
    SPELL_FIRE_BOMB             = 66313,
    SPELL_HEAD_CRACK            = 66407,
};

enum AddIds
{
    NPC_SNOBOLD_VASSAL          = 34800,
};

struct MANGOS_DLL_DECL boss_toc_gormokAI: public boss_trial_of_the_crusaderAI
{
    boss_toc_gormokAI(Creature* pCreature):
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
};

void AddSC_boss_gormok()
{
    Script *newscript;

    REGISTER_SCRIPT(boss_toc_gormok);
}
