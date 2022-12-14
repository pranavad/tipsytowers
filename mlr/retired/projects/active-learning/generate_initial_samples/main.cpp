#include <JK/utils/sampler.h>
#include <JK/utils/oracle.h>
#include <Core/array.h>
#include <iostream>
int main(int, const char**) {
  ofstream os("samples.data");
  Traysampler;
  InsideOracle o;
	mlr::Array<arr> sample;
  for (int i=0; i<1000; ++i) {
    do {
      sampler.sample(sample);
    } while(!o.classify(sample, 0));
    os << sample << std::endl;
  }
    
}
