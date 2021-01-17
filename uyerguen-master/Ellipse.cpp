#include "Ellipse.h"
#include <iostream>

std::vector<QPoint> Ellipse::get_points() {
    return  m_points;
}

QString Ellipse::ID() {
    return QString("Ellipse");
}

void Ellipse::set_points(std::vector<QPoint> new_points) {
    m_points = std::move(new_points);
}
void Ellipse::set_text(QString neu_inhalt) {
    neu_inhalt = "ERROR: kein Text!";;
}

QString Ellipse::get_text() {
    return "ERROR: kein Text!";
}