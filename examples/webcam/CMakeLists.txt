add_executable(webcam webcam.cpp)
target_link_libraries(webcam videoproc ${GSTREAMER_LIBRARIES})
