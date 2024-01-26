#include "BaseModsList.hpp"
#include "ModListCell.hpp"

using namespace geode::prelude;

BaseModsList* BaseModsList::create(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    auto ret = new BaseModsList;
    if (ret && ret->init(layer, size, display)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BaseModsList::init(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    if (!CCNode::init())
        return false;

    m_layer = layer;
    m_size = size;
    m_display = display;
    this->setContentSize(size);

    m_listLabel = CCLabelBMFont::create("", "bigFont.fnt");

    m_listLabel->setPosition(size / 2);
    m_listLabel->setScale(.6f);
    m_listLabel->setVisible(false);
    m_listLabel->setZOrder(1001);

    this->addChild(m_listLabel);

    return true;
}

CCSize BaseModsList::getCellSize() const {
    return {
        this->getContentSize().width,
        m_display == ModListDisplay::Expanded ? 60.f : 40.f
    };
}

void BaseModsList::updateList(CCArray* items) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto size = this->getContentSize();

    // float scroll = 0.0f;
    // if (m_list) {
    //     scroll = m_list->m_listView->m_tableView->m_contentLayer->getPositionY();
    // }

    // remove old list
    if (m_list) m_list->removeFromParent();

    // create new list
    auto list = ListView::create(
        items,
        this->getCellSize().height,
        size.width,
        size.height
    );
    
    // list->m_tableView->m_contentLayer->setPositionY(scroll);

    // set list
    // TODO: 6th param dont know
    m_list = GJListLayer::create(list, nullptr, { 0, 0, 0, 180 }, size.width, size.height, 0);
    m_list->setZOrder(2);
    m_list->setPosition(winSize / 2 - m_list->getScaledContentSize() / 2);
    this->addChild(m_list);

    // check if the user has searched something,
    // and show visual indicator if so
#if 0
    auto hasQuery = m_searchInput->getString().size() > 0;
    m_searchBtn->setVisible(!hasQuery);
    m_searchClearBtn->setVisible(hasQuery);
#endif

    handleTouchPriority(this);
};

void BaseModsList::updateDisplay(ModListDisplay display) {
    m_display = display;
}

void BaseModsList::updatePage(int page) {
    m_page = page;
}

void BaseModsList::reloadList() {
    this->updateList(CCArray::create());
}

int BaseModsList::itemCount() {
    return 0;
}