cc_library(
    name = "tacopie",
    srcs = [
        "sources/network/common/tcp_socket.cpp",
        "sources/network/io_service.cpp",
        "sources/network/tcp_client.cpp",
        "sources/network/tcp_server.cpp",
        "sources/network/unix/unix_self_pipe.cpp",
        "sources/network/unix/unix_tcp_socket.cpp",
        "sources/network/windows/windows_self_pipe.cpp",
        "sources/network/windows/windows_tcp_socket.cpp",
        "sources/utils/error.cpp",
        "sources/utils/logger.cpp",
        "sources/utils/thread_pool.cpp",
    ],
    hdrs = [
        "includes/tacopie/network/io_service.hpp",
        "includes/tacopie/network/self_pipe.hpp",
        "includes/tacopie/network/tcp_client.hpp",
        "includes/tacopie/network/tcp_server.hpp",
        "includes/tacopie/network/tcp_socket.hpp",
        "includes/tacopie/tacopie",
        "includes/tacopie/utils/error.hpp",
        "includes/tacopie/utils/logger.hpp",
        "includes/tacopie/utils/thread_pool.hpp",
        "includes/tacopie/utils/typedefs.hpp",
    ],
    strip_include_prefix = "includes",
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "example_logger",
    srcs = ["examples/logger.cpp"],
    # This example is pointless if logging is disabled, so enable it.
    copts = ["-D__TACOPIE_LOGGING_ENABLED=1"],
    deps = ["tacopie"],
)

cc_binary(
    name = "example_tcp_client",
    srcs = ["examples/tcp_client.cpp"],
    # TODO (steple): For windows, link ws2_32 instead.
    linkopts = ["-lpthread"],
    deps = ["tacopie"],
)

cc_binary(
    name = "example_tcp_server",
    srcs = ["examples/tcp_server.cpp"],
    # TODO (steple): For windows, link ws2_32 instead.
    linkopts = ["-lpthread"],
    deps = ["tacopie"],
)

# Note: Basic infrastructure for gtest-based tests exists, but no tests are
# actually implemented (this will always pass).
cc_test(
    name = "test",
    srcs = ["tests/sources/main.cpp"],
    deps = ["@gtest"],
)
