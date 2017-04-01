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

#ifndef AMETSUCHI_TX_STORE_H
#define AMETSUCHI_TX_STORE_H

#include <ametsuchi/generated/transaction_generated.h>
#include <string>
#include <vector>

namespace ametsuchi {
namespace tx_store {


void append(const std::vector<ByteArray> &batch);

void append(const ByteArray &tx);

void commit();

void rollback();


std::vector<ByteArray> getAddTxByCreator(const std::string &publicKey);
std::vector<ByteArray> getTransferTxBySender(const std::string &publicKey);
std::vector<ByteArray> getTransferTxByReceiver(const std::string &publicKey);

}  // namespace tx_store
}  // namespace ametsuchi

#endif  // AMETSUCHI_TX_STORE_H
