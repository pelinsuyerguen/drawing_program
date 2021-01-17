#include "shape_text.h"
#include <utility>


QString shape_text::ID() {
    return QString("Text");
}

std::vector<QPoint> shape_text::get_points() {
    return m_points;
}

void shape_text::set_points(std::vector<QPoint> new_points) {
    m_points = std::move(new_points);
}

void shape_text::set_text(QString neu_inhalt) {
    m_inhalt = std::move(neu_inhalt);
}

QString shape_text::get_text() {
    return m_inhalt;
}
