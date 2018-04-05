/*
 * Copyright 2018 Codeplay Software Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SYCLDNN_INCLUDE_CONV2D_TILED_SELECTOR_H_
#define SYCLDNN_INCLUDE_CONV2D_TILED_SELECTOR_H_

#include "sycldnn/conv2d/algorithm.h"
#include "sycldnn/conv2d/params.h"

#include "sycldnn/conv2d/selector/selector.h"

namespace sycldnn {
namespace conv2d {
/** A selector which will returns the tiled algorithm if supported. */
class TiledSelector final : public Selector {
 public:
  Algorithm select(Conv2DParams const& params) override {
    if (params.window_rows != params.window_cols ||
        params.stride_rows != params.stride_cols) {
      return Algorithm::NotSupported;
    }
    if (params.window_rows == 1 && params.stride_rows == 2) {
      return Algorithm::Tiled;
    }
    if (params.window_rows == 1 && params.stride_rows == 1) {
      return Algorithm::Tiled;
    }
    if (params.window_rows == 3 && params.stride_rows == 2) {
      return Algorithm::Tiled;
    }
    if (params.window_rows == 3 && params.stride_rows == 1) {
      return Algorithm::Tiled;
    }
    if (params.window_rows == 5 && params.stride_rows == 1) {
      return Algorithm::Tiled;
    }
    return Algorithm::NotSupported;
  }
};
}  // namespace conv2d
}  // namespace sycldnn
#endif  // SYCLDNN_INCLUDE_CONV2D_TILED_SELECTOR_H_
