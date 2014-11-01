#ifndef ENTITY_BLOB_HPP_
#define ENTITY_BLOB_HPP_

#include "common.hpp"

#include <vector>
#include <cstdint>
#include <utility>
#include <string>

namespace entity {

class Blob {
public:
  Blob(const int num_row, const int num_col = 1) 
      : num_row_(num_row), num_col_(num_col), count_(num_row * num_col) { 
    if (num_col == 1 
        || entity::Context::dist_metric_mode() == entity::Context::DIAG) {
      data_ = new float[num_row_];
    } else if (entity::Context::dist_metric_mode() == entity::Context::EDIAG) {
      data_ = new float[1];
    } else if (entity::Context::dist_metric_mode() == entity::Context::FULL) {
      data_ = new float[count_];
    } else {
      LOG(FATAL) << "Unknown metric mode.";
    }
  };

  ~Blob() { delete data_; };
  
  // read only 
  const float* data() const { return data_; }
  // mutable
  float* mutable_data() { return data_; }

  inline int count() const { return count_; }

  inline float data_at(const int r, const int c = 0) const {
#ifdef DEBUG
    CHECK_LT(r, num_row_);
    CHECK_LT(c, num_col_);
#endif
    return *(data() + offset(r, c));
  }
  
  void init_data_at(const float init_val, const int r, const int c = 0) {
#ifdef DEBUG
    CHECK_LT(r, num_row_);
    CHECK_LT(c, num_col_);
#endif
    *(mutable_data() + offset(r, c)) = init_val;   
  }
  
  void CopyFrom(const Blob* source, const float coeff = 1.0) {
#ifdef DEBUG
    CHECK_EQ(count_, source->count());
#endif
    //memcpy(data_, source->data(), sizeof(float) * count_);
    const float* source_data = source->data();
    for (int idx = 0; idx < count_; ++idx) {
      data_[idx] = source_data[idx] * coeff;
    }  
  }
  void Accumulate(const Blob* source, const float coeff = 1.0){
#ifdef DEBUG
    CHECK_EQ(count_, source->count());
#endif
    const float* source_data = source->data();
    for (int idx = 0; idx < count_; ++idx) {
      data_[idx] += source_data[idx] * coeff;
    }  
  }
 
private:

  inline int offset(const int r, const int c) const {
    if (num_col_ == 1 
        || entity::Context::dist_metric_mode() == entity::Context::DIAG) {
      return r;
    } else if (entity::Context::dist_metric_mode() == entity::Context::EDIAG) {
      return 0;
    } else if (entity::Context::dist_metric_mode() == entity::Context::FULL) {
      return r * num_col_ + c;
    } else {
      LOG(FATAL) << "Unknown metric mode.";
    }
  }
 
private: 
  float* data_;

  // first dimention 
  int num_row_;
  // second dimention
  // = 1 if data_ is a vector
  int num_col_;
  // number of elements
  int count_;
};

}  // namespace entity

#endif
