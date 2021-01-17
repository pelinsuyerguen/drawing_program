#ifndef EIS_DRAW_ELLIPSE_H
#define EIS_DRAW_ELLIPSE_H

#include "shape.h"

class Ellipse : public shape{
    //vector mit 2 Punkte: Topleft[0] , Bottomright[1]
    std::vector<QPoint> m_points;
public:
    //Konstruktor
    explicit Ellipse();
    //Konstruktor -vector Punkte Topleft[0] , Bottomright[1]
    explicit Ellipse(std::vector<QPoint> points) : m_points(points) {};
    //get_ID : return Ellipse
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
