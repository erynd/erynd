#include "Index2.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

void Index2::getPageItems(int page, IndexQuery2 const& query, MiniFunction<void(std::vector<IndexItem2> const&)> callback, MiniFunction<void(std::string const&)> error) {
    std::thread([=]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        Loader::get()->queueInMainThread([=]() {
            std::vector<IndexItem2> items;
            for (int i = 0; i < m_pageLimit; i++) {
                IndexItem2 item;
                item.m_modId = "mod" + std::to_string(i + page * m_pageLimit);
                item.m_version = VersionInfo(1, 0, 0);
                item.m_downloadUrl = "https://example.com/mod" + std::to_string(i + page * m_pageLimit) + ".zip";
                item.m_name = "Mod " + std::to_string(i + page * m_pageLimit);
                item.m_description = "This is a mod";
                item.m_developer = "Geode";
                item.m_isAPI = false;
                items.push_back(item);
            }
            callback(std::move(items));
        });
    }).detach();
}