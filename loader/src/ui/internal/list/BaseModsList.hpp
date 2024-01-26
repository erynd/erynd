#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;


enum class ModListDisplay {
    Concise,
    Expanded,
};


struct ModListQuery {
    /**
     * Keywords; matches name, id, description, details, developer
     */
    std::optional<std::string> keywords;
    /**
     * Force already installed mods to be shown on the list unless they explicitly mismatch some
     * tags (used to show installed mods on index)
     */
    bool forceVisibility;
    /**
     * Force not installable mods to be shown on the list unless they explicitly mismatch some
     * tags (used to show installed mods on index)
     */
    bool forceInvalid;
    /**
     * Empty means current platform
     */
    std::unordered_set<PlatformID> platforms = { GEODE_PLATFORM_TARGET };
    std::unordered_set<std::string> tags;
};

class ModListLayer;

class BaseModsList : public CCNode {
public:
    ModListLayer* m_layer = nullptr;
    CCSize m_size;
    ModListDisplay m_display;
    ModListQuery m_query;
    int m_page = 0;
    int m_pageLimit = 20;

    GJListLayer* m_list = nullptr;
    CCLabelBMFont* m_listLabel = nullptr;

    static BaseModsList* create(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    bool init(ModListLayer* layer, CCSize const& size, ModListDisplay display);

    virtual int itemCount();
    virtual void reloadList();
    virtual void updateList(CCArray* items);
    virtual void updateDisplay(ModListDisplay display);
    virtual void updatePage(int page);
    CCSize getCellSize() const;
    
};