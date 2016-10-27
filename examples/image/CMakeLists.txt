add_executable(image image.cpp)
target_link_libraries(image videoproc ${GSTREAMER_LIBRARIES})
