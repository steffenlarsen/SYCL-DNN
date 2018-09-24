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
#ifndef SYCLDNN_BENCH_POOLING_FIXTURE_H_
#define SYCLDNN_BENCH_POOLING_FIXTURE_H_

#include <benchmark/benchmark.h>

#include "sycldnn/pooling/sizes.h"

extern const char* commit_date;
extern const char* commit_hash;

class BasePoolingBenchmark : public benchmark::Fixture {
 protected:
  using State = benchmark::State;
  using PoolingParams = sycldnn::pooling::PoolingParams;
  using PoolingSizes = sycldnn::pooling::PoolingSizes;

  // Adds the pooling parameters to the counter set.
  void add_param_counters(State& state, PoolingParams const& params);

  // Adds theoretical best-case bandwidth requirements to the counter set.
  template <typename T>
  void add_bandwidth_counters(State& state, PoolingSizes const& sizes);

  // Records the number of elements processed to the counter set. How this
  // calculated varies based on the type of operation.
  template <typename Direction>
  void set_items_processed(benchmark::State& state,
                           PoolingParams const& params);

  // Serializes the key-value map into a single comma separated string and
  // stores it in the benchmark label.
  void set_label(State& state);

  // A map holding key-value pairs to be emitted along with the counter set.
  std::map<std::string, std::string> key_value_map;
};

// Add a full set of counters corresponding to the pooling parameters.
void BasePoolingBenchmark::add_param_counters(benchmark::State& state,
                                              PoolingParams const& params) {
  state.counters["batch"] = params.batch;
  state.counters["in_rows"] = params.in_rows;
  state.counters["in_cols"] = params.in_cols;
  state.counters["channels"] = params.channels;
  state.counters["out_rows"] = params.out_rows;
  state.counters["out_cols"] = params.out_cols;
  state.counters["stride_rows"] = params.stride_rows;
  state.counters["stride_cols"] = params.stride_cols;
  state.counters["fil_rows"] = params.window_rows;
  state.counters["fil_cols"] = params.window_cols;
  state.counters["pad_rows"] = params.pad_rows;
  state.counters["pad_cols"] = params.pad_cols;
}

// Calculate the optimal bandwidth requirements, and add corresponding counters.
// This assumes each input element is read exactly once, rather than the actual
// behaviour where multiple threads may re-read the same values.
template <typename ElementType>
void BasePoolingBenchmark::add_bandwidth_counters(benchmark::State& state,
                                                  PoolingSizes const& sizes) {
  // Compute the size of each element in bytes.
  auto element_bytes = sizeof(ElementType);

  state.counters["bytes_read"] = sizes.input_size * element_bytes;
  state.counters["bytes_written"] = sizes.output_size * element_bytes;
}

// Records the number of elements processed to the counter set. How this
// calculated varies based on the type of operation.
template <>
void BasePoolingBenchmark::set_items_processed<sycldnn::pooling::Forward>(
    benchmark::State& state, PoolingParams const& params) {
  // We define items processed as neighbourhood size * output tensor size for
  // forwards pooling operations.
  auto window_size = params.window_rows * params.window_cols;
  auto tensor_size =
      params.batch * params.out_rows * params.out_cols * params.channels;

  state.SetItemsProcessed(state.iterations() * window_size * tensor_size);
}

void BasePoolingBenchmark::set_label(benchmark::State& state) {
  std::string label;
  for (auto& kv : key_value_map) {
    if (label.size()) label += ",";

    label += kv.first + "=" + kv.second;
  }
  state.SetLabel(label);
}

#endif  // define SYCLDNN_BENCH_POOLING_FIXTURE_H_
