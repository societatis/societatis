// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020, Societatis.io
//
// This file is part of Societatis.
//
// Societatis is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Societatis is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Societatis.  If not, see <http://www.gnu.org/licenses/>.

#include <vector>

#include "gtest/gtest.h"

#include "CryptoNoteCore/CryptoNoteBasic.h"
#include "CryptoNoteCore/UpgradeDetector.h"

#include "Logging/ConsoleLogger.h"

namespace {
  using CryptoNote::BLOCK_MAJOR_VERSION_1;
  using CryptoNote::BLOCK_MAJOR_VERSION_2;
  using CryptoNote::BLOCK_MINOR_VERSION_0;
  using CryptoNote::BLOCK_MINOR_VERSION_1;

  struct BlockEx {
    CryptoNote::Block bl;
  };

  typedef std::vector<BlockEx> BlockVector;
  typedef CryptoNote::BasicUpgradeDetector<BlockVector> UpgradeDetector;

  class UpgradeTest : public ::testing::Test {
  public:

    CryptoNote::Currency createCurrency(uint32_t upgradeHeight = UpgradeDetector::UNDEF_HEIGHT) {
      CryptoNote::CurrencyBuilder currencyBuilder(logger);
      currencyBuilder.upgradeVotingThreshold(90);
      currencyBuilder.upgradeVotingWindow(720);
      currencyBuilder.upgradeWindow(720);
      currencyBuilder.upgradeHeightV2(upgradeHeight);
      currencyBuilder.upgradeHeightV3(CryptoNote::UpgradeDetectorBase::UNDEF_HEIGHT);
      return currencyBuilder.currency();
    }

  protected:

    Logging::ConsoleLogger logger;
  };

  class UpgradeDetector_voting_init : public UpgradeTest {};
  class UpgradeDetector_upgradeHeight_init : public UpgradeTest {};
  class UpgradeDetector_voting : public UpgradeTest {};

  void createBlocks(BlockVector& blockchain, size_t count, uint8_t majorVersion, uint8_t minorVersion) {
    for (size_t i = 0; i < count; ++i) {
      BlockEx b;
      b.bl.majorVersion = majorVersion;
      b.bl.minorVersion = minorVersion;
      b.bl.timestamp = 0;
      blockchain.push_back(b);
    }
  }

  void createBlocks(BlockVector& blockchain, UpgradeDetector& upgradeDetector, size_t count, uint8_t majorVersion, uint8_t minorVersion) {
    for (size_t i = 0; i < count; ++i) {
      BlockEx b;
      b.bl.majorVersion = majorVersion;
      b.bl.minorVersion = minorVersion;
      b.bl.timestamp = 0;
      blockchain.push_back(b);
      upgradeDetector.blockPushed();
    }
  }

  void popBlocks(BlockVector& blockchain, UpgradeDetector& upgradeDetector, size_t count) {
    for (size_t i = 0; i < count; ++i) {
      blockchain.pop_back();
      upgradeDetector.blockPopped();
    }
  }

  TEST_F(UpgradeDetector_voting_init, handlesEmptyBlockchain) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_voting_init, votingIsNotCompleteDueShortBlockchain) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    createBlocks(blocks, currency.upgradeVotingWindow() - 1, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_voting_init, votingIsCompleteAfterMinimumNumberOfBlocks) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    createBlocks(blocks, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), currency.upgradeVotingWindow() - 1);
  }

  TEST_F(UpgradeDetector_voting_init, votingIsNotCompleteDueLackOfVoices) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    createBlocks(blocks, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, currency.minNumberVotingBlocks() - 1, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_voting_init, votingIsCompleteAfterMinimumNumberOfVoices) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    createBlocks(blocks, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, currency.minNumberVotingBlocks(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), blocks.size() - 1);
  }

  TEST_F(UpgradeDetector_voting_init, handlesOneCompleteUpgrade) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    createBlocks(blocks, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    uint64_t upgradeHeight = currency.calculateUpgradeHeight(blocks.size() - 1);
    createBlocks(blocks, upgradeHeight - blocks.size(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    // Upgrade is here
    createBlocks(blocks, 1, BLOCK_MAJOR_VERSION_2, BLOCK_MINOR_VERSION_0);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), currency.upgradeVotingWindow() - 1);
    ASSERT_EQ(upgradeDetector.upgradeHeight(), upgradeHeight);
  }

  TEST_F(UpgradeDetector_upgradeHeight_init, handlesEmptyBlockchain) {
    const uint64_t upgradeHeight = 17;
    CryptoNote::Currency currency = createCurrency(upgradeHeight);
    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.upgradeHeight(), upgradeHeight);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_upgradeHeight_init, handlesBlockchainBeforeUpgrade) {
    const uint64_t upgradeHeight = 17;
    CryptoNote::Currency currency = createCurrency(upgradeHeight);
    BlockVector blocks;
    createBlocks(blocks, upgradeHeight, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.upgradeHeight(), upgradeHeight);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_upgradeHeight_init, handlesBlockchainAtUpgrade) {
    const uint64_t upgradeHeight = 17;
    CryptoNote::Currency currency = createCurrency(upgradeHeight);
    BlockVector blocks;
    createBlocks(blocks, upgradeHeight + 1, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.upgradeHeight(), upgradeHeight);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_upgradeHeight_init, handlesBlockchainAfterUpgrade) {
    const uint64_t upgradeHeight = 17;
    CryptoNote::Currency currency = createCurrency(upgradeHeight);
    BlockVector blocks;
    createBlocks(blocks, upgradeHeight + 1, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    createBlocks(blocks, 1, BLOCK_MAJOR_VERSION_2, BLOCK_MINOR_VERSION_0);

    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());
    ASSERT_EQ(upgradeDetector.upgradeHeight(), upgradeHeight);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_voting, handlesVotingCompleteStartingEmptyBlockchain) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());

    createBlocks(blocks, upgradeDetector, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, upgradeDetector, currency.minNumberVotingBlocks(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), blocks.size() - 1);
  }

  TEST_F(UpgradeDetector_voting, handlesVotingCompleteStartingNonEmptyBlockchain) {
    CryptoNote::Currency currency = createCurrency();
    assert(currency.minNumberVotingBlocks() >= 2);
    const uint64_t portion = currency.minNumberVotingBlocks() - currency.minNumberVotingBlocks() / 2;

    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);

    createBlocks(blocks, upgradeDetector, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, upgradeDetector, currency.minNumberVotingBlocks() - portion, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);

    ASSERT_TRUE(upgradeDetector.init());
    createBlocks(blocks, upgradeDetector, portion, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), blocks.size() - 1);
  }

  TEST_F(UpgradeDetector_voting, handlesVotingCancelling) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());

    createBlocks(blocks, upgradeDetector, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, upgradeDetector, currency.minNumberVotingBlocks(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    uint64_t votingCompleteHeight = blocks.size() - 1;
    uint64_t hadrforkHeight = currency.calculateUpgradeHeight(votingCompleteHeight);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), votingCompleteHeight);

    createBlocks(blocks, upgradeDetector, hadrforkHeight - votingCompleteHeight - 1, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), votingCompleteHeight);

    // Cancel voting
    popBlocks(blocks, upgradeDetector, hadrforkHeight - votingCompleteHeight - 1);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), votingCompleteHeight);
    popBlocks(blocks, upgradeDetector, 1);
    ASSERT_EQ(upgradeDetector.votingCompleteHeight(), UpgradeDetector::UNDEF_HEIGHT);
  }

  TEST_F(UpgradeDetector_voting, handlesVotingAndUpgradeCancelling) {
    CryptoNote::Currency currency = createCurrency();
    BlockVector blocks;
    UpgradeDetector upgradeDetector(currency, blocks, BLOCK_MAJOR_VERSION_2, logger);
    ASSERT_TRUE(upgradeDetector.init());

    createBlocks(blocks, upgradeDetector, currency.upgradeVotingWindow(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, upgradeDetector, currency.minNumberVotingBlocks(), BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_1);
    uint64_t votingCompleteHeight = blocks.size() - 1;
    uint64_t hadrforkHeight = currency.calculateUpgradeHeight(votingCompleteHeight);
    ASSERT_EQ(votingCompleteHeight, upgradeDetector.votingCompleteHeight());

    createBlocks(blocks, upgradeDetector, hadrforkHeight - votingCompleteHeight, BLOCK_MAJOR_VERSION_1, BLOCK_MINOR_VERSION_0);
    createBlocks(blocks, upgradeDetector, 1, BLOCK_MAJOR_VERSION_2, BLOCK_MINOR_VERSION_0);
    ASSERT_EQ(votingCompleteHeight, upgradeDetector.votingCompleteHeight());

    // Cancel upgrade (pop block v2)
    popBlocks(blocks, upgradeDetector, 1);
    ASSERT_EQ(votingCompleteHeight, upgradeDetector.votingCompleteHeight());

    // Pop blocks after voting
    popBlocks(blocks, upgradeDetector, hadrforkHeight - votingCompleteHeight);
    ASSERT_EQ(votingCompleteHeight, upgradeDetector.votingCompleteHeight());

    // Cancel voting
    popBlocks(blocks, upgradeDetector, 1);
    ASSERT_EQ(UpgradeDetector::UNDEF_HEIGHT, upgradeDetector.votingCompleteHeight());
  }
}
