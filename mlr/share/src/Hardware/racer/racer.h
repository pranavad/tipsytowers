#include <Core/array.h>
#include <Gui/opengl.h>

struct Racer;

struct _RacerBalancingBenchmark:ScalarFunction{
  uint T;
  bool display;
  double exploration;
  double noise;
  double theta0;
  int fixedRandomSeed;
  _RacerBalancingBenchmark();
  virtual double fs(arr& g, arr& H, const arr& x);
};
extern _RacerBalancingBenchmark RacerBalancingBenchmark;


struct DynFct:VectorFunction{
  Racer& R;
  DynFct(Racer& _R):R(_R){
    VectorFunction::operator=(
      [this](arr& y, arr& J, const arr& x) -> void {
        return fv(y, J, x);
      }
    );
  }
  DynFct& operator()(){ return *this; }
  void fv(arr& y, arr& J, const arr& q__q_dot);
};

struct ObsFct:VectorFunction{
  Racer& R;
  ObsFct(Racer& _R):R(_R){
    VectorFunction::operator=(
      [this](arr& y, arr& J, const arr& x) -> void {
        return fv(y, J, x);
      }
    );
  }
  ObsFct& operator()(){ return *this; }
  void fv(arr& y, arr& J, const arr& q__q_dot);
};

struct Racer{
  //state
  arr q, q_dot;
  //buffer of results calling fv
  arr q_ddot,M,Minv,F,B;

  //controls
  double u, tau;

  //constant parameters
  double r, l, lC, mA, mB, IA, IB, g, noise_dynamics; //dynamics parameters
  double c1, c2, c3, c4, c5, noise_accel, noise_gyro, noise_enc; //observation parameters

  Racer();
  ~Racer();
  void getJacobians(arr& J_A, arr& J_B, arr& J_B_dash=NoArr, arr &J_B_ddash=NoArr, bool getJ_C=false);
  void getDynamics(arr& M, arr& F, arr& B, arr &M_dash=NoArr, arr &M_ddash=NoArr, arr &F_dash=NoArr);
  void getDynamicsAB(arr& A, arr& a, arr& barB);
  double getEnergy();
  void getObservation(arr& y, arr& C=NoArr, arr& c=NoArr, arr &W=NoArr);
  void stepDynamics(double _u);
  void stepDynamicsAcc(double u_acc);

  DynFct dynamicsFct; //returns the acceleration given the state -> used by rk4
  ObsFct observationFct;

  OpenGL& gl();
  void glDraw();
  static void glStaticDraw(void*);
  OpenGL *_gl;
};


