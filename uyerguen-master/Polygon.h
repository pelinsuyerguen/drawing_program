#ifndef EIS_DRAW_POLYGON_H
#define EIS_DRAW_POLYGON_H

#include "shape.h"
//Oberklasse: Shape
class Polygon : public shape{
    //Points : Rechtklicks
    std::vector<QPoint> m_points;
public:
    //Konstruktor
    explicit Polygon();
    //Konstruktor - vektor Links-Click Positionen
    explicit Polygon(std::vector<QPoint> points) : m_points(points) {};
    //get_ID : Polygon
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
