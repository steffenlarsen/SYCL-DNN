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
#ifndef SYCLDNN_INCLUDE_CONV2D_CONV_TYPE_H_
#define SYCLDNN_INCLUDE_CONV2D_CONV_TYPE_H_
namespace sycldnn {
namespace conv2d {
/**
 * The possible types of convolution to run. Either the forward pass, the input
 * backprop or the filter backprop.
 */
namespace conv_type {
struct Forward {};
struct InputBackprop {};
struct FilterBackprop {};
}  // namespace conv_type
}  // namespace conv2d
}  // namespace sycldnn
#endif  // SYCLDNN_INCLUDE_CONV2D_CONV_TYPE_H_
