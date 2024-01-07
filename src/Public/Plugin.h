#pragma once

#include "../Database/IDatabase.h"
#include "json.hpp"
#include <random>


#include "Requests.h"

namespace Plugin
{
	inline nlohmann::json config;
	inline std::unique_ptr<IDatabase> database;
}

