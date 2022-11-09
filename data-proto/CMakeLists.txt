find_package(Protobuf REQUIRED)

add_custom_target(proto ALL
    COMMAND protoc -I=${PROJECT_SOURCE_DIR}/data-proto --cpp_out=${PROJECT_SOURCE_DIR}/data-proto/build ${PROJECT_SOURCE_DIR}/data-proto/*.proto
)

file(GLOB PROTO_SRCS "${PROJECT_SOURCE_DIR}/data-proto/build/*.cc")

add_library(proto-lib SHARED ${PROTO_SRCS})
target_link_libraries(proto-lib PRIVATE ${Protobuf_LIBRARIES})
target_include_directories(proto-lib
    PUBLIC ${PROJECT_SOURCE_DIR}/data-proto/build
    PUBLIC ${Protobuf_INCLUDE_DIRS}
)