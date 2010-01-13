#include "precompiled.h"

#define SPELL_FIREBLAST    59637
#define SPELL_FROSTBOLT    59638

struct MANGOS_DLL_DECL MirrorAI : public ScriptedAI
{
    MirrorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        PlayerGUID = 0;
        if (Player* pPlayer = GetPlayerAtMinimumRange(0.5f))
            PlayerGUID = pPlayer->GetGUID();
        Reset();
    }

    uint64 PlayerGUID;
    uint32 Frostbolt;
    uint32 Fireblast;

    void Reset()
    {
        Frostbolt = 3000 + rand()%500;
        Fireblast = 6000 + rand()%500;
    }

    void UpdateAI(const uint32 diff)
    {
        if (PlayerGUID)
        {
            if (Unit* pPlayer = Unit::GetUnit(*m_creature, PlayerGUID))
                AttackStart(pPlayer->getAttackerForHelper());
        }

        if (!m_creature->getVictim())
            return;

        if (Fireblast < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_FIREBLAST);
            Fireblast = 6000 + rand()%500;
        }else Fireblast -= diff;

        if (Frostbolt < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_FROSTBOLT);
            Frostbolt = 3000 + rand()%500;
        }else Frostbolt -= diff;
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