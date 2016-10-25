#ifndef SHADOW_LAYERS_CONNECTED_LAYER_HPP
#define SHADOW_LAYERS_CONNECTED_LAYER_HPP

#include "shadow/layers/layer.hpp"

class ConnectedLayer : public Layer {
 public:
  ConnectedLayer() {}
  explicit ConnectedLayer(const shadow::LayerParameter &layer_param)
      : Layer(layer_param) {}
  ~ConnectedLayer() { Release(); }

  void Reshape();
  void Forward();
  void Release();

 private:
  int num_output_;

  Blob<float> biases_multiplier_;
};

#endif  // SHADOW_LAYERS_CONNECTED_LAYER_HPP
