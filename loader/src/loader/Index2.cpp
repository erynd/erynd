#include "Index2.hpp"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// yes this is a macro.. so we can take advantage of string literal concat
#define GEODE_INDEX_URL "https://api.geode-sdk.org/v1"

void Index2::getPageItems(int page, IndexQuery2 const& query, MiniFunction<void(std::vector<IndexItem2> const&)> callback, MiniFunction<void(std::string const&)> error) {
    web::AsyncWebRequest()
        .userAgent("Geode Loader")
        .get(GEODE_INDEX_URL "/mods?gd=2.204")
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
            error(msg);
        });
}