#ifndef SD_graspISF_h
#define SD_graspISF_h

#include <Core/array.h>
#include <MT/robot.h>
#include "potentialTaskVariables.h"
#include "miscTaskVariables.h"

struct GraspObject;
struct ControllerProcess;

struct GraspISFTask:public TaskAbstraction{
  GraspObject *graspobj;
  GraspISFTask();
  double phiAtFrame(mlr::Transformation& X, arr &grad, double *sig);
  void plot_append_data(ControllerProcess*);
  void plot_all();

  mlr::Array<mlr::Shape*> tipsN;
  mlr::Array<mlr::Shape*> fingsN;
  arr  skins;
  mlr::Shape *palm; // palm center wrt wrist body

  double skin_prec;
  bool open_skin;
  arr skin_state;

  bool  grip;

  uint t, T; // time index and max steps

  // plot data
  arr plot_data; // here we collect the data to plot

  /* ------- Task Vars -------- */
  TaskVariableList TVs_all; 
  TaskVariable *TV_tipAlign;
  TaskVariable *TV_fingAlign;
  TaskVariable *TV_palm;
  TaskVariable *TV_oppose;
  PotentialValuesTaskVariable *TV_zeroLevel;

  /* ------ TAsk Abstraction ----- */

  virtual void updateTaskVariables(ControllerProcess*); //overloading the virtual
  virtual void initTaskVariables(ControllerProcess*);

  /* configuration */
  double tv_palm_prec_m,
         tv_palm_trgt_m,
         tv_oppose_prec,
         tv_zeroLevel_prec_m,
         tv_fingAlign_prec_m,
         tv_fingAlign_sin_m,
         tv_tipAlign_prec_m,
         tv_q_v_prec,
         tv_q_y_prec,

         tv_skin_trgt,
         tv_skin_fake,
         tv_skin_prec_thr_zeroLevel;
};



#ifdef  MLR_IMPLEMENTATION
#  include "graspISF.cpp"
#endif


#endif// header ifdef
