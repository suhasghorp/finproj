# Testing library
find_package(Catch2 3 REQUIRED)
# Adds Catch2::Catch2

# Tests need to be added as executables first
add_executable(tests
        TestCalendar.cpp
        TestChronoDate.cpp
        TestDayCount.cpp
        TestInterpolator.cpp
        TestDiscountCurveZeros.cpp
        TestIborFuture.cpp
        TestIborSwap.cpp TestIborSingleCurve.cpp TestCreditCurve.cpp TestCDS.cpp)

# I'm using C++17 in the test
target_compile_features(tests PRIVATE cxx_std_20)

target_compile_options(finproj PRIVATE -Werror)

# Should be linked to the main library, as well as the Catch2 testing library
#target_link_libraries(tests PRIVATE finproj Catch2::Catch2WithMain fmt::fmt)
target_link_libraries(tests PRIVATE finproj Catch2::Catch2WithMain)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
#add_test(NAME testlibtest COMMAND testlib) # Command can be a target
