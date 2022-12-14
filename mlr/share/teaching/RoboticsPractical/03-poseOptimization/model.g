#Include = 'arm7.kvg'
Include = '../../../data/pr2_model/pr2_model.ors'


body table{ type=0, X=<T t(0 -1.2 .6)>, size=[1. 1. .04 .0], color=[.8 .5 .3] fixed, contact }
shape leg1(table) { rel=<T t(-.3 -.3 -.3)>  type=0  size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg2(table) { rel=<T t(-.3 .3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg3(table) { rel=<T t(.3 .3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg4(table) { rel=<T t(.3 -.3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }

body table2{ type=0, X=<T t(1.2 0 .6)>, size=[1. 1. .04 .0], color=[.8 .5 .3] fixed, contact }
shape leg1(table2) { rel=<T t(-.3 -.3 -.3)>  type=0  size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg2(table2) { rel=<T t(-.3 .3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg3(table2) { rel=<T t(.3 .3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }
shape leg4(table2) { rel=<T t(.3 -.3 -.3)>  type=0 size=[.04 .04 .6 0] color=[0.5 0.3 0.15] }

body obj1 { size=[.1 .1 .2 .02] type=9 contact }
#body obj2 { size=[.1 .1 .2 0] type=0 contact }
#body obj3 { size=[.1 .1 .2 0] type=0 contact }
#body obj4 { size=[.1 .1 .2 0] type=0 contact }
#body obj5 { size=[.1 .1 .2 0] type=0 contact }
#body obj6 { size=[.1 .1 .2 0] type=0 contact }

joint (table obj1) { from=<T t(-.1 .2 0)> to=<T t(0 0 .1)> type=10 }
#joint (table obj2) { from=<T t(-.2 .1 0)> to=<T t(0 0 .1)> type=10 }
#joint (table obj3) { from=<T t(-.3 -.1 0)> to=<T t(0 0 .1)> type=10 }
#joint (table obj4) { from=<T t(-.1 -.1 0)> to=<T t(0 0 .1)> type=10 }
#joint (table obj5) { from=<T t(-.1 -.2 0)> to=<T t(0 0 .1)> type=10 }
#joint (table obj6) { from=<T t(-.1 .3 0)> to=<T t(0 0 .1)> type=10 }

body graspRef { type=5 size=[.1 0 0 0] }
#joint graspJoint(arm7 graspRef){ A=<T t(0 0 .5)> type=11 }
#shape endeff(arm7){ type=1 rel=<T t(0 0 .4)> size=[0 0 0 .02] color=[1 1 0] }
joint graspJoint(r_wrist_roll_link graspRef){ A=<T t(.3 0 0)> type=11 }
shape endeff(r_wrist_roll_link){ type=9 rel=<T t(.3 0 0)> size=[.01 .01 .01 .02] color=[1 1 0] }

shape target(table2){ type=5 rel=<T t(0 0 .15)> size=[.3 0 0 0] color=[1 1 0]}
