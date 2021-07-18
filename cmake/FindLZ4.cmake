# - Find lz4
# Find the native lz4 headers and libraries
#
# LZ4_FOUND - True if lz4 was found, false otherwise
# LZ4_INCLUDE_DIR - where to find the header file lz4.h etc.
# LZ4_LIBRARIES - List of libraries when using lz4

find_path(LZ4_INCLUDE_DIR NAMES lz4.h)

find_library(LZ4_LIBRARIES NAMES lz4)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZ4 DEFAULT_MSG LZ4_LIBRARIES LZ4_INCLUDE_DIR)
