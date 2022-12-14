//#define MLR_IMPLEMENTATION
#define MLR_BUMBLE
#include <signal.h>
#define REALCAMERA

#include <hardware/hardware.h>
#include <MT/earlyVisionModule.h>
#include <MT/perceptionModule.h>
#include <MT/guiModule.h>


static bool STOP=false;
void shutdown(int) {
  cout <<"shutting down!" <<endl;
  STOP=true;
}

int main(int argc,char** argv) {
  mlr::initCmdLine(argc,argv);
  signal(SIGINT,shutdown);
  
  mlr::KinematicWorld G;
  G <<FILE("schunk.ors");
  //mlr::Body * o = G.getBodyByName("OBJECTS");
  //o->X.p(0) = 100;
  
  //Variables
  CameraImages currentCameraImages;
  q_currentReferenceVar q_var;
  EarlyVisionOutput evisOutput;
  PerceptionOutput percOutput;
  
  //Processes
  EarlyVisionModule evis;
  CameraModule cam;
  cam.output = &currentCameraImages;
  evis.input = &currentCameraImages;
  evis.output = &evisOutput;
  
#ifndef REALCAMERA
  currentCameraImages.loadDummyImages();
#endif
  PerceptionModule perc;  perc.input=evis.output;
  perc.output = &percOutput;
  G.getJointState(q_var.q_reference);
  
  GuiModule gui;  gui.cameraVar=evis.input;  gui.perceptionOutputVar=perc.output;
  gui.q_referenceVar = &q_var;
  gui.createOrsClones(&G);
  
#ifdef REALCAMERA
  cam .threadLoop();
#endif
  evis.threadLoop();
  perc.threadLoop();
  gui .threadLoop();
  bool bSave = mlr::Parameter<bool>("saveImage");
  for(uint i=0; !STOP && i<1000; i++) {
    if(bSave == 1) {
      evis.output->readAccess(NULL);
      //   if(evis.output.hsvThetaL.N){
      //    write_ppm(cam.output.rgbL,"hsvTheta.ppm");
      //  }
      
      //save the hsv image
      floatA hsvL;// hsvL.resizeAs(cam.output.rgbL);
      floatA tmpImg;
      byte2float(tmpImg,currentCameraImages.rgbL);
      rgb2hsv(hsvL , tmpImg);
      byteA hsvInt(hsvL.d0,hsvL.d1,hsvL.d2);
      for(uint x = 0; x < hsvL.d0; x++)
        for(uint y = 0; y < hsvL.d1; y++)
          for(uint z = 0; z < hsvL.d2; z++)
            hsvInt(x,y,z) = hsvL(x,y,z)*256;
            
      write_ppm(hsvInt,"hsvTheta.ppm");
      evis.output->deAccess(NULL);
    }
    mlr::wait(.1);
    cout <<"\r" <<i <<flush;
  }
  
  gui.threadClose();
  perc.threadClose();
  evis.threadClose();
  cam.threadClose();
  
  return 0;
}
