include(ECMMarkAsTest)

macro(NWINEFFECTS_UNIT_TESTS)
   foreach(_testname ${ARGN})
      add_executable(${_testname} ${_testname}.cpp)
      add_test(NAME nwineffects-${_testname} COMMAND ${_testname})
      target_link_libraries(${_testname} Qt::Test nwin)
      ecm_mark_as_test(${_testname})
   endforeach()
endmacro()

nwineffects_unit_tests(
    windowquadlisttest
    timelinetest
)

add_executable(nwinglplatformtest nwinglplatformtest.cpp ../../src/opengl/glplatform.cpp ../../src/utils/version.cpp)
add_test(NAME nwineffects-nwinglplatformtest COMMAND nwinglplatformtest)
target_link_libraries(nwinglplatformtest Qt::Test Qt::Gui KF6::ConfigCore)
ecm_mark_as_test(nwinglplatformtest)
