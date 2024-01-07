#pragma once

#include "../hdr/sqlite_modern_cpp.h"

#include <Tools.h>

#include "IDatabase.h"

class SqlLite : public IDatabase
{
public:
	explicit SqlLite(const std::string& path)
		: db_(path.empty()
			? AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/" + PROJECT_NAME + ".db"
			: path)
	{
		try
		{
			//db_ << "PRAGMA journal_mode=WAL;";

			db_ << "CREATE TABLE IF NOT EXISTS DiscordLinker ("
				"Id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
				"EosId TEXT NOT NULL UNIQUE"
				");";
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
			std::string query = fmt::format("INSERT INTO {} (EosId) VALUES('{}');", PROJECT_NAME, eos_id.ToString());

			db_ << query;
			
			return db_.last_insert_rowid() > 0;
		}
		catch (const std::exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

	bool IsPlayerExists(const FString& eos_id) override
	{
		int count = 0;

		try
		{
			std::string query = fmt::format("SELECT count(1) FROM {} WHERE EosId = '{}';", PROJECT_NAME, eos_id.ToString());

			db_ << query >> count;

			return count > 0;
		}
		catch (const sqlite::sqlite_exception& exception)
		{
			Log::GetLog()->error("({} {}) Unexpected DB error {}", __FILE__, __FUNCTION__, exception.what());
			return false;
		}
	}

private:
	sqlite::database db_;
};
