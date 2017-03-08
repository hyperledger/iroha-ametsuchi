/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>
#include <benchmark/benchmark.h>

static void FileWrite(benchmark::State& state) {
  using ametsuchi::ByteArray;
  using ametsuchi::file::ReadWriteFile;

  std::string filename = "/tmp/test1";
  ReadWriteFile f(filename);

  //    ByteArray data(state.range(0), 1u);

  while (state.KeepRunning()) {
    f.remove();
    f.open();

    for (uint32_t i = 0; i < state.range(0); i++) {
      f.write(ByteArray({1, 2, 3, 4, 5, 6, 7}));
    }
    f.close();
  }
}

BENCHMARK(FileWrite)->Range(1, 1 << 20);
BENCHMARK_MAIN()