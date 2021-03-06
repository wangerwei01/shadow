#include "workspace.hpp"

namespace Shadow {

Workspace::Workspace(const ArgumentHelper &arguments) {
#if defined(USE_CUDA)
  context_ = GetContext<DeviceType::kGPU>(arguments);
#else
  context_ = GetContext<DeviceType::kCPU>(arguments);
#endif
}

Context *Workspace::Ctx() {
  CHECK_NOTNULL(context_);
  return context_.get();
}

bool Workspace::HasBlob(const std::string &name) const {
  return static_cast<bool>(blob_map_.count(name));
}

DataType Workspace::GetBlobDataType(const std::string &name) const {
  CHECK(HasBlob(name)) << "Unknown blob: " + name;
  return blob_map_.at(name)->data_type();
}

std::vector<int> Workspace::GetBlobShape(const std::string &name) const {
  CHECK(HasBlob(name)) << "Unknown blob: " + name;
  return blob_map_.at(name)->shape();
}

std::shared_ptr<Blob> Workspace::GetBlob(const std::string &name) const {
  if (HasBlob(name)) {
    return blob_map_.at(name);
  } else {
    DLOG(WARNING) << "Blob " << name << " not in the workspace.";
    return nullptr;
  }
}

std::shared_ptr<Blob> Workspace::CreateBlob(const std::string &name,
                                            DataType data_type,
                                            Allocator *allocator) {
  if (!HasBlob(name)) {
    blob_map_[name] = std::make_shared<Blob>(
        name, data_type,
        (allocator == nullptr) ? context_->allocator() : allocator);
  }
  return GetBlob(name);
}

std::shared_ptr<Blob> Workspace::CreateTempBlob(const std::vector<int> &shape,
                                                DataType data_type) {
  auto count =
      std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<size_t>());
  CHECK_GT(count, 0);
  auto temp_blob =
      std::make_shared<Blob>("temp", data_type, context_->allocator());
  temp_blob->share_data(GetTempPtr(count * temp_blob->elem_size()), shape);
  return temp_blob;
}

void Workspace::GrowTempBuffer(size_t raw_size) {
  auto temp_blob = CreateBlob("temp_blob", DataType::kI32);
  size_t num_int = raw_size / temp_blob->elem_size() + 1;
  CHECK_LE(num_int, std::numeric_limits<int>::max());
  temp_blob->reshape({static_cast<int>(num_int)});
  temp_offset_ = 0;
}

size_t Workspace::GetWorkspaceSize() const {
  size_t mem_size = 0;
  for (const auto &blob_it : blob_map_) {
    mem_size += blob_it.second->max_size();
  }
  return mem_size;
}

size_t Workspace::GetWorkspaceTempSize() const {
  const auto temp_blob = GetBlob("temp_blob");
  return temp_blob != nullptr ? temp_blob->max_size() : 0;
}

const void *Workspace::GetTempPtr(size_t raw_size) {
  const auto temp_blob = GetBlob("temp_blob");
  CHECK_NOTNULL(temp_blob);
  CHECK_LE(temp_offset_ + raw_size, temp_blob->max_size());
  const auto *ptr = temp_blob->data<unsigned char>() + temp_offset_;
  temp_offset_ += raw_size;
  return ptr;
}

}  // namespace Shadow
