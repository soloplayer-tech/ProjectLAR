#include "LPlayerSkillDatabase.h"

ULPlayerSkillDataAsset* ULPlayerSkillDatabase::FindSkillDataAsset(ELPlayerSkillID SkillID) const
{
	if (SkillID == ELPlayerSkillID::None)
	{
		return nullptr;
	}

	ULPlayerSkillDataAsset* FoundSkillDataAsset = nullptr;

	for (const TObjectPtr<ULPlayerSkillDataAsset>& SkillDataAsset : SkillDataAssets)
	{
		if (!SkillDataAsset || SkillDataAsset->SkillID != SkillID)
		{
			continue;
		}

		if (FoundSkillDataAsset)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Duplicate SkillID in SkillDatabase: %s"),
				*UEnum::GetValueAsString(SkillID)
			);

			continue;
		}

		FoundSkillDataAsset = SkillDataAsset.Get();
	}

	return FoundSkillDataAsset;
}
