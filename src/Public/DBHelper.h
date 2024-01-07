#pragma once

#include <API/ARK/Ark.h>
#include <API/UE/Math/ColorList.h>
#include <Tools.h>

namespace Plugin::DBHelper
{
	/**
	 * \brief Checks if player exists in database
	 * \param eos_id
	 * \return True if exists, false otherwise
	 */
	bool IsPlayerExists(const FString& eos_id);
} // namespace DBHelper // namespace Plugin

