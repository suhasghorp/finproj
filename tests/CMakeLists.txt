# Testing library
find_package(Catch2 3 REQUIRED)
# Adds Catch2::Catch2

find_package(Matplot++ REQUIRED)

# Tests need to be added as executables first
add_executable(tests
        TestCalendar.cpp
        TestChronoDate.cpp
        TestDayCount.cpp
        TestInterpolator.cpp
        TestDiscountCurveZeros.cpp
        TestIborFuture.cpp
        TestIborSwap.cpp
        TestIborSingleCurve.cpp
        TestCreditCurve.cpp
        TestCDS.cpp
        TestCDSBasket.cpp)

# I'm using C++20 in the test
target_compile_features(tests PRIVATE cxx_std_20)

target_compile_options(finproj PRIVATE -Werror)
set(ENV{PYTHONPATH} "/home/sghorp/miniconda3/bin/python")
include_directories(/home/sghorp/miniconda3/lib/python3.8/site-packages/numpy/core/include)

# Should be linked to the main library, as well as the Catch2 testing library
#target_link_libraries(tests PRIVATE finproj Catch2::Catch2WithMain fmt::fmt)
target_link_libraries(tests PUBLIC finproj Catch2::Catch2WithMain Eigen3::Eigen Boost::boost Python3::Python)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
#add_test(NAME testlibtest COMMAND testlib) # Command can be a target

add_custom_command( TARGET tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying unit test data.."
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_HOME_DIRECTORY}/data ${CMAKE_BINARY_DIR}
        )
