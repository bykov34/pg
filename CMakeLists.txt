cmake_minimum_required(VERSION 3.5)
project(pg VERSION 0.1.0)

include_directories(
    include
    libpq/include
)

add_library(${PROJECT_NAME}
    source/channel.cpp
    source/client.cpp
    source/command.cpp
    source/config.cpp
    source/connection.cpp
    source/consumer.cpp
    source/context.cpp
    source/dispatcher.cpp
    source/error.cpp
    source/field.cpp
    source/ichannel.cpp
    source/job.cpp
    source/receiver.cpp
    source/result.cpp
    source/row.cpp
    source/statement.cpp
    source/status.cpp
    source/time.cpp
    source/transaction.cpp
    source/visitable.cpp
    source/visitors.cpp
    source/worker.cpp
)

target_compile_features(pg PUBLIC cxx_std_17)

add_subdirectory(libpq ${PROJECT_BINARY_DIR}/libpq)
target_link_libraries(${PROJECT_NAME} pq)
