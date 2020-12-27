# Generate version.h header
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/Global/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
    @ONLY
)
message(STATUS "Generated version.h header: ${CMAKE_CURRENT_BINARY_DIR}/version.h")

set(SocietatisFramework_INCLUDE_DIRS
    "${Boost_INCLUDE_DIRS}"
    "${Breakpad_INCLUDE_DIRS}"
    "${MINIUPNP_INCLUDE_DIRS}"
    "${SPARSEHASH_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../include"
)

if(MSVC)
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows")
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/msc")
elseif(APPLE)
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX")
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
elseif(ANDROID)
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Android")
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
elseif(FREEBSD)
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD")
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
else()
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux")
    list(APPEND SocietatisFramework_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix")
endif()

set(SocietatisFramework_INCLUDE_DIRS ${SocietatisFramework_INCLUDE_DIRS} PARENT_SCOPE)

# SocietatisFramework

set(SocietatisFramework_LIBS
    SocietatisFramework_BlockchainExplorer
    SocietatisFramework_Breakpad
    SocietatisFramework_Common
    SocietatisFramework_Crypto
    SocietatisFramework_CryptoNoteCore
    SocietatisFramework_CryptoNoteProtocol
    SocietatisFramework_Global
    SocietatisFramework_Http
    SocietatisFramework_InProcessNode
    SocietatisFramework_JsonRpcServer
    SocietatisFramework_Logging
    SocietatisFramework_Mnemonics
    SocietatisFramework_NodeRpcProxy
    SocietatisFramework_P2p
    SocietatisFramework_PaymentGate
    SocietatisFramework_Rpc
    SocietatisFramework_Serialization
    SocietatisFramework_System
    SocietatisFramework_Transfers
    SocietatisFramework_Wallet
)

add_custom_target(SocietatisFramework)
add_dependencies(SocietatisFramework ${SocietatisFramework_LIBS})

# SocietatisFramework::BlockchainExplorer

set(SocietatisFramework_BlockchainExplorer_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorer.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerData.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerDataBuilder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerDataBuilder.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/BlockchainExplorerErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/IBlockchainExplorer.h"
    "${CMAKE_CURRENT_LIST_DIR}/BlockchainExplorer/IBlockchainObserver.h"
)

set(SocietatisFramework_BlockchainExplorer_LIBS
    SocietatisFramework::Global
)

add_library(SocietatisFramework_BlockchainExplorer STATIC ${SocietatisFramework_BlockchainExplorer_SOURCES})
add_library(SocietatisFramework::BlockchainExplorer ALIAS SocietatisFramework_BlockchainExplorer)
target_include_directories(SocietatisFramework_BlockchainExplorer PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_BlockchainExplorer PRIVATE ${SocietatisFramework_BlockchainExplorer_LIBS})

# SocietatisFramework::Breakpad

set(SocietatisFramework_Breakpad_SOURCES
    ${Breakpad_SOURCES} # Compile Google Brekapad sources as part of this lib
    "${CMAKE_CURRENT_LIST_DIR}/Breakpad/Breakpad.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Breakpad/Breakpad.h"
)

set(SocietatisFramework_Breakpad_LIBS
    SocietatisFramework::Global
)

if(APPLE)
    find_library(CoreFoundationLibrary CoreFoundation)
    find_library(CFNetworkLibrary CFNetwork)
    set(SocietatisFramework_Breakpad_LIBS
        ${SocietatisFramework_Breakpad_LIBS}
        ${CoreFoundationLibrary}
        ${CFNetworkLibrary}
    )
endif()

add_library(SocietatisFramework_Breakpad STATIC ${SocietatisFramework_Breakpad_SOURCES})
add_library(SocietatisFramework::Breakpad ALIAS SocietatisFramework_Breakpad)
target_include_directories(SocietatisFramework_Breakpad PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Breakpad PRIVATE ${SocietatisFramework_Breakpad_LIBS})

if(WIN32)
    # Fix for "cannot convert argument 1 from 'const wchar_t *' to 'LPCSTR'" and similar issues
    target_compile_definitions(SocietatisFramework_Breakpad PRIVATE -DUNICODE -D_UNICODE)
endif()

# SocietatisFramework::Common

set(SocietatisFramework_Common_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Common/ArrayRef.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ArrayView.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Base58.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Base58.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/BlockingQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CommandLine.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/CommandLine.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ConsoleTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/DnsTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/DnsTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/FileMappedVector.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/IInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/IOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/JsonValue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/JsonValue.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Lazy.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Math.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/MemoryInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/MemoryInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ObserverManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/PathTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/PathTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ScopeExit.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ScopeExit.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/ShuffleGenerator.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/SignalHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/SignalHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StdOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StreamTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StreamTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringBuffer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringInputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringInputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringView.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/StringView.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Util.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Util.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/Varint.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/VectorOutputStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/VectorOutputStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/VectorUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/int-util.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/pod-class.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/static_assert.h"
    "${CMAKE_CURRENT_LIST_DIR}/Common/HardwareInfo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Common/HardwareInfo.h")

set(SocietatisFramework_Common_LIBS
    Threads::Threads
    SocietatisFramework::Crypto
    SocietatisFramework::Global
)

add_library(SocietatisFramework_Common STATIC ${SocietatisFramework_Common_SOURCES})
add_library(SocietatisFramework::Common ALIAS SocietatisFramework_Common)
target_include_directories(SocietatisFramework_Common PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Common PRIVATE ${SocietatisFramework_Common_LIBS})

if(UNIX AND NOT ANDROID)
    target_link_libraries(SocietatisFramework_Common PUBLIC -lresolv)
endif()

# SocietatisFramework::Crypto

set(SocietatisFramework_Crypto_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/crypto/aesb.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/aesb.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/blake256.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/blake256.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/chacha8.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/chacha8.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/cn_slow_hash.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/cn_slow_hash_hard_arm.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/cn_slow_hash_hard_intel.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/cn_slow_hash_soft.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/crypto-ops-data.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/crypto-ops.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/crypto-ops.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/crypto.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/crypto.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/generic-ops.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/groestl.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/groestl.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/groestl_tables.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash-extra-blake.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash-extra-groestl.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash-extra-jh.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash-extra-skein.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash-ops.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/hash.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/initializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/jh.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/jh.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/keccak.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/keccak.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/oaes_config.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/oaes_lib.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/oaes_lib.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/random.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/random.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/skein.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/skein.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/skein_port.h"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/slow-hash.c"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/slow-hash.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/slow-hash.inl"
    "${CMAKE_CURRENT_LIST_DIR}/crypto/tree-hash.c"
)

add_library(SocietatisFramework_Crypto STATIC ${SocietatisFramework_Crypto_SOURCES})
add_library(SocietatisFramework::Crypto ALIAS SocietatisFramework_Crypto)
target_include_directories(SocietatisFramework_Crypto PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

if(NOT WIN32 AND NOT ARM AND NOT PPC64LE)
    message(STATUS "[SocietatisFramework::Crypto] AES support enabled.")
    target_compile_options(SocietatisFramework_Crypto PRIVATE -maes)
elseif(PPC64LE)
    message(STATUS "[SocietatisFramework::Crypto] AES support disabled.")
endif()

# SocietatisFramework::CryptoNoteCore

set(SocietatisFramework_CryptoNoteCore_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Account.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Account.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockIndex.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockIndex.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Blockchain.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockchainIndices.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockchainIndices.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockchainMessages.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/BlockchainMessages.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Checkpoints.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Checkpoints.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Core.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Core.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CoreConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CoreConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasic.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasic.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasicImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteBasicImpl.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteFormatUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteFormatUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteSerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteStatInfo.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteTools.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/CryptoNoteTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Currency.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Currency.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Difficulty.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Difficulty.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IBlock.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IBlockchainStorageObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICore.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ICoreObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IMinerHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ITimeProvider.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ITransaction.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ITransactionValidator.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/ITxPoolObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/IntrusiveLinkedList.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/MessageQueue.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Miner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Miner.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/MinerConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/MinerConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/OnceInInterval.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/SwappedMap.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/SwappedVector.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/Transaction.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionApi.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionApiExtra.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionExtra.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionExtra.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionPool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionPool.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionPrefixImpl.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/TransactionUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/UpgradeDetector.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteCore/VerificationContext.h"
)

set(SocietatisFramework_CryptoNoteCore_LIBS
    Boost::filesystem
    Boost::program_options
    sparsehash::sparsehash
    SocietatisFramework::BlockchainExplorer
    SocietatisFramework::Common
    SocietatisFramework::Crypto
    SocietatisFramework::Global
    SocietatisFramework::Logging
    SocietatisFramework::Serialization
)

add_library(SocietatisFramework_CryptoNoteCore STATIC ${SocietatisFramework_CryptoNoteCore_SOURCES})
add_library(SocietatisFramework::CryptoNoteCore ALIAS SocietatisFramework_CryptoNoteCore)
target_include_directories(SocietatisFramework_CryptoNoteCore PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_CryptoNoteCore PRIVATE ${SocietatisFramework_CryptoNoteCore_LIBS})

# SocietatisFramework::CryptoNoteProtocol

set(SocietatisFramework_CryptoNoteProtocol_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandler.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/CryptoNoteProtocolHandlerCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/ICryptoNoteProtocolObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoNoteProtocol/ICryptoNoteProtocolQuery.h"
)

set(SocietatisFramework_CryptoNoteProtocol_LIBS
    SocietatisFramework::Global
)

add_library(SocietatisFramework_CryptoNoteProtocol STATIC ${SocietatisFramework_CryptoNoteProtocol_SOURCES})
add_library(SocietatisFramework::CryptoNoteProtocol ALIAS SocietatisFramework_CryptoNoteProtocol)
target_include_directories(SocietatisFramework_CryptoNoteProtocol PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_CryptoNoteProtocol PRIVATE ${SocietatisFramework_CryptoNoteProtocol_LIBS})

# SocietatisFramework::Global

set(SocietatisFramework_Global_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Global/Checkpoints.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/Constants.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Global/Constants.h"
    "${CMAKE_CURRENT_LIST_DIR}/Global/CryptoNoteConfig.h"
)

add_library(SocietatisFramework_Global STATIC ${SocietatisFramework_Global_SOURCES})
add_library(SocietatisFramework::Global ALIAS SocietatisFramework_Global)
target_include_directories(SocietatisFramework_Global PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

# SocietatisFramework::Http

set(SocietatisFramework_Http_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParser.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParser.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParserErrorCodes.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpParserErrorCodes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpRequest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpRequest.h"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpResponse.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Http/HttpResponse.h"
)

add_library(SocietatisFramework_Http STATIC ${SocietatisFramework_Http_SOURCES})
add_library(SocietatisFramework::Http ALIAS SocietatisFramework_Http)
target_include_directories(SocietatisFramework_Http PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

# SocietatisFramework::InProcessNode

set(SocietatisFramework_InProcessNode_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/InProcessNode/InProcessNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InProcessNode/InProcessNode.h"
    "${CMAKE_CURRENT_LIST_DIR}/InProcessNode/InProcessNodeErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/InProcessNode/InProcessNodeErrors.h"
)

set(SocietatisFramework_InProcessNode_LIBS
    SocietatisFramework::Global
)

add_library(SocietatisFramework_InProcessNode STATIC ${SocietatisFramework_InProcessNode_SOURCES})
add_library(SocietatisFramework::InProcessNode ALIAS SocietatisFramework_InProcessNode)
target_include_directories(SocietatisFramework_InProcessNode PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_InProcessNode PRIVATE ${SocietatisFramework_InProcessNode_LIBS})

# SocietatisFramework::JsonRpcServer

set(SocietatisFramework_JsonRpcServer_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/JsonRpcServer/JsonRpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/JsonRpcServer/JsonRpcServer.h"
)

add_library(SocietatisFramework_JsonRpcServer STATIC ${SocietatisFramework_JsonRpcServer_SOURCES})
add_library(SocietatisFramework::JsonRpcServer ALIAS SocietatisFramework_JsonRpcServer)
target_include_directories(SocietatisFramework_JsonRpcServer PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

# SocietatisFramework::Logging

set(SocietatisFramework_Logging_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Logging/CommonLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/CommonLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ConsoleLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ConsoleLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/FileLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/FileLogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ILogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/ILogger.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerGroup.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerMessage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerMessage.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerRef.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/LoggerRef.h"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/StreamLogger.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Logging/StreamLogger.h"
)

add_library(SocietatisFramework_Logging STATIC ${SocietatisFramework_Logging_SOURCES})
add_library(SocietatisFramework::Logging ALIAS SocietatisFramework_Logging)
target_include_directories(SocietatisFramework_Logging PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

# SocietatisFramework::Mnemonics

set(SocietatisFramework_Mnemonics_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/chinese_simplified.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/dutch.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/electrum-words.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/electrum-words.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/english.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/french.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/german.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/italian.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/japanese.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/language_base.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/polish.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/portuguese.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/russian.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/singleton.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/spanish.h"
    "${CMAKE_CURRENT_LIST_DIR}/Mnemonics/ukrainian.h"
)

add_library(SocietatisFramework_Mnemonics STATIC ${SocietatisFramework_Mnemonics_SOURCES})
add_library(SocietatisFramework::Mnemonics ALIAS SocietatisFramework_Mnemonics)
target_include_directories(SocietatisFramework_Mnemonics PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

# SocietatisFramework::NodeRpcProxy

set(SocietatisFramework_NodeRpcProxy_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeRpcProxy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxy/NodeRpcProxy.h"
)

set(SocietatisFramework_NodeRpcProxy_LIBS
    SocietatisFramework::Global
    SocietatisFramework::Http
    SocietatisFramework::Rpc
    SocietatisFramework::System
)

add_library(SocietatisFramework_NodeRpcProxy STATIC ${SocietatisFramework_NodeRpcProxy_SOURCES})
add_library(SocietatisFramework::NodeRpcProxy ALIAS SocietatisFramework_NodeRpcProxy)
target_include_directories(SocietatisFramework_NodeRpcProxy PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_NodeRpcProxy PRIVATE ${SocietatisFramework_NodeRpcProxy_LIBS})

# SocietatisFramework::P2p

set(SocietatisFramework_P2p_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/P2p/ConnectionContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/IP2pNodeInternal.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/LevinProtocol.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/LevinProtocol.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNode.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/NetNodeConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pConnectionProxy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pConnectionProxy.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContext.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContextOwner.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pContextOwner.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pInterfaces.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNetworks.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNode.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNodeConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pNodeConfig.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pProtocolDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/P2pProtocolTypes.h"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/PeerListManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/P2p/PeerListManager.h"
)

set(SocietatisFramework_P2p_LIBS
    MiniUPnP::miniupnpc
    SocietatisFramework::CryptoNoteProtocol
    SocietatisFramework::Global
)

if(WIN32 AND MSVC)
    list(APPEND SocietatisFramework_P2p_LIBS Bcrypt)
endif()

add_library(SocietatisFramework_P2p STATIC ${SocietatisFramework_P2p_SOURCES})
add_library(SocietatisFramework::P2p ALIAS SocietatisFramework_P2p)
target_include_directories(SocietatisFramework_P2p PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_P2p PRIVATE ${SocietatisFramework_P2p_LIBS})

# SocietatisFramework::PaymentGate

set(SocietatisFramework_PaymentGate_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/NodeFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/NodeFactory.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/PaymentServiceJsonRpcMessages.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/PaymentServiceJsonRpcMessages.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/PaymentServiceJsonRpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/PaymentServiceJsonRpcServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/WalletService.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/WalletService.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/WalletServiceErrorCategory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGate/WalletServiceErrorCategory.h"
)

set(SocietatisFramework_PaymentGate_LIBS
    SocietatisFramework::JsonRpcServer
    SocietatisFramework::NodeRpcProxy
    SocietatisFramework::System
    SocietatisFramework::Wallet
)

add_library(SocietatisFramework_PaymentGate STATIC ${SocietatisFramework_PaymentGate_SOURCES})
add_library(SocietatisFramework::PaymentGate ALIAS SocietatisFramework_PaymentGate)
target_include_directories(SocietatisFramework_PaymentGate PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_PaymentGate PRIVATE ${SocietatisFramework_PaymentGate_LIBS})

# SocietatisFramework::Platform # TODO: Merge this target into SocietatisFramework::System

# SocietatisFramework::Rpc

set(SocietatisFramework_Rpc_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/CoreRpcServerCommandsDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/CoreRpcServerErrorCodes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpClient.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpClient.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/HttpServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/JsonRpc.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/JsonRpc.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServerConfig.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Rpc/RpcServerConfig.h"
)

set(SocietatisFramework_Rpc_LIBS
    SocietatisFramework::BlockchainExplorer
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Global
    SocietatisFramework::P2p
    SocietatisFramework::PaymentGate
    SocietatisFramework::Serialization
    SocietatisFramework::System
    SocietatisFramework::Http
)

add_library(SocietatisFramework_Rpc STATIC ${SocietatisFramework_Rpc_SOURCES})
add_library(SocietatisFramework::Rpc ALIAS SocietatisFramework_Rpc)
target_include_directories(SocietatisFramework_Rpc PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Rpc PRIVATE ${SocietatisFramework_Rpc_LIBS})

# SocietatisFramework::Serialization

set(SocietatisFramework_Serialization_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinaryOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BinarySerializationTools.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BlockchainExplorerDataSerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/BlockchainExplorerDataSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/ISerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/IStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputValueSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonInputValueSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/JsonOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryCommon.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryInputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryInputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryOutputStreamSerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/KVBinaryOutputStreamSerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/MemoryStream.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationOverloads.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationOverloads.h"
    "${CMAKE_CURRENT_LIST_DIR}/Serialization/SerializationTools.h"
)

add_library(SocietatisFramework_Serialization STATIC ${SocietatisFramework_Serialization_SOURCES})
add_library(SocietatisFramework::Serialization ALIAS SocietatisFramework_Serialization)
target_include_directories(SocietatisFramework_Serialization PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Serialization PRIVATE SocietatisFramework::Common)

# SocietatisFramework::System

set(SocietatisFramework_System_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/System/Context.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroup.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroup.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroupTimeout.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/ContextGroupTimeout.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/Event.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/Event.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/EventLock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/EventLock.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/InterruptedException.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/Ipv4Address.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/Ipv4Address.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/OperationTimeout.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteEventLock.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/RemoteEventLock.h"
    "${CMAKE_CURRENT_LIST_DIR}/System/TcpStream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/System/TcpStream.h"
)

if(MSVC)
    list(APPEND SocietatisFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/MemoryMappedFile.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Windows/System/Timer.h"
    )
elseif(APPLE)
    list(APPEND SocietatisFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Context.c"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/OSX/System/asm.s"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
elseif(ANDROID)
    list(APPEND SocietatisFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Android/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
elseif(FREEBSD)
    list(APPEND SocietatisFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Context.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/FreeBSD/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
else()
    list(APPEND SocietatisFramework_System_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Dispatcher.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Dispatcher.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/ErrorMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/ErrorMessage.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Future.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Ipv4Resolver.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Ipv4Resolver.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnection.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnection.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnector.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpConnector.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpListener.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/TcpListener.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Timer.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Linux/System/Timer.h"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/Platform/Posix/System/MemoryMappedFile.h"
    )
endif()

add_library(SocietatisFramework_System STATIC ${SocietatisFramework_System_SOURCES})
add_library(SocietatisFramework::System ALIAS SocietatisFramework_System)
target_include_directories(SocietatisFramework_System PRIVATE ${SocietatisFramework_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(SocietatisFramework_System PRIVATE ws2_32)
endif()

# SocietatisFramework::Transfers

set(SocietatisFramework_Transfers_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/BlockchainSynchronizer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/BlockchainSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/CommonTypes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/IBlockchainSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/IObservableImpl.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/SynchronizationState.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/SynchronizationState.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersConsumer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersConsumer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersContainer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSubscription.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSubscription.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSynchronizer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TransfersSynchronizer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Transfers/TypeHelpers.h"
)

set(SocietatisFramework_Transfers_LIBS
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Global
)

add_library(SocietatisFramework_Transfers STATIC ${SocietatisFramework_Transfers_SOURCES})
add_library(SocietatisFramework::Transfers ALIAS SocietatisFramework_Transfers)
target_include_directories(SocietatisFramework_Transfers PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Transfers PRIVATE ${SocietatisFramework_Transfers_LIBS})

# SocietatisFramework::Wallet

set(SocietatisFramework_Wallet_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/IFusionManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/IWallet.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/LegacyKeysImporter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/LegacyKeysImporter.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletAsyncContextCounter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletAsyncContextCounter.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletErrors.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletErrors.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletGreen.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletGreen.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletIndices.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletRpcServer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletRpcServer.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletRpcServerCommandsDefinitions.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletRpcServerErrorCodes.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV1.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV1.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV2.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletSerializationV2.h"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Wallet/WalletUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/IWalletLegacy.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/KeysStorage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/KeysStorage.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletHelper.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletHelper.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacy.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacyEvent.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacySerialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacySerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacySerializer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletLegacySerializer.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletRequest.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletSendTransactionContext.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletTransactionSender.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletTransactionSender.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletUnconfirmedTransactions.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletUnconfirmedTransactions.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletUserTransactionsCache.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletUserTransactionsCache.h"
    "${CMAKE_CURRENT_LIST_DIR}/WalletLegacy/WalletUtils.h"
)

set(SocietatisFramework_Wallet_LIBS
    Boost::filesystem
    SocietatisFramework::Common
    SocietatisFramework::Crypto
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Global
    SocietatisFramework::Logging
    SocietatisFramework::Mnemonics
    SocietatisFramework::Rpc
    SocietatisFramework::Transfers
)

add_library(SocietatisFramework_Wallet STATIC ${SocietatisFramework_Wallet_SOURCES})
add_library(SocietatisFramework::Wallet ALIAS SocietatisFramework_Wallet)
target_include_directories(SocietatisFramework_Wallet PRIVATE ${SocietatisFramework_INCLUDE_DIRS})
target_link_libraries(SocietatisFramework_Wallet PRIVATE ${SocietatisFramework_Wallet_LIBS})

# SocietatisFramework::WalletLegacy # TODO: Merge this target into SocietatisFramework::Wallet

if(BUILD_WITH_COVERAGE AND (${CMAKE_BUILD_TYPE} MATCHES Debug))
    message(STATUS "Included code-coverage settings.")
    message(STATUS "NOTE: Code-coverage is enabled by default when CMAKE_BUILD_TYPE=Debug.")

    # Enable code-coverage settings for each library
    foreach(lib ${SocietatisFramework_LIBS})
        target_link_libraries(${lib} PUBLIC codecov)
    endforeach()
endif()

install(TARGETS
            SocietatisFramework_BlockchainExplorer
            SocietatisFramework_Common
            SocietatisFramework_Crypto
            SocietatisFramework_CryptoNoteCore
            SocietatisFramework_CryptoNoteProtocol
            SocietatisFramework_Http
            SocietatisFramework_InProcessNode
            SocietatisFramework_JsonRpcServer
            SocietatisFramework_Logging
            SocietatisFramework_Mnemonics
            SocietatisFramework_NodeRpcProxy
            SocietatisFramework_P2p
            SocietatisFramework_PaymentGate
            SocietatisFramework_Rpc
            SocietatisFramework_Serialization
            SocietatisFramework_System
            SocietatisFramework_Transfers
            SocietatisFramework_Wallet
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
