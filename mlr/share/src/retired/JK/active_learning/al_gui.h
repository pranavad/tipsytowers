#ifndef _GUI_H_
#define _GUI_H_

#include <System/biros.h>

class sGui;

class GuiDataV : public Variable {
  public:
    GuiDataV() : Variable("Gui Data Variable"), sample(NULL) {} ;
    mlr::Array<arr>* sample;
};

class Gui : public Process {
  private:
    sGui* s;

  public:
    Gui(const char* orsFile);
      ~Gui();
    void open();
    void step();
    void close();

    GuiDataV* guiData;
};

#endif
