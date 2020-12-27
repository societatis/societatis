set(SocietatisTools_INCLUDE_DIRS
    ${SocietatisFramework_INCLUDE_DIRS}
    "${CMAKE_CURRENT_LIST_DIR}"
)

set(SocietatisTools_INCLUDE_DIRS ${SocietatisTools_INCLUDE_DIRS} PARENT_SCOPE)

# SocietatisTools

add_custom_target(SocietatisTools)
add_dependencies(SocietatisTools
    SocietatisTools::Daemon
    SocietatisTools::PaymentGateService
    SocietatisTools::SimpleWallet
)

# SocietatisTools::BinaryInfo # NOTE: Ignore this. It's not a target.

# SocietatisTools::Daemon

set(SocietatisTools_Daemon_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/Daemon.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Daemon/DaemonCommandsHandler.h"
)

if(WIN32)
    list(APPEND SocietatisTools_Daemon_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/daemon.rc"
    )
endif()

set(SocietatisTools_Daemon_LIBS
    Boost::filesystem
    SocietatisFramework::Breakpad
    SocietatisFramework::Common
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Global
    SocietatisFramework::Logging
    SocietatisFramework::P2p
    SocietatisFramework::Rpc
    SocietatisFramework::Serialization
    SocietatisFramework::System
)

add_executable(SocietatisTools_Daemon ${SocietatisTools_Daemon_SOURCES})
add_executable(SocietatisTools::Daemon ALIAS SocietatisTools_Daemon)
target_include_directories(SocietatisTools_Daemon PRIVATE ${SocietatisTools_INCLUDE_DIRS})
target_link_libraries(SocietatisTools_Daemon PRIVATE ${SocietatisTools_Daemon_LIBS})
set_target_properties(SocietatisTools_Daemon PROPERTIES OUTPUT_NAME "societatisd")

# SocietatisTools::PaymentGateService

set(SocietatisTools_PaymentGateService_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/ConfigurationManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/ConfigurationManager.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/PaymentGateService.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/PaymentGateService.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/PaymentServiceConfiguration.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/PaymentServiceConfiguration.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/RpcNodeConfiguration.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/RpcNodeConfiguration.h"
    "${CMAKE_CURRENT_LIST_DIR}/PaymentGateService/main.cpp"
)

if(WIN32)
    list(APPEND SocietatisTools_PaymentGateService_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/walletd.rc"
    )
endif()

set(SocietatisTools_PaymentGateService_LIBS
    Boost::filesystem
    Boost::program_options
    SocietatisFramework::Breakpad
    SocietatisFramework::Common
    SocietatisFramework::CryptoNoteCore
    SocietatisFramework::Global
    SocietatisFramework::InProcessNode
    SocietatisFramework::Logging
    SocietatisFramework::Rpc
    SocietatisFramework::System
)

add_executable(SocietatisTools_PaymentGateService ${SocietatisTools_PaymentGateService_SOURCES})
add_executable(SocietatisTools::PaymentGateService ALIAS SocietatisTools_PaymentGateService)
target_include_directories(SocietatisTools_PaymentGateService PRIVATE ${SocietatisTools_INCLUDE_DIRS})
target_link_libraries(SocietatisTools_PaymentGateService ${SocietatisTools_PaymentGateService_LIBS})
set_target_properties(SocietatisTools_PaymentGateService PROPERTIES OUTPUT_NAME "walletd")

# SocietatisTools::SimpleWallet

set(SocietatisTools_SimpleWallet_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/PasswordContainer.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/PasswordContainer.h"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/SimpleWallet.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/SimpleWallet/SimpleWallet.h"
)

if(WIN32)
    list(APPEND SocietatisTools_SimpleWallet_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/BinaryInfo/simplewallet.rc"
    )
endif()

set(SocietatisTools_SimpleWallet_LIBS
    Boost::filesystem
    Boost::program_options
    SocietatisFramework::Breakpad
    SocietatisFramework::Common
    SocietatisFramework::Global
    SocietatisFramework::Logging
    SocietatisFramework::NodeRpcProxy
    SocietatisFramework::Rpc
    SocietatisFramework::Serialization
    SocietatisFramework::Wallet
)

if(WIN32)
    list(APPEND SocietatisTools_SimpleWallet_LIBS ws2_32)
endif()

add_executable(SocietatisTools_SimpleWallet ${SocietatisTools_SimpleWallet_SOURCES})
add_executable(SocietatisTools::SimpleWallet ALIAS SocietatisTools_SimpleWallet)
target_include_directories(SocietatisTools_SimpleWallet PRIVATE ${SocietatisTools_INCLUDE_DIRS})
target_link_libraries(SocietatisTools_SimpleWallet ${SocietatisTools_SimpleWallet_LIBS})
set_target_properties(SocietatisTools_SimpleWallet PROPERTIES OUTPUT_NAME "simplewallet")

# Installation

install(TARGETS
            SocietatisTools_Daemon
            SocietatisTools_PaymentGateService
            SocietatisTools_SimpleWallet
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static
)
