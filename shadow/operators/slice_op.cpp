#include "slice_op.hpp"

namespace Shadow {

void SliceOp::Forward() {
  CHECK_GE(tops_size(), 2);

  const auto bottom = bottoms(0);

  int num_tops = tops_size();

  VecInt slices;
  int bottom_slice_axis = bottom->shape(slice_axis_);
  if (slice_point_.empty()) {
    CHECK_EQ(bottom_slice_axis % num_tops, 0);
    slices.resize(num_tops, bottom_slice_axis / num_tops);
  } else {
    CHECK_EQ(slice_point_.size(), num_tops - 1);
    int prev = 0;
    for (auto point : slice_point_) {
      CHECK_GT(point, prev);
      slices.push_back(point - prev);
      prev = point;
    }
    CHECK_GT(bottom_slice_axis, prev);
    slices.push_back(bottom_slice_axis - prev);
  }

  int count = 0;
  auto top_shape = bottom->shape();
  for (int n = 0; n < num_tops; ++n) {
    auto top = tops(n);
    top_shape[slice_axis_] = slices[n];
    top->reshape(top_shape);
    count += top->count();
  }
  CHECK_EQ(count, bottom->count());

  int offset_slice_axis = 0;
  int num_slices = bottom->count(0, slice_axis_);
  int slice_size = bottom->count(slice_axis_ + 1);
  for (int n = 0; n < tops_size(); ++n) {
    auto top = tops(n);
    int top_slice_axis = top->shape(slice_axis_);
    Vision::Slice(bottom->data<float>(), top->count(), num_slices, slice_size,
                  bottom_slice_axis, top_slice_axis, offset_slice_axis,
                  top->mutable_data<float>(), ws_->Ctx());
    offset_slice_axis += top_slice_axis;
  }
}

REGISTER_OPERATOR(Slice, SliceOp);

namespace Vision {

#if !defined(USE_CUDA)
template <typename T>
void Slice(const T *in_data, int count, int num_slices, int slice_size,
           int bottom_slice_axis, int top_slice_axis, int offset_slice_axis,
           T *out_data, Context *context) {
  for (int n = 0; n < num_slices; ++n) {
    memcpy(out_data + n * top_slice_axis * slice_size,
           in_data + (n * bottom_slice_axis + offset_slice_axis) * slice_size,
           top_slice_axis * slice_size * sizeof(T));
  }
}

template void Slice(const float *, int, int, int, int, int, int, float *,
                    Context *);
#endif

}  // namespace Vision

}  // namespace Shadow
