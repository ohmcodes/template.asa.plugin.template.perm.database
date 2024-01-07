#include "Database/MysqlDB.h"
#include "Database/SqlLiteDB.h"

#include "Plugin.h"

#include <fstream>

#include "Logger/Logger.h"

#include <DBHelper.h>

//#include <Store.h> // example
#include <Permissions.h> //example

#pragma comment(lib, "AsaApi.lib")
#pragma comment(lib, "Permissions.lib")

template <typename T>
T GetText(const std::string& section, const std::string& value)
{
    try
    {
        return Plugin::config[section][value].get<T>();
    }
    catch (const nlohmann::json::exception& error)
    {
        // Check if the exception is due to a missing key
        if (error.id == 302)
        {
            return T{};
        }

        Log::GetLog()->error("GetText() error: {}", error.what());
        throw;
    }
}

template <typename T>
T GetJson(const std::string& section, const std::string& value)
{
    try
    {
        auto jsonValue = Plugin::config[section][value];

        // Check if the value is null
        if (jsonValue.is_null() || jsonValue.empty()) {
            return T{};
        }

        return jsonValue.get<T>();
    }
    catch (const nlohmann::json::exception& error)
    {
        Log::GetLog()->error("GetJson error: {}", error.what());
        throw;
    }
}


// Sample Permission call
// Permissions::IsPlayerInGroup(eos_id, group_name.key().c_str())


void ReadConfig()
{
    const std::string config_path = AsaApi::Tools::GetCurrentDir() + "/ArkApi/Plugins/" + PROJECT_NAME + "/config.json";
    std::ifstream file{ config_path };
    if (!file.is_open())
    {
        throw std::runtime_error("Can't open config.json");
    }

    file >> Plugin::config;

    file.close();
}

void ReloadConfig(APlayerController* player_controller, FString* /*unused*/, bool /*unused*/)
{
    auto* shooter_controller = static_cast<AShooterPlayerController*>(player_controller);

    try
    {
        ReadConfig();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());
        AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Red, "Failed to reload config Error: {}", error.what());
        return;
    }

    AsaApi::GetApiUtils().SendServerMessage(shooter_controller, FColorList::Green, "Reloaded config");
}

void ReloadConfigRcon(RCONClientConnection* rcon_connection, RCONPacket* rcon_packet, UWorld* /*unused*/)
{
    FString reply;

    try
    {
        ReadConfig();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error(error.what());
        reply = error.what();
        rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
        return;
    }

    reply = "Reloaded config";
    rcon_connection->SendMessageW(rcon_packet->Id, 0, &reply);
}

void AddRemoveCommands(bool addCmd = true)
{
    const FString console_reload = GetText<std::string>("Commands", "ConsoleReload").c_str();

    const FString rcon_reload = GetText<std::string>("Commands", "ConsoleReload").c_str();

    // Add command if true
    if (addCmd)
    {
        AsaApi::GetCommands().AddConsoleCommand(console_reload, &ReloadConfig);
        AsaApi::GetCommands().AddRconCommand(rcon_reload, &ReloadConfigRcon);
    }
    else
    {
        AsaApi::GetCommands().RemoveConsoleCommand(console_reload);
        AsaApi::GetCommands().RemoveRconCommand(rcon_reload);
    }
}

void LoadDatabase()
{
    try
    {
        const auto& mysql_conf = Plugin::config["Mysql"];

        const bool use_mysql = mysql_conf["UseMysql"];
        if (use_mysql)
        {
            Plugin::database = std::make_unique<MySql>(
                mysql_conf.value("MysqlHost", ""),
                mysql_conf.value("MysqlUser", ""),
                mysql_conf.value("MysqlPass", ""),
                mysql_conf.value("MysqlDB", ""),
                mysql_conf.value("MysqlTable", PROJECT_NAME),
                mysql_conf.value("MysqlPort", 3306));
        }
        else
        {
            const std::string db_path = Plugin::config["Mysql"]["DbPathOverride"];


            Plugin::database = std::make_unique<SqlLite>(db_path);
        }
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("Database error {}", error.what());
        throw;
    }
}

void LoadConfig()
{
    try
    {
        ReadConfig();
    }
    catch (const std::exception& error)
    {
        Log::GetLog()->error("LoadConfig error: {}", error.what());
        throw;
    }
}


// Called when the server is ready, do post-"server ready" initialization here
void OnServerReady()
{
    Log::GetLog()->info("Hello, world!");

    LoadConfig();
    LoadDatabase();
    AddRemoveCommands();
}

// Hook that triggers once when the server is ready
DECLARE_HOOK(AShooterGameMode_BeginPlay, void, AShooterGameMode*);
void Hook_AShooterGameMode_BeginPlay(AShooterGameMode* _this)
{
    AShooterGameMode_BeginPlay_original(_this);

    OnServerReady();
}

// Called by AsaApi when the plugin is loaded, do pre-"server ready" initialization here
extern "C" __declspec(dllexport) void Plugin_Init()
{
    Log::Get().Init(PROJECT_NAME);

    AsaApi::GetHooks().SetHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay,
        &AShooterGameMode_BeginPlay_original);

    if (AsaApi::GetApiUtils().GetStatus() == AsaApi::ServerStatus::Ready)
        OnServerReady();
}

// Called by AsaApi when the plugin is unloaded, do cleanup here
extern "C" __declspec(dllexport) void Plugin_Unload()
{
    AsaApi::GetHooks().DisableHook("AShooterGameMode.BeginPlay()", Hook_AShooterGameMode_BeginPlay);
}