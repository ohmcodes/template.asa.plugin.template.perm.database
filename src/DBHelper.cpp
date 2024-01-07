#include <DBHelper.h>

#include "DiscordLinker.h"

namespace DiscordLinker::DBHelper
{
	bool IsPlayerExists(const FString& eos_id)
	{
		return database->IsPlayerExists(eos_id);
	}

	bool IsNotLinked(const FString& eos_id)
	{
		return database->IsNotLinked(eos_id);
	}

	bool TryLinkDiscord(const FString& eos_id, const FString& token)
	{
		return database->TryLinkDiscord(eos_id, token);
	}

	FString IsTokenExists(const FString& token)
	{
		return database->IsTokenExists(token);
	}
	FString GetToken(const FString& eos_id)
	{
		return database->GetToken(eos_id);
	}
} // namespace DBHelper
