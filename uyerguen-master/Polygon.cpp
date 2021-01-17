#include "Polygon.h"
#include <iostream>

QString Polygon::ID() {
    return QString("Polygon");
}

std::vector<QPoint> Polygon::get_points() {
    return m_points;
}

void Polygon::set_points(std::vector<QPoint> new_points) {
    m_points = std::move(new_points);
}
void Polygon::set_text(QString neu_inhalt) {
    neu_inhalt = "ERROR: kein Text!";
}

QString Polygon::get_text() {
    return "ERROR: kein Text!";
}