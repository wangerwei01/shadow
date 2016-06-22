#ifndef SHADOW_UTIL_ACTIVATIONS_HPP
#define SHADOW_UTIL_ACTIVATIONS_HPP

#include "shadow/kernel.hpp"

inline float Activate(float x, const shadow::ActivateType &type) {
  switch (type) {
    case shadow::ActivateType::Linear:
      return x;
    case shadow::ActivateType::Relu:
      return x * (x > 0);
    case shadow::ActivateType::Leaky:
      return (x > 0) ? x : .1f * x;
  }
  return x;
}

namespace Activations {

template <typename T>
void ActivateArray(int N, const shadow::ActivateType &type, T *out_data) {
#if !defined(USE_CUDA) & !defined(USE_CL)
  for (int i = 0; i < N; ++i) {
    out_data[i] = Activate(out_data[i], type);
  }

#else
  Kernel::ActivateArray(N, type, out_data);
#endif
}

}  // namespace Activations

#endif  // SHADOW_UTIL_ACTIVATIONS_HPP
