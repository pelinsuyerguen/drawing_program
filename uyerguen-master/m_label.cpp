#include "m_label.h"
#include <iostream>
////konstuktor
m_label::m_label() {
    setMouseTracking(true);
    mouse_click = false;
    int click_counter = 0;
    mouse_double_click = false;
}

void m_label::mousePressEvent(QMouseEvent* event) {
    QLabel::mousePressEvent(event);
    if (event->button() == Qt::RightButton) {
        click_right = event->pos();
    } else {
        mouse_click = true;
        click_left = event->pos();
        ++click_counter;
    }
}
void m_label::mouseReleaseEvent(QMouseEvent *event){
    QLabel::mouseReleaseEvent(event);
    mouse_click = false;
    mouse_double_click = false;
    update();
}
//wichtig zum mouse-position verfolgen auf draw-ebene
void m_label::mouseMoveEvent(QMouseEvent *event){
    QLabel::mouseMoveEvent(event);
    if (event->type() == QEvent::MouseMove)
    {
        current_mouse_point = event->pos();
    }
    update();
}

void m_label::mouseDoubleClickEvent(QMouseEvent *event) {
    QLabel::mouseDoubleClickEvent(event);
    mouse_double_click = true;
    double_click = event->pos();
}
