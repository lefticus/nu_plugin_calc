cmake_minimum_required(VERSION 3.8)

project(nu_plugin_calc)

# Link this 'library' to use the following warnings
add_library(project_warnings INTERFACE)

if(CMAKE_COMPILER_IS_GNUCC)
  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

  if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0)
  endif()
endif()

if(MSVC)
  target_compile_options(project_warnings INTERFACE /W4)
else()
  target_compile_options(project_warnings
                         INTERFACE -Wall
                                   -Wextra # reasonable and standard
                                   -Wshadow # warn the user if a variable
                                            # declaration shadows one from a
                                            # parent context
                                   -Wnon-virtual-dtor # warn the user if a class
                                                      # with virtual functions
                                                      # has a non-virtual
                                                      # destructor. This helps
                                                      # catch hard to track down
                                                      # memory errors
                                   -Wold-style-cast # warn for c-style casts
                                   -Wcast-align # warn for potential performance
                                                # problem casts
                                   -Wunused # warn on anything being unused
                                   -Woverloaded-virtual # warn if you overload
                                                        # (not override) a
                                                        # virtual function
                                   -Wpedantic # warn if non-standard C++ is used
                                   -Wconversion # warn on type conversions that
                                                # may lose data
                                   -Wsign-conversion # warn on sign conversions
                                   -Wmisleading-indentation # warn if identation
                                                            # implies blocks
                                                            # where blocks do
                                                            # not exist
                                   -Wduplicated-cond # warn if if / else chain
                                                     # has duplicated conditions
                                   -Wduplicated-branches # warn if if / else
                                                         # branches have
                                                         # duplicated code
                                   -Wlogical-op # warn about logical operations
                                                # being used where bitwise were
                                                # probably wanted
                                   -Wnull-dereference # warn if a null
                                                      # dereference is detected
                                   -Wuseless-cast # warn if you perform a cast
                                                  # to the same type
                                   -Wdouble-promotion # warn if float is
                                                      # implicit promoted to
                                                      # double
                                   -Wformat=2 # warn on security issues around
                                              # functions that format output (ie
                                              # printf)
                         )
endif()

set(CMAKE_CONAN_VERSION "v0.14")
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake ${CMAKE_CONAN_VERSION} from https://github.com/conan-io/cmake-conan"
    )
  file(
    DOWNLOAD
    "https://github.com/conan-io/cmake-conan/raw/${CMAKE_CONAN_VERSION}/conan.cmake"
    "${CMAKE_BINARY_DIR}/conan.cmake")
else()
  message(STATUS "Using existing conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_check(REQUIRED)

message(STATUS "RUNNING CONAN")

conan_add_remote(NAME
                 bincrafters
                 URL
                 https://api.bintray.com/conan/bincrafters/public-conan)

# This will create the conanbuildinfo.cmake in the current binary dir, not the
# cmake_binary_dir
conan_cmake_run(REQUIRES
                jsonformoderncpp/3.7.0@vthiery/stable
                fmt/5.3.0@bincrafters/stable
                spdlog/1.3.1@bincrafters/stable
                BASIC_SETUP
                CMAKE_TARGETS
                NO_OUTPUT_DIRS
                GENERATORS
                cmake_find_package
                OPTIONS
                BUILD
                missing)

set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
find_package(jsonformoderncpp)
find_package(fmt)
find_package(spdlog)

add_executable(nu_plugin_calc src/nu_plugin_calc.cpp)
target_compile_features(nu_plugin_calc PRIVATE cxx_std_17)
target_link_libraries(nu_plugin_calc
                      PRIVATE jsonformoderncpp::jsonformoderncpp
                              fmt::fmt
                              spdlog::spdlog
                              project_warnings)

enable_testing()
