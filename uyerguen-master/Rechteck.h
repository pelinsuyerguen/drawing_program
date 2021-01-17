#ifndef EIS_DRAW_RECHTECK_H
#define EIS_DRAW_RECHTECK_H

#include "shape.h"

//Oberklasse: Shape
class Rechteck : public shape {
    //Points: [0]: TopLeft, [1] Bottom Right
    std::vector<QPoint> m_points;
public:
    //Konstruktor
    explicit Rechteck();
    //Konstruktor - vektor TopLeft, BottomRight Punkte von Rechteck
    explicit Rechteck(std::vector<QPoint> points) : m_points(points) {};
    //get_ID : return Rechteck
    QString ID() override;
    // return m_points
    std::vector<QPoint> get_points() override;
    //set m_points
    void set_points(std::vector<QPoint> new_points) override;
    // set text
    void set_text(QString neu_inhalt) override;
    //get text
    QString get_text() override;
};
#endif

