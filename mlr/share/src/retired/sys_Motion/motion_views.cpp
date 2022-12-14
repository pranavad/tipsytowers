#include "motion.h"

#include <Gui/opengl.h>

REGISTER_VIEW(PoseView, arr);

PoseView::PoseView():View() {
  geo.init("GeometricState", NULL); //the pose view gets itself a copy of the central ors
}

PoseView::PoseView(arr& q, RWLock *lock, GtkWidget *container):View() {
  geo.init("GeometricState", NULL); //the pose view gets itself a copy of the central ors
  object = &q;
  objectLock = lock;
  gtkNew(container);
}

void PoseView::glInit() {
  gl->setClearColors(1.,1.,1.,1.);
  gl->camera.setPosition(10.,-15.,8.);
  gl->camera.focus(0,0,1.);
  gl->camera.upright();
  gl->update();
}

void PoseView::glDraw() {
  if(objectLock) objectLock->readLock();
  arr q = *(arr*) object;
  if(objectLock) objectLock->unlock();
  geo.pull();
  uint n=geo().ors.getJointStateDimension();
  if(q.nd==1){
    if (q.N==2*n) q = q.sub(0,q.N/2-1); //check dynamic state
    if (q.N!=n){ MLR_MSG("pose view on wrong dimension");  return; }
    geo().ors.setJointState(q); //it's using the ors copy to interpret the pose array (field->p)
  }
  if(q.nd==2){
    t++;
    if(t>=q.d0) t=0;
    geo().ors.setJointState(q[t]);
  }
  glStandardScene(NULL);
  geo().ors.glDraw(); //..and draws it
}



