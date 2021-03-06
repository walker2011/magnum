#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015
#             Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

# Files shared between main library and unit test library
set(MagnumSceneGraph_SRCS
    Animable.cpp)

# Files compiled with different flags for main library and unit test library
set(MagnumSceneGraph_GracefulAssert_SRCS
    instantiation.cpp)

set(MagnumSceneGraph_HEADERS
    AbstractFeature.h
    AbstractFeature.hpp
    AbstractGroupedFeature.h
    AbstractObject.h
    AbstractTransformation.h
    AbstractTranslation.h
    AbstractTranslationRotation2D.h
    AbstractTranslationRotation3D.h
    AbstractTranslationRotationScaling2D.h
    AbstractTranslationRotationScaling3D.h
    Animable.h
    Animable.hpp
    AnimableGroup.h
    Camera.h
    Camera.hpp
    Drawable.h
    Drawable.hpp
    DualComplexTransformation.h
    DualQuaternionTransformation.h
    RigidMatrixTransformation2D.h
    RigidMatrixTransformation3D.h
    FeatureGroup.h
    FeatureGroup.hpp
    MatrixTransformation2D.h
    MatrixTransformation3D.h
    Object.h
    Object.hpp
    Scene.h
    SceneGraph.h
    TranslationTransformation.h

    visibility.h)

if(MAGNUM_BUILD_DEPRECATED)
    list(APPEND MagnumSceneGraph_HEADERS
        AbstractCamera.h
        AbstractCamera.hpp
        Camera2D.h
        Camera2D.hpp
        Camera3D.h
        Camera3D.hpp)
endif()

# Objects shared between main and test library
add_library(MagnumSceneGraphObjects OBJECT
    ${MagnumSceneGraph_SRCS}
    ${MagnumSceneGraph_HEADERS})
if(NOT BUILD_STATIC)
    set_target_properties(MagnumSceneGraphObjects PROPERTIES COMPILE_FLAGS "-DMagnumSceneGraphObjects_EXPORTS")
endif()
if(NOT BUILD_STATIC OR BUILD_STATIC_PIC)
    set_target_properties(MagnumSceneGraphObjects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Main SceneGraph library
add_library(MagnumSceneGraph ${SHARED_OR_STATIC}
    $<TARGET_OBJECTS:MagnumSceneGraphObjects>
    ${MagnumSceneGraph_GracefulAssert_SRCS})
set_target_properties(MagnumSceneGraph PROPERTIES DEBUG_POSTFIX "-d")
if(BUILD_STATIC_PIC)
    set_target_properties(MagnumSceneGraph PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_link_libraries(MagnumSceneGraph Magnum)

install(TARGETS MagnumSceneGraph
    RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
    LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
    ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
install(FILES ${MagnumSceneGraph_HEADERS} DESTINATION ${MAGNUM_INCLUDE_INSTALL_DIR}/SceneGraph)

if(BUILD_TESTS)
    # Library with graceful assert for testing
    add_library(MagnumSceneGraphTestLib ${SHARED_OR_STATIC}
        $<TARGET_OBJECTS:MagnumSceneGraphObjects>
        ${MagnumSceneGraph_GracefulAssert_SRCS})
    set_target_properties(MagnumSceneGraphTestLib PROPERTIES
        COMPILE_FLAGS "-DCORRADE_GRACEFUL_ASSERT -DMagnumSceneGraph_EXPORTS"
        DEBUG_POSTFIX "-d")
    target_link_libraries(MagnumSceneGraphTestLib MagnumMathTestLib)

    # On Windows we need to install first and then run the tests to avoid "DLL
    # not found" hell, thus we need to install this too
    if(CORRADE_TARGET_WINDOWS AND NOT CMAKE_CROSSCOMPILING AND NOT BUILD_STATIC)
        install(TARGETS MagnumSceneGraphTestLib
            RUNTIME DESTINATION ${MAGNUM_BINARY_INSTALL_DIR}
            LIBRARY DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR}
            ARCHIVE DESTINATION ${MAGNUM_LIBRARY_INSTALL_DIR})
    endif()

    add_subdirectory(Test)
endif()
