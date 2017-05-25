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


#include <ametsuchi/utils/transaction_parser.h>
#include <sstream>

namespace ametsuchi{
namespace utils{

void TransactionParser::split(std::string raw) {
  char delimeter = '_';
  std::stringstream ss(raw); // Turn the string into a stream.
  std::string tok;

  bool is_first  = true;
  while(getline(ss, tok, delimeter)) {
    if (is_first)
    {
      hash = tok;
      is_first = false;

    }
    else
    {
      actions.push_back(tok);
    }

  }

}
std::vector<std::string> TransactionParser::get_actions() {
  return actions;
}
std::string TransactionParser::get_hash() {
  return hash;
}
TransactionParser::TransactionParser(std::string raw) {
  split(raw);
}

}
}