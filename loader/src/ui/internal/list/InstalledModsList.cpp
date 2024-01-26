#include "InstalledModsList.hpp"
#include "ModListCell.hpp"

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <Geode/external/fts/fts_fuzzy_match.h>

using namespace geode::prelude;

static std::optional<int> fuzzyMatch(std::string const& kw, std::string const& str) {
    int score;
    if (fts::fuzzy_match(kw.c_str(), str.c_str(), score)) {
        return score;
    }
    return std::nullopt;
}

static std::optional<int> queryMatchKeywords(
    ModListQuery const& query,
    ModMetadata const& metadata
) {
    double weighted = 0;

    // fuzzy match keywords
    if (query.keywords) {
        bool someMatched = false;
        auto weightedMatch = [&](std::string const& str, double weight) {
            if (auto match = fuzzyMatch(query.keywords.value(), str)) {
                weighted = std::max<double>(match.value() * weight, weighted);
                someMatched = true;
            }
        };
        weightedMatch(metadata.getName(), 2);
        weightedMatch(metadata.getID(), 1);
        weightedMatch(metadata.getDeveloper(), 0.5);
        weightedMatch(metadata.getDetails().value_or(""), 0.05);
        weightedMatch(metadata.getDescription().value_or(""), 0.2);
        if (!someMatched) {
            return std::nullopt;
        }
    }
    else {
        if (metadata.getID() == "geode.loader") {
            return 900;
        }
        // this is like the dumbest way you could possibly sort alphabetically 
        // but it does enough to make the mods list somewhat alphabetically 
        // sorted, at least enough so that if you're scrolling it based on 
        // alphabetical order you will find the part you're looking for easily 
        // so it's fine
        return static_cast<int>(-tolower(metadata.getName()[0]));
    }

    // if the weight is relatively small we can ignore it
    if (weighted < 2) {
        return std::nullopt;
    }

    // empty keywords always match
    return static_cast<int>(weighted);
}

static std::optional<int> queryMatch(ModListQuery const& query, Mod* mod) {
    // Only checking keywords makes sense for mods since their 
    // platform always matches, they are always visible and they don't 
    // currently list their tags
    return queryMatchKeywords(query, mod->getMetadata());
}

static std::optional<int> queryMatch(ModListQuery const& query, IndexItemHandle item) {
    // if no force visibility was provided and item is already installed, don't show it
    if (!query.forceVisibility && Loader::get()->isModInstalled(item->getMetadata().getID())) {
        return std::nullopt;
    }
    // make sure all tags match
    for (auto& tag : query.tags) {
        if (!item->getTags().count(tag)) {
            return std::nullopt;
        }
    }
    // make sure at least some platform matches
    if (!ranges::contains(query.platforms, [item](PlatformID id) {
        return item->getAvailablePlatforms().count(id);
    })) {
        return std::nullopt;
    }
    // if no force visibility was provided and item is already installed, don't show it
    auto canInstall = Index::get()->canInstall(item);
    if (!query.forceInvalid && !canInstall) {
        // log::warn(
        //     "Removing {} from the list because it cannot be installed: {}",
        //     item->getMetadata().getID(),
        //     canInstall.unwrapErr()
        // );
        return std::nullopt;
    }
    // otherwise match keywords
    if (auto match = queryMatchKeywords(query, item->getMetadata())) {
        auto weighted = match.value();
        // add extra weight on tag matches
        if (query.keywords) {
            if (auto match = fuzzyMatch(query.keywords.value(), ranges::join(item->getTags(), " "))) {
                weighted += match.value() * 1.4;                    
            }
        }
        // add extra weight to featured items to keep power consolidated in the 
        // hands of the rich Geode bourgeoisie
        // the number 420 is a reference to the number one bourgeois of modern 
        // society, elon musk
        weighted += item->isFeatured() ? 420 : 0;
        return static_cast<int>(weighted);
    }
    // keywords must match bruh
    else {
        return std::nullopt;
    }
}

static std::optional<int> queryMatch(ModListQuery const& query, InvalidGeodeFile const& info) {
    // if any explicit filters were provided, no match
    if (!query.tags.empty() || query.keywords.has_value()) {
        return std::nullopt;
    }
    return 0;
}

InstalledModsList* InstalledModsList::create(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    auto* pRet = new InstalledModsList();
    if (pRet && pRet->init(layer, size, display)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool InstalledModsList::init(ModListLayer* layer, CCSize const& size, ModListDisplay display) {
    if (!BaseModsList::init(layer, size, display))
        return false;

    // create the list
    this->updateList(this->createModCells());

    return true;
}

CCArray* InstalledModsList::createModCells() {
    std::vector<ModListCell*> cells;

    // problems first
    if (!Loader::get()->getProblems().empty()) {
        cells.push_back(ProblemsCell::create(m_layer, m_display, this->getCellSize()));
    }

    // sort the mods by match score
    std::multimap<int, ModListCell*> sorted;

    // then other mods

#if 0
    // newly installed
    for (auto const& item : Index::get()->getItems()) {
        if (!item->isInstalled() ||
            Loader::get()->isModInstalled(item->getMetadata().getID()) ||
            Loader::get()->isModLoaded(item->getMetadata().getID()))
            continue;
        // match the same as other installed mods
        if (auto match = queryMatchKeywords(m_query, item->getMetadata())) {
            auto cell = IndexItemCell::create(item, m_layer, m_display, this->getCellSize());
            sorted.insert({ match.value(), cell });
        }
    }
#endif

    // loaded
    for (auto const& mod : Loader::get()->getAllMods()) {
        if (auto match = queryMatch(m_query, mod)) {
            auto cell = ModCell::create(mod, m_layer, m_display, this->getCellSize());
            sorted.insert({ match.value(), cell });
        }
    }

    

    // add the mods sorted
    for (auto& [score, cell] : ranges::reverse(sorted)) {
        cells.push_back(cell);
    }

    CCArray* mods = CCArray::create();

    for (int i = m_page * m_pageLimit; i < cells.size() && i < (m_page + 1) * m_pageLimit; i++) {
        mods->addObject(cells[i]);
    }

    return mods;
}

int InstalledModsList::itemCount() {
    int count = 0;
    if (!Loader::get()->getProblems().empty()) {
        count++;
    }
    for (auto const& mod : Loader::get()->getAllMods()) {
        if (auto match = queryMatch(m_query, mod)) {
            count++;
        }
    }
    return count;
}

void InstalledModsList::updateList(CCArray* items) {
    BaseModsList::updateList(items);

    // set list status
    if (!items->count()) {
        m_listLabel->setVisible(true);
        m_listLabel->setString("No mods found");
    } else {
        m_listLabel->setVisible(false);
    }

    // please forgive me for this code
    if (m_list->m_listView->m_entries->count() > 0) {
        auto problemsCell = typeinfo_cast<ProblemsCell*>(m_list->m_listView->m_entries->objectAtIndex(0));
        if (problemsCell) {
            auto cellView =
                typeinfo_cast<TableViewCell*>(m_list->m_listView->m_tableView->m_cellArray->objectAtIndex(0));
            if (cellView && problemsCell->getColor()) {
                cellView->m_backgroundLayer->setColor(*problemsCell->getColor());
            }
        }
    }
}

void InstalledModsList::updateDisplay(ModListDisplay display) {
    BaseModsList::updateDisplay(display);
    // update the list
    this->updateList(this->createModCells());
}

void InstalledModsList::updatePage(int page) {
    BaseModsList::updatePage(page);
    // update the list
    this->updateList(this->createModCells());
}

void InstalledModsList::reloadList() {
    this->updateList(this->createModCells());
}