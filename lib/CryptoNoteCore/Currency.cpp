// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2016-2018  zawy12
// Copyright (c) 2016-2018, The Karbowanec developers
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

#include <algorithm>
#include <cctype>
#include <cmath>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/round.hpp>
#include <Common/Base58.h>
#include <Common/int-util.h>
#include <Common/Math.h>
#include <Common/StringTools.h>
#include <CryptoNoteCore/Account.h>
#include <CryptoNoteCore/CryptoNoteBasicImpl.h>
#include <CryptoNoteCore/CryptoNoteFormatUtils.h>
#include <CryptoNoteCore/CryptoNoteTools.h>
#include <CryptoNoteCore/Currency.h>
#include <CryptoNoteCore/TransactionExtra.h>
#include <CryptoNoteCore/UpgradeDetector.h>
#include <Global/Constants.h>
#include <Global/CryptoNoteConfig.h>

#undef ERROR

using namespace Logging;
using namespace Common;
using namespace Societatis;

namespace CryptoNote {

bool Currency::init()
{
    if (!generateGenesisBlock()) {
        logger(ERROR, BRIGHT_RED) << "Failed to generate genesis block";
        return false;
    }

    if (!get_block_hash(m_genesisBlock, m_genesisBlockHash)) {
        logger(ERROR, BRIGHT_RED) << "Failed to get genesis block hash";
        return false;
    }

    if (isTestnet()) {
        m_upgradeHeightV6 = 100;
        m_governancePercent = 10;
        m_governanceHeightStart = 1;
        m_governanceHeightEnd = 100;
        m_blocksFileName = "testnet_" + m_blocksFileName;
        m_blocksCacheFileName = "testnet_" + m_blocksCacheFileName;
        m_blockIndexesFileName = "testnet_" + m_blockIndexesFileName;
        m_txPoolFileName = "testnet_" + m_txPoolFileName;
        m_blockchainIndicesFileName = "testnet_" + m_blockchainIndicesFileName;
    }

    return true;
}

bool Currency::generateGenesisBlock()
{
    m_genesisBlock = boost::value_initialized<Block>();

    // Hard code coinbase tx in genesis block, because "tru" generating tx use random,
    // but genesis should be always the same
    std::string genesisCoinbaseTxHex = GENESIS_COINBASE_TX_HEX;
    BinaryArray minerTxBlob;

    bool r = fromHex(genesisCoinbaseTxHex, minerTxBlob)
             && fromBinaryArray(m_genesisBlock.baseTransaction, minerTxBlob);

    if (!r) {
        logger(ERROR, BRIGHT_RED) << "failed to parse coinbase tx from hard coded blob";
        return false;
    }

    m_genesisBlock.majorVersion = BLOCK_MAJOR_VERSION_1;
    m_genesisBlock.minorVersion = BLOCK_MINOR_VERSION_0;
    m_genesisBlock.timestamp = 0;
    m_genesisBlock.nonce = 70;
    if (m_testnet) {
        ++m_genesisBlock.nonce;
    }

    //miner::find_nonce_for_given_block(bl, 1, 0);

    return true;
}

size_t Currency::blockGrantedFullRewardZoneByBlockVersion(uint8_t blockMajorVersion) const
{
    return CryptoNote::parameters::CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;
}

uint32_t Currency::upgradeHeight(uint8_t majorVersion) const
{
    if (majorVersion == BLOCK_MAJOR_VERSION_2) {
        return m_upgradeHeightV6;
    } else {
        return static_cast<uint32_t>(-1);
    }
}

bool Currency::getBlockReward(
    uint8_t blockMajorVersion,
    size_t medianSize,
    size_t currentBlockSize,
    uint64_t alreadyGeneratedCoins,
    uint64_t fee,
    uint64_t &reward,
    int64_t &emissionChange,
    uint32_t height,
    uint64_t blockTarget) const
{
    //assert(alreadyGeneratedCoins <= m_moneySupply);
    assert(m_emissionSpeedFactor > 0 && m_emissionSpeedFactor <= 8 * sizeof(uint64_t));

    // Consistency
    double consistency = 1.0;
    double exponent = 0.25; 
    if (height >= CryptoNote::parameters::UPGRADE_HEIGHT_V2 && difficultyTarget() != 0) {
        // blockTarget is (Timestamp of New Block - Timestamp of Previous Block)
        consistency = (double) blockTarget / (double) difficultyTarget();

        // consistency range is 0..2
        if (consistency < 1.0) {
            consistency = std::max<double>(consistency, 0.0);
        }
        else if (consistency > 1.0) {
            consistency = pow(consistency, exponent);
            consistency = std::min<double>(consistency, 2.0);
        }
        else {
            consistency = 1.0;
        }
    }  

    // Tail emission

    uint64_t baseReward = ((m_moneySupply - alreadyGeneratedCoins) >> m_emissionSpeedFactor) * consistency;
    if (alreadyGeneratedCoins + CryptoNote::parameters::TAIL_EMISSION_REWARD >= m_moneySupply
        || baseReward < CryptoNote::parameters::TAIL_EMISSION_REWARD) {
        baseReward = CryptoNote::parameters::TAIL_EMISSION_REWARD;
    }

    size_t blockGrantedFullRewardZone = blockGrantedFullRewardZoneByBlockVersion(blockMajorVersion);
    medianSize = std::max(medianSize, blockGrantedFullRewardZone);
    if (currentBlockSize > medianSize * UINT64_C(2)) {
        logger(TRACE)
            << "Block cumulative size is too big: "
            << currentBlockSize
            << ", expected less than "
            << medianSize * UINT64_C(2);
        return false;
    }

    uint64_t penalizedBaseReward = getPenalizedAmount(baseReward, medianSize, currentBlockSize);
    uint64_t penalizedFee = fee;
    if (cryptonoteCoinVersion() == 1) {
        penalizedFee = getPenalizedAmount(fee, medianSize, currentBlockSize);
    }

    emissionChange = penalizedBaseReward - (fee - penalizedFee);
    reward = penalizedBaseReward + penalizedFee;

    return true;
}

size_t Currency::maxBlockCumulativeSize(uint64_t height) const
{
    assert(height <= std::numeric_limits<uint64_t>::max() / m_maxBlockSizeGrowthSpeedNumerator);

    size_t maxSize =
        static_cast<size_t>(m_maxBlockSizeInitial
        + (height * m_maxBlockSizeGrowthSpeedNumerator) / m_maxBlockSizeGrowthSpeedDenominator);

    assert(maxSize >= m_maxBlockSizeInitial);

    return maxSize;
}

bool Currency::isGovernanceEnabled(uint32_t height) const
{
    if (height >= m_governanceHeightStart && height <= m_governanceHeightEnd) {
        return true;
    }
    else {
        return false;
    }
}

uint64_t Currency::getGovernanceReward(uint64_t base_reward) const
{
    // minimum is 1% to avoid a zero amount and maximum is 50%
    uint16_t percent = (m_governancePercent < 1) ? 1 : (m_governancePercent > 50) ? 50 : m_governancePercent;
    return (uint64_t)(base_reward * (percent * 0.01));
}

bool Currency::validate_government_fee(const Transaction &baseTx) const
{
    AccountKeys governanceKeys;
    getGovernanceAddressAndKey(governanceKeys);

    Crypto::PublicKey txPublicKey = getTransactionPublicKeyFromExtra(baseTx.extra);

    Crypto::KeyDerivation derivation;
    if (!Crypto::generate_key_derivation( txPublicKey,
                                  governanceKeys.viewSecretKey,
                                  derivation)) {
        return false;
    }

    uint64_t minerReward = 0;
    uint64_t governmentFee = 0;
    for (size_t idx = 0; idx < baseTx.outputs.size(); ++idx) {
        minerReward += baseTx.outputs[idx].amount;
        if (baseTx.outputs[idx].target.type() != typeid(CryptoNote::KeyOutput))
            continue;
        Crypto::PublicKey outEphemeralKey;
        Crypto::derive_public_key(derivation, idx,
            governanceKeys.address.spendPublicKey, outEphemeralKey);
        if (outEphemeralKey == boost::get<KeyOutput>(baseTx.outputs[idx].target).key)
            governmentFee += baseTx.outputs[idx].amount;
    }

    return (governmentFee == getGovernanceReward(minerReward));
}

bool Currency::getGovernanceAddressAndKey(AccountKeys& governanceKeys) const
{
    std::string address       = GOVERNANCE_WALLET_ADDRESS;
    std::string viewSecretkey = GOVERNANCE_VIEW_SECRET_KEY;

    AccountPublicAddress governanceAddress = boost::value_initialized<AccountPublicAddress>();
    if (!parseAccountAddressString(address, governanceAddress)) {
        logger(Logging::ERROR)
            << "Failed to parse governance wallet address ("
            << address
            << "), "
            << "Check /lib/Global/CryptoNoteConfig.h";
        return false;
    }

    Crypto::SecretKey governanceViewSecretKey;
    if (!Common::podFromHex(viewSecretkey, governanceViewSecretKey)) {
        logger(Logging::ERROR)
            << "Failed to parse governance view secret key"
            << "Check /lib/Global/CryptoNoteConfig.h";
        return false;
    }

    governanceKeys.address = governanceAddress;
    governanceKeys.viewSecretKey = governanceViewSecretKey;

    return true;
}

bool Currency::constructMinerTx(
    uint8_t blockMajorVersion,
    uint32_t height,
    size_t medianSize,
    uint64_t alreadyGeneratedCoins,
    size_t currentBlockSize,
    uint64_t fee,
    const AccountPublicAddress &minerAddress,
    Transaction &tx,
    const BinaryArray &extraNonce /* = BinaryArray()*/,
    size_t maxOuts /* = 1*/,
    uint64_t blockTarget /* = 0xffffffffffffffff*/) const
{
    if (blockTarget == 0xffffffffffffffff)
        blockTarget = difficultyTarget();
    tx.inputs.clear();
    tx.outputs.clear();
    tx.extra.clear();

    KeyPair txkey = generateKeyPair();
    addTransactionPublicKeyToExtra(tx.extra, txkey.publicKey);
    if (!extraNonce.empty()) {
        if (!addExtraNonceToTransactionExtra(tx.extra, extraNonce)) {
            return false;
        }
    }

    BaseInput in;
    in.blockIndex = height;

    uint64_t governanceReward = 0;
    uint64_t totalRewardWGR = 0; //totalRewardWithoutGovernanceReward

    uint64_t blockReward;
    int64_t emissionChange;

    if (!getBlockReward(
            blockMajorVersion,
            medianSize,
            currentBlockSize,
            alreadyGeneratedCoins,
            fee,
            blockReward,
            emissionChange,
            height,
            blockTarget)
        ) {
        logger(INFO) << "Block is too big";
        return false;
    }

    totalRewardWGR = blockReward;

    // If Governance Fee blockReward is decreased by GOVERNANCE_PERCENT_FEE
    bool enableGovernance = isGovernanceEnabled(height);

    if (enableGovernance) {
        governanceReward = getGovernanceReward(blockReward);

        if (alreadyGeneratedCoins != 0) {
            blockReward -= governanceReward;
            totalRewardWGR  = blockReward + governanceReward;
        }
    }

    std::vector<uint64_t> outAmounts;
    decompose_amount_into_digits(
        blockReward,
        UINT64_C(0),
        [&outAmounts](uint64_t a_chunk) { outAmounts.push_back(a_chunk); },
        [&outAmounts](uint64_t a_dust) { outAmounts.push_back(a_dust); }
    );

    if (maxOuts < 1) {
        logger(ERROR, BRIGHT_RED) << "max_out must be non-zero";
        return false;
    }
    while (maxOuts < outAmounts.size()) {
        outAmounts[outAmounts.size() - 2] += outAmounts.back();
        outAmounts.resize(outAmounts.size() - 1);
    }

    uint64_t summaryAmounts = 0;
    for (size_t no = 0; no < outAmounts.size(); no++) {
        Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
        Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();

        bool r = Crypto::generate_key_derivation(
            minerAddress.viewPublicKey,
            txkey.secretKey,
            derivation
        );

        if (!(r)) {
            logger(ERROR, BRIGHT_RED)
                << "while creating outs: failed to generate_key_derivation("
                << minerAddress.viewPublicKey
                << ", "
                << txkey.secretKey
                << ")";
            return false;
        }

        r = Crypto::derive_public_key(derivation,no,minerAddress.spendPublicKey,outEphemeralPubKey);

        if (!(r)) {
            logger(ERROR, BRIGHT_RED)
                << "while creating outs: failed to derive_public_key("
                << derivation << ", "
                << no << ", "
                << minerAddress.spendPublicKey
                << ")";
            return false;
        }

        KeyOutput tk;
        tk.key = outEphemeralPubKey;

        TransactionOutput out;
        summaryAmounts += out.amount = outAmounts[no];
        out.target = tk;
        tx.outputs.push_back(out);
    }

    if (enableGovernance) {
        AccountKeys governanceKeys;
        getGovernanceAddressAndKey(governanceKeys);

        Crypto::KeyDerivation derivation = boost::value_initialized<Crypto::KeyDerivation>();
        Crypto::PublicKey outEphemeralPubKey = boost::value_initialized<Crypto::PublicKey>();

        bool r = Crypto::generate_key_derivation(governanceKeys.address.viewPublicKey, txkey.secretKey, derivation);
        if (!(r)) {
            logger(ERROR, BRIGHT_RED)
                << "while creating outs: failed to generate_key_derivation("
                << governanceKeys.address.viewPublicKey << ", " << txkey.secretKey << ")";
            return false;
        }
        size_t pos = tx.outputs.size();
        r = Crypto::derive_public_key(derivation, pos++, governanceKeys.address.spendPublicKey, outEphemeralPubKey);
        if (!(r)) {
            logger(ERROR, BRIGHT_RED)
                << "while creating outs: failed to derive_public_key("
                << derivation << ", " << 0 << ", "
                << governanceKeys.address.spendPublicKey << ")";
            return false;
        }
        KeyOutput tk;
        tk.key = outEphemeralPubKey;

        TransactionOutput out;
        summaryAmounts += out.amount = governanceReward;
        out.target = tk;
        tx.outputs.push_back(out);
    }

    if (summaryAmounts != totalRewardWGR) {
        logger(ERROR, BRIGHT_RED)
            << "Failed to construct miner tx, summaryAmounts = "
            << summaryAmounts
            << " not equal blockReward = "
            << blockReward;
        return false;
    }

    tx.version = CURRENT_TRANSACTION_VERSION;
    //lock
    tx.unlockTime = height + m_minedMoneyUnlockWindow;
    tx.inputs.push_back(in);

    return true;
}

bool Currency::isFusionTransaction(
    const std::vector<uint64_t> &inputsAmounts,
    const std::vector<uint64_t> &outputsAmounts,
    size_t size,
    uint32_t height) const
{
    //TODO
    if (size > CryptoNote::parameters::CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE * 30 / 100) {
        logger(ERROR) << "Fusion transaction verification failed: size exceeded max allowed size.";
        return false;
    }

    if (inputsAmounts.size() < fusionTxMinInputCount()) {
        logger(ERROR)
            << "Fusion transaction verification failed: "
            << "inputs count is less than minimum.";
        return false;
    }

    if (inputsAmounts.size() < outputsAmounts.size() * fusionTxMinInOutCountRatio()) {
        logger(ERROR)
            << "Fusion transaction verification failed: "
            << "inputs to outputs count ratio is less than minimum.";
        return false;
    }

    uint64_t inputAmount = 0;
    for (auto amount : inputsAmounts) {
        if (height < CryptoNote::parameters::UPGRADE_HEIGHT_V2) {
            if (amount < defaultDustThreshold()) {
                logger(ERROR)
                    << "Fusion transaction verification failed: amount "
                    << amount
                    << " is less than dust threshold.";
                return false;
            }
        }
        inputAmount += amount;
    }

    std::vector<uint64_t> expectedOutputsAmounts;
    expectedOutputsAmounts.reserve(outputsAmounts.size());
    decomposeAmount(
        inputAmount,
        height < CryptoNote::parameters::UPGRADE_HEIGHT_V2
        ? defaultDustThreshold()
        : UINT64_C(0), expectedOutputsAmounts);
    std::sort(expectedOutputsAmounts.begin(), expectedOutputsAmounts.end());

    bool decompose = expectedOutputsAmounts == outputsAmounts;
    if (!decompose) {
        logger(ERROR)
            << "Fusion transaction verification failed: "
            << "decomposed output amounts do not match expected.";
        return false;
    }

    return true;
}

bool Currency::isFusionTransaction(const Transaction &transaction,size_t size,uint32_t height) const
{
    assert(getObjectBinarySize(transaction) == size);

    std::vector<uint64_t> outputsAmounts;
    outputsAmounts.reserve(transaction.outputs.size());
    for (const TransactionOutput &output : transaction.outputs) {
        outputsAmounts.push_back(output.amount);
    }

    return isFusionTransaction(getInputsAmounts(transaction), outputsAmounts, size, height);
}

bool Currency::isFusionTransaction(const Transaction &transaction, uint32_t height) const
{
    return isFusionTransaction(transaction, getObjectBinarySize(transaction), height);
}

bool Currency::isAmountApplicableInFusionTransactionInput(
    uint64_t amount,
    uint64_t threshold,
    uint32_t height) const
{
    uint8_t ignore;
    return isAmountApplicableInFusionTransactionInput(amount, threshold, ignore, height);
}

bool Currency::isAmountApplicableInFusionTransactionInput(
    uint64_t amount,
    uint64_t threshold,
    uint8_t &amountPowerOfTen,
    uint32_t height) const
{
    if (amount >= threshold) {
        return false;
    }

    if (amount < defaultDustThreshold()) {
        return false;
    }

    auto it = std::lower_bound(
        Constants::PRETTY_AMOUNTS.begin(),
        Constants::PRETTY_AMOUNTS.end(),
        amount
    );
    if (it == Constants::PRETTY_AMOUNTS.end() || amount != *it) {
        return false;
    }

    amountPowerOfTen = static_cast<uint8_t>(std::distance(Constants::PRETTY_AMOUNTS.begin(), it)/9);

    return true;
}

std::string Currency::accountAddressAsString(const AccountBase &account) const
{
    return getAccountAddressAsStr(m_publicAddressBase58Prefix, account.getAccountKeys().address);
}

std::string Currency::accountAddressAsString(const AccountPublicAddress &accountPublicAddress) const
{
    return getAccountAddressAsStr(m_publicAddressBase58Prefix, accountPublicAddress);
}

bool Currency::parseAccountAddressString(const std::string &str, AccountPublicAddress &addr) const
{
    uint64_t prefix;
    if (!CryptoNote::parseAccountAddressString(prefix, addr, str)) {
        return false;
    }

    if (prefix != m_publicAddressBase58Prefix) {
        logger(DEBUGGING)
        << "Wrong address prefix: " << prefix
        << ", expected " << m_publicAddressBase58Prefix;
        return false;
    }

    return true;
}

std::string Currency::formatAmount(uint64_t amount) const
{
    std::string s = std::to_string(amount);
    if (s.size() < m_numberOfDecimalPlaces + 1) {
        s.insert(0, m_numberOfDecimalPlaces + 1 - s.size(), '0');
    }
    s.insert(s.size() - m_numberOfDecimalPlaces, ".");

    return s;
}

std::string Currency::formatAmount(int64_t amount) const
{
    std::string s = formatAmount(static_cast<uint64_t>(std::abs(amount)));

    if (amount < 0) {
        s.insert(0, "-");
    }

    return s;
}

bool Currency::parseAmount(const std::string &str, uint64_t &amount) const
{
    std::string strAmount = str;
    boost::algorithm::trim(strAmount);

    size_t pointIndex = strAmount.find_first_of('.');
    size_t fractionSize;
    if (std::string::npos != pointIndex) {
        fractionSize = strAmount.size() - pointIndex - 1;
        while (m_numberOfDecimalPlaces < fractionSize && '0' == strAmount.back()) {
            strAmount.erase(strAmount.size() - 1, 1);
            --fractionSize;
        }
        if (m_numberOfDecimalPlaces < fractionSize) {
            return false;
        }
        strAmount.erase(pointIndex, 1);
    } else {
        fractionSize = 0;
    }

    if (strAmount.empty()) {
        return false;
    }

    if (!std::all_of(strAmount.begin(), strAmount.end(), ::isdigit)) {
        return false;
    }

    if (fractionSize < m_numberOfDecimalPlaces) {
        strAmount.append(m_numberOfDecimalPlaces - fractionSize, '0');
    }

    return Common::fromString(strAmount, amount);
}

// Copyright (c) 2017-2018 Zawy
// http://zawy1.blogspot.com/2017/12/using-difficulty-to-get-constant-value.html
// Moore's law application by Sergey Kozlov
uint64_t Currency::getMinimalFee(
    uint64_t dailyDifficulty,
    uint64_t reward,
    uint64_t avgHistoricalDifficulty,
    uint64_t medianHistoricalReward,
    uint32_t height) const
{
    return CryptoNote::parameters::MINIMUM_FEE;
}

uint64_t Currency::roundUpMinFee(uint64_t minimalFee, int digits) const
{
    uint64_t ret(0);
    std::string minFeeString = formatAmount(minimalFee);
    double minFee = boost::lexical_cast<double>(minFeeString);
    double scale = pow(10., floor(log10(fabs(minFee))) + (1 - digits));
    double roundedFee = ceil(minFee / scale) * scale;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(12) << roundedFee;
    std::string roundedFeeString = ss.str();
    parseAmount(roundedFeeString, ret);

    return ret;
}

difficulty_type Currency::nextDifficulty(uint32_t height,
    uint8_t blockMajorVersion,
    std::vector<uint64_t> timestamps,
    std::vector<difficulty_type> cumulativeDifficulties,
    uint64_t nextBlockTime, lazy_stat_callback_type &lazy_stat_cb) const
{
    // check if we use special scenario with some fixed diff
    if (CryptoNote::parameters::FIXED_DIFFICULTY > 0)
    {
        logger (WARNING) << "Fixed difficulty is used: " <<
                            CryptoNote::parameters::FIXED_DIFFICULTY;
        return CryptoNote::parameters::FIXED_DIFFICULTY;
    }
    if (m_fixedDifficulty > 0)
    {
        logger (WARNING) << "Fixed difficulty is used: " <<
                            m_fixedDifficulty;
        return m_fixedDifficulty;
    }

    uint64_t last_timestamp = 0;
    if (!timestamps.empty()) {
        last_timestamp = timestamps.back();
    }
    if ((blockMajorVersion >= BLOCK_MAJOR_VERSION_2) &&
            (nextBlockTime > last_timestamp + CryptoNote::parameters::CRYPTONOTE_CLIF_THRESHOLD)) {
        size_t array_size = cumulativeDifficulties.size();
        difficulty_type last_difficulty = 1;
        if (array_size >= 2) {
            last_difficulty = cumulativeDifficulties[array_size - 1] - cumulativeDifficulties[array_size - 2];
        }
        uint64_t currentSolveTime = nextBlockTime - last_timestamp;
        return getClifDifficulty(height, blockMajorVersion,
                               last_difficulty, last_timestamp,
                               currentSolveTime, lazy_stat_cb);
    }

    if (blockMajorVersion >= BLOCK_MAJOR_VERSION_2) {
        return nextDifficultyV6(blockMajorVersion, timestamps, cumulativeDifficulties, height);
    }
    else {
        return nextDifficultyV1(timestamps, cumulativeDifficulties);
    }
}

difficulty_type Currency::nextDifficultyV1(
    std::vector<uint64_t> timestamps,
    std::vector<difficulty_type> cumulativeDifficulties) const
{
    assert(m_difficultyWindow >= 2);

    if (timestamps.size() > m_difficultyWindow) {
        timestamps.resize(m_difficultyWindow);
        cumulativeDifficulties.resize(m_difficultyWindow);
    }

    size_t length = timestamps.size();
    assert(length == cumulativeDifficulties.size());
    assert(length <= m_difficultyWindow);
    if (length <= 1) {
        return 1;
    }

    sort(timestamps.begin(), timestamps.end());

    size_t cutBegin, cutEnd;
    assert(2 * m_difficultyCut <= m_difficultyWindow - 2);
    if (length <= m_difficultyWindow - 2 * m_difficultyCut) {
        cutBegin = 0;
        cutEnd = length;
    } else {
        cutBegin = (length - (m_difficultyWindow - 2 * m_difficultyCut) + 1) / 2;
        cutEnd = cutBegin + (m_difficultyWindow - 2 * m_difficultyCut);
    }
    assert(/*cut_begin >= 0 &&*/ cutBegin + 2 <= cutEnd && cutEnd <= length);
    uint64_t timeSpan = timestamps[cutEnd - 1] - timestamps[cutBegin];
    if (timeSpan == 0) {
        timeSpan = 1;
    }

    difficulty_type totalWork = cumulativeDifficulties[cutEnd - 1]-cumulativeDifficulties[cutBegin];
    assert(totalWork > 0);

    uint64_t low, high;
    low = mul128(totalWork, m_difficultyTarget, &high);
    if (high != 0 || low + timeSpan - 1 < low) {
        return 0;
    }

    return (low + timeSpan - 1) / timeSpan;
}

// difficulty for block version 2.0
difficulty_type Currency::nextDifficultyV6(uint8_t blockMajorVersion,
    std::vector<uint64_t> timestamps,
    std::vector<difficulty_type> cumulativeDifficulties,
    uint32_t height) const
{
    if(isTestnet()){
        return CryptoNote::parameters::DEFAULT_DIFFICULTY;
    }

    if (timestamps.empty()) {
        return CryptoNote::parameters::DEFAULT_DIFFICULTY;
    }
    // Dynamic difficulty calculation window
    uint32_t diffWindow = timestamps.size() - 1;

    difficulty_type nextDiffV6 = CryptoNote::parameters::DEFAULT_DIFFICULTY;
    difficulty_type min_difficulty = CryptoNote::parameters::DEFAULT_DIFFICULTY;
    // Condition #1 When starting a chain or a working testnet requiring
    // block sample gathering until enough blocks are available (Kick-off Scenario)
    // We shall set a baseline for the hashrate that is specific to mining
    // algo and I propose doing so in the config file.
    // *** During this initial sampling period, a best guess is the best strategy.
    // Consider this as a service or trial period.
    // With EPoW reward algo in place, we don't really need to worry about attackers
    // or large miners taking advanatage of our system.
    if (height < CryptoNote::parameters::UPGRADE_HEIGHT_V2 + diffWindow) {
        return nextDiffV6;
    }

    // check all values in input vectors greater than previous value to calc adjacent differences later
    if (std::adjacent_find(timestamps.begin(), timestamps.end(), std::greater<uint64_t>()) != timestamps.end()) {
        logger (ERROR) << "Invalid timestamps for difficulty calculation";
        return nextDiffV6;
    }
    if (std::adjacent_find(cumulativeDifficulties.begin(), cumulativeDifficulties.end(), std::greater_equal<difficulty_type>()) != cumulativeDifficulties.end()) {
        logger (ERROR) << "Invalid cumulativeDifficulties for difficulty calculation";
        return nextDiffV6;
    }

    uint64_t difficulty_target = CryptoNote::parameters::DIFFICULTY_TARGET;
    uint64_t window_target = difficulty_target * diffWindow;
    uint64_t window_time = timestamps.back() - timestamps.front(); // now subtraction is safe,  we know it is not negative

    // get solvetimes from timestamps
    std::vector<uint64_t> solveTimes;
    solveTimes.resize(timestamps.size());
    std::adjacent_difference(timestamps.begin(), timestamps.end(), solveTimes.begin()); // we check it before and all diffs are positive
    solveTimes.erase(solveTimes.begin());

    // get difficulties from the cumulative difficulties
    std::vector<difficulty_type> difficulties;
    difficulties.resize(cumulativeDifficulties.size());
    std::adjacent_difference(cumulativeDifficulties.begin(),
                             cumulativeDifficulties.end(),
                             difficulties.begin()); // we check it before and all diffs are positive
    difficulties.erase(difficulties.begin());
    difficulty_type prev_difficulty = difficulties.back();

    // calc stat values to detect outliers
    double avg_solvetime = Common::meanValue(solveTimes);
    double stddev_solvetime = Common::stddevValue(solveTimes);
    double solvetime_lowborder = 1.0;
    if(avg_solvetime > stddev_solvetime)
        solvetime_lowborder = avg_solvetime - stddev_solvetime;
    double solvetime_highborder = avg_solvetime + stddev_solvetime;
    size_t valid_solvetime_number = 0;
    uint64_t valid_solvetime_sum = 0;
    size_t invalid_solvetime_number = 0;
    uint64_t invalid_solvetime_sum = 0;
    std::for_each(solveTimes.begin(), solveTimes.end(),
                  [solvetime_lowborder, solvetime_highborder,
                   &valid_solvetime_number, &valid_solvetime_sum,
                   &invalid_solvetime_number, &invalid_solvetime_sum] (uint64_t st) {
        if ((st >= solvetime_lowborder) && (st <= solvetime_highborder)) {
            valid_solvetime_number++;
            valid_solvetime_sum += st;
        } else {
            invalid_solvetime_number++;
            invalid_solvetime_sum += st;
        }
    });

    // if there is no "invalid" solvetimes we can use previous difficulty value
    if (invalid_solvetime_number == 0) {
        return std::max(prev_difficulty, min_difficulty);
    }

    // process data with "invalid" solvetimes
    double valid_solvetime_mean = double(valid_solvetime_sum) / valid_solvetime_number;
    double invalid_solvetime_mean = double(invalid_solvetime_sum) / invalid_solvetime_number;

    if ( (window_time >= window_target * 0.97) &&
         (window_time <= window_target * 1.03) ) {
        if (valid_solvetime_mean >= invalid_solvetime_mean) {
            double coef = double(difficulty_target) / double(valid_solvetime_mean);
            if (valid_solvetime_mean < difficulty_target) {
                nextDiffV6 = prev_difficulty * std::min(1.01, coef) + 0.5;
            } else {
                nextDiffV6 = prev_difficulty * std::max(0.99, coef) + 0.5;
            }
        } else {
            double coef = double(difficulty_target) / double(invalid_solvetime_mean);
            if (invalid_solvetime_mean < difficulty_target) {
                nextDiffV6 = prev_difficulty * std::min(1.01, coef) + 0.5;
            } else {
                nextDiffV6 = prev_difficulty * std::max(0.99, coef) + 0.5;
            }
        }
    } else if (window_time < window_target * 0.97) {
        nextDiffV6 = prev_difficulty * 1.02 + 0.5;
    } else {
        nextDiffV6 = prev_difficulty * 0.98 + 0.5;
    }

    return std::max(nextDiffV6, min_difficulty);
}

difficulty_type Currency::getClifDifficulty(uint32_t height,
                                          uint8_t blockMajorVersion,
                                          difficulty_type last_difficulty,
                                          uint64_t last_timestamp,
                                          uint64_t currentSolveTime,
                                          lazy_stat_callback_type &lazy_stat_cb) const
{
    logger (INFO) << "CLIF difficulty inputs: height " << height <<
                     ", block version " << (int)blockMajorVersion <<
                     ", last difficulty " << last_difficulty <<
                     ", current solve time " << currentSolveTime;

    difficulty_type new_diff = last_difficulty;

    if (new_diff > CryptoNote::parameters::DEFAULT_DIFFICULTY) {
        uint64_t correction_interval = currentSolveTime -
                CryptoNote::parameters::CRYPTONOTE_CLIF_THRESHOLD;
        //below equation shall return quotient of the division.
        int decrease_counter = ((int)correction_interval / (int)CryptoNote::parameters::DIFFICULTY_TARGET) + 1;
        int round_counter = 1;

        new_diff = new_diff / 2;
        logger (INFO) << "CLIF decreased difficulty " << round_counter <<
            " times, intermediate difficulty is " << new_diff;
        difficulty_type mean_diff = lazy_stat_cb(IMinerHandler::stat_period::hour, last_timestamp);
        logger (INFO) << "Last hour average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);
        mean_diff = lazy_stat_cb(IMinerHandler::stat_period::day, last_timestamp);
        logger (INFO) << "Last day average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);
        mean_diff = lazy_stat_cb(IMinerHandler::stat_period::week, last_timestamp);
        logger (INFO) << "Last week average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);
        mean_diff = lazy_stat_cb(IMinerHandler::stat_period::month, last_timestamp);
        logger (INFO) << "Last month average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);
        mean_diff = lazy_stat_cb(IMinerHandler::stat_period::halfyear, last_timestamp);
        logger (INFO) << "Last halfyear average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);
        mean_diff = lazy_stat_cb(IMinerHandler::stat_period::year, last_timestamp);
        logger (INFO) << "Last year average difficulty is " << mean_diff;
        if (mean_diff > 0)
            new_diff = std::min(mean_diff, new_diff);

        if (decrease_counter > 1) {
            while (round_counter < decrease_counter) {
                new_diff = new_diff / 2;
                round_counter++;
                if (new_diff <= CryptoNote::parameters::DEFAULT_DIFFICULTY)
                    break;
            }
            logger (INFO) << "CLIF decreased difficulty " << round_counter <<
                " times, intermediate difficulty is " << new_diff;
        }

        new_diff = std::max(new_diff, difficulty_type(CryptoNote::parameters::DEFAULT_DIFFICULTY));
    }

    logger (INFO) << "CLIF difficulty result: " << new_diff;
    return new_diff;
}

bool Currency::checkProofOfWorkV1(
    Crypto::cn_context &context,
    const Block &block,
    difficulty_type currentDiffic,
    Crypto::Hash &proofOfWork) const
{
    if (!get_block_longhash(context, block, proofOfWork)) {
        return false;
    }

    return check_hash(proofOfWork, currentDiffic);
}

bool Currency::checkProofOfWork(
    Crypto::cn_context &context,
    const Block &block,
    difficulty_type currentDiffic,
    Crypto::Hash &proofOfWork) const
{
    switch (block.majorVersion) {
    case BLOCK_MAJOR_VERSION_1:
    case BLOCK_MAJOR_VERSION_2:
        return checkProofOfWorkV1(context, block, currentDiffic, proofOfWork);
    }

    logger(ERROR, BRIGHT_RED)
        << "Unknown block major version: "
        << block.majorVersion
        << "."
        << block.minorVersion;

    return false;
}

size_t Currency::getApproximateMaximumInputCount(
    size_t transactionSize,
    size_t outputCount,
    size_t mixinCount) const
{
    const size_t KEY_IMAGE_SIZE = sizeof(Crypto::KeyImage);
    const size_t OUTPUT_KEY_SIZE = sizeof(decltype(KeyOutput::key));
    const size_t AMOUNT_SIZE = sizeof(uint64_t) + 2; // varint
    const size_t GLOBAL_INDEXES_VECTOR_SIZE_SIZE = sizeof(uint8_t); // varint
    const size_t GLOBAL_INDEXES_INITIAL_VALUE_SIZE = sizeof(uint32_t); // varint
    const size_t GLOBAL_INDEXES_DIFFERENCE_SIZE = sizeof(uint32_t); // varint
    const size_t SIGNATURE_SIZE = sizeof(Crypto::Signature);
    const size_t EXTRA_TAG_SIZE = sizeof(uint8_t);
    const size_t INPUT_TAG_SIZE = sizeof(uint8_t);
    const size_t OUTPUT_TAG_SIZE = sizeof(uint8_t);
    const size_t PUBLIC_KEY_SIZE = sizeof(Crypto::PublicKey);
    const size_t TRANSACTION_VERSION_SIZE = sizeof(uint8_t);
    const size_t TRANSACTION_UNLOCK_TIME_SIZE = sizeof(uint64_t);

    const size_t outputsSize = outputCount * (OUTPUT_TAG_SIZE + OUTPUT_KEY_SIZE + AMOUNT_SIZE);
    const size_t headerSize = TRANSACTION_VERSION_SIZE
                              + TRANSACTION_UNLOCK_TIME_SIZE
                              + EXTRA_TAG_SIZE
                              + PUBLIC_KEY_SIZE;
    const size_t inputSize = INPUT_TAG_SIZE
                             + AMOUNT_SIZE
                             + KEY_IMAGE_SIZE
                             + SIGNATURE_SIZE
                             + GLOBAL_INDEXES_VECTOR_SIZE_SIZE
                             + GLOBAL_INDEXES_INITIAL_VALUE_SIZE
                             + mixinCount * (GLOBAL_INDEXES_DIFFERENCE_SIZE + SIGNATURE_SIZE);

    return (transactionSize - headerSize - outputsSize) / inputSize;
}

CurrencyBuilder::CurrencyBuilder(Logging::ILogger &log)
    : m_currency(log)
{
    maxBlockNumber(parameters::CRYPTONOTE_MAX_BLOCK_NUMBER);
    maxBlockBlobSize(parameters::CRYPTONOTE_MAX_BLOCK_BLOB_SIZE);
    maxTxSize(parameters::CRYPTONOTE_MAX_TX_SIZE);
    publicAddressBase58Prefix(parameters::CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX);
    minedMoneyUnlockWindow(parameters::CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW);
    transactionSpendableAge(parameters::CRYPTONOTE_TX_SPENDABLE_AGE);
    safeTransactionSpendableAge(parameters::CRYPTONOTE_SAFE_TX_SPENDABLE_AGE);
    expectedNumberOfBlocksPerDay(parameters::EXPECTED_NUMBER_OF_BLOCKS_PER_DAY);

    timestampCheckWindow(parameters::BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW);
    blockFutureTimeLimit(parameters::CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT);

    moneySupply(parameters::MONEY_SUPPLY);
    emissionSpeedFactor(parameters::EMISSION_SPEED_FACTOR);
    cryptonoteCoinVersion(parameters::CRYPTONOTE_COIN_VERSION);

    rewardBlocksWindow(parameters::CRYPTONOTE_REWARD_BLOCKS_WINDOW);
    blockGrantedFullRewardZone(parameters::CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
    minerTxBlobReservedSize(parameters::CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE);
    maxTransactionSizeLimit(parameters::MAX_TRANSACTION_SIZE_LIMIT);

    governancePercent(parameters::GOVERNANCE_PERCENT_FEE);
    governanceHeightStart(parameters::GOVERNANCE_HEIGHT_START);
    governanceHeightEnd(parameters::GOVERNANCE_HEIGHT_END);

    minMixin(parameters::MIN_TX_MIXIN_SIZE);
    maxMixin(parameters::MAX_TX_MIXIN_SIZE);

    numberOfDecimalPlaces(parameters::CRYPTONOTE_DISPLAY_DECIMAL_POINT);

    minimumFee(parameters::MINIMUM_FEE);
    defaultDustThreshold(parameters::DEFAULT_DUST_THRESHOLD);

    difficultyTarget(parameters::DIFFICULTY_TARGET);
    difficultyWindow(parameters::DIFFICULTY_WINDOW);
    difficultyLag(parameters::DIFFICULTY_LAG);
    difficultyCut(parameters::DIFFICULTY_CUT);

    maxBlockSizeInitial(parameters::MAX_BLOCK_SIZE_INITIAL);
    maxBlockSizeGrowthSpeedNumerator(parameters::MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR);
    maxBlockSizeGrowthSpeedDenominator(parameters::MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR);

    lockedTxAllowedDeltaSeconds(parameters::CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS);
    lockedTxAllowedDeltaBlocks(parameters::CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS);

    mempoolTxLiveTime(parameters::CRYPTONOTE_MEMPOOL_TX_LIVETIME);
    mempoolTxFromAltBlockLiveTime(parameters::CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME);
    numberOfPeriodsToForgetTxDeletedFromPool(
        parameters::CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL
    );

    fusionTxMaxSize(parameters::FUSION_TX_MAX_SIZE);
    fusionTxMinInputCount(parameters::FUSION_TX_MIN_INPUT_COUNT);
    fusionTxMinInOutCountRatio(parameters::FUSION_TX_MIN_IN_OUT_COUNT_RATIO);

    upgradeHeightV6(parameters::UPGRADE_HEIGHT_V2);
    upgradeVotingThreshold(parameters::UPGRADE_VOTING_THRESHOLD);
    upgradeVotingWindow(parameters::UPGRADE_VOTING_WINDOW);
    upgradeWindow(parameters::UPGRADE_WINDOW);

    blocksFileName(parameters::CRYPTONOTE_BLOCKS_FILENAME);
    blocksCacheFileName(parameters::CRYPTONOTE_BLOCKSCACHE_FILENAME);
    blockIndexesFileName(parameters::CRYPTONOTE_BLOCKINDEXES_FILENAME);
    txPoolFileName(parameters::CRYPTONOTE_POOLDATA_FILENAME);
    blockchainIndicesFileName(parameters::CRYPTONOTE_BLOCKCHAIN_INDICES_FILENAME);

    testnet(false);
    fix_difficulty(0);
}

Transaction CurrencyBuilder::generateGenesisTransaction()
{
    CryptoNote::Transaction tx;
    auto ac = boost::value_initialized<CryptoNote::AccountPublicAddress>();

    m_currency.constructMinerTx(1, 0, 0, 0, 0, 0, ac, tx); // zero fee in genesis

    return tx;
}

CurrencyBuilder& CurrencyBuilder::emissionSpeedFactor(unsigned int val)
{
    if (val <= 0 || val > 8 * sizeof(uint64_t)) {
        throw std::invalid_argument("val at emissionSpeedFactor()");
    }

    m_currency.m_emissionSpeedFactor = val;

    return *this;
}

CurrencyBuilder &CurrencyBuilder::numberOfDecimalPlaces(size_t val)
{
    m_currency.m_numberOfDecimalPlaces = val;
    m_currency.m_coin = 1;
    for (size_t i = 0; i < m_currency.m_numberOfDecimalPlaces; ++i) {
        m_currency.m_coin *= 10;
    }

    return *this;
}

CurrencyBuilder &CurrencyBuilder::difficultyWindow(size_t val)
{
    if (val < 2) {
        throw std::invalid_argument("val at difficultyWindow()");
    }

    m_currency.m_difficultyWindow = val;

    return *this;
}

CurrencyBuilder& CurrencyBuilder::upgradeVotingThreshold(unsigned int val)
{
    if (val <= 0 || val > 100) {
        throw std::invalid_argument("val at upgradeVotingThreshold()");
    }

    m_currency.m_upgradeVotingThreshold = val;

    return *this;
}

CurrencyBuilder &CurrencyBuilder::upgradeWindow(size_t val)
{
    if (val <= 0) {
        throw std::invalid_argument("val at upgradeWindow()");
    }

    m_currency.m_upgradeWindow = static_cast<uint32_t>(val);

    return *this;
}

} // namespace CryptoNote
