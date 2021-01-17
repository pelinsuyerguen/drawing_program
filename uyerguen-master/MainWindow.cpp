#include "MainWindow.h"
#include "shape.h"
#include "Polygon.h"
#include "Rechteck.h"
#include "Ellipse.h"
#include "m_label.h"
#include "shape_text.h"
#include <vector>
#include <iostream>

//info Lambda (Message-box) für info action
auto info(QWidget *parent) {
    return [parent](){
        QMessageBox::about(parent,"EIS_Draw - Info:",
                           "Dieses Zeichenprogramm ist eine Lösung für das Eis-Projekt 2020. "
                           "Wie man untershiedliche Tools bedienen kann wird immer in dem Info-Box auf der "
                           "rechten Seite des Fensters beschrieben. Viel Spaß!");};
}

//ARCHITEKTUR  Idee =
//alle Actions sowohl in Menubar als auch in toolbar da, was die machen sind gleich, Icons sind gleich
//-> mit dieser make_main_action funktion muss man die nur einmal initialisieren
//erweiterbar -> für jede action muss zusatzlich nur eine neue Lambda funktion geschrieben werden(deswegen template)
template <typename funk>
void make_main_action(QMenu *menu, QToolBar *toolbar,  const QIcon& icon,const QString& text, bool has_shortcut,
                      const QKeySequence& shortcut, QObject *receiver,funk function){
    //receiver ist in dem fall "this" (parent)
    auto action = new QAction(icon,text,receiver);
    //falls shortcut vorhanden -> set shortcut
    if(has_shortcut)
        action->setShortcut(shortcut);
    //QAction bzw. main_menu / tool_bar - Icon mit Lambda-Funktion verbinden
    QMainWindow::connect(action,&QAction::triggered,receiver,function);
    menu->addAction(action);
    toolbar->addAction(action);
    //Icons werden separiert
    toolbar->addSeparator();
}
/*
void make_main_action(QMenu *menu, QToolBar *toolbar,  const QIcon& icon,const QString& text, bool has_shortcut,
                      const QKeySequence& shortcut, QObject *receiver,funk function){
    //receiver ist in dem fall "this" (parent)
    auto action = new QAction(icon,text,receiver);
    //falls shortcut vorhanden -> set shortcut
    if(has_shortcut)
        action->setShortcut(shortcut);
    //QAction bzw. main_menu / tool_bar - Icon mit Lambda-Funktion verbinden
    QMainWindow::connect(action,&QAction::triggered,receiver,function);
    menu->addAction(action);
    toolbar->addAction(action);
    //Icons werden separiert
    toolbar->addSeparator();
}*/


//ARCHITEKTUR  Idee =
//alle Zeichen-Objekte sind gehören zu class "shape" und wenn was gezeichnet wird, werden die Punkten in einem Vector als
//klassenmember von shape mitgespeichert, und die Actions in dem Main Window main_vector mit gespeichert;
// sodass man die wichtige Punkte des shapes und Name beim zeichen direkt merkt
// (beim Polygon sind die wichtige Punkte mouse-klicks; bei allen anderen erste maus klick und zweite maus klick bzw. mouseposition
// (bei Text bottom-left, top right von bounding rect(mit aktualisierten positionen) und geänderten rect durch zoom))

// vom Vektor zeichnen-Funktion
void MainWindow::draw_from_vector(){
    for(int i = 0; i < main_vector.size(); ++i) { // Iteration durch ZeichenObjekte im Vector
        //je nach welches Zeichenobjekt es ist..
        if (main_vector[i]->ID() == QString("Polygon")){
            //von Anfangspunkt -> linie zu nächste..
            for(int j = 0; j< main_vector[i]->get_points().size()-1; ++j) // Iteration - Einzelne Punkte im Vector des i'ten Polygons
            {m_painterpath->moveTo(main_vector[i]->get_points()[j]);
                m_painterpath->lineTo(main_vector[i]->get_points()[j+1]);}
            //wenn fertig -> line zu Anfangpunkt und geschlossenen Polygon
            //mit der methode force_to_close; wenn es noch nicht gechlossen wurde

            //draw und close
            m_painter->drawPath(*m_painterpath);
            m_painterpath->closeSubpath();
        }
        else if(main_vector[i]->ID() == QString("Rechteck")){ //p0 : Anfangspoint ; p1 : Endpoint
            m_painterpath->addRect(main_vector[i]->get_points()[0].x(),main_vector[i]->get_points()[0].y(), //x und y Werte von anfangspunkt
                         (main_vector[i]->get_points()[1].x()-main_vector[i]->get_points()[0].x()), //width (x1 -x0)
                         (main_vector[i]->get_points()[1].y()-main_vector[i]->get_points()[0].y())); //height (y1 -y0)
            //draw und close
            m_painter->drawPath(*m_painterpath);
            m_painterpath->closeSubpath();
        }
        else if(main_vector[i]->ID() == QString("Ellipse")){  //p0 : Anfangspoint ; p1 : Endpoint
            m_painterpath->addEllipse(main_vector[i]->get_points()[0]+(main_vector[i]->get_points()[1]-main_vector[i]->get_points()[0])/2, //center
                                                                                                                                //center = p0 + (p1-p0)/2
                            std::abs((1.0 *main_vector[i]->get_points()[0].x()-main_vector[i]->get_points()[1].x())/2),  //radius_x = |(x1-x0)/2|
                            std::abs((1.0* main_vector[i]->get_points()[0].y()-main_vector[i]->get_points()[1].y())/2)); //radius_y  = |(y1-y0)/2|
            //draw und close
            m_painter->drawPath(*m_painterpath);
            m_painterpath->closeSubpath();
        }
        else if(main_vector[i]->ID() == QString("Text")){
            //add text Funktion von Painterpath füllt innen den Buchstaben nicht
            //und Text sieht deswegen dann ganz komisch aus -> also hier habe ich einfach painter->draw Text benutzt

            //ARCHITEKTUR IDEE: Am Anfang, wenn der Text eingegeben wird, haben wir boundingRect des Textes gemerkt.(s. new text methode)
            //und die wichtige Punkte m_points vector gespeichert. (Top_left und Bottom_right von bounding reckteck

            // main_vector[i]->get_points()[0] : topleft bounding rect (mit aktualisierten positionen)
            //main_vector[i]->get_points()[1] : bottomright bR (//)
            //main_vector[i]->get_points()[2] : top Left text rechteck
            //main_vector[i]->get_points()[3] : bottom right text rechteck bzw. mouse position (ausser zoom)

            // Bottom Right ist in dem Fall current_mouse_position)
            QRect *new_rect = new QRect();
            new_rect->setBottomRight(main_vector[i]->get_points()[3]); // bottom right = mouse position
            new_rect->setTopLeft(main_vector[i]->get_points()[2]);     // top Left = text rechteck
            //jetzt wenn der aktuelle Rechteck vergrößert wird; passen wir pointSize an
            //mit dem Faktor height unseren Rechteck/ height bounding Rechteck (bR)
            m_font.setPointSizeF(11); //PointF : point float damit es mit nachkommastellen richtig faktorisiert werden kann
            //aktualisierten fontsizeF = fontSizeF * |height_aktuellen rechteck| / |height boundingRechteck|
            m_font.setPointSizeF(m_font.pointSizeF()* std::abs(1.0*(main_vector[i]->get_points()[3].y()-main_vector[i]->get_points()[2].y())/ // height-aktuellen Rechteck
            (main_vector[i]->get_points()[1].y()-main_vector[i]->get_points()[0].y())));//height bR
            //aktualisier den Font des painters
            m_painter->setFont(m_font);

            //ARCHITEKTUR IDEE:
            //Vorteil davon ist, dass beim dem Text kein zusätzliches Vorfaktor als Member gemerkt werden muss
            //Unseren draw_from_vector funktion übernimmt somit diese Anpassung der Textgrösse

            m_painter->drawText(new_rect->x(),new_rect->y(),new_rect->width(),new_rect->height(),Qt::TextExpandTabs,main_vector[i]->get_text(),new_rect);
        }
    }
}
//update_from_vector methode zum refreshen des displays
void MainWindow::update_from_vector(){
    m_canvas = QImage(m_display->width(),m_display->height(),QImage::Format_RGBA8888);
    m_canvas.fill(QColor(230,230,230));
    m_painter->begin(&m_canvas);//new
    m_painter->setPen(Qt::black);
    draw_from_vector();
    m_painter->end();
    m_display->clear();
    m_display->setPixmap(QPixmap::fromImage(m_canvas));
    m_display->repaint();
}

//Konstruktor für main methode
MainWindow::MainWindow() : QMainWindow() {

    //ARCHITEKTUR  = Window title sollte sich mit den neuen Namen des Dokumentes aktualisieren
    //falls noch nicht gespecihert -> * am Ende
    set_datei_name("unnamed");
    resize(800,540);

    auto Menu_datei = new QMenu(this);
    //Quelle für "menuBar()-> addMenu" https://doc.qt.io/archives/qt-4.8/qmenubar.html
    auto main_Toolbar = new QToolBar;

    //Save/Open/Close Actions hinfügen...
    Menu_datei = menuBar()->addMenu("&Datei");
    make_main_action(Menu_datei,main_Toolbar,QIcon::fromTheme("document-new", QIcon(":/new.png")),"Neu",true,QKeySequence::New,
                     this,[&](){new_file();polygone_force_to_close();});
    make_main_action(Menu_datei,main_Toolbar,QIcon::fromTheme("document-save", QIcon(":/save.png")),"Speichern",true,QKeySequence::Save,
                     this,[&](){save();polygone_force_to_close();});
    make_main_action(Menu_datei,main_Toolbar,QIcon::fromTheme("document-save-as", QIcon(":/save-as.png")),"Speichern als..",true,QKeySequence::SaveAs,
                     this,[&](){save_as();polygone_force_to_close();});
    make_main_action(Menu_datei,main_Toolbar,QIcon::fromTheme("document-open", QIcon(":/open.png")),"Öffnen",true,QKeySequence::Open,
                     this,[&](){open();polygone_force_to_close();});

    Menu_datei = menuBar()->addMenu("&Hilfe");

    //Quellen für QIconen aus den Iconenlisten
    //https://doc.qt.io/qt-5/qicon.html
    //https://gist.github.com/peteristhegreat/c0ca6e1a57e5d4b9cd0bb1d7b3be1d6a

    //main_Toolbar->addAction(QIcon::fromTheme("document-new", QIcon(":/new.png")),"Neu",this,&MainWindow::new_file);
    main_Toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    make_main_action(Menu_datei,main_Toolbar,style()->standardIcon(QStyle::SP_MessageBoxInformation),
                     QString("Info"),true,QKeySequence::HelpContents,this,info(this));


    make_main_action(Menu_datei,main_Toolbar,QIcon::fromTheme("application-exit", QIcon(":/exit.png")),"Schließen",true,QKeySequence::Close,
                     this,[&](){leave();polygone_force_to_close();});

    addToolBar(main_Toolbar);
    addToolBarBreak();

    //-----------------------------------------------------------------------------------

    auto poly_Toolbar = new QToolBar;
    poly_Toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea,poly_Toolbar);

    //ARCHITEKTUR IDEE:
    //Die Vektor-ändernde Actionen dürfen nicht zu gleichenzeit "checkable sein"
    //Meine erste Idee war; dass man durch setChekable(true/false) steuert; allerdings bessere Methode ist QActiongrup
    //Quelle Text für die Idee = https://doc.qt.io/archives/qt-4.8/qactiongroup.html
    /*In some situations it is useful to group QAction objects together.
     * For example, if you have a Left Align action, a Right Align action, a Justify action,
     * and a Center action, only one of these actions should be active at any one time.
     * One simple way of achieving this is to group the actions together in an action group.*/

    /////////Anfang Polytoolbar
    ///Alle poly- bzw. draw/edit toolbar actionen:
    //einfach in toolbar,actiongroup adden und Action-mache (in dem Fall Info-Box aktualisieren)
    //_______________________________________________________________________________________________
    auto *action_group = new QActionGroup(this);
    polygon_action = new QAction(QIcon::fromTheme("software-update-urgent"),"Polygon",this);
    polygon_action->setCheckable(true);
    connect(polygon_action,&QAction::toggled,this, [&](){Info_beschreibung->
    setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""<br/> ""<u>Polygon-Tool:</u>""<br/>""<br/>"
            "mit diesem Tool können Polygone gezeichnet werden! ""(sehr wichtiges Werkzeug, deswegen Aufrufezeichen) "
            "<br/>""<u>Bedienung funktioniert so;</u>""<br/>""<b>-</b> Mit der linken Maustaste klicken, um den"
            " Anfangspunkt zu setzen. ""<br/> ""<b>-</b> Jeder weitere links-klick fügt ein weiteres ""Liniensegment hinzu."
            "<br/> ""<b>-</b> Ein rechts-klick schließt das Polygon.""<br/> ""<b>-</b> Ein weiterer links_klick beginnt das nächste "
            "Polygon.""<br/>""<br/>""Viel Spass beim Zeichnen!");});

    ellipse_action = new QAction(QIcon::fromTheme("media-record", QIcon(":/record.png")),"Ellipse",this);
    ellipse_action->setCheckable(true);
    connect(ellipse_action,&QAction::toggled,this, [&](){
        Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""<br/> ""<u>Ellipse-Tool:</u>""<br/> ""<br/> "
                                   "mit diesem Tool können Ellipse und ""Kreise gezeichnet werden. ""<br/> ""<u>Bedienung funktioniert so;</u>"
                                   "<br/> ""<b>-</b> Mit der linken Maustaste klicken, um den"" Anfangspunkt zu setzen. ""<br/> "
                                   "<b>-</b> Die Maustaste gedrückt halten und die Höhe/ ""Breite der Ellipse einstellen;"
                                   "<br/> ""<b>-</b> und wenn die Taste losgelassen werden, ""wird die Ellipse gespeichert.""<br/> "
                                   "<b>-</b> Durch Drücken""auf <b>[Strg]</b>  bzw. [CTRL]-Taste wird ein <b>Kreis</b> erzeugt."
                                   "(wird die Breite zur Höhe angepasst)""<br/>""<b>-</b> Ein weiteres links-Klick beginnt nächste Ellipse..""<br/>"
                                   "<br/>""Viel Spass beim Zeichnen!");
        polygone_force_to_close();}); ////+Polygon to force

    rechteck_action = new QAction(QIcon::fromTheme("media-playback-stop", QIcon(":/stop.png")),"Rechteck",this);
    rechteck_action->setCheckable(true);
    connect(rechteck_action,&QAction::toggled,this, [&](){
        Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""<br/> ""<u>Rechteck-Tool:</u>""<br/> ""<br/> "
                                   "mit diesem Tool können Rechtecke und ""Quadrate gezeichnet werden. ""<br/> ""<u>Bedienung funktioniert so;</u>"
                                   "<br/> ""<b>-</b> Mit der linken Maustaste klicken, um den"" Anfangspunkt zu setzen. ""<br/> "
                                   "<b>-</b> Die Maustaste gedrückt halten und die Höhe/ ""Breite des Rechtecks einstellen;""<br/> "
                                   "<b>-</b> und wenn die Taste losgelassen werden, ""wird es gespeichert.""<br/> "
                                   "<b>-</b> Durch gleichzeitiges Drücken""auf <b>[Strg]</b> bzw. [CTRL]-Taste erzeugt ein <b>Quadrat</b>."
                                   "(wird die Breite zur Höhe angepasst)""<br/>""<b>-</b> Ein weiteres links-Klick beginnt nächstes Rechteck.."
                                   "<br/>""<br/>""Viel Spass beim Zeichnen!");
        polygone_force_to_close();}); ////+Polygon to force


    text_action = new QAction(QIcon::fromTheme("font-x-generic", QIcon(":/text.png")),"Text",this);
    text_action->setCheckable(true);
    connect(text_action,&QAction::toggled,this, [&](){
        Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""<br/> ""<u>Text-Tool:</u>""<br/> ""<br/> "
                                   "mit diesem Tool können Texte erzeugt werden.""<br/> ""<u>Bedienung funktioniert so;</u>""<br/> "
                                   "<b>-</b> In Pop-Up Diaologbox geben Sie den Text ein ""<br/> ""<b>-</b> Um den Text zu platzieren klicken Sie "
                                   "linke Maustaste.""<br/> ""<b>-</b> Wenn Sie einen anderen Text eingeben wollen; wechseln Sie bitte"
                                   " kurz zum einen anderen Tool und drücken Sie nochmal auf dem ""Text button. ""<br/>"
                                   "<b>Vorzicht!</b> Bei jedem neuen Platzieren des Textes"" wird die Textgröße verändert!"
                                   "<br/>""<br/>""Viel Spass beim Zeichnen!");
        polygone_force_to_close();});////+Polygon to force
    connect(text_action,&QAction::toggled,this, &MainWindow::new_text); // Beim text - Eingügen QTDialog
    // (s. die new_text Funktion-Definition)

    zoom_action = new QAction(QIcon::fromTheme("edit-find", QIcon(":/find.png")),"Zoom",this);
    zoom_action->setCheckable(true);
    connect(zoom_action,&QAction::toggled,this, [&](){
        Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""!!!<b>Vorsicht:</b> Neue Koordinaten werden gespeichert!"
                                   "<br/> ""<br/> ""<u>Zoom-Tool:</u>""<br/> ""mit diesem Tool kann das Bild <u>relativ zu Mouse Position</u> "
                                   "vergößert/ verkleinert werden. ""<br/> ""<u>Bedienung funktioniert so;</u>""<br/> ""<b>-</b>"
                                   "Zuerst stellen Sie ein mit dem eins unter stehenden Mause-Scroll-Faktor Taste, ob Sie normal schnell oder"
                                   " schneller ( x2 / x3 ) ein/aus zoomen. Bei jedem drücken stellen Sie die Zoom Geschwindigkeit um. (Bei langsameren Mause-Rad ist "
                                   "x3 für stufenlosen Zoomen empfohlen.) Jetzt Können Sie mit Maus-Rad Zoomen; wie folgend erklärt;""<br/> "
                                   "<b>Zoom-In:</b> <u>Scroll-Up </u>mit Mouserad ins Bild ;""<br/> ""<b>Zoom-Out:</b> <u> Scroll- Down </u> Mouserad ins Bild.");
        polygone_force_to_close();}
        );////+Polygon to force


    pan_action = new QAction(QIcon::fromTheme("object-flip-vertical", QIcon(":/jump.png")),"Pan",this);
    pan_action->setCheckable(true);
    connect(pan_action,&QAction::toggled,this, [&](){
        Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""!!!<b>Vorsicht:</b> Neue Koordinaten werden gespeichert!"
                                   "<br/> ""<br/> ""<u>Panning-Tool:</u>""<br/> ""<br/> ""mit diesem Tool kann das Bild in <u>allen</u> Richtungen "
                                   "verschieben. ""<br/> ""<u>Bedienung funktioniert so;</u>""<br/> ""<b>-</b> Mit der linken Maustaste ins Bild klicken und halten. "
                                   "<br/> ""<b>-</b> Bildausschnitt folgt dem Ziehen bzw. Mouse-Position.""<br/> ""<b>-</b> Wenn die Maustaste losgelassen wird, bleibt der Bild "
                                   "Ausschnitt bestehen");
        polygone_force_to_close();});////+Polygon to force
    ///(s. die polygon_force_to_close Funktion-Definition)

    ///Quelle für die Hilfe bzw. Info-beschreibungen; EIS Musterlösung :)


    action_group->addAction(zoom_action);
    action_group->addAction(pan_action);
    action_group->addAction(polygon_action);
    action_group->addAction(rechteck_action);
    action_group->addAction(ellipse_action);
    action_group->addAction(text_action);

    poly_Toolbar->addAction(polygon_action);
    poly_Toolbar->addAction(ellipse_action);
    poly_Toolbar->addAction(rechteck_action);
    poly_Toolbar->addSeparator();
    poly_Toolbar->addAction(text_action);
    poly_Toolbar->addSeparator();
    poly_Toolbar->addAction(zoom_action);
    poly_Toolbar->addSeparator();

    //damit bei langsamen mausen Stufenfrei die zoom Action (mit mause scroll-bar) funktioniert "langsamer/schneller" zoomen
    auto mouse_scroll_faktor_action = new QAction("x" + QString::number(1)+ "\n" + QString("Scroll")+ "\n" + QString("Speed"),this);
    //QAction bzw. main_menu / tool_bar - Icon mit Lambda-Funktion verbinden
    QMainWindow::connect(mouse_scroll_faktor_action,&QAction::triggered,this,[=](){
        if (mouse_scroll_factor < 3 ){
            ++mouse_scroll_factor;
            mouse_scroll_faktor_action->setText("x" + QString::number(mouse_scroll_factor)+ "\n" + QString("Scroll")+ "\n" + QString("Speed"));
        }
        else {mouse_scroll_factor = 1;
            mouse_scroll_faktor_action->setText("x" + QString::number(1)+ "\n" + QString("Scroll")+ "\n" + QString("Speed"));}});

    poly_Toolbar->addAction(mouse_scroll_faktor_action);
    poly_Toolbar->addSeparator();
    //Icons werden separiert
    poly_Toolbar->addAction(pan_action);
    //______________________________________________________
    //////////ENDE Polytoolbar


    //Infobox im Fenster:
    //TextEditor die 1)Readonly 2)wird aktualisiert je nah widget -> merken als member
    Info_beschreibung = new QTextEdit(this);
    Info_beschreibung->setReadOnly(true);
    Info_beschreibung->setAlignment(Qt::AlignLeft);
    //https://doc.qt.io/qt-5/qml-qtquick-text.html Quelle für <b> ...</b> bold Text
    Info_beschreibung->setText("<b>EIS_Draw 2020 - Hilfe-Box</b> ""<br/> ""<br/> ""Willkommen bei EIS_Draw 2020"
                               "<br/> ""<br/> ""Das ist eine Lösung für das EIS Projekt""im SS 2020. Sie können in diesem Programm "
                               "Polygone, Ellipse und Rechtecke zeichnen; ""und Texte einfügen. Zoom und Pan Tools stehen "
                               "auch noch zur Verfügung. Wenn Sie nicht ""sicher sind, wie man ein Tool bedient, "
                               "können Sie (wie jetzt) jeder Zeit ""dieses Hilfe-Box lesen, nachdem Sie das"" Tool-Button gedrückt haben."
                               "<br/>""<br/>""Viel Spass!");


    m_dockwidget = new QDockWidget;
    m_dockwidget->setWidget(Info_beschreibung);
    addDockWidget(Qt::RightDockWidgetArea,m_dockwidget);
    m_dockwidget->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    m_dockwidget->isMinimized();


    //ARCHITEKTUR =
    //da man eigentlich nur die Mousepositionen auf dem "draw-Ebene" braucht; QLabel als Klassenmember merken
    m_display= new m_label;
    setCentralWidget(m_display);
    m_display->setMinimumSize(1, 1);
    QImage canvas(m_display->width(),m_display->height(),QImage::Format_RGBA8888);
    //ARCHITEKTUR=
    //damit man mit allen überall bei allen funktionen painter-painterpath und image aufrufen kann
    //als klassenmember merken
    m_canvas = canvas;
    m_painter = new QPainter;
    m_painterpath = new QPainterPath;

    setMouseTracking(true); // wenn mouse klcik/ dopppelklick macht findet man positionen
    //grabMouse();
    //releaseMouse(); //nicht mehr grab

}
//----------------------Ende Konstruktor

//Methode zum datei name und windows title zu aktualisieren, wenn keine ungespiecherte änderungen gibt
//daher -> set vector_genandert false wenn set_datei name aufgerufen wird
void MainWindow::set_datei_name(const QString& name) {
    Datei_name = name;
    setWindowTitle("EIS_Draw-"+ Datei_name);
    vector_geandert = false;
}
//--------------------------------------------------------------------------------------------
//FOLGENDE METHODE -EVENTS SIND FÜR SAVE -LOAD - NEW - CLOSE WICHTIG

//Quelle: https://doc.qt.io/qt-5/qtwidgets-tutorials-addressbook-part6-example.html
//starkt orientiert zw zeilen 410-420
//https://doc.qt.io/qt-5/qdatastream.html
//methode zum save_as_in_file
void MainWindow::save_as_in_file(const QString& file_name){
    //wenn keine file_name gibt; macht nichts und return zu main window
    if (file_name.isEmpty())
        return;
    else {
        set_datei_name(file_name);
        QFile file(file_name);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, "EIS_DRAW - WARNING:",  "Speichern in diese Datei nicht möglich.");
            return;
        }
        QDataStream out(&file);
        out << QString("*EIS_DRAW_2020*"); //Das ist eigentlich wie ein Key
        if(main_vector.size() != 0){
            out << int(main_vector.size());
            for(int i = 0; i < main_vector.size(); ++i) { // Iteration durch ZeichenObjekte im Vector
                //geschtieben : ID , text_inhalt(wichtig nur für shape_text), Anzahl der gespeicherter punkte, und die Punkte als QPoint iterativ
                out << main_vector[i]->ID();
                out << main_vector[i]->get_text();
                out << int(main_vector[i]->get_points().size());
                for (int j = 0; j < main_vector[i]->get_points().size(); ++j) {
                    out << QPoint(main_vector[i]->get_points()[j]);
                }
            }
        }
    }
}
//save as methode für save speichern als action
void MainWindow::save_as(){
    //get filename mit filedialog box
    QString file_name = QFileDialog::getSaveFileName(this,"EIS_DRAW : Speichern als..", "","EIS_DRAW (*.eis20)");
    if (!file_name.isEmpty()){
        //datei format wird am ende hingefügt
        if (!(file_name.endsWith(".eis20")))
            file_name= QString(file_name+".eis20");
        save_as_in_file(file_name);
    }
    //else return;
}

//save methode für speicher action
void MainWindow::save(){
    if (Datei_name.isEmpty() || Datei_name == "unnamed") {
        //wenn noch nichts gespeichert -> save as und hol den datei name ggf
        return save_as();
    } else {//sonst speicher mit aktuellen datei name
        return save_as_in_file(Datei_name);
    }
}
//leave methode für schliessen action
void MainWindow::leave() {
    //wenn vector geandert wird -> frag nach save (s save message box)
    if (!vector_geandert || !save_message_box())
        MainWindow::close();
}
//open methode für laden action
void MainWindow::open(){
    //wenn vector geandert wird -> frag nach save (s save message box)
    if (!vector_geandert|| (vector_geandert && !save_message_box())) {
        QString file_name = QFileDialog::getOpenFileName(this);//nach file gefragt
        if (!file_name.isEmpty())
            load_file(file_name);//wenn name -> load file (s. load_file methode)
        else return;
    }
}

void MainWindow::new_file(){
    //wenn vector geandert wird -> frag nach save (s save message box)
    if (!vector_geandert || (vector_geandert && !save_message_box())) {
        //clear main_vector ; update display; set dateiname "unnamed"
        main_vector.clear();
        update_from_vector();
        vector_geandert = false;
        set_datei_name("unnamed");
    }
}
//methode save_message_box frag nach save
//bool : true wenn save oder cancel
bool MainWindow::save_message_box() {
    int m_msgbx =  QMessageBox::question(this,"EIS_DRAW - WARNING: ","Letzte Veränderungen wurden nicht gespeichert. Wollen Sie die jetzt speichern?",
                                         "Nicht Speichern","Cancel","Speichern",2,1);
    if (m_msgbx == 2) {//also wenn doch speichern
        save();
        return true;
    }
    else if(m_msgbx == 0){
        return false;
    }
    else return true;
}
//load file methode zum laden
void MainWindow::load_file(QString &file_name) {
    if (file_name.isEmpty())
        return;
    else {
        QFile file(file_name);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, "EIS_DRAW - WARNING:",  "Die Datei kann nicht geöffnet werden.");
            return;
        }
        QDataStream in(&file);
        QString m_dokument;
        in >> m_dokument;
        if(m_dokument != QString("*EIS_DRAW_2020*")){//Datei format überprüfen
            QMessageBox::information(this, "EIS_DRAW - WARNING:","Die Datei kann nicht geöffnet werden. Unbekannte Datei-Typ!");
            return;
        }

        set_datei_name(file_name);

        int new_main_vector_size;
        in >> new_main_vector_size;
        if(new_main_vector_size == 0){//falls geladene datei bzw vector leer
            //QMessageBox::information(this,"EIS_DRAW - INFO:", "Die Datei ist leer!.");
            main_vector.clear();
            update_from_vector();
            return;
        }
        //alten painterpath leeren (aktualisieren mit swap ohne zu delete)
        auto *path2 = new QPainterPath();
        m_painterpath->swap(*path2);
        delete path2;
        main_vector.clear();
        update_from_vector();
        update();
        //FOR LOOP
        for(int i = 0; i <new_main_vector_size; ++i) { // Iteration - ZeichenObjekte im Vector
            QString loading_ID;
            in >> loading_ID;

            QString setting_text;
            in >> setting_text;

            int point_number;
            in >> point_number;

            std::vector<QPoint> loading_points;
            QPoint input_number;
            for (int j = 0; j < point_number; ++j)
            {in >> input_number;
                loading_points.push_back(input_number);}

            shape *loadin_Shape;
            if (loading_ID == QString("Polygon"))
                loadin_Shape = new Polygon(loading_points);
            else if (loading_ID == QString("Ellipse"))
                loadin_Shape= new Ellipse(loading_points);
            else if (loading_ID == QString("Rechteck"))
                loadin_Shape = new Rechteck(loading_points);
            else if (loading_ID == QString("Text"))
                loadin_Shape = new shape_text(setting_text,loading_points);
            else{QMessageBox::information(this,
                                          "EIS_DRAW : Unbekannte geometrische Formen! "
                                          "Die Datei kann nicht geladen werden", file.errorString());
                return;}
            main_vector.push_back(loadin_Shape);
        }//FOR LOOP ENDE
        //display updaten mit update from vector
        update_from_vector();
        update();
        return;
    }}


void MainWindow::closeEvent(QCloseEvent *event) {
    //wenn vector geandert wurde frag nach save (s. save message box)
    if (!vector_geandert || (vector_geandert && !save_message_box()))
        MainWindow::close();
    else return;
}
//---------------------------------------------------------------------------------------------------------------------
//resize event
void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    //koordinaten updaten; wie immer mit dem Vektor
    MainWindow::update_from_vector();
}

//_____________________________________________________________________________________________________________________
//FOLGENDE METHODE / EVENTS SIND FÜR EIGENTLICHEN DRAW WICHTIG

//mouse move
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    //die Quelle ist nur für folgende Zeile (move + Left-button gedrückt)
    //https://stackoverflow.com/questions/10778936/qt-mousemoveevent-qtleftbutton
    //-> if ( e->buttons() & Qt::LeftButton ) (ende Quelle)
    if(event->buttons() & Qt::LeftButton){
        //pan Action
        while(pan_action->isChecked()){
            //
            if(!main_vector.empty()){
                setWindowTitle("EIS_Draw-"+ Datei_name + "*");
                vector_geandert = true;
            }
            ///alte path löschen damit display rictig updated werden kann
            auto *path2 = new QPainterPath();
            m_painterpath->swap(*path2);
            delete path2;
            //alle Positionen von shapes aktualisieren mit 2 forloops ein für shapes-vektor...
            for(int i = 0; i < main_vector.size(); ++i) {
                //(new_positions merken)
                std::vector<QPoint> new_points;
                new_points.reserve(main_vector[i]->get_points().size());
                //.....ein for loop für QPoints Vektor von shapes
                for (int j = 0; j < main_vector[i]->get_points().size(); ++j)
                    // Abstand zw neue mouse position und alte position (am anfang klicked) zu den Punkten addieren
                    new_points.push_back(main_vector[i]->get_points()[j] + (m_display->current_mouse_point - m_display->click_left));
                main_vector[i]->set_points(new_points);
            }
            //(damit shapes sich richtig bewegen)
            //alte click_left position aktualisieren; da es eigentlich "immernoch" geclicked wird (move & Qt::LeftButton)
            m_display->click_left = m_display->current_mouse_point;
            //repaint
            update();
            update_from_vector();
            repaint();
            //falls nicht mehr toggled -> ende while loop
            if(pan_action->isChecked())break;
        }
        //pan -Action Ende

        //ellipse -Rectecke actions
        // -> buttom right punkt aktualisieren (-> current mouse position von display)
        while ((rechteck_action->isChecked()||ellipse_action->isChecked()) && begin_draw) {
            shape_update_with_mousemove();
            if(rechteck_action->isChecked()||ellipse_action->isChecked())
                break;
        }
    }
    //-----------------
    //polygon action -> bei offenen polygonen letzten line durch mouse position von display
    // (bzw auf draw ebene) aktualisieren
    while (polygon_action->isChecked() && begin_draw) {
        shape_update_with_mousemove();
        if(polygon_action->isChecked())
            break;
    }
    //*/

    while(text_action->isChecked()){

        if(begin_text){

            //und mouse position ist also buttom_right;
            //topleft muss berechnet werden
            QPoint top_left_aktuell;
            top_left_aktuell.setX(m_display->current_mouse_point.x()- //mouse position.x - |width|
                                  std::abs(main_vector[main_vector.size() - 1]->get_points()[1].x()-main_vector[main_vector.size() - 1]->get_points()[0].x()));
            top_left_aktuell.setY(m_display->current_mouse_point.y()- //mouse position.y - |height|
                                  std::abs(main_vector[main_vector.size() - 1]->get_points()[1].y()-main_vector[main_vector.size() - 1]->get_points()[0].y()));
            //points von aktuellen shape(text) von main_vector holen
            std::vector<QPoint> new_points = main_vector[main_vector.size() - 1]->get_points();
            new_points.pop_back();
            new_points.pop_back();
            //top_left_aktuell pushen
            new_points.push_back(top_left_aktuell);
            //current mouse-position pushen
            new_points.push_back(m_display->current_mouse_point);
            //Points von "aktualisierten" Points von aktuellen shape in main Vektor pushen
            main_vector[main_vector.size() - 1]->set_points(new_points);
            //Repaint wie immer
            //neues PainterPath
            auto *path2 = new QPainterPath();
            m_painterpath->swap(*path2);
            delete path2;
            //update from vector und repaint
            update_from_vector();
            repaint();
        }
        update_from_vector();
        if(text_action->isChecked()){
            //main_vector.pop_back(); //vllt poreblematisch
            break;
        }

    }

    update();
}


void MainWindow::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);

    //Actions für right-Button-pressed-event
    if (event->button() == Qt::RightButton){

        while (polygon_action->isChecked()) {

            if(begin_draw){
                ///alte path löschen damit display rictig updated werden kann
                QPainterPath *path2 = new QPainterPath();
                m_painterpath->swap(*path2);
                delete path2;

                //letzte polygon merken
                std::vector<QPoint> new_points;
                //alte positionen
                new_points = main_vector[main_vector.size() - 1]->get_points();
                //polygon abschliessen -> line to anfangspunkt hinfügen
                new_points.push_back(new_points[0]);
                //polygon_punkte in main_vector aktualisieren/ überschreiben
                main_vector[main_vector.size() - 1]->set_points(new_points);
                //update display from vector
                update();
                update_from_vector();
                repaint();
                //bis neue polygon anfaengt; wird somit keine mouse position folgende linien gezeigt mit mousemove
                begin_draw=false;
            }
            if(polygon_action->isChecked())
                break;
        }


    }
     //Actions für left-Button-pressed-event
    if(event->button() == Qt::LeftButton){
        //Polygon action
        while(polygon_action->isChecked()){
            if(!begin_draw) {
               // std::cout << "begin_polygon" << std::endl;
                begin_draw = true;
                //da neue aenderug -> Title updaten und zeigen dass die veraenderungen ungespeichert sind
                setWindowTitle("EIS_Draw-"+ Datei_name + "*");
                vector_geandert = true;

                //neue Polygon in main_vector speichern
                //Rechtsklick anfangspunkt 2 mal gespeichert, danach 2. immer aktualisiert (1. line to -> 2. )
                std::vector<QPoint> new_vector = {m_display->click_left, m_display->click_left};
                auto *new_polygon = new Polygon(new_vector);
                //in main_vector speichern
                main_vector.push_back(new_polygon);
            }
            else{
                std::vector<QPoint> new_points;
                new_points = main_vector[main_vector.size() - 1]->get_points();
                //neuen RechtsKlickpunkt pushen
                new_points.push_back(m_display->click_left);
                main_vector[main_vector.size() - 1]->set_points(new_points);

                //wie immer _> Path und display updaten
                auto *path2 = new QPainterPath();
                m_painterpath->swap(*path2);
                delete path2;

                update();
                update_from_vector();
                repaint();
            }

            if(polygon_action->isChecked()){
                break;
            }

        }

        //Rechteck_action
        while(rechteck_action->isChecked()){
            if(!begin_draw){
                begin_draw = true;
                //da neue aenderug -> Title updaten und zeigen dass die veraenderungen ungespeichert sind
                setWindowTitle("EIS_Draw-"+ Datei_name + "*");
                vector_geandert = true;

                //neue Rechtecke in main_vector speichern
                //Am Anfang -> Topleft (newvector[0]) = BottomRight (newVector[1]); später B.r. aktualisieren
                std::vector<QPoint> new_vector = {m_display->click_left,m_display->click_left};
                auto *new_polygon = new Rechteck(new_vector);
                main_vector.push_back(new_polygon);
            }

            if(rechteck_action->isChecked()){
                break;
            }

        }

        //Ellipse_action
        while(ellipse_action->isChecked()){
            if(!begin_draw){
            begin_draw = true;
            //da neue aenderug -> Title updaten und zeigen dass die veraenderungen ungespeichert sind
            setWindowTitle("EIS_Draw-"+ Datei_name + "*");
            vector_geandert = true;

            //neue Ellipse in main_vector speichern
            std::vector<QPoint> new_vector = {m_display->click_left,m_display->click_left};
            auto *new_polygon = new Ellipse(new_vector);
            main_vector.push_back(new_polygon);
            }
            if(ellipse_action->isChecked()){
                break;
            }
        }

        while(text_action->isChecked()){
            if (begin_text){
                //da neue aenderug -> Title updaten und zeigen dass die veraenderungen ungespeichert sind
                setWindowTitle("EIS_Draw-"+ Datei_name + "*");
                vector_geandert = true;
                //std::cout<<"Text!"<<std::endl;
                QPoint top_left_aktuell;   //und mouse position ist also buttom_right;
                top_left_aktuell.setX(m_display->current_mouse_point.x()- //mouse position.x - |width|
                                      std::abs(main_vector[main_vector.size() - 1]->get_points()[1].x()-main_vector[main_vector.size() - 1]->get_points()[0].x()));
                top_left_aktuell.setY(m_display->current_mouse_point.y()- //mouse position.y - |height|
                                      std::abs(main_vector[main_vector.size() - 1]->get_points()[1].y()-main_vector[main_vector.size() - 1]->get_points()[0].y()));

                std::vector<QPoint> new_points = main_vector[main_vector.size() - 1]->get_points();
                new_points.pop_back();
                new_points.pop_back();
                //top_left_aktuell pushen
                new_points.push_back(top_left_aktuell);
                //current mouse-position pushen
                new_points.push_back(m_display->current_mouse_point);

                //Points von "aktualisierten" Points von aktuellen shape in main Vektok pushen
                auto *my_text = new shape_text(main_vector[main_vector.size()-1]->get_text());
                my_text->set_points(new_points);
                main_vector.push_back(my_text);

                //Repaint wie immer
                //neues PainterPath
                auto *path2 = new QPainterPath();
                m_painterpath->swap(*path2);
                delete path2;
                //update from vector und repaint
                update_from_vector();
                repaint();
            }
        if(text_action->isChecked()){
            break;
        }}

    }
    update();

}


void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    //wenn ellipse oder rechteck gezeichnet werden...(bzw. wenn eins von folgenden Actions toggled)
    while (ellipse_action->isChecked()||rechteck_action->isChecked()) {
        //wird beim release begin_draw beendet und damit letzte Version von den Ellipse/Recteck gespeichert
        if(begin_draw)
            begin_draw=false;
        if(ellipse_action->isChecked()||rechteck_action->isChecked())
            break;
    }
}


//die funktion rufen wir aus wenn wenn neue - tool gewählt wird und polygon nicht fertig gezeichnet wurde
void MainWindow::polygone_force_to_close() {
    if(begin_draw){
        //wenn polygon gezeichnet wurde..
        std::vector<QPoint> new_points;
        new_points = main_vector[main_vector.size() - 1]->get_points();
        //current mouse_position aus dem vector löschen da gerade ein neues tool gewählt
        // und eigentlich keine neue Linie gezeichnet wird
        new_points.pop_back();
        //Polygon abschliessen
        new_points.push_back(new_points[0]);
        main_vector[main_vector.size() - 1]->set_points(new_points);

        //Repaint wie immer
        //neues PainterPath
        auto *path2 = new QPainterPath();
        m_painterpath->swap(*path2);
        delete path2;
        //update from vector und repaint
        update_from_vector();
        repaint();
        begin_draw=false;
    }
}

//diese funktion ist für alle draw-Actions gedacht (ausser text)
//Methode löscht den letzte mouse position in m_points vector von shapes und speichert die current mouse posititon
//als letzten Element in m_points vector-> und updatet display with update_from_vector
void MainWindow::shape_update_with_mousemove() {
    //alle bisherige Points von Aktuellen shape merken
    std::vector<QPoint> new_points;
    new_points = main_vector[main_vector.size() - 1]->get_points();
    //letzte mouse-position löschen
    new_points.pop_back();
    //current mouse-position pushen
    new_points.push_back(m_display->current_mouse_point);
    //Points von "aktualisierten" Points von aktuellen shape in main Vektor pushen
    main_vector[main_vector.size() - 1]->set_points(new_points);
    //Repaint wie immer
    //neues PainterPath
    auto *path2 = new QPainterPath();
    m_painterpath->swap(*path2);
    delete path2;
    //update from vector und repaint
    update_from_vector();
    repaint();

}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    QWidget::keyPressEvent(event);
    //da event->key() == Qt::CTRL nicht funktioniert hat
    //also Quelle für nur die "Qt::Key_Control"
    //https://stackoverflow.com/questions/20746488/how-to-catch-ctrl-key-release
    if(event->key() == Qt::Key_Control){
        while(ellipse_action->isChecked()||rechteck_action->isChecked()){
            if(begin_draw){//wichtig! nur wenn ellpise/rechteck gezeochnet wird..
                std::vector<QPoint> new_points;
                new_points = main_vector[main_vector.size() - 1]->get_points();
                new_points.pop_back();
                new_points.push_back(m_display->current_mouse_point);
                //ARCHITEKTUR IDEE:
                //X Position von Anfangspunkt aktualisieren -> forcen zu quadrat bzw. kreise zu erzeugen
                //Die Idee ist hier dass man nur den X wert von Anfangspoint (ersten mouse_click) aendert
                //und   sodass ->  |x1-x0| = |y1-y0|
                //damit anfangs vorzeichen von x1 - x0 bei neuen x0 wert berücksichtigt wird, kann man schreiben
                // x0 = x1 - |y1-y0|*(x1-x0)/|(x1-x0)|
                new_points[0].setX(new_points[1].x()-std::abs(new_points[1].y()-new_points[0].y()) //x1 - |y1-y0|
                *(new_points[1].x()-new_points[0].x())/std::abs(new_points[1].x()-new_points[0].x())) ; //*(x1-x0)/|(x1-x0)|
                //und aktualisieren wir somit die alte points
                main_vector[main_vector.size() - 1]->set_points(new_points);
                //update display
                auto *path2 = new QPainterPath();
                m_painterpath->swap(*path2);
                delete path2;
                update_from_vector();
                repaint();
            }
            if(ellipse_action->isChecked()||rechteck_action->isChecked())
                break;//nochmal gecheckt bzw einen anderen tool gewählt -> break while loop
        }

    }
}

//https://doc.qt.io/qt-5/qwheelevent.html#angleDelta
//also wenn scroll up -> angleDelta > 0
//also wenn scroll down -> angleDelta < 0
void MainWindow::wheelEvent(QWheelEvent *event) {
    QWidget::wheelEvent(event);

    //Zoom (in) Action
    if (event->angleDelta().y() > 0){
        //while Zoom Action is aktiv (tooggled):
        while(zoom_action->isChecked()){
            if(!main_vector.empty()){//datei wird dadurch geändert -> * am ende
                setWindowTitle("EIS_Draw-"+ Datei_name + "*");
                vector_geandert = true;
            }
            ///alte path löschen damit display rictig updated werden kann
            QPainterPath *path2 = new QPainterPath();
            m_painterpath->swap(*path2);
            delete path2;

            //alle Positionen von shapes 0.1 zoomen mit doppelforloop ein für shapes-vektor...
            for(int i = 0; i < main_vector.size(); ++i) {
                //(new_positions merken)
                std::vector<QPoint> new_points;
                new_points.reserve(main_vector[i]->get_points().size());
                //.....eins für QPoints Vektor von shapes
                if(main_vector[i]->ID() == "Text"){
                    //1)bounding rect ist jetzt letztere text rect
                    new_points.push_back(main_vector[i]->get_points()[0]);
                    new_points.push_back(main_vector[i]->get_points()[1]);
                    //neue text rect vergrösern -> punkten iterativ aktualisieren
                    for (int j = 2; j < main_vector[i]->get_points().size(); ++j)
                        //mouse_position - (Abstand zw. mouse und shape Point) * (1 + scroll faktor*0.1 )
                        //Also vergrößerung relativ zu mouse_position (scroll faktor *10 %)
                        new_points.push_back(m_display->current_mouse_point-
                                             (m_display->current_mouse_point-main_vector[i]->get_points()[j]) * (1 + mouse_scroll_factor*0.1));}
                else{//für alle andere geometrisch shapes punkten iterativ aktualisieren
                    for (int j = 0; j < main_vector[i]->get_points().size(); ++j)
                        //mouse_position - (Abstand zw. mouse und shape Point) * (1 + scroll faktor*0.1 )
                        //Also vergrößerung relativ zu mouse_position (scroll faktor *10 %)
                        new_points.push_back(m_display->current_mouse_point-
                                             (m_display->current_mouse_point-main_vector[i]->get_points()[j]) * (1 + mouse_scroll_factor*0.1));}

                main_vector[i]->set_points(new_points);
            }
            //repaint
            update();
            update_from_vector();
            repaint();
            //falls nicht toggled -> ende while loop
            if(zoom_action->isChecked())break;
        }
    }


    else if (event->angleDelta().y() < 0){    ////Zoom (out) Action
        //Zoom Action is aktiv (tooggled):
        //Zoom (out) Action
        while(zoom_action->isChecked()){
            ///alte path löschen damit display rictig updated werden kann
            QPainterPath *path2 = new QPainterPath();
            m_painterpath->swap(*path2);
            delete path2;

            //alle Positionen von shapes 0.1 aus-zoomen mit doppelforloop ein für shapes-vektor...
            for(int i = 0; i < main_vector.size(); ++i) {
                //(new_positions merken)
                std::vector<QPoint> new_points;
                new_points.reserve(main_vector[i]->get_points().size());
                //.....eins für QPoints Vektor von shapes
                if(main_vector[i]->ID() == "Text"){//wie oben bei zoom in..
                    new_points.push_back(main_vector[i]->get_points()[0]);
                    new_points.push_back(main_vector[i]->get_points()[1]);
                    for (int j = 2; j < main_vector[i]->get_points().size(); ++j)
                        //mouse_position - (Abstand zw. mouse und shape Point) * (1 - scroll faktor*0.1 )
                        //Also Verkleinerung relativ zur mouse_position (scroll faktor * 10 %)
                        new_points.push_back(m_display->current_mouse_point-
                                             (m_display->current_mouse_point-main_vector[i]->get_points()[j]) * (1-mouse_scroll_factor*0.1) );
                    main_vector[i]->set_points(new_points);}
                else{
                for (int j = 0; j < main_vector[i]->get_points().size(); ++j)
                    //mouse_position - (Abstand zw. mouse und shape Point) * (0.9)
                    //Also Verkleinerung relativ zur mouse_position (1 - scroll faktor*0.1 )
                    new_points.push_back(m_display->current_mouse_point-
                                         (m_display->current_mouse_point-main_vector[i]->get_points()[j]) * (1-mouse_scroll_factor*0.1) );
                main_vector[i]->set_points(new_points);}
            }
            update();
            update_from_vector();
            repaint();

            if(zoom_action->isChecked())break;
        }}


}

//Quelle QInputDialog: https://doc.qt.io/qt-5/qinputdialog.html
//diese Methode wird aufgerufen wenn Text Action toggled
void MainWindow::new_text() {
    if (begin_text)
    {//falls zum schliessen text action changed wurde -> letzt gezeigten, in main vector ungespeicherten text löschen
        main_vector.pop_back();
        ///alte path löschen damit display rictig updated werden kann
        QPainterPath *path2 = new QPainterPath();
        m_painterpath->swap(*path2);
        delete path2;
        update_from_vector();
        begin_text=false;}
    else if (!(begin_text || polygon_action->isChecked() ||rechteck_action->isChecked()||zoom_action->isChecked()
             ||pan_action->isChecked()||ellipse_action->isChecked())){
        bool ok;
        QInputDialog inputDialog;
        QString text = inputDialog.getText(this, "EIS_Draw: Text",
                                             "Bitte Text einegeben:", QLineEdit::Normal,
                                             QString("42 is the best number ever"), &ok);
        if (ok && !text.isEmpty()){
            //man kann hier die update_from_vector funktion nicht benutzen da in dem Fall Painter nicht aktiv ist
            //also -> update_from_vector nachmachen
            m_canvas = QImage(m_display->width(),m_display->height(),QImage::Format_RGBA8888);
            m_canvas.fill(QColor(230,230,230));
            m_painter->begin(&m_canvas);
            m_painter->setPen(Qt::black);

            //new text;
            shape_text *my_text = new shape_text(text);

            //boundingRect für text rechnen und als ersten rect (bzw erste 2 points) in vector merken
            QRect zw_rect = QRect(0,0,0,0);
            QRect rect = m_painter->boundingRect(zw_rect,Qt::AlignAbsolute,my_text->get_text());//QRectF
            QPoint point1 = rect.topLeft() ;//topleft, immer (0,0)
            QPoint point2 = rect.bottomRight(); //bottom right

            std::vector<QPoint> new_vector = {point2,point1,point2,point1};
            //also -> ersten 2 QPoints sind immer boundingrect und nur bei pan und zoom action werden die aktualisiert

            //points von new text in new text; new text in main vector gespeichert
            my_text->set_points(new_vector);
            main_vector.push_back(my_text);

            //update display
            draw_from_vector();
            m_painter->end();

            m_display->clear();
            m_display->setPixmap(QPixmap::fromImage(m_canvas));
            m_display->repaint();
            begin_text= true;
        }
    }

}
