#pragma once

#include <Geode/Geode.hpp>
#include "BaseModsList.hpp"
#include "../../../loader/Index2.hpp"

using namespace geode::prelude;

class IndexModsList : public BaseModsList {
public:
    std::vector<IndexItem2> m_items;
    int m_itemCount = 0;
    LoadingCircle* m_loadingCircle = nullptr;
    
    ~IndexModsList();

    static IndexModsList* create(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    bool init(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    void addLoadingCircle();
    void removeLoadingCircle();

    void handleItems(std::vector<IndexItem2> const& items);
    void handleError(std::string const& error);
    void updateItems();

    int itemCount() override;
    void reloadList() override;
    void updateList(CCArray* items) override;
    void updateDisplay(ModListDisplay display) override;
    void updatePage(int page) override;
};