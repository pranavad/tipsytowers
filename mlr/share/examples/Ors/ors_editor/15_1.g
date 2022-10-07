Include = '../../../data/man_model.ors'
Edit waist { X=<T t(.8 0. 1.) d(-90 0 0 1)> }
body table{ type=9, X=<T t(.2 0 .9)>, size=[.8 1.4 .04 .02], color=[.3 .3 .5] fixed }
body marker{ type=9, X=<T t(0 0 .93)>, size=[0.025 .025 .001 .001], color=[1 0 0] fixed }
shape humanR (handR){ type=5 size=[.1 0 0 0] color=[1 1 0] rel=<T t(0 0 -.05) d(90 0 0 1)> }
shape humanL (handL){ type=5 size=[.1 0 0 0] color=[1 1 0] rel=<T t(0 0 -.05) d(90 0 0 1)> }
body b0 { type=9 size=[.05 .05 .05 .005] color=[.92 .75 .52] contact }
joint (table b0)    { from=<T t(-0.0653189761905266  -0.06697358873661083  0.022) d(26.66774197874267 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }
body b1 { type=9 size=[.05 .05 .05 .005] color=[.92 .75 .52] contact }
joint (table b1)    { from=<T t(0.0585163660706734  -0.042441802759042344  0.022) d(42.00422692567738 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }
body b2 { type=9 size=[.05 .05 .05 .005] color=[.92 .75 .52] contact }
joint (table b2)    { from=<T t(-0.03214846952347146  0.040747855047482504  0.022) d(0.14214994601510544 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }
body b3 { type=9 size=[.05 .05 .05 .005] color=[.92 .75 .52] contact }
joint (table b3)    { from=<T t(0.0736263789660378  0.07547884865585999  0.022) d(78.6407909867903 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }
body b4 { type=9 size=[.05 .05 .05 .005] color=[.92 .75 .52] contact }
joint (table b4)    { from=<T t(0.009029463145351857  -0.10852298580310599  0.022) d(21.645658644154892 0 0 1)> to=<T t(0 0 .025)> type=JT_rigid }
# abs_positions 
#b0 (0.134681,-0.0669736,0.947)
#b1 (0.258516,-0.0424418,0.947)
#b2 (0.167852,0.0407479,0.947)
#b3 (0.273626,0.0754788,0.947)
#b4 (0.209029,-0.108523,0.947)
