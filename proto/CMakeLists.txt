find_package(Protobuf REQUIRED)

file(GLOB PROTO_ORIGN "${PROJECT_SOURCE_DIR}/proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_ORIGN})

add_library(proto-lib SHARED ${PROTO_SRCS})

target_link_libraries(proto-lib PUBLIC ${Protobuf_LIBRARIES})
target_include_directories(proto-lib
    PUBLIC ${Protobuf_INCLUDE_DIRS}
)

target_include_directories(proto-lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})