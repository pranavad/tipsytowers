#ifndef MLR_gui_h
#define MLR_gui_h

#include <QtCore/QTextStream>
#include <QtCore/QEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <iostream>
#include "win_ui.h"

class Gui:public QDialog{
Q_OBJECT
public:
  Ui_win ui;

  Gui(){
    ui.setupUi(this);
    show();

    ui.Slider2->setValue(13);
    ui.filename->setText("init.txt");
    ui.op2->toggle();
    //OpenGL1->add(0,&glDraw);
    //OpenGL2->add(0,&glDraw);

    connect(ui.okB, SIGNAL(clicked()), this, SLOT(read()));
    connect(ui.cancelB, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui.browseB, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui.userB, SIGNAL(clicked()), this, SLOT(userfunction()));
  }

public slots:
  void browse(){
    //const char* str=;
    ui.filename->setText(QFileDialog::getOpenFileName(0,"tst.h","*",""));
  }
  void read(){
    std::cout <<"hello" <<std::endl;
    std::cout <<"selected number: " <<ui.number->value() <<"\n";
    //std::cout <<"selected option: " <<(const char*)ui.group->selected()->text() <<"\n";
    std::cout <<"selected file:   " <<(const char*)ui.filename->text().toAscii() <<"\n";
    accept();
    //qApp->exit_loop();
  }
  void userfunction(){
    QMessageBox box("error","This is the USER FUNCTION",QMessageBox::Critical,
                    QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
    box.exec();
    
    //QGLWidget gl;
    /*
      NeuralGas ngas;
      ngas <<FILE("../neuralGas/output");
      NeuralGas::node n;
      forall(n,ngas){ n->feat.x=n->weights(0); n->feat.y=n->weights(1); }
      ngas.watch();
    */
  }
};

#endif
