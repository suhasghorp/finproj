set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
#SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
#SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
# setting the above flag will create gmon.out file
# gprof app > app.output && nano app.output


#set(ENV{PYTHONHOME} "/home/sghorp/miniconda3")
# must set following before running ./app in terminal
#export PYTHONHOME=/home/sghorp/miniconda3
include_directories(/home/sghorp/miniconda3/lib/python3.8/site-packages/numpy/core/include)

add_executable(app Application.cpp app.cpp)
target_compile_options(app PRIVATE -Wall -Wextra -pedantic -Werror)

target_link_libraries(app PRIVATE finproj Eigen3::Eigen Python3::Python)

add_custom_command( TARGET app
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying test data.."
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_HOME_DIRECTORY}/data ${CMAKE_BINARY_DIR}
        )

