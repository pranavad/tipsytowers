BASE   = ../../..
-include $(BASE)/build/config.mk

MSGFILE = msg/JointState.msg
ROSDIR = /opt/ros/$(ROS_DISTRO)
OUTPUTDIR = $(BASE)/include/ros_msg
PKGNAME = marc_controller_pkg


default:
	python2 $(ROSDIR)/lib/gencpp/gen_cpp.py \
    	$(MSGFILE) \
        -p $(PKGNAME) \
        -e $(ROSDIR)/share/gencpp \
        -I "std_msgs:$(ROSDIR)/share/std_msgs/msg" \
        -I "geometry_msgs:$(ROSDIR)/share/geometry_msgs/msg" \
        -o $(OUTPUTDIR)
