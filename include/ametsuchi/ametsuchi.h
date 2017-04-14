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

#ifndef AMETSUCHI_DB_H
#define AMETSUCHI_DB_H

#include <ametsuchi/currency.h>
#include <ametsuchi/generated/commands_generated.h>
#include <ametsuchi/merkle_tree/merkle_tree.h>
#include <ametsuchi/tx_store.h>
#include <ametsuchi/wsv.h>
#include <flatbuffers/flatbuffers.h>
#include <lmdb.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

extern "C" {
#include <SimpleFIPS202.h>
}

#ifndef AMETSUCHI_MAX_DB_SIZE
#define AMETSUCHI_MAX_DB_SIZE (8L * 1024 * 1024 * 1024)  // 8 GB
#endif

#ifndef AMETSUCHI_BLOCK_SIZE
#define AMETSUCHI_BLOCK_SIZE (1024)  // the number of leafs in merkle tree
#endif

namespace ametsuchi {


/**
 * Main class for the database.
 *  - single Ametsuchi instance for the single database
 *  - single writer thread
 *  - multiple readers threads, new read-only transaction for each thread
 *  - all data is stored as root flatbuffers
 */
class Ametsuchi {
 public:
  explicit Ametsuchi(const std::string &db_folder);
  ~Ametsuchi();

  /**
   * Append root flatbuffer Transaction to the Ametsuchi database.
   * @throw exception::InvalidTransaction with the reason (one of enum values)
   * @throw exception::InternalError with the reason (one of enum values)
   * @param tx root type Transaction (contents of TransactionWrapper->tx array)
   * @return new merkle root
   */
  merkle::hash_t append(const flatbuffers::Vector<uint8_t> *tx);
  merkle::hash_t append(
      const std::vector<flatbuffers::Vector<uint8_t> *> &batch);

  /**
   * Commit appended data to database. Commit creates the latest 'checkpoint',
   * when you can not rollback.
   */
  void commit();

  /**
   * You can rollback appended transaction(s) to previous commit.
   */
  void rollback();

  merkle::hash_t merkle_root();

  /**
 * Returns all assets, which belong to user with \p pubKey.
 * @param pubKey - account's public key
 * @param uncommitted - if true, include uncommitted changes to search.
 * Otherwise create new read-only TX
 * @return 0 or * pairs <pointer, size>, which are mmaped into memory.
 */
  std::vector<AM_val> accountGetAllAssets(const flatbuffers::String *pubKey,
                                          bool uncommitted = false);

  /**
   * Returns specific asset, which belong to user with \p pubKey.
   * @param pubKey - account's public key
   * @param ln - ledger name
   * @param dn - domain name
   * @param an - asset (currency) name
   * @param uncommitted - if true, include uncommitted changes to search.
 * Otherwise create new read-only TX
   * @return pair <pointer, size>, which are mmaped from disk
   */
  AM_val accountGetAsset(const flatbuffers::String *pubKey,
                         const flatbuffers::String *ln,
                         const flatbuffers::String *dn,
                         const flatbuffers::String *cn,
                         bool uncommitted = false);

 private:
  /* for internal use only */

  std::string path_;
  MDB_env *env;
  MDB_stat mst;
  MDB_txn *append_tx_;  // pointer to db transaction

  TxStore tx_store;
  WSV wsv;


  void init();

  void init_append_tx();
  void abort_append_tx();


  // [ledger+domain+asset] => ComplexAsset/Currency flatbuffer (without amount)
  std::unordered_map<std::string, std::vector<uint8_t>> created_assets_;

  merkle::MerkleTree tree;
};

}  // namespace ametsuchi

#endif  // AMETSUCHI_DB_H
