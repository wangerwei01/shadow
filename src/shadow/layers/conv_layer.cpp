#include "shadow/layers/conv_layer.hpp"
#include "shadow/util/blas.hpp"
#include "shadow/util/image.hpp"

inline int convolutional_out_size(int s, int size, int pad, int stride) {
  return (s + 2 * pad - size) / stride + 1;
}

void ConvLayer::Reshape() {
  num_output_ = layer_param_.convolution_param().num_output();
  kernel_size_ = layer_param_.convolution_param().kernel_size();
  stride_ = layer_param_.convolution_param().stride();
  pad_ = layer_param_.convolution_param().pad();

  int in_c = bottoms_[0]->shape(1), in_h = bottoms_[0]->shape(2),
      in_w = bottoms_[0]->shape(3);

  VecInt top_shape = bottoms_[0]->shape();
  top_shape[1] = num_output_;
  top_shape[2] = convolutional_out_size(in_h, kernel_size_, pad_, stride_);
  top_shape[3] = convolutional_out_size(in_w, kernel_size_, pad_, stride_);
  tops_[0]->reshape(top_shape);

  out_spatial_dim_ = tops_[0]->count(2);
  kernel_dim_ = kernel_size_ * kernel_size_ * in_c;

  biases_multiplier_.reshape(out_spatial_dim_);
  Blas::Set(out_spatial_dim_, 1, biases_multiplier_.mutable_data(), 0);
  col_image_.reshape(kernel_dim_, out_spatial_dim_);

  std::stringstream out;
  out << layer_name_ << ": "
      << Util::format_vector(bottoms_[0]->shape(), ",", "(", ")") << " -> "
      << num_output_ << "_" << kernel_size_ << "x" << kernel_size_ << "_s"
      << stride_ << "_p" << pad_ << " -> "
      << Util::format_vector(tops_[0]->shape(), ",", "(", ")");
  DInfo(out.str());
}

void ConvLayer::Forward() {
  int batch = bottoms_[0]->shape(0);
  int top_num = tops_[0]->num(), bottom_num = bottoms_[0]->num();
  for (int b = 0; b < batch; ++b) {
    Image::Im2Col(bottoms_[0]->data(), bottoms_[0]->shape(), b * bottom_num,
                  kernel_size_, stride_, pad_, tops_[0]->shape(),
                  col_image_.mutable_data());
    Blas::BlasSgemm(0, 0, num_output_, out_spatial_dim_, kernel_dim_, 1,
                    blobs_[0]->data(), 0, col_image_.data(), 0, 0,
                    tops_[0]->mutable_data(), b * top_num);
    Blas::BlasSgemm(0, 0, num_output_, out_spatial_dim_, 1, 1,
                    blobs_[1]->data(), 0, biases_multiplier_.data(), 0, 1,
                    tops_[0]->mutable_data(), b * top_num);
  }
}

void ConvLayer::Release() {
  bottoms_.clear();
  tops_.clear();
  for (int i = 0; i < blobs_.size(); ++i) {
    blobs_[i]->clear();
  }
  blobs_.clear();

  biases_multiplier_.clear();
  col_image_.clear();

  // DInfo("Free ConvLayer!");
}
