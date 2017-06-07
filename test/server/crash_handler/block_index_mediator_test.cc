//
// Created by kamilsa on 06.06.17.
//

#include <gtest/gtest.h>
#include <block_store_flat.h>
#include <block_index_redis.h>
#include <hash.h>
#include <block_index_mediator.h>

class BlockIndexMediatorTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cpp_redis::redis_client client;
    client.connect(host_, port_);
    client.flushall();
    client.disconnect();
    system(("rm -rf " + block_store_path).c_str());
  }

  std::string host_ = "localhost";
  size_t port_ = 6379;

  std::string block_store_path = "/tmp/dump";
};

TEST_F(BlockIndexMediatorTest, valid){
  block_store::BlockStoreFlat bl_store(block_store_path);
  block_index::BlockIndexRedis bl_index("localhost", 6379);

  std::vector<uint8_t > blob1({0x1, 0x2, 0x2});
  std::vector<uint8_t > blob2({0x3, 0x4});

  std::string hash1(32, '\0');
  utils::sha3_256((unsigned char *)&hash1.at(0), blob1.data(), 0);

  std::string hash2(32, '\0');
  utils::sha3_256((unsigned char *)&hash2.at(0), blob2.data(), 0);

  size_t id1 = bl_store.append(blob1);
  bl_index.add_blockhash_blockid(hash1, id1);

  size_t id2 = bl_store.append(blob2);
  bl_index.add_blockhash_blockid(hash2, id2);

  ASSERT_EQ(bl_index.get_last_blockid(), bl_store.last_id());

  crash_handler::BlockIndexMediator bim(bl_index, bl_store);
  ASSERT_TRUE(bim.validate());
}

TEST_F(BlockIndexMediatorTest, invalid_to_valid){
  block_store::BlockStoreFlat bl_store(block_store_path);
  block_index::BlockIndexRedis bl_index("localhost", 6379);

  std::vector<uint8_t > blob1({0x1, 0x2, 0x2});
  std::vector<uint8_t > blob2({0x3, 0x4});

  std::string hash1(32, '\0');
  utils::sha3_256((unsigned char *)&hash1.at(0), blob1.data(), 0);

  std::string hash2(32, '\0');
  utils::sha3_256((unsigned char *)&hash2.at(0), blob2.data(), 0);

  size_t id1 = bl_store.append(blob1);
  bl_index.add_blockhash_blockid(hash1, id1);

  size_t id2 = bl_store.append(blob2);

  ASSERT_NE(bl_index.get_last_blockid(), bl_store.last_id());

  crash_handler::BlockIndexMediator bim(bl_index, bl_store);
  ASSERT_TRUE(bim.validate());
  ASSERT_EQ(bl_index.get_last_blockid(), bl_store.last_id());
}