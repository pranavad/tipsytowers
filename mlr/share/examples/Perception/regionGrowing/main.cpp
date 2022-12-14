#include <Perception/kinect2pointCloud.h>
#include <Perception/modelEnsemble.h>
#include <Gui/opengl.h>
#include <Geo/mesh.h>

const double scale=10000.;

static const double colorsTab[6][3] = {
  {0.2, 0.2, 1.0}, // blue
  {1.0, 0.8, 0.0}, // gold
  {1.0, 0.0, 0.0}, // red
  {0.7, 0.7, 0.7}, // gray
  {0.0, 1.0, 1.0}, // white
  {0.2, 1.0, 0.2}
};


void test(){
  uint16A kinect_depth(FILE("z.kinect_depth"));
  mlr::Mesh mesh;
  arr& pts = mesh.V;
  arr& cols = mesh.C;
  arr costs;
  depthData2pointCloud(pts, kinect_depth);
  cols.resizeAs(pts);
  costs = pts.col(2).reshape(pts.d0);
  for(auto& z:costs) if(z<0.) z=0.; //points with negative depth get cost zero
  costs *= costs;
  costs /= sum(costs);

  DataNeighbored D(pts);
  D.setGridNeighborhood(kinect_depth.d0, kinect_depth.d1);
  D.setCosts(costs);
//  D.removeNonOk();

  ModelEnsemble M;

  OpenGL gl;
  gl.add(mesh);
  gl.addDrawer(&M);
  gl.camera.setKinect();
  gl.camera.setPosition(0., 0., -1.);
  gl.watch();


  for(uint l=0;l<20;l++){
    bool succ=M.addNewRegionGrowingModel(D);
    if(succ){
      //M.models.last()->colorPixelsWithWeights(cols);

      M.reoptimizeModels(D);
//      M.reestimateVert();
      M.report();
      gl.update();
//      gl.watch();
    }
  }
  M.report();
  gl.watch();

}



int MAIN(int argc,char** argv){
//  rnd.clockSeed();

  test();

  return 0;
}
