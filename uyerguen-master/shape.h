#ifndef EIS_DRAW_SHAPE_H
#define EIS_DRAW_SHAPE_H

#include <QString>
#include <QObject>
#include <QVector>
#include <QAction>
#include <utility>
//Oberklasse für Shapes
class shape : public QObject {
    std::vector<QPoint> m_points;
public:
    //alle methoden virtual
    virtual QString ID(){return QString();};
    virtual std::vector<QPoint> get_points(){return m_points;};
    virtual void set_points(std::vector<QPoint> new_points){m_points = std::move(new_points);}
    virtual void set_text(QString neu_inhalt) {};
    virtual QString get_text() {return QString();};
};

#endif
