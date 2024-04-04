#include <Geode/binding/StatsCell.hpp>

#include "list_cell.hpp"

bool JBListCell::init(CCSize const& size) {
    m_width = size.width;
    m_height = size.height;
    this->setContentSize(size);
    this->setID("nong-list-cell");
    return true;
}

void JBListCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}
