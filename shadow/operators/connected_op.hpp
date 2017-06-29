#ifndef SHADOW_OPERATORS_CONNECTED_OP_HPP
#define SHADOW_OPERATORS_CONNECTED_OP_HPP

#include "core/operator.hpp"

namespace Shadow {

class ConnectedOp : public Operator {
 public:
  explicit ConnectedOp(const shadow::OpParam &op_param, Workspace *ws)
      : Operator(op_param, ws) {}
  ~ConnectedOp() { Release(); }

  void Setup();
  void Reshape();
  void Forward();
  void Release();

 private:
  int num_output_;
  bool bias_term_, transpose_;

  BlobF biases_multiplier_;
};

}  // namespace Shadow

#endif  // SHADOW_OPERATORS_CONNECTED_OP_HPP
