cmake_minimum_required(VERSION 2.8)
project(leveldb)

include(CheckIncludeFile)
check_include_file(atomic HAVE_ATOMIC)

if(HAVE_ATOMIC)
  add_definitions(-DLEVELDB_ATOMIC_PRESENT)
endif()

if(SNAPPY)
  add_definitions(-DSNAPPY)
  set(SNAPPY_LIBRARIES snappy)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(MEMCMP_FLAG "-fno-builtin-memcmp")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x")
endif()

if(APPLE)
  add_definitions(${MEMCMP_FLAG} -DOS_MACOSX -DLEVELDB_PLATFORM_POSIX)
  set(PORT_SRC port/port_posix.cc port/port_posix_sse.cc util/env_posix.cc)
elseif(UNIX)
  add_definitions(${MEMCMP_FLAG} -pthread -DOS_LINUX -DLEVELDB_PLATFORM_POSIX)
  set(PORT_SRC port/port_posix.cc port/port_posix_sse.cc util/env_posix.cc)
  set(PLATFORM_LIBRARIES pthread)
elseif(WIN32)
  add_definitions(${MEMCMP_FLAG} -DOS_WINDOWS -DLEVELDB_PLATFORM_WINDOWS)
  if(MINGW)
    add_definitions(-D_WIN32_WINNT=0x0600)
  endif()
  set(PORT_SRC port/port_win.cc util/env_win.cc)
  set(PLATFORM_LIBRARIES shlwapi)
else()
  message(FATAL_ERROR "Unknown platform")
endif()

set(SRC 
  db/builder.cc
  db/c.cc
  db/db_impl.cc
  db/db_iter.cc
  db/dbformat.cc
  db/dumpfile.cc
  db/filename.cc
  db/log_reader.cc
  db/log_writer.cc
  db/memtable.cc
  db/repair.cc
  db/table_cache.cc
  db/version_edit.cc
  db/version_set.cc
  db/write_batch.cc
  table/block.cc
  table/block_builder.cc
  table/filter_block.cc
  table/format.cc
  table/iterator.cc
  table/merger.cc
  table/table.cc
  table/table_builder.cc
  table/two_level_iterator.cc
  util/arena.cc util/bloom.cc
  util/cache.cc util/coding.cc
  util/comparator.cc
  util/crc32c.cc
  util/env.cc
  util/filter_policy.cc
  util/hash.cc
  util/histogram.cc
  util/logging.cc
  util/options.cc
  util/status.cc)

set(MEMENV_SRC helpers/memenv/memenv.cc)

include_directories(. include)

add_library(leveldb SHARED ${SRC} ${PORT_SRC})
target_link_libraries(leveldb ${PLATFORM_LIBRARIES} ${SNAPPY_LIBRARIES})

add_library(testutil util/testutil.cc)

add_library(testharness util/testharness.cc)
target_link_libraries(testharness testutil leveldb)

set(TESTS
  db/autocompact_test
  util/arena_test 
  util/bloom_test 
  db/c_test
  util/cache_test 
  util/coding_test 
  db/corruption_test 
  util/crc32c_test 
  db/db_test 
  db/dbformat_test 
  util/env_test
  util/hash_test
  issues/issue178_test
  issues/issue200_test
  db/filename_test 
  table/filter_block_test 
  db/log_test 
  helpers/memenv/memenv_test 
  db/skiplist_test 
  table/table_test 
  db/version_edit_test 
  db/version_set_test 
  db/write_batch_test)

enable_testing()
foreach(test ${TESTS})
  get_filename_component(n ${test} NAME)
  list(APPEND TEST_TARGETS ${n})
  if (n STREQUAL memenv_test)
    add_executable(${n} ${test} ${MEMENV_SRC})
    target_link_libraries(${n} testharness)
  else()
    add_executable(${n} ${test})
    target_link_libraries(${n} testharness)
  endif()
  add_test(${n} ${n})
endforeach()

install(TARGETS leveldb
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY include/leveldb/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/leveldb FILES_MATCHING PATTERN "*.h")

if(INSTALL_TESTS)
  install(TARGETS ${TEST_TARGETS} CONFIGURATIONS Release DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/)
endif()
