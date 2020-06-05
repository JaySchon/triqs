#include <triqs/gfs.hpp>
using namespace triqs::gfs;
using namespace triqs::mesh;

int main() {
  double tmin = 0, tmax = 1.0;
  int nt = 100;
  auto g = gf<prod<retime, retime>, scalar_valued>{{{tmin, tmax, nt}, {tmin, tmax, nt}}};
  triqs::clef::placeholder<0> t1_;
  triqs::clef::placeholder<1> t2_;
  g(t1_, t2_) << 2. * t1_;
  std::cout << g(0.24, 0.36) << std::endl;
}
