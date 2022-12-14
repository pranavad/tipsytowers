#ifndef _KINECT_H_
#define _KINECT_H_

#include <Core/thread.h>
#include <functional>

#define Kinect_image_width 640
#define Kinect_image_height 480

struct KinectThread : Thread {
  struct sKinectThread *s;

  ACCESS(byteA, kinect_rgb)
  ACCESS(uint16A, kinect_depth)

  KinectThread();
  virtual ~KinectThread();

  void open();
  void step();
  void close();
};

namespace mlr {
  /// convert raw image data into depth and color arrays like in a pointcloud
  void images2pointcloud(byteA& rgb, uint16A& depth, arr& pts, arr& cols);

  /// pack 16bit depth image into 3 8-bit channels
  void pack_kindepth2rgb(const uint16A& depth, byteA& buffer);

  /// Typedef for depth image received event callbacks
  typedef std::function<void(const uint16A&, double)> kinect_depth_cb;
  /// Typedef for video image received event callbacks
  typedef std::function<void(const byteA&, double)> kinect_video_cb;

  class KinectCallbackReceiver {
  private:
    struct sKinectCallbackReceiver *s;
    int cameraNum;
  public:
    KinectCallbackReceiver(kinect_depth_cb depth_cb, kinect_video_cb video_cb, int cameraNum=0);
    virtual ~KinectCallbackReceiver();

    void startStreaming();
    void stopStreaming();
  };
}

#endif
