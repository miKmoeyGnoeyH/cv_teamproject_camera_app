# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/cv_teamproject_camera_app_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/cv_teamproject_camera_app_autogen.dir/ParseCache.txt"
  "cv_teamproject_camera_app_autogen"
  )
endif()
