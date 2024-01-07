#pragma once

#include <API/Ark/Ark.h>

class IDatabase
{
public:
	virtual ~IDatabase() = default;

	//const nlohmann::json data
	virtual bool TryAddNewPlayer(const FString& eos_id) = 0;
	virtual bool IsPlayerExists(const FString& eos_id) = 0;
};