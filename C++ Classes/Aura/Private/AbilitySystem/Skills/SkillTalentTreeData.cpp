// Copyright Nono Studios


#include "AbilitySystem/Skills/SkillTalentTreeData.h"

const FTalentData* USkillTalentTreeData::GetTalentData(const FGameplayTag& TalentTag) const
{
	for (const FTalentData& Talent : TalentsInformations)
	{
		if (Talent.TalentTag == TalentTag)
		{
			return &Talent;
		}
	}
	return nullptr;
}
