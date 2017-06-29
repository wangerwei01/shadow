#ifndef SHADOW_OPERATORS_FLATTEN_OP_HPP
#define SHADOW_OPERATORS_FLATTEN_OP_HPP

#include "core/operator.hpp"

namespace Shadow {

class FlattenOp : public Operator {
 public:
  explicit FlattenOp(const shadow::OpParam &op_param, Workspace *ws)
      : Operator(op_param, ws) {}
  ~FlattenOp() { Release(); }

  void Setup();
  void Reshape();
  void Forward();
  void Release();

 private:
  int axis_, end_axis_;
};

}  // namespace Shadow

#endif  // SHADOW_OPERATORS_FLATTEN_OP_HPP
