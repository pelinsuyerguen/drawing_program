//
// Created by pelinsu on 05.08.20.
//

#ifndef EIS_DRAW_M_LABEL_H
#define EIS_DRAW_M_LABEL_H
#include <QLabel>
#include <QtWidgets>
//////von QLabel abgeleitete Klasse
//für richtige ousetracking ohne punkt-zransformationen in main window
//main widget in mainwindow
class m_label : public QLabel {
public:
    //konstruktor
    m_label();
    //selbst erklärende members
    QPoint click_left;
    QPoint click_right;
    QPoint current_mouse_point;
    QPoint double_click;
    int click_counter;
    bool mouse_click;
    bool mouse_double_click;

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;


};


#endif //EIS_DRAW_M_LABEL_H
