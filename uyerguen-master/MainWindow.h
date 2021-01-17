#ifndef EIS_DRAW_MAINWINDOW_H
#define EIS_DRAW_MAINWINDOW_H

#include <QtWidgets>
#include "shape.h"
#include "m_label.h"

/*
 *Klasse für Main Window des programmes */

class MainWindow : public QMainWindow {
    //Name des Programmes (da während programm geändert des Programms geändert wird, merken)
    QString Datei_name;

    //Info bechreibungen auf der rechten seite und dockwidget zum zeigen der beschreibung
    QTextEdit* Info_beschreibung = nullptr;
    QDockWidget* m_dockwidget = nullptr;


    //vector für gezeichnete "shapes" (s. klasse shape)
    // bool -> wenn was neues gezeichnet wird : true
    std::vector<shape*> main_vector;
    bool vector_geandert = false;

    //Painter/Painterpath/canvas bzw. display hintergrund (s. m_label klasse)
    //da man Mousepositionen auf dem "draw-Ebene" braucht; QLabel als Klassenmember merken
    //und eigentlichen "mouse_tracking" da machen bzw. mouse positionen dadurch folgen
    //dadurch sind keine punkt transformationen nötig
    //(m_label ist eine aus der QLabel abgeleitete Klasse, s. m_label)
    QPainter* m_painter = nullptr;
    QPainterPath* m_painterpath =nullptr;
    QImage m_canvas;
    m_label* m_display = nullptr;

    //alle folgende aktionen sind checkable
    //Polygon / Ellipse /Rechteck zeichnen Actionen
    //falls zeichnen anfängt -> begin_draw : true
    QAction* polygon_action = nullptr;
    QAction* ellipse_action = nullptr;
    QAction* rechteck_action = nullptr;
    bool begin_draw = false;

    //Text Zeichenen Action
    //falls text action anfängt -> begin_text : true
    QAction* text_action = nullptr;
    bool begin_text = false;
    //font für text_zeichnen; welche während zeichnen geändert wird falls gezoomt wird
    QFont m_font = QFont("Arial",10.000);

    //zoom action
    // wheelEvent updatet die shapes wenn action gecheckt wird;
    // mouse scroll factor =1 (gemerkt damit einstellbar)
    QAction* zoom_action = nullptr;
    int mouse_scroll_factor =1;

    //pan action (click & grab)
    //mouseMoveEvent (& Qt::LeftButton) updatet die shapes wenn action gecheckt wird ;
    QAction* pan_action = nullptr;

public:
    //Konstruktor für mainwindow
    MainWindow();
    //methode für datei name aktualisieren und main window title zu andern (parameter: neue dateiname)
    void set_datei_name(const QString& name);

    //--------------------
    //methode für saven in gegebenen datei (parameter: gegebenen dateiname)
    void save_as_in_file(const QString& file_name);

    void save_as();
    void save();
    //methode zum fragen mit einem questionbox dem nutzer, ob die veränderungen gespeichert werden wollen
    bool save_message_box();
    void open();
    void new_file();

    //methode für öffnen gegebenen datei (parameter: gegebenen dateiname)
    void load_file(QString& file_name);
    void leave();
    //-----------------------

    // vom Vektor zeichnen-Funktion
    void draw_from_vector();
    //display wird mit neuen vector updated
    void update_from_vector();

    //methode zum polygon abschliessen;
    //wenn polygon noch nicht gelossen war und eine andere action gewöhlt wird
    void polygone_force_to_close();

    //dadurch wird letzt gespeicherten punkt bei einem shape m_points vector aktualisiert mit mouse_move
    // (also aktualisiert zur letzten current mouse position)
    void shape_update_with_mousemove();

    //diese Methode wird aufgerufen wenn Text Action toggled
    //frag mit einem Dialogfenster nach dem Text und speichert bounding rect von diesem text als points in shape-m_points vestor
    void new_text();

protected:
    //overrided events mit *event parameters...

    //wenn resize event --> uodate_from_vector wird aufgerufen -> display updated
    void resizeEvent(QResizeEvent *event) override;
    //close event nur, wenn keine veränderungen gemacht wurde oder diese nicht gespeichert werden will
    void closeEvent(QCloseEvent *event) override;

    //bei folgenden events werden überprüft ob die gelistete QActions "Checked" (bzw toggled) sind ...
    //wenn ja -> while loop bis nächsten "checked" (nicht mehr toggled) -> break loop

    void mouseMoveEvent(QMouseEvent *event) override;
    //-polygon_action
    //-text_action
    //-text_action
    //&LeftButtonPressed :
    //-pan_action
    //-ellipse_action
    //-rechteck_action


    void mouseReleaseEvent(QMouseEvent *event) override;
    //-ellipse_action
    //-rechteck_action

    void mousePressEvent(QMouseEvent *event) override;
    //== RightButton:
    //-polygon_action
    //== LeftButton
    //-polygon_action
    //-ellipse_action
    //-rechteck_action
    //-text_action

    void keyPressEvent(QKeyEvent *event) override;
    //CTRL:
    //-ellipse_action
    //-rechteck_action

    void wheelEvent(QWheelEvent *event) override;
    //-zoom_action

};


#endif //EIS_DRAW_MAINWINDOW_H
