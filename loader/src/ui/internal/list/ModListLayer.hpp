#pragma once

#include <Geode/binding/TextInputDelegate.hpp>
#include <Geode/loader/Index.hpp>
#include "BaseModsList.hpp"

using namespace geode::prelude;

class SearchFilterPopup;
class ModListCell;

enum class ModListType {
    Installed,
    Download,
    Featured,
};


class ModListLayer : public CCLayer, public TextInputDelegate {
protected:
    GJListLayer* m_list = nullptr;
    CCClippingNode* m_tabsGradientNode = nullptr;
    CCSprite* m_tabsGradientSprite = nullptr;
    CCSprite* m_tabsGradientStencil = nullptr;
    CCLabelBMFont* m_listLabel;
    CCLabelBMFont* m_indexUpdateLabel;
    CCMenu* m_menu;
    CCMenu* m_topMenu;
    CCMenuItemToggler* m_installedTabBtn;
    CCMenuItemToggler* m_downloadTabBtn;
    CCMenuItemToggler* m_featuredTabBtn;
    CCMenuItemSpriteExtra* m_searchBtn;
    CCMenuItemSpriteExtra* m_searchClearBtn;
    CCMenuItemSpriteExtra* m_checkForUpdatesBtn = nullptr;
    CCNode* m_searchBG = nullptr;
    CCTextInputNode* m_searchInput = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_filterBtn;
    ModListQuery m_query;
    ModListDisplay m_display = ModListDisplay::Concise;
    EventListener<IndexUpdateFilter> m_indexListener;
    CCMenuItemSpriteExtra* m_leftArrow;
    CCMenuItemSpriteExtra* m_rightArrow;
    BaseModsList* m_modList = nullptr;
    int m_page = 0;

    virtual ~ModListLayer();

    void onPageLeft(CCObject* sender);
    void onPageRight(CCObject* sender);
    void updateArrowState();

    bool init() override;

    void onExit(CCObject*);
    void onReload(CCObject*);
    void onCheckForUpdates(CCObject*);
    void onOpenFolder(CCObject*);
    void onResetSearch(CCObject*);
    void onExpand(CCObject*);
    void onTab(CCObject*);
    void onFilters(CCObject*);
    void keyDown(enumKeyCodes) override;
    void textChanged(CCTextInputNode*) override;
    void createSearchControl();
    void onIndexUpdate(IndexUpdateEvent* event);

    // most requested feature of all time
    void keyBackClicked() override;

    CCArray* createModCells(ModListType type, ModListQuery const& query);
    CCSize getCellSize() const;
    CCSize getListSize() const;

public:
    static ModListLayer* create();
    static ModListLayer* scene();
    void updateAllStates();

    ModListDisplay getDisplay() const;
    ModListQuery& getQuery();

    void reloadList(bool keepScroll = true, std::optional<ModListQuery> const& query = std::nullopt);
    void updateList(CCArray* items, bool keepScroll = true);
};
