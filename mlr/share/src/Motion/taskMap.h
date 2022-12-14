/*  ------------------------------------------------------------------
    Copyright 2016 Marc Toussaint
    email: marc.toussaint@informatik.uni-stuttgart.de
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version. This program is distributed without
    any warranty. See the GNU General Public License for more details.
    You should have received a COPYING file of the full GNU General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>
    --------------------------------------------------------------  */


#pragma once
#include <Ors/ors.h>

/// defines only a map (task space), not yet the costs in this space
struct TaskMap {
  uint order;       ///< 0=position, 1=vel, etc
  virtual void phi(arr& y, arr& J, const mlr::KinematicWorld& G, int t=-1) = 0; ///< this needs to be overloaded
  virtual void phi(arr& y, arr& J, const WorldL& G, double tau, int t=-1); ///< if not overloaded this computes the generic pos/vel/acc depending on order
  virtual uint dim_phi(const mlr::KinematicWorld& G) = 0; //the dimensionality of $y$
  virtual uint dim_phi(const WorldL& G, int t){ return dim_phi(*G.last()); }

  arr phi(const mlr::KinematicWorld& G){ arr y; phi(y,NoArr,G); return y; }

  VectorFunction vf(mlr::KinematicWorld& G){
    return [this, &G](arr& y, arr& J, const arr& x) -> void {
      G.setJointState(x);
      phi(y, J, G, -1);
    };
  }

  TaskMap():order(0) {}
  virtual ~TaskMap() {}
  virtual mlr::String shortTag(const mlr::KinematicWorld& G){ NIY; }

  static TaskMap *newTaskMap(const Graph& specs, const mlr::KinematicWorld& world); ///< creates a task map based on specs
  static TaskMap *newTaskMap(const Node* specs, const mlr::KinematicWorld& world); ///< creates a task map based on specs
};
