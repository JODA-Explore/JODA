# - Print CMake and OS distribution version
#

function(system_info DISTRO)
    message(STATUS "CMake version: ${CMAKE_VERSION}")
    if (UNIX AND NOT APPLE)
        distro_name(DISTRO_NAME)
        set(DISTRO ${DISTRO_NAME} PARENT_SCOPE)
        message(STATUS "Linux distribution: ${DISTRO_NAME}")
    else (UNIX AND NOT APPLE)
        message(STATUS "Operating system: ${CMAKE_SYSTEM}")
    endif (UNIX AND NOT APPLE)
endfunction(system_info)

# probe various system files that may be found
function(distro_name varname)
    file(GLOB has_os_release /etc/os-release)
    file(GLOB has_lsb_release /etc/lsb-release)
    file(GLOB has_sys_release /etc/system-release)
    file(GLOB has_redhat_release /etc/redhat-release)
    set(_descr)
    # start with /etc/os-release,
    # see <http://0pointer.de/blog/projects/os-release.html>
    if (NOT has_os_release STREQUAL "")
        read_release(PRETTY_NAME FROM /etc/os-release INTO _descr)
        # previous "standard", used on older Ubuntu and Debian
    elseif (NOT has_lsb_release STREQUAL "")
        read_release(DISTRIB_DESCRIPTION FROM /etc/lsb-release INTO _descr)
    endif (NOT has_os_release STREQUAL "")
    # RHEL/CentOS etc. has just a text-file
    if (NOT _descr)
        if (NOT has_sys_release STREQUAL "")
            file(READ /etc/system-release _descr)
        elseif (NOT has_redhat_release STREQUAL "")
            file(READ /etc/redhat-release _descr)
        else (NOT has_sys_release STREQUAL "")
            # no yet known release file found
            set(_descr "unknown")
        endif (NOT has_sys_release STREQUAL "")
    endif (NOT _descr)
    # return from function (into appropriate variable)
    string(STRIP "${_descr}" _descr)
    set(${varname} "${_descr}" PARENT_SCOPE)
endfunction(distro_name varname)

# read property from the newer /etc/os-release
function(read_release valuename FROM filename INTO varname)
    file(STRINGS ${filename} _distrib
            REGEX "^${valuename}="
            )
    string(REGEX REPLACE
            "^${valuename}=\"?\(.*\)" "\\1" ${varname} "${_distrib}"
            )
    # remove trailing quote that got globbed by the wildcard (greedy match)
    string(REGEX REPLACE
            "\"$" "" ${varname} "${${varname}}"
            )
    set(${varname} "${${varname}}" PARENT_SCOPE)
endfunction(read_release valuename FROM filename INTO varname)
