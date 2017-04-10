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

#include <ametsuchi/merkle_tree/merkle_tree.h>

namespace ametsuchi {
namespace merkle {

/**
 * Returns floor(log2(x))
 * log2(0) => undefined behaviour
 */
static inline size_t log2(size_t x) {
  size_t y = 0;
  while (x > 1) {
    x >>= 1;
    y++;
  }
  return y;
}

/**
 * Returns integer log2 of x
 * ceil2(5) = 8
 */
static inline size_t ceil2(size_t x) {
  size_t l = 1u << log2(x);
  return l == x ? l : l + 1;
}

MerkleTree::MerkleTree(size_t leafs) : leafs_(0) {
  // round number of leafs to the power of 2
  leafs_ = ceil2(leafs);

  // full tree size
  size_ = leafs_ * 2 - 1;
  tree_.resize(size_);

  i_current_ = leafs_ - 1;
  i_root_ = i_current_;
}

hash_t MerkleTree::root() { return tree_[i_root_]; }

void MerkleTree::push(const hash_t &item) {
  if (i_current_ == leafs_ - 1) {
    // this is the very first push. just move item to the leftmost leaf
    tree_[i_current_] = item;
    i_root_ = i_current_++;
    return;
  }

  if (i_current_ == size_) {
    // tree is complete, logically means creation of a NEW BLOCK
    tree_[leafs_ - 1] = tree_[0];  // copy root to leftmost leaf
    i_root_ = leafs_ - 1;          // change root pointer
    i_current_ = leafs_;           // change pointer to current free cell
  }

  // copy hash to current empty position
  tree_[i_current_] = item;

  // find LCA(leftmost leaf, i_current_)
  // LCA is this many levels above:
  size_t np = 1 + log2(i_current_ - (leafs_ - 1));
  size_t subtree_root = parent(i_current_);
  size_t current = i_current_;
  for (size_t i = 0; i < np; i++) {
    size_t left = this->left(subtree_root);
    size_t right = this->right(subtree_root);
    if (current == left) {
      // no right child, just pass left child as hash to parent
      tree_[subtree_root] = tree_[left];
    } else {
      tree_[subtree_root] = hash(tree_[left], tree_[right]);
    }

    // new root resides at this cell:
    i_root_ = subtree_root;

    subtree_root = parent(subtree_root);
    current = parent(current);
  }

  i_current_++;
}

void MerkleTree::push(hash_t &&item) {
  auto it = std::move(item);
  this->push(it);
}

inline size_t MerkleTree::left(size_t parent) { return parent * 2 + 1; }

inline size_t MerkleTree::right(size_t parent) { return parent * 2 + 2; }

inline size_t MerkleTree::parent(size_t node) {
  return node == 0 ? 0 : (node - 1) / 2;
}

hash_t MerkleTree::hash(const hash_t &a, const hash_t &b) {
  std::array<uint8_t, 2 * HASH_LEN> input;
  hash_t output;

  // concatenate input hashes
  std::copy(a.begin(), a.end(), &input[0]);
  std::copy(b.begin(), b.end(), &input[HASH_LEN]);

  SHA3_256(output.data(), input.data(), input.size());

  return output;
}

hash_t MerkleTree::hash(const std::vector<uint8_t> &data) {
  hash_t output;
  SHA3_256(output.data(), data.data(), data.size());
  return output;
}

}  // namespace merkle
}  // namespace ametsuchi
