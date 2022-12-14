#include "al_logistic_regression.h"
#include "al_util.h"

#include <Core/array.h>
#include <JK/utils/util.h>
#include <JK/utils/sampler.h>
#include <JK/utils/featureGenerator.h>

class LogisticRegressionEvaluator : public Evaluator<mlr::Array<arr> > {
    sLogisticRegression* p;
  public:
    LogisticRegressionEvaluator(sLogisticRegression* p, const ActiveLearningProblem& prb) : p(p), problem(prb) {};
    double evaluate(mlr::Array<arr>& sample);
    const ActiveLearningProblem problem;
};

class sLogisticRegression {
  public:
    sLogisticRegression() {};
    arr beta;  

    arr data;
    arr classes;

    double ridge, sigma;
    int rbfConst;

    void logisticRegressionMultiClass(arr& beta, const arr& X, const arr& y, double lambda);

};

void createClassMatrix(arr& matrix, const intA& classes, const uint nClasses) {
  for (uint i = 0; i < classes.d0; ++i) {
    arr row = zeros(1, nClasses);
    row(0, classes(i)) = 1;
    matrix.append(row);
  }
  matrix.reshape(classes.d0, nClasses);
  
}

double LogisticRegressionEvaluator::evaluate(mlr::Array<arr>& sample) {
  arr Phi, x;
  flatten(x, sample);
  problem.generator->makeFeatures(Phi, x);
  arr classes = Phi*p->beta;
  //return 1;
  return -fabs(classes(0, 0));
}

LogisticRegression::LogisticRegression(ActiveLearningProblem& prob) : s(new sLogisticRegression) {
  this->problem = prob;
  s->ridge = mlr::getParameter<double>("ridge",1e-10);
  s->rbfConst = mlr::getParameter<int>("rbfConst", 0);
  s->sigma = mlr::getParameter<double>("sigma", 0.1);
}

//void sLogisticRegression::makeFeatures(arr& Z, const arr& X, const arr& Xtrain ) {
  //Z.resize(X.d0, Xtrain.d0+rbfConst);
  //for(uint i=0; i<Z.d0; i++){
    //if(rbfConst) Z(i, 0) = 1.; //constant feature also for rbfs
    //for(uint j=0; j<Xtrain.d0; j++){
      //double d=euclideanDistance(X[i], Xtrain[j])/sigma;
      //Z(i, j+rbfConst) = std::exp(-.5*d*d);
    //}
  //}
//}

void sLogisticRegression::logisticRegressionMultiClass(arr& beta, const arr& X, const arr& y, double lambda){
  std::cout << "Start logistic regresseion opt.... " << std::endl;
  int t = time(NULL);
  CHECK(y.nd==2 && y.d0==X.d0, "");
  // M is the number of classes. y contains a row for each class holding a 0
  // if X(i) is not in this class and a 1 if it is in that class.
  uint n=y.d0, d=X.d1, M=y.d1;
  arr Xt;
  transpose(Xt, X);
  
  arr XtWX, XtWXinv, I;
  I.setDiag(lambda, X.d1);
  I(0, 0)=1e-10;
  
  arr f(n, M), p(n, M), w(n), beta_update;
  double logLike, lastLogLike, alpha=1.;
  beta.resize(d, M);
  beta.setZero();
  int invtime = 0;
  for(uint k=0; k<100; k++){
    f = X*beta;
    for(uint i=0; i<f.N; i++) mlr::clip(f.elem(i), -100., 100);  //constrain the discriminative values to avoid NANs...
    p = exp(f);
    logLike=0.;
    for(uint i=0; i<n; i++){
      p[i]() /= sum(p[i]); //normalize the exp(f(x)) along each row
      for(uint c=0; c<M; c++) logLike += y(i, c)*log(p(i, c));
    }
    // rprop
    //optionally reject the update
    if(k && logLike<lastLogLike){
      //cout <<"REJECT" <<endl;
      beta -= alpha*beta_update;
      alpha *= .1;
      beta += alpha*beta_update;
      if(alpha*absMax(beta_update)<1e-5) break;
      continue;
    }else{
      alpha = pow(alpha, .8);
    }
    lastLogLike=logLike;
    
    //construct the Hessian matrix as block matrix of size d*M-times-d*M (the beta is of size d*M)
    XtWX = zeros(beta.N);
    for(uint c1=0; c1<M; c1++) for(uint c2=0; c2<M; c2++){
        for(uint i=0; i<n; i++) w(i) = p(i, c1)*((c1==c2?1.:0.)-p(i, c2));
        XtWX.setMatrixBlock(Xt*diagProduct(w, X) + (c1==c2?2.:0.)*I, c1*d, c2*d);
      }
    int preinvtime = time(NULL);
    inverse_SymPosDef(XtWXinv, XtWX);
    invtime += time(0) - preinvtime;
    //inverse(XtWXinv, XtWX);
    //compute the beta update
    beta_update = (Xt*(y-p) - 2.*I*beta); //the gradient as d-times-M matrix
    beta_update = ~beta_update;           //... as M-times-d matrix
    beta_update.reshape(d*M);             //... as one big vector
    beta_update = XtWXinv * beta_update;  //multiply the Hessian
    beta_update.reshape(M, d);             //... as M-times-d matrix
    beta_update = ~beta_update;           //... and back as d-times-M matrix
    
    beta += alpha*beta_update;
    
    if(alpha*absMax(beta_update)<1e-3) break;
  }
  cout << time(NULL) - t << " secs. ( " << invtime << " for inversion)" << std::endl;
  std::cout << "done." <<std::endl;
}


void LogisticRegression::setTrainingsData(const mlr::Array<arr>& data, const intA& classes) {
  arr X, x, Phi, matrix;
  
  flatten(x, data);
  createClassMatrix(matrix, classes, 2);

  s->classes = matrix;
  s->data = x;

  problem.generator->makeFeatures(Phi,x);
  s->logisticRegressionMultiClass(s->beta, Phi, matrix, s->ridge);
}
void LogisticRegression::addData(const mlr::Array<arr>& data, const int class_) {
  arr x;
  flatten(x, data);
  s->data.append(x);
  arr matrix = zeros(1, s->classes.d1);
  matrix(0, class_) = 1;
  s->classes.append(matrix);

  arr Phi;
  problem.generator->makeFeatures(Phi, s->data);
  s->logisticRegressionMultiClass(s->beta, Phi, s->classes, s->ridge);
  
}
int LogisticRegression::nextSample(mlr::Array<arr>& sample) const {
  rejectionSampling<mlr::Array<arr> >(sample, problem.sampler, new LogisticRegressionEvaluator(s, problem), 100);
  return sample.N;
}
int LogisticRegression::classify(const mlr::Array<arr>& data, const int set) const {
  arr Phi, x;
  flatten(x, data);
  problem.generator->makeFeatures(Phi, x);
  arr classes = Phi*s->beta;
  return classes.maxIndex();
}

void LogisticRegression::setProblem(ActiveLearningProblem& prob) { problem = prob; }
