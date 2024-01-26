#include "IndexModsList.hpp"
#include "ModListCell.hpp"

using namespace geode::prelude;

IndexModsList* IndexModsList::create(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    auto pRet = new IndexModsList();
    if (pRet && pRet->init(layer, size, display)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

IndexModsList::~IndexModsList() {
    // todo: invalidate the request here or smth
}

bool IndexModsList::init(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    if (!BaseModsList::init(layer, size, display))
        return false;
    
    this->updateItems();

    return true;
}

void IndexModsList::addLoadingCircle() {
    auto size = this->getContentSize();
    if (!m_loadingCircle) {
        m_loadingCircle = LoadingCircle::create();
        m_loadingCircle->setPosition(size / 2);
        m_loadingCircle->setVisible(false);
        m_loadingCircle->setZOrder(1001);
        
        this->addChild(m_loadingCircle);
    }
}

void IndexModsList::removeLoadingCircle() {
    if (m_loadingCircle) {
        m_loadingCircle->removeFromParent();
        m_loadingCircle = nullptr;
    }
}

void IndexModsList::updateDisplay(ModListDisplay display) {
    BaseModsList::updateDisplay(display);
    this->handleItems(m_items);
}

void IndexModsList::updatePage(int page) {
    BaseModsList::updatePage(page);
    this->updateItems();
}

void IndexModsList::updateList(CCArray* items) {
    BaseModsList::updateList(items);
}

void IndexModsList::handleError(std::string const& error) {
    this->removeLoadingCircle();
    m_listLabel->setString(error.c_str());
    m_listLabel->setVisible(true);
}

void IndexModsList::updateItems() {
    this->addLoadingCircle();

    Index2::get().getPageItems(m_page, IndexQuery2(), 
        [this](std::vector<IndexItem2> const& items) {
        this->handleItems(items);
    }, [this](std::string const& error) {
        this->handleError(error);
    });
}

void IndexModsList::handleItems(std::vector<IndexItem2> const& items) {
    this->removeLoadingCircle();
    m_items = items;

    auto mods = CCArray::create();
    for (auto& item : items) {
        mods->addObject(IndexItemCell::create(
            item, m_layer, m_display, this->getCellSize()
        ));
    }
    this->updateList(mods);
}

void IndexModsList::reloadList() {
    this->updateItems();
}