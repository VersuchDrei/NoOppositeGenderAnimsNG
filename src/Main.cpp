#include <stddef.h>

using namespace RE::BSScript;
using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

namespace {
    void InitializeLogging() {
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    }

    void MessageHandler(SKSE::MessagingInterface::Message* a_msg) {
        switch (a_msg->type) {
            case SKSE::MessagingInterface::kDataLoaded: {
                RE::TESDataHandler* handler = RE::TESDataHandler::GetSingleton();
                auto& npcs = handler->GetFormArray<RE::TESNPC>();
                for (RE::TESNPC* npc : npcs) {
                    npc->actorData.actorBaseFlags.reset(RE::ACTOR_BASE_DATA::Flag::kOppositeGenderanims);
                }

                RE::TESRace* NordRace = handler->LookupForm<RE::TESRace>(0x013746, "Skyrim.esm");

                RE::TESRace* OrcRace = handler->LookupForm<RE::TESRace>(0x013747, "Skyrim.esm");
                OrcRace->behaviorGraphs[RE::SEXES::kFemale] = NordRace->behaviorGraphs[RE::SEXES::kFemale];
                OrcRace->rootBehaviorGraphNames[RE::SEXES::kFemale] = NordRace->rootBehaviorGraphNames[RE::SEXES::kFemale];
                OrcRace->behaviorGraphProjectNames[RE::SEXES::kFemale] = NordRace->behaviorGraphProjectNames[RE::SEXES::kFemale];

                RE::TESRace* OrcRaceVampire = handler->LookupForm<RE::TESRace>(0x0A82B9, "Skyrim.esm");
                OrcRaceVampire->behaviorGraphs[RE::SEXES::kFemale] = NordRace->behaviorGraphs[RE::SEXES::kFemale];
                OrcRaceVampire->rootBehaviorGraphNames[RE::SEXES::kFemale] = NordRace->rootBehaviorGraphNames[RE::SEXES::kFemale];
                OrcRaceVampire->behaviorGraphProjectNames[RE::SEXES::kFemale] = NordRace->behaviorGraphProjectNames[RE::SEXES::kFemale];
            }
        }
    }
}

SKSEPluginLoad(const LoadInterface* skse) {
    InitializeLogging();

    auto* plugin = PluginDeclaration::GetSingleton();
    auto version = plugin->GetVersion();
    log::info("{} {} is loading...", plugin->GetName(), version);

    Init(skse);

    auto message = SKSE::GetMessagingInterface();
    if (!message->RegisterListener(MessageHandler)) {
        return false;
    }

    log::info("{} has finished loading.", plugin->GetName());
    return true;
}
