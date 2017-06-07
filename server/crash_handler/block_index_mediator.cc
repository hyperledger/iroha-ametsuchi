#include <block_index_mediator.h>
#include <block_parser_protobuf.h>
#include <block_store_flat.h>
#include <flat_iterator.h>

namespace crash_handler {

BlockIndexMediator::BlockIndexMediator(BlockIndex &block_index,
                                       BlockStore &block_store)
    : block_index_(block_index), block_store_(block_store) {}

bool BlockIndexMediator::validate() {
  if (block_index_.get_last_blockid() == block_store_.last_id()) {
    return true;
  } else {
    auto last_blockid = block_index_.get_last_blockid();
    block_store::BlockStoreFlat block_store_flat =
        *dynamic_cast<block_store::BlockStoreFlat *>(
            &block_store_);  // TODO remove kakaha
    auto it = block_store_flat.begin() + last_blockid;
    for (;
         it < block_store_flat.end(); it++) {
      auto block_blob = *it;

      std::string hash(32, '\0');
      utils::sha3_256((unsigned char*)&hash.at(0), block_blob.data(), 0);
      block_index_.add_blockhash_blockid(hash, ++last_blockid);
    }
    return true;
  }
}
}