#include <Perception/surfels.h>
#include <Gui/opengl.h>
#include <Perception/kinect2pointCloud.h>

void TEST(Surfels) {
  Surfels S;
  S.setRandom(20);
//  S.gl.update();

  S.recomputeSurfelIndices();
  write_ppm((byte)4*S.mask, "z.ppm");

//  OpenGL gl("watch",640,480);
//  gl.camera=kinectCam;
//  gl.add(glDrawSurfels, &S);
//  gl.update();


//  OpenGL gl2("mask",640,480);
//  gl2.watchImage((byte)4*S.mask, true, 1);
}

int main(int argc,char **argv) {
  mlr::initCmdLine(argc,argv);

  testSurfels();

  return 0;
}
