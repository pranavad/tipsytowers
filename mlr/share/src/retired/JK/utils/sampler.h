#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include <Core/array.h>
#include <relational/robotManipulationSimulator.h>
#include <devTools/logging.h>
SET_LOG(sampler, INFO);

#include <limits>

template<class S> class Sampler {
  public:
    virtual void sample(S& sample) = 0;
};

template<class S> class Evaluator {
  public:
    virtual double evaluate(S& sample) = 0;
};

class BlocksWorldSampler : public Sampler<mlr::Array<arr> > {
  public: 
    virtual void sample(mlr::Array<arr>& sample);
};

class TraySampler : public Sampler<mlr::Array<arr> > {
  public:
    virtual void sample(mlr::Array<arr>& sample);
};

class OutOfReachSampler : public Sampler<mlr::Array<arr> > {
  public:
    virtual void sample(mlr::Array<arr>& sample);
};
class UprightSampler : public Sampler<mlr::Array<arr> > {
  public:
    virtual void sample(mlr::Array<arr>& sample);
};

template<class Data> void rejectionSampling(Data& sample, Sampler<Data>* s, Evaluator<Data>* e, const uint nSamples = 10000) {
  Data testSample;

  double maxEvaluation = - std::numeric_limits<double>::max();

  for (uint i = 0; i < nSamples; ++i) {
    s->sample(testSample);
    double evaluation = e->evaluate(testSample);
    if (evaluation > maxEvaluation) {
      sample = testSample;  
      maxEvaluation = evaluation;
    }
  }
}

#endif //_SAMPLER_H_
