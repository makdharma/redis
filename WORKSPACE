bind(
    name = "nanopb",
    actual = "//third_party/nanopb",
)
bind(
    name = "cares",
    actual = "@submodule_cares//:ares",
)
bind(
    name = "libssl",
    actual = "@submodule_boringssl//:ssl",
)
bind(
    name = "zlib",
    actual = "@submodule_zlib//:z",
)
bind(
  name = "grpc_cpp_plugin",
  actual = "@io_grpc//:grpc_cpp_plugin",
)
bind(
  name = "grpc++",
  actual = "@io_grpc//:grpc++",
)
bind(
  name = "grpc++_codegen_proto",
  actual = "@io_grpc//:grpc++_codegen_proto",
)

bind(
    name = "protobuf",
    actual = "@submodule_protobuf//:protobuf",
)

bind(
    name = "protobuf_clib",
    actual = "@submodule_protobuf//:protoc_lib",
)

bind(
    name = "protocol_compiler",
    actual = "@submodule_protobuf//:protoc",
)

new_local_repository(
  name = "submodule_protobuf",
  build_file = "third_party/protobuf/BUILD",
  path = "third_party/protobuf",
)
git_repository(
  name = "io_grpc",
  remote = "https://github.com/grpc/grpc",
  commit = "c090c619c117912b8c9e88a3c4bc8f778a94d582",
)
new_local_repository(
    name = "submodule_zlib",
    build_file = "third_party/zlib.BUILD",
    path = "third_party/zlib",
)
new_local_repository(
    name = "submodule_boringssl",
    build_file = "third_party/boringssl-with-bazel/BUILD",
    path = "third_party/boringssl-with-bazel",
)
new_local_repository(
    name = "submodule_cares",
    path = "third_party/cares",
    build_file = "third_party/cares/cares.BUILD",
)
