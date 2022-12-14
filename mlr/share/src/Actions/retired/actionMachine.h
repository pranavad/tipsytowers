#pragma once

#include <Core/array.h>
//#include <System/engine.h>
#include <RosCom/roscom.h>
#include <RosCom/subscribeAlvarMarkers.h>
#include <Hardware/gamepad/gamepad.h>

#include "actions.h"

//===========================================================================
// Module System integration
/** ActionMachine integrates the GroundedActions into the module system.
 *
 * The ActionMachine (usually a singleton?) does two things in each step
 *  (1) It checks the states of all GroundedActions and transitions them depending on their dependencies. This
 *      also removes actions that have been completed from the list A.
 *  (2) It takes all currently active actions in A and translates these to concrete motion control using
 *      operational space control.
 *  The ActionMachine should loop (with ~0.01) as a module.
 *  The user methods allow the user to directly modify the ActionList A */
struct ActionMachine : Thread {
  struct sActionMachine *s;

  ACCESS(CtrlMsg, ctrl_ref);
  ACCESS(CtrlMsg, ctrl_obs);
  ACCESS(arr, gamepadState);
  ACCESS(ActionL, A);
  ACCESS(Graph, KB);
  ACCESS(AlvarMarkers, ar_pose_marker);

  ActionMachine();
  ~ActionMachine();

  arr Kp, Kd;
  //-- user methods
  const mlr::KinematicWorld *world;
  ofstream fil;
  bool initStateFromRos;

  /** Add a sequence of actions started one after the other..
   * The first one is started right away, the others depend on
   * the previous action and are queued.
   * TODO use initilizer_list or varadic templates to allow arbitrarily many
   * actions */
  void add_sequence(Action *action1,
                    Action *action2,
                    Action *action3=NULL,
                    Action *action4=NULL);

  void removeAction(Action* a, bool hasLock=false);
  /// Block till the given action `a` is done
  void waitForActionCompletion(Action* a);
  /// Block till all actions (excluding CoreTasks) are done
  void waitForActionCompletion();
  /// Block till the quit symbol is true in the KB
  void waitForQuitSymbol();

  void parseTaskDescription(Graph& td);
  void parseTaskDescriptions(const Graph& tds);

  /// @name module implementations
  void open();
  void step();
  void close();

  void transition();
  void transitionFOL(double time, bool forceChaining);
  double getContactForce();
};

//===========================================================================

struct ActionSystem{
  ACCESS(CtrlMsg, ctrl_ref);
  ACCESS(CtrlMsg, ctrl_obs);
  ACCESS(arr, gamepadState);
  ActionMachine *machine;
  ActionSystem() : machine(NULL) {
    machine = addModule<ActionMachine>(NULL, /*Module::loopWithBeat,*/ .01);
    new GamepadInterface;

    if(mlr::getParameter<bool>("useRos", false)) {
      cout << "USING ROS" <<endl;
      new RosCom_Spinner();
      new SubscriberConvNoHeader<marc_controller_pkg::JointState, CtrlMsg, &conv_JointState2CtrlMsg>("/marc_rt_controller/jointState", ctrl_obs);
      new PublisherConv<marc_controller_pkg::JointState, CtrlMsg, &conv_CtrlMsg2JointState>("/marc_rt_controller/jointReference", ctrl_ref);
#ifdef MLR_ROS
      addModule<ROSSUB_ar_pose_marker>(NULL, /*Module::loopWithBeat,*/ 0.1);
#endif
      // addModule<RosCom_ForceSensorSync>(NULL, /*Module::loopWithBeat,*/ 1.);
    }
    //connect();
  }
};
