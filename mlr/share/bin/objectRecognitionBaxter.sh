roslaunch freenect_launch freenect-registered-xyzrgb.launch data_skip:=2 &
sleep 5
rosrun object_recognition_core detection -c `rospack find object_recognition_tabletop`/conf/detection.table.ros.ork &
sleep 5
rosrun rviz rviz 

#rosnode kill /object_recognition_server
#killall -9 roscore rosmaster

