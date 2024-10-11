
set(SRCS
  MadgwickAHRS.cpp
  main.cpp
  mainwindow.cpp
  objectgl.cpp
  rOc_serial.cpp
  rOc_timer.cpp
)

set(HDRS
  MadgwickAHRS.h
  mainwindow.h
  objectgl.h
  rOc_serial.h
  rOc_timer.h
)


add_executable(mpu9250gui ${SRCS} ${HDRS})
target_link_libraries(mpu9250gui Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets)

