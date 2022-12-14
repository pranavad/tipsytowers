#include "motion.h"
#include "FeedbackControlTasks.h"

#include <System/biros.h>
#include <hardware/hardware.h>

void prepare_skin(TaskVariable *skin, const arr& skinState, bool cut_and_nil);

void DoNothing_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  listDelete(TVs);
  requiresInit = false;
}

void DoNothing_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
}

void Stop_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  listDelete(TVs);
  TaskVariable *q = new DefaultTaskVariable("qitself", ors, qItselfTVT, 0, 0, 0, 0, 0);
  TVs = {q};
  activateAll(TVs, true);
  q->y_prec=0.;
  q->v_prec=1e2;
  q->y_target.setZero();
  q->v_target.setZero();
  TVs.append(q);
  requiresInit = false;
}

void Stop_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
}

void Homing_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  double margin = biros().getParameter<double>("TV_margin",.03);
  arr limits = biros().getParameter<arr>("TV_limits");
  listDelete(TVs);
  TaskVariable *q    = new DefaultTaskVariable("qitself", ors, qItselfTVT, 0, 0, 0, 0, 0);
  TaskVariable *col  = new DefaultTaskVariable("collision", ors, collTVT, 0, 0, 0, 0, ARR(margin));
  TaskVariable *lim  = new DefaultTaskVariable("limits", ors, qLimitsTVT, 0, 0, 0, 0, limits);
  TVs = {q, col, lim};
  activateAll(TVs, true);
  q->y_prec=0;
  q->v_prec=biros().getParameter<double>("TV_q_vprec", 1e0);
  q->y_target.setZero();
  q->v_target.setZero();
  col->y_prec=biros().getParameter<double>("TV_col_yprec", 1e0);
  col->v_prec=0;
  col->y_target.setZero();
  col->v_target.setZero();
  lim->y_prec=biros().getParameter<double>("TV_lim_yprec", 1e3);
  lim->v_prec=0;
  lim->y_target.setZero();
  lim->v_target.setZero();
  requiresInit = false;
}

void Homing_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
  TaskVariable *q = TVs(0);
  q->v_target = -5.*q->y;
  double vmax=.3, v=absMax(q->v_target);
  if (v>vmax) q->v_target*=vmax/v;
}

void OpenHand_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  count=0;
  listDelete(TVs);
  TaskVariable *q = new DefaultTaskVariable("qitself", ors, qItselfTVT, 0, 0, 0, 0, 0);
  TVs = {q};
  activateAll(TVs, true);
  q->y_prec=1e1;
  q->v_prec=biros().getParameter<double>("TV_q_vprec", 1e-1);
  q->y_target.setZero();
  q->v_target.setZero();
  q->y_target = q->y;
  q->y_target(8)=q->y_target(10)=q->y_target(12)=-1.;
  q->y_target(9)=q->y_target(11)=q->y_target(13)= .6;
  requiresInit = false;
}

void OpenHand_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
  count ++;
  if(count > 300) {
    done = true;
  }
}

void CloseHand_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  biros().getVariable(skinPressure, "SkinPressure", NULL);

  count = 0;
  
  //double pressure = biros().getParameter<double>("closeHandPressure", .03);
  listDelete(TVs);
  arr skinIndex(6);
  skinIndex(0) = ors.getBodyByName("tip3")->index;
  skinIndex(1) = ors.getBodyByName("fing3")->index;
  skinIndex(2) = ors.getBodyByName("tip1")->index;
  skinIndex(3) = ors.getBodyByName("fing1")->index;
  skinIndex(4) = ors.getBodyByName("tip2")->index;
  skinIndex(5) = ors.getBodyByName("fing2")->index;
  TaskVariable *q    = new DefaultTaskVariable("qitself", ors, qItselfTVT, 0, 0, 0, 0, 0);
  TaskVariable *skin = new DefaultTaskVariable("skin", ors, skinTVT, 0, 0, 0, 0, skinIndex);
  TVs = {q, skin};
  activateAll(TVs, true);
  q->y_prec=0;
  q->v_prec=biros().getParameter<double>("TV_q_vprec", 1e-2);
  q->y_target.setZero();
  q->v_target.setZero();
  skin->y_prec=biros().getParameter<double>("TV_skin_yprec", 1e3);
  skin->v_prec=0;
  skin->y_target=ARR(.007, 0, .02, 0, .007, 0);//pressure, 0, pressure, 0, pressure, 0);
  skin->v_target.setZero();
  requiresInit = false;
}

void CloseHand_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
  TaskVariable *skin = TVs(1);
  arr skinState = ARR(.01,.01,.01,.01,.01,.01);
  if(skinPressure) skinPressure->get_y_real(NULL); //TODO specify process
  prepare_skin(skin, skinState, true);

  count ++;
  //decide if done
  //cout << max(skin->y_target - skinState) << " (max) " << endl;
  if(max(skin->y_target - skinState)<.0071 || count > 600) {
    done = true;
  }
}

void Reach_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  double margin = biros().getParameter<double>("TV_margin",.03);
  arr limits = biros().getParameter<arr>("TV_limits");
  listDelete(TVs);
  TaskVariable *eff  = new DefaultTaskVariable("endeffector", ors, posTVT, "m9", "<t(0 0 -.24)>", 0, 0, 0);
  TaskVariable *col  = new DefaultTaskVariable("collision", ors, collTVT, 0, 0, 0, 0, ARR(margin));
  TaskVariable *lim  = new DefaultTaskVariable("limits", ors, qLimitsTVT, 0, 0, 0, 0, limits);
  TVs = {eff, col, lim};
  activateAll(TVs, true);
  eff->y_prec= 0.;
  eff->v_prec= 1e-1;
  eff->y_target.setZero();
  eff->v_target.setZero();
  col->y_prec=biros().getParameter<double>("TV_col_yprec", 1e0);
  col->v_prec=0;
  col->y_target.setZero();
  col->v_target.setZero();
  lim->y_prec=biros().getParameter<double>("TV_lim_yprec", 1e3);
  lim->v_prec=0;
  lim->y_target.setZero();
  lim->v_target.setZero();
  requiresInit = false;
}

void Reach_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
  TaskVariable *eff = TVs(0);
  eff->v_target = reachPoint - eff->y;
  double vmax=.2, v=length(eff->v_target);
  if (v>vmax) eff->v_target*=vmax/v;
}

void Gamepad_FeedbackControlTask::initTaskVariables(const mlr::KinematicWorld& ors) {
  //access the gamepad Variable
  biros().getVariable(gamepadState, "GamepadState", NULL); //TODO get process pid
  biros().getVariable(skinPressure, "SkinPressure", NULL);
  gamepadRate = biros().getParameter<double>("GamepadRate");
  
  double margin = biros().getParameter<double>("TV_margin", .03);
  arr limits = biros().getParameter<arr>("TV_limits");
  arr skinIndex(6);
  skinIndex(0) = ors.getBodyByName("tip3")->index;
  skinIndex(1) = ors.getBodyByName("fing3")->index;
  skinIndex(2) = ors.getBodyByName("tip1")->index;
  skinIndex(3) = ors.getBodyByName("fing1")->index;
  skinIndex(4) = ors.getBodyByName("tip2")->index;
  skinIndex(5) = ors.getBodyByName("fing2")->index;
  
  listDelete(TVs);
  TaskVariable *eff  = new DefaultTaskVariable("endeffector", ors, posTVT, "m9", "<t(0 0 -.24)>", 0, 0, 0);
  TaskVariable *q    = new DefaultTaskVariable("qitself", ors, qItselfTVT, 0, 0, 0, 0, 0);
  TaskVariable *rot  = new DefaultTaskVariable("endeffector rotation", ors, rotTVT, "m9", 0, 0, 0, 0);
  TaskVariable *col  = new DefaultTaskVariable("collision", ors, collTVT, 0, 0, 0, 0, ARR(margin));
  TaskVariable *lim  = new DefaultTaskVariable("limits", ors, qLimitsTVT, 0, 0, 0, 0, limits);
  TaskVariable *skin = new DefaultTaskVariable("skin", ors, skinTVT, 0, 0, 0, 0, skinIndex);
  TVs = {eff, q, rot, col, lim, skin};
  activateAll(TVs, true);
  eff->y_prec=0.; //biros().getParameter<double>("TV_eff_yprec", 1e3);
  eff->v_prec=defaultEff_vprec=biros().getParameter<double>("TV_eff_vprec", 1e1);
  q->y_prec=0;
  q->v_prec=biros().getParameter<double>("TV_q_vprec", 1e0);
  rot->y_prec=0.;
  rot->v_prec=biros().getParameter<double>("TV_rot_vprec", 1e-1);
  col->y_prec=biros().getParameter<double>("TV_col_yprec", 1e0);
  col->v_prec=0;
  col->y_target.setZero();
  col->v_target.setZero();
  lim->y_prec=biros().getParameter<double>("TV_lim_yprec", 1e3);
  lim->v_prec=0;
  lim->y_target.setZero();
  lim->v_target.setZero();
  skin->y_prec=biros().getParameter<double>("TV_skin_yprec", 1e3);
  skin->v_prec=0;
  requiresInit = false;
}

void Gamepad_FeedbackControlTask::updateTaskVariableGoals(const mlr::KinematicWorld& ors) {
  TaskVariable *eff = TVs(0);
  TaskVariable *q = TVs(1);
  TaskVariable *rot = TVs(2);
  TaskVariable *col = TVs(3);
  TaskVariable *lim = TVs(4);
  TaskVariable *skin = TVs(5);
  
  activateAll(TVs, false);
  col->active=true;
  lim->active=true;
  q->active=true;
  q->y_prec=0.;
  q->v_target.setZero();
  eff->v_prec=defaultEff_vprec;
  
  arr skinState = skinPressure->get_y_real(NULL); //TODO specify process
  intA gamepads = gamepadState->get_state(NULL);
  if (gamepads.N<8) { gamepads.resize(8);  gamepads.setZero(); }
  
  prepare_skin(skin, skinState, gamepads(0)!=2);
  
  switch (gamepads(0)) {
    case 0: { //(NIL) motion rate control
      eff->active=true;
      eff->y_target = eff->y;
      eff->v_target(0) = -gamepadRate*mlr::sign(gamepads(3))*(.25*(exp(mlr::sqr(gamepads(3))/10000.)-1.));
      eff->v_target(1) = +gamepadRate*mlr::sign(gamepads(6))*(.25*(exp(mlr::sqr(gamepads(6))/10000.)-1.));
      eff->v_target(2) = -gamepadRate*mlr::sign(gamepads(2))*(.25*(exp(mlr::sqr(gamepads(2))/10000.)-1.));
      break;
    }
    case 1: { //(1) homing
      q->v_target = -5.*q->y;
      double vmax=.3, v=absMax(q->v_target);
      if (v>vmax) q->v_target*=vmax/v;
      break;
    }
    case 2: { //(2) CRAZY tactile guiding
      skin->active=true;
      skin->y_prec = 5e1;
      skin->y_target=ARR(.0, .0, .0, .0, .0, .0);
      //ON SIMULATION: since it is set to (.01, .01, .01) this will always give a repelling force!
      break;
    }
    case 4: { //(3) controlling the rotation rate
      eff->active=true;
      eff->v_target = 0.;      eff->v_prec = 1e5;
      rot->active=true;
      rot->v_target(0) = -3.*gamepadRate*mlr::sign(gamepads(3))*(.25*(exp(mlr::sqr(gamepads(3))/10000.)-1.));
      rot->v_target(1) = +3.*gamepadRate*mlr::sign(gamepads(6))*(.25*(exp(mlr::sqr(gamepads(6))/10000.)-1.));
      rot->v_target(2) = -3.*gamepadRate*mlr::sign(gamepads(1))*(.25*(exp(mlr::sqr(gamepads(1))/10000.)-1.));
      break;
    }
    case 8: { //(4) motion rate without rotation
      rot->active=true;
      rot->v_target.setZero();
    }
    case 256: { //(select)close hand
      skin->active=true;
      skin->y_target=ARR(.007, 0, .02, 0, .007, 0);
      break;
    }
    case 512: { //(start)open hand
      arr target = q->y;
      target(8)=target(10)=target(12)=-.8;
      target(9)=target(11)=target(13)= .6;
      q->v_target = 1.*(target - q->y);
      double vmax=.5, v=absMax(q->v_target);
      if (v>vmax) q->v_target*=vmax/v;
      break;
    }
  }
}

void prepare_skin(TaskVariable *skin, const arr& skinState, bool cut_and_nil) {
  if (skinState.N) {
    skin->y = skinState;
  } else {
    skin->y = ARR(.01, 0, .01, 0, .01, 0);
  }
  
  if (cut_and_nil) {
    //cut of the skin signal... :-(
    for (uint i=0; i<skin->y.N; i++) if (skin->y(i)>.02) skin->y(i)=.02;
    //nil certain parts of the skin jacobian: all arm joints and hand 0-joint... :-(
    for (uint i=0; i<skin->J.d0; i++) for (uint j=0; j<8; j++) skin->J(i, j)=0.;
    transpose(skin->Jt, skin->J);
  }
  
}

