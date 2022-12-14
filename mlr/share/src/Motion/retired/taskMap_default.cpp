/*  ---------------------------------------------------------------------
    Copyright 2014 Marc Toussaint
    email: marc.toussaint@informatik.uni-stuttgart.de
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a COPYING file of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>
    -----------------------------------------------------------------  */
#include "taskMap_default.h"

TaskMap_Default::TaskMap_Default(TaskMap_DefaultType _type,
                               int iShape, const mlr::Vector& _ivec,
                               int jShape, const mlr::Vector& _jvec,
                               const arr& _params):type(_type), i(iShape), j(jShape){

  if(&_ivec) ivec=_ivec; else ivec.setZero();
  if(&_jvec) jvec=_jvec; else jvec.setZero();
  if(&_params) params=_params;
}

TaskMap_Default::TaskMap_Default(TaskMap_DefaultType _type, const mlr::KinematicWorld &G,
                               const char* iShapeName, const mlr::Vector& _ivec,
                               const char* jShapeName, const mlr::Vector& _jvec,
                               const arr& _params):type(_type), i(-1), j(-1){
  mlr::Shape *a = iShapeName ? G.getShapeByName(iShapeName):NULL;
  mlr::Shape *b = jShapeName ? G.getShapeByName(jShapeName):NULL;
  if(a) i=a->index;
  if(b) j=b->index;
  if(&_ivec) ivec=_ivec; else ivec.setZero();
  if(&_jvec) jvec=_jvec; else jvec.setZero();
  if(&_params) params=_params;
}


void TaskMap_Default::phi(arr& y, arr& J, const mlr::KinematicWorld& G) {
  mlr::Body *body_i = i<0?NULL: G.shapes(i)->body;
  mlr::Body *body_j = j<0?NULL: G.shapes(j)->body;

  //get state
  switch(type) {
    case posTMT:{
      mlr::Vector vec_i = i<0?ivec: G.shapes(i)->rel*ivec;
      mlr::Vector vec_j = j<0?jvec: G.shapes(j)->rel*jvec;
      if(body_j==NULL) {
        G.kinematicsPos(y, J, body_i, &vec_i);
        y -= conv_vec2arr(vec_j);
        break;
      }
      mlr::Vector pi = body_i->X * vec_i;
      mlr::Vector pj = body_j->X * vec_j;
      mlr::Vector c = body_j->X.rot / (pi-pj);
      y = conv_vec2arr(c);
      if(&J) {
        arr Ji, Jj, JRj;
        G.kinematicsPos(NoArr, Ji, body_i, &vec_i);
        G.kinematicsPos(NoArr, Jj, body_j, &vec_j);
        G.axesMatrix(JRj, body_j);
        J.resize(3, Jj.d1);
        for(uint k=0; k<Jj.d1; k++) {
          mlr::Vector vi(Ji(0, k), Ji(1, k), Ji(2, k));
          mlr::Vector vj(Jj(0, k), Jj(1, k), Jj(2, k));
          mlr::Vector r (JRj(0, k), JRj(1, k), JRj(2, k));
          mlr::Vector jk =  body_j->X.rot / (vi - vj);
          jk -= body_j->X.rot / (r ^(pi - pj));
          J(0, k)=jk.x; J(1, k)=jk.y; J(2, k)=jk.z;
        }
      }
    } break;
    case vecTMT:{
      mlr::Vector vec_i = i<0?ivec: G.shapes(i)->rel.rot*ivec;
//      mlr::Vector vec_j = j<0?jvec: G.shapes(j)->rel.rot*jvec;
      if(body_j==NULL) {
        G.kinematicsVec(y, J, body_i, &vec_i);
        break;
      }
      //relative
      MLR_MSG("warning - don't have a correct Jacobian for this TMType yet");
//      fi = G.bodies(body_i)->X; fi.appendTransformation(irel);
//      fj = G.bodies(body_j)->X; fj.appendTransformation(jrel);
//      f.setDifference(fi, fj);
//      f.rot.getZ(c);
//      y = conv_vec2arr(c);
      NIY; //TODO: Jacobian?
    } break;
    case vecAlignTMT: {
      CHECK(fabs(ivec.length()-1.)<1e-10,"vector references must be normalized");
      CHECK(fabs(jvec.length()-1.)<1e-10,"vector references must be normalized");
      mlr::Vector vec_i = i<0?ivec: G.shapes(i)->rel.rot*ivec;
      mlr::Vector vec_j = j<0?jvec: G.shapes(j)->rel.rot*jvec;
      arr zi,Ji,zj,Jj;
      G.kinematicsVec(zi, Ji, body_i, &vec_i);
      if(body_j==NULL) {
        zj = conv_vec2arr(vec_j);
        if(&J) { Jj.resizeAs(Ji); Jj.setZero(); }
      } else {
        G.kinematicsVec(zj, Jj, body_j, &vec_j);
      }
      y.resize(1);
      y(0) = scalarProduct(zi, zj);
      if(&J) {
        J = ~zj * Ji + ~zi * Jj;
        J.reshape(1, G.getJointStateDimension());
      }
    } break;
    case quatTMT:
      if(body_j==NULL) {
        G.kinematicsQuat(y, J, body_i);
        break;
      }
      NIY;
      break;
    case qItselfTMT:{
      G.getJointState(y);
      if(&J) J.setId(y.N);
    } break;
    case qLinearTMT:{
      arr q;
      G.getJointState(q);
      if(params.N==q.N){
        y=params%q; if(&J) J.setDiag(params);
      }else{
        y=params*q; if(&J) J=params;
      }
    } break;
    case qSquaredTMT: {
      arr q;
      G.getJointState(q);
      y.resize(1);  y(0) = scalarProduct(params, q, q);
      if(&J) {
        J = params * q;
        J *= (double)2.;
        J.reshape(1, q.N);
      }
    } break;
    case qSingleTMT: {
      arr q;
      G.getJointState(q);
      y.resize(1);  y(0)=q(-i);
      if(&J) {
        J.resize(1, G.getJointStateDimension());
        J.setZero();
        J(0, -i) = 1.;
      }
    } break;
    case qLimitsTMT:   if(!params.N) params=G.getLimits();  G.kinematicsLimitsCost(y, J, params);  break;
    case comTMT:       G.getCenterOfMass(y);     y.resizeCopy(2); if(&J) { G.getComGradient(J);  J.resizeCopy(2, J.d1); }  break;
    case collTMT:      G.kinematicsProxyCost(y, J, params(0));  break;
    case colConTMT:    G.kinematicsContactConstraints(y, J);  break;
    case skinTMT: {
      arr Ji, zi;
      mlr::Vector vi;
      y.resize(params.N);
      y.setZero();
      if(&J) {
        J.clear();
        for(uint k=0; k<params.N; k++) {
          uint l=(uint)params(k);
          G.kinematicsPos(NoArr, Ji, G.bodies(l), NULL);
          vi = G.bodies(l)->X.rot.getY();
          vi *= -1.;
          zi = conv_vec2arr(vi);
          J.append(~zi*Ji);
        }
        J.reshape(params.N, J.N/params.N);
      }
    } break;
    default:  HALT("no such TVT");
  }
}

uint TaskMap_Default::dim_phi(const mlr::KinematicWorld& G) {
  //get state
  switch(type) {
    case posTMT: return 3;
    case vecTMT: return 3;
    case quatTMT: return 4;
    case qItselfTMT: return G.getJointStateDimension();
    case qLinearTMT: return params.d0;
    case qSquaredTMT: return 1;
    case qSingleTMT: return 1;
    case qLimitsTMT: return 1;
    case comTMT: return 2;
    case collTMT: return 1;
    case colConTMT: return 1;
    case skinTMT: return params.N;
    case vecAlignTMT: return 1;
    default:  HALT("no such TMT");
  }
}
