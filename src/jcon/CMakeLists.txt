project(jcon)

set(QTDIR $ENV{QTDIR})
set(Q5_DIR $ENV{QTDIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH ${QTDIR})

file(GLOB ${PROJECT_NAME}_headers *.h)
file(GLOB ${PROJECT_NAME}_sources *.cpp)

add_definitions(-DJCON_DLL)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})

find_package(Qt5 COMPONENTS Core Network Test WebSockets)
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::Test Qt5::WebSockets)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
