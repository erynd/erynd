#pragma once

#include <Geode/Geode.hpp>
#include "BaseModsList.hpp"

using namespace geode::prelude;

class InstalledModsList : public BaseModsList {
public:

    static InstalledModsList* create(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    bool init(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    CCArray* createModCells();

    void reloadList() override;
    void updateList(CCArray* items) override;
    void updateDisplay(ModListDisplay display) override;
    void updatePage(int page) override;
};