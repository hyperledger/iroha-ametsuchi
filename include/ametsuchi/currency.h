/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#ifndef AMETSUCHI_CURRENCY_H
#define AMETSUCHI_CURRENCY_H

#include <stdint.h>
#include <string>

namespace ametsuchi {

class Currency {
 public:
  explicit Currency(uint64_t amount, uint8_t precision = 2);

  Currency operator+(const Currency &a, const Currency &b);
  Currency operator-(const Currency &a, const Currency &b);
  Currency operator+(const Currency &a, uint64_t num);
  Currency operator-(const Currency &a, uint64_t num);

  uint64_t integer();
  uint64_t fractional();

  std::string to_string();

 private:
  uint64_t amount_;
  uint8_t precision_;
  uint64_t div_;
};
}  // namespace ametsuchi

#endif  // AMETSUCHI_CURRENCY_H