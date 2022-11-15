#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <Motion/komo.h>
#include <Gui/opengl.h>
#include <Ors/ors_swift.h>
#include <Ors/ors_physx.h>


using namespace std;


void printOutGFile(mlr::KinematicWorld& outKinWorld, string outFilePath) {
    cout << "Printing file: " << outFilePath << endl;
    FILE(STRING(outFilePath)) << outKinWorld;
}


int main(int argc,char** argv){
    int argv_index_counter = 1;

    string filePath = argv[argv_index_counter++];
    string outFilePath = argv[argv_index_counter++];
    string imageFilePath = argv[argv_index_counter++];

    bool saveImage = false;
    istringstream(argv[argv_index_counter++]) >> saveImage;

    mlr::KinematicWorld kinWorld;
    kinWorld.init(STRING(filePath));

    printOutGFile(kinWorld, outFilePath);

    kinWorld.gl().camera.setPosition(5.5,0.,3.5);
    kinWorld.gl().camera.focus(-0.5,0,1.);
    kinWorld.gl().camera.upright();
    kinWorld.qdim.clear();
    kinWorld.watch(false);
    kinWorld.qdim.clear();
    if (saveImage) write_ppm(kinWorld.gl().captureImage, imageFilePath.c_str(), true);

    return 0;
}
