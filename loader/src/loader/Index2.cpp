#include "Index2.hpp"
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

void Index2::getPageItems(int page, IndexQuery2 const& query, MiniFunction<void(std::vector<IndexItem2> const&)> callback, MiniFunction<void(std::string const&)> error) {
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
            std::vector<IndexItem2> items;
            for (auto const& entry : json["payload"]["data"].as_array()) {
                IndexItem2 item;
                auto const& latestVer = entry["versions"][0];
                item.m_modId = entry["id"].as_string();
                item.m_version = VersionInfo::parse(entry["latest_version"].as_string()).unwrap();
                item.m_downloadUrl = latestVer["download_link"].as_string();
                item.m_name = latestVer["name"].as_string();
                item.m_description = latestVer["description"].as_string();
                item.m_developer = "Lol api doesnt have this";
                item.m_isAPI = false;
                items.push_back(item);
            }
            callback(std::move(items));
        })
        .expect([=](std::string const& msg) {
            try {
                log::error("Index error {}", matjson::parse(msg).dump());
            } catch (...) {
                log::error("Index error {}", msg);
            }
            error(msg);
        });
}