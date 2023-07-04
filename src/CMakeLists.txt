# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${finproj_SOURCE_DIR}/include/finproj/*.h")
#set(HEADER_LIST "${ModernCMakeExample_SOURCE_DIR}/include/modern/lib.hpp")

# Make an automatic library - will be static or dynamic based on user setting
add_library(finproj ${HEADER_LIST}
        utils/Calendar.cpp
        utils/ChronoDate.cpp
        utils/DayCount.cpp
        utils/Schedule.cpp
        utils/Interpolator.cpp
        curves/DiscountCurve.cpp
        curves/DiscountCurveZeros.cpp
        curves/IborDeposit.cpp
        curves/IborFRA.cpp
        curves/IborFuture.cpp
        curves/SwapFixedLeg.cpp
        curves/SwapFloatLeg.cpp
        curves/IborSwap.cpp
        curves/IborSingleCurve.cpp
        curves/CDS.cpp
        curves/CreditCurve.cpp
        models/GaussCopula.cpp curves/CDSBasket.cpp curves/CDSIndexPortfolio.cpp models/StudentTCopula.cpp)

# We need this directory, and users of our library will need it too
target_include_directories(finproj PUBLIC ../include)

# This depends on (header only) boost
#target_link_libraries(finproj PRIVATE Boost::boost libInterpolate::Interpolate dlib::dlib)
target_link_libraries(finproj PRIVATE libInterpolate::Interpolate Eigen3::Eigen xtensor)

target_compile_options(finproj PRIVATE -Werror)

# All users of this library will need at least C++11
target_compile_features(finproj PUBLIC cxx_std_20)

# IDEs should put the headers in a nice place
source_group(
        TREE "${PROJECT_SOURCE_DIR}/include"
        PREFIX "Header Files"
        FILES ${HEADER_LIST})
