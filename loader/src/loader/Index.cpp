#include "Index.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <loader/LoaderImpl.hpp>

using namespace geode::prelude;

// yes this is a macro.. so we can take advantage of string literal concat
#define GEODE_INDEX_URL "https://api.geode-sdk.org/v1"

std::string urlEncode(std::string const& input) {
    std::ostringstream ss;
    ss << std::hex << std::uppercase;
    for (char c : input) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            ss << c;
        } else {
            ss << '%' << static_cast<int>(c);
        }
    }
    return ss.str();
}

std::string buildQueryString(std::initializer_list<std::pair<std::string, std::string>> const& values) {
    std::string query;
    for (auto const& [key, value] : values) {
        query += key + "=" + urlEncode(value) + "&";
    }
    return query;
}

std::shared_ptr<IndexPromise<std::vector<IndexItem> const&>> Index::searchMods(int page, IndexQuery const& query) {
    return IndexPromise<std::vector<IndexItem> const&>::create([=, this](auto resolve, auto reject) {
        web::AsyncWebRequest()
            .userAgent("Geode Loader")
            .get(GEODE_INDEX_URL "/mods?" + buildQueryString({
                { "gd", LoaderImpl::get()->getGameVersion() },
                { "page", std::to_string(page + 1) },
                { "per_page", std::to_string(m_pageLimit) },
                { "search", query.m_search },
            }))
            .json()
            .then([=](matjson::Value const& json) {
                std::vector<IndexItem> items;
                for (auto const& entry : json["payload"]["data"].as_array()) {
                    IndexItem item;
                    auto const& latestVer = entry["versions"][0];
                    item.m_modId = entry["id"].as_string();
                    item.m_version = VersionInfo::parse(entry["latest_version"].as_string()).unwrap();
                    item.m_downloadUrl = latestVer["download_link"].as_string();
                    item.m_name = latestVer["name"].as_string();
                    item.m_description = latestVer["description"].as_string();
                    item.m_developer = "Lol api doesnt have this";
                    item.m_isAPI = latestVer["api"].as_bool();
                    items.push_back(item);
                }
                resolve(std::move(items));
            })
            .expect([=](std::string const& msg) {
                try {
                    log::error("Index error {}", matjson::parse(msg).dump());
                } catch (...) {
                    log::error("Index error {}", msg);
                }
                reject(msg);
            });

    });
}

std::shared_ptr<IndexPromise<DetailedIndexItem const&>> Index::fetchModInfo(std::string const& modId) {
    return IndexPromise<DetailedIndexItem const&>::create([=](auto resolve, auto reject) {
        web::AsyncWebRequest()
            .userAgent("Geode Loader")
            .get(GEODE_INDEX_URL "/mods/" + modId)
            .json()
            .then([=](matjson::Value const& json) {
                DetailedIndexItem item;
                auto const data = json["payload"];
                auto const& latestVer = data["versions"][0];
                item.m_modId = data["id"].as_string();
                item.m_version = VersionInfo::parse(latestVer["version"].as_string()).unwrap();
                item.m_downloadUrl = latestVer["download_link"].as_string();
                item.m_name = latestVer["name"].as_string();
                item.m_description = latestVer["description"].as_string();
                item.m_developer = "Lol api doesnt have this";
                item.m_isAPI = latestVer["api"].as_bool();
                item.m_about = data["about"].is_string() ? std::make_optional(data["about"].as_string()) : std::nullopt;
                item.m_changelog = data["changelog"].is_string() ? std::make_optional(data["changelog"].as_string()) : std::nullopt;
                resolve(std::move(item));
            })
            .expect([=](std::string const& msg) {
                try {
                    log::error("Index error {}", matjson::parse(msg).dump());
                } catch (...) {
                    log::error("Index error {}", msg);
                }
                reject(msg);
            });
    });
}

ModInstallEvent::ModInstallEvent(
    std::string const& id, const UpdateStatus status
) : modID(id), status(status) {}

ListenerResult ModInstallFilter::handle(utils::MiniFunction<Callback> fn, ModInstallEvent* event) {
    if (m_id == event->modID) {
        fn(event);
    }
    return ListenerResult::Propagate;
}

ModInstallFilter::ModInstallFilter(std::string const& id) : m_id(id) {}

std::string IndexItem::getDownloadURL() const {
    return {};
}
std::string IndexItem::getPackageHash() const {
    return {};
}
std::unordered_set<PlatformID> IndexItem::getAvailablePlatforms() const {
    return {};
}
bool IndexItem::isFeatured() const {
    return {};
}
std::unordered_set<std::string> IndexItem::getTags() const {
    return {};
}
bool IndexItem::isInstalled() const {
    return {};
}