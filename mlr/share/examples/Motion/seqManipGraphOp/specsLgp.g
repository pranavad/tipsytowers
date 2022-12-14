Include = '../../../data/keywords.g'
Include = '../../../../usr/MT/LGP-player/LGP-world.g'

KOMO{
  T = 100
  duration = 5
  makeConvexHulls, activateAllContacts
}

#transition costs
(MinSumOfSqr qItself){ order=2 time=[0 1] Hmetric=1e-1 }

#kinematic switches
(MakeJoint rigidZero graspRef obj1){ time=.49 }
(MakeJoint delete table2 obj1){ time=.49 }

#reach:
(MinSumOfSqr posDiff graspRef obj1){ time=[0.49 0.5] scale=1e3 }
(MinSumOfSqr quatDiff graspRef obj1){ time=[0.49 0.5] scale=1e3 }
(MinSumOfSqr qItself){ order=1 time=[0.49 0.5] scale=1e1 }

#move up:
(MinSumOfSqr pos obj1){ order=1 scale=1e1 time=[0.53 0.55] target=[0 0 .5] }

#place:
(MinSumOfSqr posDiff obj1 target){ time=[1 1] scale=1e3 }
(MinSumOfSqr qItself){ order=1 time=[1 1] scale=1e1 }
(MinSumOfSqr quatDiff obj1 target){ time=[1 1] target=[1 0 0 0] scale=1e3 }

#rigid grasp:
(MinSumOfSqr qItself graspJoint){ order=1 time=[0.51 .99] scale=1e3 }

#(LowerEqualZero collisionExceptPairs obj1 graspRef obj1 endeff obj1 table){ margin=.05 }
#(LowerEqualZero collisionPairs obj1 table){ time=[.6 1] margin=.05 }
cout <<"PATH PROBLEM:" <<*pathProblemSpecs <<endl;

(EqualZero GJK eff cyl_1) { time=[ 1 1 ]  scale=100 }
(MakeJoint delete cyl_1) { time=1 }
(MakeJoint rigid eff cyl_1) { time=1 }
PATH PROBLEM: (s=2
(Release eff cyl_1 table1),
(MinSumOfSqr qItself) { time=[ 0.1 1 ]  order=2 }
(EqualZero GJK cyl_1 table1) { time=[ 1 1 ]  target=[ 0 0 0.05 ]  scale=100 }
(MinSumOfSqr posDiff cyl_1 table1) { time=[ 1 1 ]  target=[ 0 0 0.5 ]  scale=10 }
(MinSumOfSqr vec cyl_1) { time=[ 1 1 ]  vec1=[ 0 0 1 ]  target=[ 0 0 1 ]  scale=100 }
(MakeJoint delete eff cyl_1) { time=1 }
(MakeJoint rigid table1 cyl_1) { time=1 }
