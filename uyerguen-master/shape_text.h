#ifndef EIS_DRAW_SHAPE_TEXT_H
#define EIS_DRAW_SHAPE_TEXT_H

#include "shape.h"

class shape_text : public shape{
    //Bounding Rect TopLeft[0], BottomRight[1] , aktuellen Rect TopLeft[2], BottomRight[3]
    std::vector<QPoint> m_points;
    //von Dialog geholt
    QString m_inhalt;
public:
    //Konstruktor
    explicit shape_text();
    //Konstruktor nur die inhalt festlegen
    explicit shape_text(QString inhalt) : m_inhalt(inhalt){};
    //Konstruktor - inhalt und Points
    explicit shape_text(QString inhalt,std::vector<QPoint> points) : m_inhalt(inhalt),m_points(points){};
    //get_ID : return Text
    QString ID() override;
    // return m_points
    std::vector<QPoint> get_points() override;
    //set m_points
    void set_points(std::vector<QPoint> new_points) override;
    // set text -> wird auf draw-Ebene hingefügt
    void set_text(QString neu_inhalt) override;
    //return text inhalt
    QString get_text() override;
};
#endif
