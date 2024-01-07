#include <DBHelper.h>

#include "Plugin.h"

namespace Plugin::DBHelper
{
	bool IsPlayerExists(const FString& eos_id)
	{
		return database->IsPlayerExists(eos_id);
	}
} // namespace DBHelper
