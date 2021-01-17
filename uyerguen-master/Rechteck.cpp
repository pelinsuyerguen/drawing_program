#include "Rechteck.h"
#include <iostream>

std::vector<QPoint> Rechteck::get_points() {
    return m_points;
}

QString Rechteck::ID() {
    return QString("Rechteck");
}
void Rechteck::set_points(std::vector<QPoint> new_points) {
    m_points = std::move(new_points);
}
void Rechteck::set_text(QString neu_inhalt) {
    neu_inhalt = "ERROR: kein Text!";
}

QString Rechteck::get_text() {
    return "ERROR: kein Text!";
}