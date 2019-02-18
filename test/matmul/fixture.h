/*
 * Copyright 2018 Codeplay Software Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use these files except in compliance with the License.
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
#ifndef SYCLDNN_TEST_MATMUL_FIXTURE_H_
#define SYCLDNN_TEST_MATMUL_FIXTURE_H_

#include <gtest/gtest.h>
#include <vector>

// TODO(jwlawson): remove cassert when no longer needed before Eigen include
#include <cassert>
#include <unsupported/Eigen/CXX11/Tensor>

#include "sycldnn/backend/eigen_backend.h"
#include "sycldnn/matmul/launch.h"
#include "test/backend/backend_test_fixture.h"
#include "test/gen/iota_initialised_data.h"

template <typename T, bool TransposeLhs, bool TransposeRhs>
struct MatmulFixture
    : public BackendTestFixture<sycldnn::backend::EigenBackend> {
  using DataType = T;

 protected:
  void run(std::vector<DataType> const& exp, int batches, int m, int k, int n,
           DataType beta, int lhs_offset, int rhs_offset, int out_offset,
           DataType max_val) {
    size_t lhs_size = batches * m * k + lhs_offset;
    size_t rhs_size = batches * k * n + rhs_offset;
    size_t out_size = batches * m * n + out_offset;
    ASSERT_EQ(out_size, exp.size());

    std::vector<DataType> lhs_data = iota_initialised_data(lhs_size, max_val);
    std::vector<DataType> rhs_data = iota_initialised_data(rhs_size, max_val);
    std::vector<DataType> out_data = iota_initialised_data(out_size, max_val);

    auto& provider = this->provider_;
    auto& backend = provider.get_backend();

    {
      auto lhs_gpu = provider.get_initialised_device_memory(lhs_size, lhs_data);
      auto rhs_gpu = provider.get_initialised_device_memory(rhs_size, rhs_data);
      auto out_gpu = provider.get_initialised_device_memory(out_size, out_data);

      auto status =
          sycldnn::matmul::launch<DataType, TransposeLhs, TransposeRhs>(
              lhs_gpu + lhs_offset, rhs_gpu + rhs_offset, out_gpu + out_offset,
              batches, m, k, n, beta, backend);

      ASSERT_EQ(sycldnn::StatusCode::OK, status.status);
      status.event.wait_and_throw();

      provider.copy_device_data_to_host(out_size, out_gpu, out_data);
      provider.deallocate_ptr(lhs_gpu);
      provider.deallocate_ptr(rhs_gpu);
      provider.deallocate_ptr(out_gpu);
    }

    for (size_t i = 0; i < exp.size(); ++i) {
      SCOPED_TRACE("Element: " + std::to_string(i));
      if (std::is_same<DataType, double>::value) {
        EXPECT_DOUBLE_EQ(exp[i], out_data[i]);
      } else {
        EXPECT_FLOAT_EQ(exp[i], out_data[i]);
      }
    }
  }
};

#endif  // SYCLDNN_TEST_MATMUL_FIXTURE_H_
