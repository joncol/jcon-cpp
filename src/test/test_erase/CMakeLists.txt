project(test_erase)

set(${PROJECT_NAME}_headers )
file(GLOB ${PROJECT_NAME}_sources *.cpp **/*.cpp)

if (NOT WIN32)
    add_definitions(-DBOOST_TEST_DYN_LINK)
endif()

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_headers} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)
