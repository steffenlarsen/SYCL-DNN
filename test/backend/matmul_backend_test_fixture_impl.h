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
#ifndef SYCLDNN_TEST_BACKEND_MATMUL_BACKEND_TEST_FIXTURE_IMPL_H_
#define SYCLDNN_TEST_BACKEND_MATMUL_BACKEND_TEST_FIXTURE_IMPL_H_
#include "test/backend/matmul_backend_test_fixture.h"

#include "sycldnn/helpers/scope_exit.h"

template <typename Backend>
template <bool TransposeLHS, bool TransposeRHS, typename T, typename Index>
void BackendMatmul<Backend>::test_square_matmul(std::vector<T>& lhs,
                                                std::vector<T>& rhs,
                                                std::vector<T>& expected,
                                                Index dim) {
  test_nonsquare_matmul<TransposeLHS, TransposeRHS>(lhs, rhs, expected, dim,
                                                    dim, dim);
}

template <typename Backend>
template <bool TransposeLHS, bool TransposeRHS, typename T, typename Index>
void BackendMatmul<Backend>::test_nonsquare_matmul(std::vector<T>& lhs,
                                                   std::vector<T>& rhs,
                                                   std::vector<T>& expected,
                                                   Index m, Index n, Index k) {
  const auto lhs_size = m * k;
  const auto rhs_size = k * n;
  const auto out_size = m * n;

  std::vector<T> output(out_size, static_cast<T>(0));

  auto& provider = this->provider_;
  auto& backend = provider.get_backend();

  T* lhs_ptr = provider.get_initialised_device_memory(lhs_size, lhs);
  T* rhs_ptr = provider.get_initialised_device_memory(rhs_size, rhs);
  T* out_ptr = provider.get_initialised_device_memory(out_size, output);

  backend.template matmul<TransposeLHS, TransposeRHS>(
      lhs_ptr, rhs_ptr, out_ptr, static_cast<T>(0), m, k, n);

  provider.copy_device_data_to_host(out_size, out_ptr, output);

  for (int i = 0; i < m * n; ++i) {
    EXPECT_EQ(expected[i], output[i]);
  }
}

template <typename Backend>
template <bool TransposeLHS, bool TransposeRHS, typename T, typename Index>
void BackendMatmul<Backend>::test_square_batch_matmul(std::vector<T>& lhs,
                                                      std::vector<T>& rhs,
                                                      std::vector<T>& expected,
                                                      Index batch, Index dim) {
  const auto size = batch * dim * dim;

  std::vector<T> output(size, static_cast<T>(0));

  auto& provider = this->provider_;
  auto& backend = provider.get_backend();

  T* lhs_ptr = provider.get_initialised_device_memory(size, lhs);
  T* rhs_ptr = provider.get_initialised_device_memory(size, rhs);
  T* out_ptr = provider.get_initialised_device_memory(size, output);
  SNN_ON_SCOPE_EXIT {
    provider.deallocate_ptr(out_ptr);
    provider.deallocate_ptr(rhs_ptr);
    provider.deallocate_ptr(lhs_ptr);
  };

  backend.template batch_matmul<TransposeLHS, TransposeRHS>(
      lhs_ptr, rhs_ptr, out_ptr, batch, dim, dim, dim);

  provider.copy_device_data_to_host(size, out_ptr, output);

  for (int i = 0; i < size; ++i) {
    EXPECT_EQ(expected[i], output[i]);
  }
}
#endif  // SYCLDNN_TEST_BACKEND_MATMUL_BACKEND_TEST_FIXTURE_IMPL_H_
