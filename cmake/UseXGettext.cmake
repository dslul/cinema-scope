# Copyright (C) 2013 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Example usage:
#
#    include(UseXGettext)
#
#    add_translations_directory(${GETTEXT_PACKAGE})
#
#    add_translations_catalog(
#        GETTEXT_PACKAGE ${GETTEXT_PACKAGE}
#        COPYRIGHT_HOLDER "Canonical Ltd."
#        SOURCE_DIRECTORIES "${CMAKE_SOURCE_DIR}/src"
#    )

cmake_minimum_required(VERSION 2.8.9)

find_package(XGettext REQUIRED)

macro(add_translations_directory GETTEXT_PACKAGE)
    set(
        _POT_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/${GETTEXT_PACKAGE}.pot"
    )

    file(
	GLOB _PO_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/*.po
    )

    gettext_create_translations(
        ${_POT_FILE}
        ALL
        ${_PO_FILES}
    )
endmacro(add_translations_directory)

macro(add_translations_catalog)
    set(_oneValueArgs GETTEXT_PACKAGE COPYRIGHT_HOLDER)
    set(_multiValueArgs SOURCE_DIRECTORIES)

    cmake_parse_arguments(_ARG "" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

    set(_GETTEXT_PACKAGE ${PROJECT})
    if(_ARG_GETTEXT_PACKAGE)
        set(_GETTEXT_PACKAGE ${_ARG_GETTEXT_PACKAGE})
    endif()

    set(
	_POT_FILE
	"${CMAKE_CURRENT_SOURCE_DIR}/${_GETTEXT_PACKAGE}.pot"
    )

    add_custom_target (pot
        COMMENT “Building translation catalog.”
        DEPENDS ${_POT_FILE}
    )

    set(_SOURCES "")

    foreach(DIR ${_ARG_SOURCE_DIRECTORIES})
        file(
            GLOB_RECURSE _DIR_SOURCES
            RELATIVE ${CMAKE_SOURCE_DIR}
            ${DIR}/*.cpp
            ${DIR}/*.cc
            ${DIR}/*.cxx
            ${DIR}/*.vala
            ${DIR}/*.c
            ${DIR}/*.h
        )
	set (_SOURCES ${_SOURCES} ${_DIR_SOURCES})
    endforeach()

    xgettext_create_pot_file(
        ${_POT_FILE}
        ALL
        CPP
        QT
        INPUT ${_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ADD_COMMENTS "TRANSLATORS"
        KEYWORDS "_" "_:1,2" "N_" "N_:1,2"
        PACKAGE_NAME ${_GETTEXT_PACKAGE}
        COPYRIGHT_HOLDER ${_ARG_COPYRIGHT_HOLDER}
    )
endmacro()

