set(SocietatisTests_INCLUDE_DIRS
    ${SocietatisFramework_INCLUDE_DIRS}
    "${CMAKE_CURRENT_LIST_DIR}"
)

# SocietatisTests

add_custom_target(SocietatisTests)
add_dependencies(SocietatisTests
    SocietatisTests::CoreTests
    SocietatisTests::CryptoTests
    SocietatisTests::DifficultyTests
    SocietatisTests::HashTargetTests
    SocietatisTests::HashTests
    SocietatisTests::NodeRpcProxyTests
    SocietatisTests::PerformanceTests
    SocietatisTests::SystemTests
    SocietatisTests::UnitTests
)

# SocietatisTests::CoreTests

set(SocietatisTests_CoreTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/AccountBoostSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/BlockReward.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/BlockReward.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/BlockValidation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/BlockValidation.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/BoostSerializationHelper.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/ChainSplit1.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/ChainSplit1.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/ChainSwitch1.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/ChainSwitch1.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Chaingen.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Chaingen.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Chaingen001.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Chaingen001.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/ChaingenMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/CryptoNoteBoostSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/DoubleSpend.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/DoubleSpend.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/IntegerOverflow.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/IntegerOverflow.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/RandomOuts.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/RandomOuts.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/RingSignature.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/RingSignature.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TestGenerator.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionBuilder.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionBuilder.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionTests.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionValidation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/TransactionValidation.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/UnorderedContainersBoostSerialization.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Upgrade.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/Upgrade.h"
    "${CMAKE_CURRENT_LIST_DIR}/CoreTests/double_spend.inl"
)

set(SocietatisTests_CoreTests_LIBS
    Boost::date_time
    Boost::program_options
    Boost::serialization
    codecov
    SocietatisFramework::Common
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::InProcessNode
    SocietatisFramework::Logging
    SocietatisFramework::Serialization
    SocietatisFramework::System
    SocietatisTests::TestGenerator
)

add_executable(SocietatisTests_CoreTests ${SocietatisTests_CoreTests_SOURCES})
add_executable(SocietatisTests::CoreTests ALIAS SocietatisTests_CoreTests)
target_include_directories(SocietatisTests_CoreTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_CoreTests PRIVATE ${SocietatisTests_CoreTests_LIBS})
set_target_properties(SocietatisTests_CoreTests PROPERTIES OUTPUT_NAME "core_tests")

add_test(
    NAME CoreTests
    COMMAND $<TARGET_FILE:SocietatisTests_CoreTests> --generate_and_play_test_data
)

# SocietatisTests::CryptoTests

set(SocietatisTests_CryptoTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/crypto-ops-data.c"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/crypto-ops.c"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/crypto-tests.h"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/crypto.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/hash.c"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/main.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/CryptoTests/random.c"
)

set(SocietatisTests_CryptoTests_LIBS codecov)

add_executable(SocietatisTests_CryptoTests ${SocietatisTests_CryptoTests_SOURCES})
add_executable(SocietatisTests::CryptoTests ALIAS SocietatisTests_CryptoTests)
target_include_directories(SocietatisTests_CryptoTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_CryptoTests PRIVATE ${SocietatisTests_CryptoTests_LIBS})
set_target_properties(SocietatisTests_CryptoTests PROPERTIES OUTPUT_NAME "crypto_tests")

add_test(
    NAME CryptoTests
    COMMAND $<TARGET_FILE:SocietatisTests_CryptoTests> ${CMAKE_CURRENT_LIST_DIR}/CryptoTests/tests.txt
)

# SocietatisTests::Data (skipped, not a library or executable target)

# SocietatisTests::DifficultyTests

set(SocietatisTests_DifficultyTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/DifficultyTests/Difficulty.cpp"
)

set(SocietatisTests_DifficultyTests_LIBS
    codecov
    SocietatisFramework::Logging
    SocietatisFramework::CryptoNoteCore
)

add_executable(SocietatisTests_DifficultyTests ${SocietatisTests_DifficultyTests_SOURCES})
add_executable(SocietatisTests::DifficultyTests ALIAS SocietatisTests_DifficultyTests)
target_include_directories(SocietatisTests_DifficultyTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_DifficultyTests PRIVATE ${SocietatisTests_DifficultyTests_LIBS})
set_target_properties(SocietatisTests_DifficultyTests PROPERTIES OUTPUT_NAME "difficulty_tests")

add_test(
    NAME DifficultyTests
    COMMAND $<TARGET_FILE:SocietatisTests_DifficultyTests> ${CMAKE_CURRENT_LIST_DIR}/DifficultyTests/data.txt
)

# SocietatisTests::HashTargetTests

set(SocietatisTests_HashTargetTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/HashTargetTests/HashTarget.cpp"
)

set(SocietatisTests_HashTargetTests_LIBS
    codecov
    SocietatisFramework::CryptoNoteCore
)

add_executable(SocietatisTests_HashTargetTests ${SocietatisTests_HashTargetTests_SOURCES})
add_executable(SocietatisTests::HashTargetTests ALIAS SocietatisTests_HashTargetTests)
target_include_directories(SocietatisTests_HashTargetTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_HashTargetTests PRIVATE ${SocietatisTests_HashTargetTests_LIBS})
set_target_properties(SocietatisTests_HashTargetTests PROPERTIES OUTPUT_NAME "hash_target_tests")

add_test(
    NAME HashTargetTests
    COMMAND $<TARGET_FILE:SocietatisTests_HashTargetTests>
)

# SocietatisTests::HashTests

set(SocietatisTests_HashTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/HashTests/main.cpp"
)

set(SocietatisTests_HashTests_LIBS
    codecov
    SocietatisFramework::Crypto
)

add_executable(SocietatisTests_HashTests ${SocietatisTests_HashTests_SOURCES})
add_executable(SocietatisTests::HashTests ALIAS SocietatisTests_HashTests)
target_include_directories(SocietatisTests_HashTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_HashTests PRIVATE ${SocietatisTests_HashTests_LIBS})
set_target_properties(SocietatisTests_HashTests PROPERTIES OUTPUT_NAME "hash_tests")

foreach(hash IN ITEMS extra-blake extra-groestl extra-jh extra-skein fast slow tree)
    add_test(
        NAME HashTests-${hash}
        COMMAND $<TARGET_FILE:SocietatisTests_HashTests> ${hash} ${CMAKE_CURRENT_LIST_DIR}/HashTests/tests-${hash}.txt
    )
endforeach()

# SocietatisTests::NodeRpcProxyTests

set(SocietatisTests_NodeRpcProxyTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/NodeRpcProxyTests/NodeRpcProxyTests.cpp"
)

set(SocietatisTests_NodeRpcProxyTests_LIBS
    codecov
    SocietatisFramework::Common
    SocietatisFramework::Logging
    SocietatisFramework::NodeRpcProxy
    SocietatisFramework::Rpc
    SocietatisFramework::Serialization
)

add_executable(SocietatisTests_NodeRpcProxyTests ${SocietatisTests_NodeRpcProxyTests_SOURCES})
add_executable(SocietatisTests::NodeRpcProxyTests ALIAS SocietatisTests_NodeRpcProxyTests)
target_include_directories(SocietatisTests_NodeRpcProxyTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_NodeRpcProxyTests PRIVATE ${SocietatisTests_NodeRpcProxyTests_LIBS})
set_target_properties(SocietatisTests_NodeRpcProxyTests PROPERTIES OUTPUT_NAME "node_rpc_proxy_tests")

add_test(
    NAME NodeRpcProxyTests
    COMMAND $<TARGET_FILE:SocietatisTests_NodeRpcProxyTests>
)

# SocietatisTests::PerformanceTests

set(SocietatisTests_PerformanceTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/CheckRingSignature.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/ConstructTransaction.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/CryptoNoteSlowHash.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/DerivePublicKey.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/DeriveSecretKey.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/GenerateKeyDerivation.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/GenerateKeyImage.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/GenerateKeyImageHelper.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/IsOutToAccount.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/MultiTransactionTestBase.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/PerformanceTests.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/PerformanceUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/SingleTransactionTestBase.h"
    "${CMAKE_CURRENT_LIST_DIR}/PerformanceTests/main.cpp"
)

set(SocietatisTests_PerformanceTests_LIBS
    Boost::chrono
    codecov
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Logging
)

add_executable(SocietatisTests_PerformanceTests ${SocietatisTests_PerformanceTests_SOURCES})
add_executable(SocietatisTests::PerformanceTests ALIAS SocietatisTests_PerformanceTests)
target_include_directories(SocietatisTests_PerformanceTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_PerformanceTests PRIVATE ${SocietatisTests_PerformanceTests_LIBS})
set_target_properties(SocietatisTests_PerformanceTests PROPERTIES OUTPUT_NAME "performance_tests")

add_test(
    NAME PerformanceTests
    COMMAND $<TARGET_FILE:SocietatisTests_PerformanceTests>
)

# SocietatisTests::SystemTests

set(SocietatisTests_SystemTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/ContextGroupTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/ContextGroupTimeoutTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/ContextTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/DispatcherTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/ErrorMessageTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/EventLockTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/EventTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/Ipv4AddressTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/Ipv4ResolverTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/OperationTimeoutTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/RemoteContextTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/TcpConnectionTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/TcpConnectorTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/TcpListenerTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/TimerTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SystemTests/main.cpp"
)

set(SocietatisTests_SystemTests_LIBS
    codecov
    GTest::gtest_main
    SocietatisFramework::System
)

add_executable(SocietatisTests_SystemTests ${SocietatisTests_SystemTests_SOURCES})
add_executable(SocietatisTests::SystemTests ALIAS SocietatisTests_SystemTests)
target_include_directories(SocietatisTests_SystemTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_SystemTests PRIVATE ${SocietatisTests_SystemTests_LIBS})
set_target_properties(SocietatisTests_SystemTests PROPERTIES OUTPUT_NAME "system_tests")

add_test(
    NAME SystemTests
    COMMAND $<TARGET_FILE:SocietatisTests_SystemTests>
)

# SocietatisTests::TestGenerator

set(SocietatisTests_TestGenerator_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/TestGenerator/TestGenerator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/TestGenerator/TestGenerator.h"
)

set(SocietatisTests_TestGenerator_LIBS codecov)

add_library(SocietatisTests_TestGenerator ${SocietatisTests_TestGenerator_SOURCES})
add_library(SocietatisTests::TestGenerator ALIAS SocietatisTests_TestGenerator)
target_include_directories(SocietatisTests_TestGenerator PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_TestGenerator PRIVATE ${SocietatisTests_TestGenerator_LIBS})

# SocietatisTests::UnitTests

set(SocietatisTests_UnitTests_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ArrayRefTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ArrayViewTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Base58.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/BinarySerializationCompatibility.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/BlockReward.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/BlockingQueue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Chacha8.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Checkpoints.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/DecomposeAmountIntoDigits.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/EventWaiter.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/EventWaiter.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ICoreStub.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ICoreStub.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ICryptoNoteProtocolQueryStub.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ICryptoNoteProtocolQueryStub.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/INodeStubs.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/INodeStubs.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/MulDiv.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/ParseAmount.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/PaymentGateTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Serialization.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/SerializationKV.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/Shuffle.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/StringBufferTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/StringViewTests.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestBcS.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestBlockchainExplorer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestBlockchainGenerator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestBlockchainGenerator.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestCurrency.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestFileMappedVector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestFormatUtils.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestInprocessNode.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestJsonValue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestMessageQueue.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestPath.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestPeerlist.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestProtocolPack.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransactionPoolDetach.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransfers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransfersConsumer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransfersContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransfersContainerKeyImage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestTransfersSubscription.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestUpgradeDetector.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestWallet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestWalletLegacy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TestWalletService.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TransactionApi.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TransactionApiHelpers.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TransactionApiHelpers.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TransactionPool.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/TransfersObserver.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/UnitTestsUtils.h"
    "${CMAKE_CURRENT_LIST_DIR}/UnitTests/main.cpp"
)

set(SocietatisTests_UnitTests_LIBS
    codecov
    GTest::gtest_main
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::CryptoNoteProtocol
    SocietatisFramework::Http
    SocietatisFramework::InProcessNode
    SocietatisFramework::Logging
    SocietatisFramework::Rpc
    SocietatisFramework::Wallet
    SocietatisTests::TestGenerator
)

add_executable(SocietatisTests_UnitTests ${SocietatisTests_UnitTests_SOURCES})
add_executable(SocietatisTests::UnitTests ALIAS SocietatisTests_UnitTests)
target_include_directories(SocietatisTests_UnitTests PRIVATE ${SocietatisTests_INCLUDE_DIRS})
target_link_libraries(SocietatisTests_UnitTests PRIVATE ${SocietatisTests_UnitTests_LIBS})
set_target_properties(SocietatisTests_UnitTests PROPERTIES OUTPUT_NAME "unit_tests")

add_test(
    NAME UnitTests
    COMMAND $<TARGET_FILE:SocietatisTests_UnitTests>
)
