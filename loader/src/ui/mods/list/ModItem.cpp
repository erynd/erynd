#include "ModItem.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/utils/ColorProvider.hpp>
#include "../GeodeStyle.hpp"
#include "../popups/ModPopup.hpp"

bool ModItem::init(ModSource&& source) {
    if (!CCNode::init())
        return false;
    
    m_source = std::move(source);

    m_bg = CCScale9Sprite::create("square02b_small.png");
    m_bg->setOpacity(0);
    m_bg->ignoreAnchorPointForPosition(false);
    m_bg->setAnchorPoint({ .5f, .5f });
    m_bg->setScale(.7f);
    this->addChild(m_bg);

    m_logo = m_source.createModLogo();
    this->addChild(m_logo);

    m_infoContainer = CCNode::create();
    m_infoContainer->setScale(.4f);
    m_infoContainer->setAnchorPoint({ .0f, .5f });
    m_infoContainer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisLineAlignment(AxisAlignment::Start)
            ->setGap(0)
    );
    m_infoContainer->getLayout()->ignoreInvisibleChildren(true);

    m_titleContainer = CCNode::create();
    m_titleContainer->setAnchorPoint({ .0f, .5f });
    m_titleContainer->setLayout(
        RowLayout::create()
            ->setDefaultScaleLimits(.1f, 1.f)
            ->setAxisAlignment(AxisAlignment::Start)
    );

    m_titleLabel = CCLabelBMFont::create(m_source.getMetadata().getName().c_str(), "bigFont.fnt");
    m_titleContainer->addChild(m_titleLabel);

    m_versionLabel = CCLabelBMFont::create("", "bigFont.fnt");
    m_versionLabel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(.7f));
    m_titleContainer->addChild(m_versionLabel);
    
    m_infoContainer->addChild(m_titleContainer);
    
    m_developers = CCMenu::create();
    m_developers->ignoreAnchorPointForPosition(false);
    m_developers->setAnchorPoint({ .0f, .5f });

    auto by = "By " + ModMetadata::formatDeveloperDisplayString(m_source.getMetadata().getDevelopers());
    m_developerLabel = CCLabelBMFont::create(by.c_str(), "goldFont.fnt");
    auto developersBtn = CCMenuItemSpriteExtra::create(
        m_developerLabel, this, nullptr
    );
    m_developers->addChild(developersBtn);

    m_developers->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
    );
    m_infoContainer->addChild(m_developers);

    m_restartRequiredLabel = createGeodeTagLabel(
        "Restart Required",
        {{
            to3B(ColorProvider::get()->color("mod-list-restart-required-label"_spr)),
            to3B(ColorProvider::get()->color("mod-list-restart-required-label-bg"_spr))
        }}
    );
    m_restartRequiredLabel->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(.75f));
    m_infoContainer->addChild(m_restartRequiredLabel);

    m_downloadBarContainer = CCNode::create();
    m_downloadBarContainer->setContentSize({ 225, 30 });
    
    m_downloadBar = Slider::create(nullptr, nullptr);
    m_downloadBar->m_touchLogic->m_thumb->setVisible(false);
    m_downloadBar->setLayoutOptions(AxisLayoutOptions::create()->setMaxScale(.35f));
    m_downloadBarContainer->addChildAtPosition(m_downloadBar, Anchor::Center, ccp(0, 0), ccp(0, 0));

    m_infoContainer->addChild(m_downloadBarContainer);

    m_downloadWaiting = CCNode::create();
    m_downloadWaiting->setContentSize({ 225, 30 });
    
    auto downloadWaitingLabel = CCLabelBMFont::create("Preparing Download...", "bigFont.fnt");
    downloadWaitingLabel->setScale(.75f);
    m_downloadWaiting->addChildAtPosition(
        downloadWaitingLabel, Anchor::Left,
        ccp(m_downloadWaiting->getContentHeight(), 0), ccp(0, .5f)
    );
    
    auto downloadWaitingSpinner = createLoadingCircle(20);
    m_downloadWaiting->addChildAtPosition(
        downloadWaitingSpinner, Anchor::Left,
        ccp(m_downloadWaiting->getContentHeight() / 2, 0)
    );

    m_infoContainer->addChild(m_downloadWaiting);

    this->addChild(m_infoContainer);

    m_viewMenu = CCMenu::create();
    m_viewMenu->setAnchorPoint({ 1.f, .5f });
    m_viewMenu->setScale(.55f);
    
    auto viewBtn = CCMenuItemSpriteExtra::create(
        createGeodeButton("View"),
        this, menu_selector(ModItem::onView)
    );
    m_viewMenu->addChild(viewBtn);

    m_viewMenu->setLayout(
        RowLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setGap(10)
    );
    m_viewMenu->getLayout()->ignoreInvisibleChildren(true);
    this->addChildAtPosition(m_viewMenu, Anchor::Right, ccp(-10, 0));

    // Handle source-specific stuff
    m_source.visit(makeVisitor {
        [this](Mod* mod) {
            // Add an enable button if the mod is enablable
            if (!mod->isInternal()) {
                m_enableToggle = CCMenuItemToggler::createWithStandardSprites(
                    this, menu_selector(ModItem::onEnable), 1.f
                );
                // Manually handle toggle state
                m_enableToggle->m_notClickable = true;
                m_viewMenu->addChild(m_enableToggle);
                m_viewMenu->updateLayout();
            }
            if (mod->hasProblems()) {
                auto viewErrorSpr = CircleButtonSprite::createWithSpriteFrameName(
                    "exclamation.png"_spr, 1.f,
                    CircleBaseColor::DarkPurple, CircleBaseSize::Small
                );
                auto viewErrorBtn = CCMenuItemSpriteExtra::create(
                    viewErrorSpr, this, menu_selector(ModItem::onViewError)
                );
                m_viewMenu->addChild(viewErrorBtn);
            }
        },
        [this](server::ServerModMetadata const& metadata) {
            if (metadata.featured) {
                auto starBG = CCScale9Sprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
                starBG->setContentSize({ 50, 38 });
                starBG->setColor(to3B(ColorProvider::get()->color("mod-list-featured-color"_spr)));
                starBG->setOpacity(45);

                auto star = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
                starBG->addChildAtPosition(star, Anchor::Center);

                m_titleContainer->addChild(starBG);
            }
        },
        [this](ModSuggestion const& suggestion) {
            m_recommendedBy = CCNode::create();
            m_recommendedBy->setContentWidth(225);

            auto byLabel = CCLabelBMFont::create("Recommended by ", "bigFont.fnt");
            byLabel->setColor("mod-list-recommended-by"_cc3b);
            m_recommendedBy->addChild(byLabel);

            auto nameLabel = CCLabelBMFont::create(suggestion.forMod->getName().c_str(), "bigFont.fnt");
            nameLabel->setColor("mod-list-recommended-by-2"_cc3b);
            m_recommendedBy->addChild(nameLabel);

            m_recommendedBy->setLayout(
                RowLayout::create()
                    ->setDefaultScaleLimits(.1f, 1.f)
                    ->setAxisAlignment(AxisAlignment::Start)
            );
            m_infoContainer->addChild(m_recommendedBy);
        },
    });

    auto updateSpr = CircleButtonSprite::createWithSpriteFrameName(
        "update.png"_spr, 1.15f, CircleBaseColor::DarkAqua
    );
    m_updateBtn = CCMenuItemSpriteExtra::create(
        updateSpr, this, menu_selector(ModItem::onInstall)
    );
    m_viewMenu->addChild(m_updateBtn);

    if (m_source.asMod()) {
        m_checkUpdateListener.bind(this, &ModItem::onCheckUpdates);
        m_checkUpdateListener.setFilter(m_source.checkUpdates());
    }

    this->updateState();

    // Only listen for updates on this mod specifically
    m_updateStateListener.bind([this](auto) { this->updateState(); });
    m_updateStateListener.setFilter(UpdateModListStateFilter(UpdateModState(m_source.getID())));

    m_downloadListener.bind([this](auto) { this->updateState(); });
    m_downloadListener.setFilter(server::ModDownloadFilter(m_source.getID()));

    return true;
}

void ModItem::updateState() {
    auto wantsRestart = m_source.wantsRestart();

    auto download = server::ModDownloadManager::get()->getDownload(m_source.getID());

    // If there is an active download ongoing, show that in place of developer name 
    if (download && download->isActive()) {
        m_updateBtn->setVisible(false);
        m_restartRequiredLabel->setVisible(false);
        m_developers->setVisible(false);

        auto status = download->getStatus();
        if (auto prog = std::get_if<server::DownloadStatusDownloading>(&status)) {
            m_downloadWaiting->setVisible(false);
            m_downloadBarContainer->setVisible(true);
            m_downloadBar->setValue(prog->percentage / 100.f);
            m_downloadBar->updateBar();
        }
        else {
            m_downloadBarContainer->setVisible(false);
            m_downloadWaiting->setVisible(true);
            // Make sure the spinner is spinning by ticking its setVisible
            m_downloadWaiting->getChildByID("loading-spinner")->setVisible(true);
        }
    }
    // Otherwise show "Restart Required" button if needed in place of dev name
    else {
        m_restartRequiredLabel->setVisible(wantsRestart);
        m_developers->setVisible(!wantsRestart);
        m_downloadBarContainer->setVisible(false);
        m_downloadWaiting->setVisible(false);
    }
    m_infoContainer->updateLayout();

    // Set default colors based on source to start off with 
    // (possibly overriding later based on state)
    m_source.visit(makeVisitor {
        [this](Mod* mod) {
            m_bg->setColor(ccWHITE);
            m_bg->setOpacity(mod->isOrWillBeEnabled() ? 25 : 10);
            m_titleLabel->setOpacity(mod->isOrWillBeEnabled() ? 255 : 155);
            m_versionLabel->setOpacity(mod->isOrWillBeEnabled() ? 255 : 155);
            m_developerLabel->setOpacity(mod->isOrWillBeEnabled() ? 255 : 155);
        },
        [this](server::ServerModMetadata const& metadata) {
            m_bg->setColor(ccWHITE);
            m_bg->setOpacity(25);
            if (metadata.featured) {
                m_bg->setColor("mod-list-featured-color"_cc3b);
                m_bg->setOpacity(40);
            }
        },
        [this](ModSuggestion const& suggestion) {
            m_bg->setColor("mod-list-recommended-bg"_cc3b);
            m_bg->setOpacity(25);
        }
    });

    if (
        auto update = m_source.hasUpdates();
        update && !(download && (download->isActive() || download->isDone()))
    ) {
        m_updateBtn->setVisible(true);
        auto updateString = m_source.getMetadata().getVersion().toString() + " -> " + update->version.toString();
        m_versionLabel->setString(updateString.c_str());
        m_versionLabel->setColor(to3B(ColorProvider::get()->color("mod-list-version-label-updates-available"_spr)));

        m_bg->setColor(to3B(ColorProvider::get()->color("mod-list-version-label-updates-available"_spr)));
        m_bg->setOpacity(40);
    }
    else {
        m_updateBtn->setVisible(false);
        m_versionLabel->setString(m_source.getMetadata().getVersion().toString().c_str());
        m_versionLabel->setColor(to3B(ColorProvider::get()->color("mod-list-version-label"_spr)));
    }
    m_viewMenu->updateLayout();
    m_titleContainer->updateLayout();

    // If there were problems, tint the BG red
    if (m_source.asMod() && m_source.asMod()->hasProblems()) {
        m_bg->setColor("mod-list-errors-found"_cc3b);
        m_bg->setOpacity(40);
    }

    // Highlight item via BG if it wants to restart for extra UI attention
    if (wantsRestart) {
        m_bg->setColor(to3B(ColorProvider::get()->color("mod-list-restart-required-label"_spr)));
        m_bg->setOpacity(40);
    }

    // Update enable toggle state
    if (m_enableToggle && m_source.asMod()) {
        m_enableToggle->toggle(m_source.asMod()->isOrWillBeEnabled());

        // Disable the toggle if the mod has been uninstalled
        if (modRequestedActionIsUninstall(m_source.asMod()->getRequestedAction())) {
            m_enableToggle->setEnabled(false);
            auto off = typeinfo_cast<CCRGBAProtocol*>(m_enableToggle->m_offButton->getNormalImage());
            auto on = typeinfo_cast<CCRGBAProtocol*>(m_enableToggle->m_onButton->getNormalImage());
            off->setColor(ccGRAY);
            off->setOpacity(105);
            on->setColor(ccGRAY);
            on->setOpacity(105);
        }
    }
}

void ModItem::updateSize(float width, bool big) {
    this->setContentSize({ width, big ? 40.f : 30.f });

    m_bg->setContentSize((m_obContentSize - ccp(6, 0)) / m_bg->getScale());
    m_bg->setPosition(m_obContentSize / 2);

    auto logoSize = m_obContentSize.height - 10;
    limitNodeSize(m_logo, { logoSize, logoSize }, 999, .1f);
    m_logo->setPosition(m_obContentSize.height / 2 + 5, m_obContentSize.height / 2);

    CCSize titleSpace {
        m_obContentSize.width / 2 - m_obContentSize.height,
        logoSize + 5
    };

    // Divide by scale of info container since that actually determines the size
    // (Since the scale of m_titleContainer and m_developers is managed by its layout)
    m_titleContainer->setContentWidth(titleSpace.width / m_infoContainer->getScale());
    m_titleContainer->updateLayout();

    m_developers->setContentWidth(titleSpace.width / m_infoContainer->getScale());
    m_developers->updateLayout();

    if (m_recommendedBy) {
        m_recommendedBy->setContentWidth(titleSpace.width / m_infoContainer->getScale());
        m_recommendedBy->updateLayout();
    }

    m_infoContainer->setPosition(m_obContentSize.height + 10, m_obContentSize.height / 2);
    m_infoContainer->setContentSize(ccp(titleSpace.width, titleSpace.height) / m_infoContainer->getScale());
    m_infoContainer->updateLayout();

    m_viewMenu->setContentWidth(m_obContentSize.width / m_viewMenu->getScaleX() / 2 - 20);
    m_viewMenu->updateLayout();

    this->updateLayout();
}

void ModItem::onCheckUpdates(typename server::ServerRequest<std::optional<server::ServerModUpdate>>::Event* event) {
    if (event->getValue() && event->getValue()->isOk()) {
        this->updateState();
    }
}

void ModItem::onView(CCObject*) {
    // Always open up the popup for the installed mod page if that is possible
    ModPopup::create(m_source.convertForPopup())->show();
}

void ModItem::onViewError(CCObject*) {
    if (auto mod = m_source.asMod()) {
        std::vector<std::string> problems;
        for (auto problem : mod->getProblems()) {
            problems.push_back(fmt::format("{} (code {})", problem.message, static_cast<int>(problem.type)));
        }
        FLAlertLayer::create(
            fmt::format("Errors with {}", mod->getName()).c_str(),
            ranges::join(problems, "\n"),
            "OK"
        )->show();
    }
}

void ModItem::onEnable(CCObject*) {
    if (auto mod = m_source.asMod()) {
        // Toggle the mod state
        auto res = mod->isOrWillBeEnabled() ? mod->disable() : mod->enable();
        if (!res) {
            FLAlertLayer::create(
                "Error Toggling Mod",
                res.unwrapErr(),
                "OK"
            )->show();
        }
    }

    // Update state of the mod item
    UpdateModListStateEvent(UpdateModState(m_source.getID())).post();
}

void ModItem::onInstall(CCObject*) {
    server::ModDownloadManager::get()->startDownload(m_source.getID(), std::nullopt);
}

ModItem* ModItem::create(ModSource&& source) {
    auto ret = new ModItem();
    if (ret && ret->init(std::move(source))) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}