// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2018-2020, The Qwertycoin Group.
// Copyright (c) 2020-2021, Societatis.io
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

#pragma once

#include <list>
#include <CryptoNoteCore/CryptoNoteBasic.h>
#include <CryptoNoteCore/CryptoNoteSerialization.h>
#include <Serialization/ISerializer.h>
#include <Serialization/SerializationOverloads.h>

namespace CryptoNote {

#define BC_COMMANDS_POOL_BASE 2000

struct BlockCompleteEntry {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(block);
        KV_MEMBER(txs);
    }

    std::string block;
    std::vector<std::string> txs;
};

struct BlockFullInfo : public BlockCompleteEntry {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(block_id);
        KV_MEMBER(block) KV_MEMBER(txs);
    }

    Crypto::Hash block_id;
};

struct TransactionPrefixInfo {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(txHash);
        KV_MEMBER(txPrefix);
    }

    Crypto::Hash txHash;
    TransactionPrefix txPrefix;
};

struct BlockShortInfo {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(blockId);
        KV_MEMBER(block);
        KV_MEMBER(txPrefixes);
    }

    Crypto::Hash blockId;
    std::string block;
    std::vector<TransactionPrefixInfo> txPrefixes;
};

struct NOTIFY_NEW_BLOCK_request {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(b);
        KV_MEMBER(current_blockchain_height);
        KV_MEMBER(hop);
    }

    BlockCompleteEntry b;
    uint32_t current_blockchain_height;
    uint32_t hop;
};

struct NOTIFY_NEW_BLOCK {
    const static int ID = BC_COMMANDS_POOL_BASE + 1;
    typedef NOTIFY_NEW_BLOCK_request request;
};

struct NOTIFY_NEW_TRANSACTIONS_request {
    void serialize(ISerializer &s) { KV_MEMBER(txs); }

    std::vector<std::string> txs;
};

struct NOTIFY_NEW_TRANSACTIONS {
    const static int ID = BC_COMMANDS_POOL_BASE + 2;
    typedef NOTIFY_NEW_TRANSACTIONS_request request;
};

struct NOTIFY_REQUEST_GET_OBJECTS_request {
    void serialize(ISerializer &s)
    {
        serializeAsBinary(txs, "txs", s);
        serializeAsBinary(blocks, "blocks", s);
    }

    std::vector<Crypto::Hash> txs;
    std::vector<Crypto::Hash> blocks;
};

struct NOTIFY_REQUEST_GET_OBJECTS {
    const static int ID = BC_COMMANDS_POOL_BASE + 3;
    typedef NOTIFY_REQUEST_GET_OBJECTS_request request;
};

struct NOTIFY_RESPONSE_GET_OBJECTS_request {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(txs);
        KV_MEMBER(blocks);
        serializeAsBinary(missed_ids, "missed_ids", s);
        KV_MEMBER(current_blockchain_height);
    }

    std::vector<std::string> txs;
    std::vector<BlockCompleteEntry> blocks;
    std::vector<Crypto::Hash> missed_ids;
    uint32_t current_blockchain_height;
};

struct NOTIFY_RESPONSE_GET_OBJECTS {
    const static int ID = BC_COMMANDS_POOL_BASE + 4;
    typedef NOTIFY_RESPONSE_GET_OBJECTS_request request;
};

struct NOTIFY_REQUEST_CHAIN {
    const static int ID = BC_COMMANDS_POOL_BASE + 6;

    struct request {
        void serialize(ISerializer &s) { serializeAsBinary(block_ids, "block_ids", s); }

        /*!
            IDs of the first 10 blocks are sequential, next goes with pow(2,n) offset,
            like 2, 4, 8, 16, 32, 64 and so on, and the last one is always genesis block
        */
        std::vector<Crypto::Hash> block_ids;
    };
};

struct NOTIFY_RESPONSE_CHAIN_ENTRY_request {
    void serialize(ISerializer &s)
    {
        KV_MEMBER(start_height);
        KV_MEMBER(total_height);
        serializeAsBinary(m_block_ids, "m_block_ids", s);
    }

    uint32_t start_height;
    uint32_t total_height;
    std::vector<Crypto::Hash> m_block_ids;
};

struct NOTIFY_RESPONSE_CHAIN_ENTRY {
    const static int ID = BC_COMMANDS_POOL_BASE + 7;
    typedef NOTIFY_RESPONSE_CHAIN_ENTRY_request request;
};

struct NOTIFY_REQUEST_TX_POOL_request {
    void serialize(ISerializer &s) { serializeAsBinary(txs, "txs", s); }

    std::vector<Crypto::Hash> txs;
};

struct NOTIFY_REQUEST_TX_POOL {
    const static int ID = BC_COMMANDS_POOL_BASE + 8;
    typedef NOTIFY_REQUEST_TX_POOL_request request;
};

} // namespace CryptoNote
