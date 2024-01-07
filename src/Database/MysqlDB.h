#pragma once

#include <mysql++11.h>

#include "IDatabase.h"

#include "json.hpp"

#include "Plugin.h"

#pragma comment(lib, "mysqlclient.lib")

class MySql : public IDatabase
{
public:
	explicit MySql(std::string server, std::string username, std::string password, std::string db_name, std::string table_discord_link, const int port)
		: table_discord_link_(move(table_discord_link))
	{
		try
		{
			daotk::mysql::connect_options options;
			options.server = move(server);
			options.username = move(username);
			options.password = move(password);
			options.dbname = move(db_name);
			options.autoreconnect = true;
			options.timeout = 30;
			options.port = port;

			bool result = db_.open(options);
			if (!result)
			{
				Log::GetLog()->critical("Failed to open database connection check your settings!");
			}

			result = db_.query(fmt::format("CREATE TABLE IF NOT EXISTS {} ("
				"Id INT NOT NULL AUTO_INCREMENT,"
				"EosId VARCHAR(50) NOT NULL,"
				"PRIMARY KEY(Id),"
				"UNIQUE INDEX EosId_UNIQUE (EosId ASC));", table_discord_link_));

			if (!result)
			{
				Log::GetLog()->critical("({} {}) Failed to create table!", __FILE__, __FUNCTION__);
			}
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
		}
	}

	bool TryAddNewPlayer(const FString& eos_id) override
	{
		try
		{
			std::string query = fmt::format("INSERT INTO {} (EosId) VALUES ('{}');", PROJECT_NAME, eos_id.ToString());

			return db_.query(query);
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	bool IsPlayerExists(const FString& eos_id) override
	{
		try
		{
			std::string query = fmt::format("SELECT count(1) FROM {} WHERE EosId = '{}';", table_discord_link_, eos_id.ToString());

			const auto result = db_.query(query).get_value<int>();

			return result > 0;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

private:
	daotk::mysql::connection db_;
	std::string table_discord_link_;
};