#include "precompiled.h"

enum Spells
{
    SPELL_FIREBLAST     = 59637,
    SPELL_FROSTBOLT     = 59638,
};

struct MANGOS_DLL_DECL MirrorAI: public ScriptedAI
{
    ObjectGuid m_PlayerGUID;
    uint32 m_FrostboltTimer;
    uint32 m_FireblastTimer;

    MirrorAI(Creature* pCreature):
        ScriptedAI(pCreature)
    {
        if (Player *pPlayer = GetPlayerAtMinimumRange(0.5f))    //FIXME: get owner player instead
            m_PlayerGUID = pPlayer->GetObjectGuid();
        Reset();
    }

    void Reset()
    {
        m_FrostboltTimer = urand(3000, 3500);
        m_FireblastTimer = urand(6000, 6500);
    }

    void UpdateAI(uint32 const diff)
    {
        if (!m_PlayerGUID.IsEmpty())
            if (Unit *pPlayer = m_creature->GetMap()->GetUnit(m_PlayerGUID))
                AttackStart(pPlayer->getAttackerForHelper());

        if (!m_creature->getVictim())
            return;

        if (m_FireblastTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FIREBLAST);
            m_FireblastTimer = urand(6000, 6500);
        }
        else
            m_FireblastTimer -= diff;

        if (m_FrostboltTimer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FROSTBOLT);
            m_FrostboltTimer = urand(3000, 3500);
        }
        else
            m_FrostboltTimer -= diff;
    }

};

CreatureAI* GetAI_mirror_image(Creature* pCreature)
{
    return new MirrorAI(pCreature);
}

void AddSC_Mirror_Image()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mirror_imageAI";
    newscript->GetAI = &GetAI_mirror_image;
    newscript->RegisterSelf();
}
