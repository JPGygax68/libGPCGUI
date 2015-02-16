find_package(Boost REQUIRED) # for some reason, specifying the component (unit_test_framework) won't work

add_executable(test_property_set property_set.cpp)
target_link_libraries(test_property_set PRIVATE libGPCGUI)
target_include_directories(test_property_set PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(test_property_set ${Boost_LIBRARIES})
add_test(NAME test_property_set COMMAND test_property_set CONFIGURATIONS Debug)
