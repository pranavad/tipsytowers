#include <motion/motion.h>
#include <motion/FeedbackControlTasks.h>
#include <biros/biros_views.h>

struct MyTask:FeedbackControlTaskAbstraction{
  TaskVariable *TV_eff;
  virtual void initTaskVariables(const mlr::KinematicWorld &ors){
    listDelete(TVs);
    TV_eff  = new DefaultTaskVariable("endeffector", ors, posTVT, "m9", "<t(0 0 -.24)>", NULL, NULL, NoArr);
    TVs = LIST<TaskVariable>(*TV_eff);
    requiresInit = false;
    TV_eff->active=true;
  }
  virtual void updateTaskVariableGoals(const mlr::KinematicWorld& ors){
    if(false){ //position control
      TV_eff->y_prec  =1e3;
      TV_eff->y_target = TV_eff->y_target + ARR(0.,0.,.002); //move upward
    }else{ //velocity control
      TV_eff->v_prec =1e3;
      TV_eff->v_target = ARR(0.,0.,.05); //move upward (recall tau=0.01 -> results in same speed)
    }
  }
};

int main(int argc, char** argv){
  mlr::initCmdLine(argc, argv);

  // variables
  GeometricState geometricState;
  MotionPrimitive motionPrimitive;
  HardwareReference hardwareReference;

  // processes
  Process* ctrl = newMotionController(&hardwareReference, &motionPrimitive, NULL);

  new OrsView(geometricState.ors, &geometricState.rwlock);
  new PoseView(hardwareReference.q_reference, &hardwareReference.rwlock);
  //new InsideOut();

  MyTask myTask;
  motionPrimitive.writeAccess(NULL);
  motionPrimitive.mode = MotionPrimitive::feedback;
  motionPrimitive.feedbackControlTask = &myTask;
  motionPrimitive.forceColLimTVs = false;
  motionPrimitive.deAccess(NULL);

  uint mode=mlr::getParameter<uint>("mode", 1);
  if(mode==0){ //non-threaded
    ctrl->open();
//  view.open();
    for(;;){
      ctrl->step();
//    view.step(); the view is automatially opened as thread and stepsOnListen...
      mlr::wait();
    }
  }
  if(mode==1){
    ctrl->threadLoopWithBeat(.01);
    mlr::wait(30.);
  }

  close(biros().processes);
  biros().dump();
  
  cout <<"bye bye" <<endl;
};



