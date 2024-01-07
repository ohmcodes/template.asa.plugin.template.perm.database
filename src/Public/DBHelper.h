#pragma once

#include <API/ARK/Ark.h>
#include <API/UE/Math/ColorList.h>
#include <Tools.h>

namespace DiscordLinker::DBHelper
{
	/**
	 * \brief Checks if player exists in shop database
	 * \param eos_id DiscordLinker steam id
	 * \return True if exists, false otherwise
	 */
	bool IsPlayerExists(const FString& eos_id);
	bool IsNotLinked(const FString& eos_id);
	bool TryLinkDiscord(const FString& discord_id, const FString& token);
	FString IsTokenExists(const FString& token);
	FString GetToken(const FString& eos_id);

} // namespace DBHelper // namespace DiscordLinker

