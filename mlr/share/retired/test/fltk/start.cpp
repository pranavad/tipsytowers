// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "start.h"

Fl_Menu_Item ParameterGUI::menu_bar[] = {
 {"item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"&File", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"Open", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {"recent", 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {"bla", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item ParameterGUI::menu_menu[] = {
 {"item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

Fl_Menu_Item ParameterGUI::menu_choice[] = {
 {"item", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0}
};

ParameterGUI::ParameterGUI() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(340, 235);
    w = o;
    o->color((Fl_Color)FL_FOREGROUND_COLOR);
    o->user_data((void*)(this));
    o->align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
    { Fl_Value_Slider* o = new Fl_Value_Slider(30, 14, 35, 177, "slider:");
      o->textsize(14);
    } // Fl_Value_Slider* o
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 50, 340, 20, "bar");
      o->menu(menu_bar);
    } // Fl_Menu_Bar* o
    { Fl_Menu_Button* o = new Fl_Menu_Button(40, 0, 75, 20, "menu");
      o->menu(menu_menu);
    } // Fl_Menu_Button* o
    { Fl_Choice* o = new Fl_Choice(135, 80, 72, 15, "choice:");
      o->down_box(FL_BORDER_BOX);
      o->menu(menu_choice);
    } // Fl_Choice* o
    o->end();
  } // Fl_Double_Window* o
}
