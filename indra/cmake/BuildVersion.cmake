# -*- cmake -*-
# Construct the viewer version number based on the indra/VIEWER_VERSION file

if (NOT DEFINED VIEWER_SHORT_VERSION) # will be true in indra/, false in indra/newview/
    set(VIEWER_VERSION_BASE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/newview/VIEWER_VERSION.txt")
    set(VIEWER_GIT_REPO_PRESENCE "${CMAKE_CURRENT_SOURCE_DIR}/../.git")

    if ( EXISTS ${VIEWER_VERSION_BASE_FILE} )
        file(STRINGS ${VIEWER_VERSION_BASE_FILE} VIEWER_SHORT_VERSION REGEX "^[0-9]+\\.[0-9]+\\.[0-9]+")
        string(REGEX REPLACE "^([0-9]+)\\.[0-9]+\\.[0-9]+" "\\1" VIEWER_VERSION_MAJOR ${VIEWER_SHORT_VERSION})
        string(REGEX REPLACE "^[0-9]+\\.([0-9]+)\\.[0-9]+" "\\1" VIEWER_VERSION_MINOR ${VIEWER_SHORT_VERSION})
        string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+)" "\\1" VIEWER_VERSION_PATCH ${VIEWER_SHORT_VERSION})

        if (DEFINED ENV{revision})
           set(VIEWER_VERSION_REVISION $ENV{revision})
           message(STATUS "Revision (from environment): ${VIEWER_VERSION_REVISION}")

        #Autobuild-1.1 forces the build_id to date and time. Kokua uses the local tip hash
        #elseif (DEFINED ENV{AUTOBUILD_BUILD_ID})
        #   set(VIEWER_VERSION_REVISION $ENV{AUTOBUILD_BUILD_ID})
        #   message(STATUS "Revision (from autobuild environment): ${VIEWER_VERSION_REVISION}")
        
        # if this is a git repo we count the commits to the branch tip
        elseif ( EXISTS ${VIEWER_GIT_REPO_PRESENCE} )
          find_program(GIT git)
          if (GIT)
            execute_process(COMMAND ${GIT} branch --show-current
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                            RESULT_VARIABLE git_cb_result
                            ERROR_VARIABLE git_cb_error
                            OUTPUT_VARIABLE GIT_CURRENT_BRANCH
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
            if (NOT ${git_cb_result} EQUAL 0)
              message(SEND_ERROR "Reading git branch failed with output (is git 2.22 or better?):\n${git_cb_error}")
            else (NOT ${git_cb_result} EQUAL 0)
              execute_process(COMMAND ${GIT} rev-list --count ${GIT_CURRENT_BRANCH}
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                            RESULT_VARIABLE git_rc_result
                            ERROR_VARIABLE git_rc_error
                            OUTPUT_VARIABLE VIEWER_VERSION_REVISION
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
              if (NOT ${git_rc_result} EQUAL 0)
                message(SEND_ERROR "Getting revision count failed with output:\n${git_rc_error}")
              else (NOT ${git_rc_result} EQUAL 0)
                message(STATUS "Revision (from git) ${VIEWER_VERSION_REVISION} on branch ${GIT_CURRENT_BRANCH}")
              endif (NOT ${git_rc_result} EQUAL 0)            
            endif (NOT ${git_cb_result} EQUAL 0)
          else (GIT)
            if (DEFINED ENV{AUTOBUILD_BUILD_ID})
	             set(VIEWER_VERSION_REVISION $ENV{AUTOBUILD_BUILD_ID})
	             message(STATUS "Revision (from autobuild environment under git (git executable not found)): ${VIEWER_VERSION_REVISION}")
            endif (DEFINED ENV{AUTOBUILD_BUILD_ID})
         endif (GIT)

        else (DEFINED ENV{revision})
          # make absolutely sure we're going to execute the find_program rather than
          # using a pre-existing (probably wrong) location
          unset (MERCURIAL)
          unset (MERCURIAL CACHE)        
          # make sure we look in the Windows registry first to avoid getting hijacked
          # by a non-executable python hg within cygwin/bin
          find_program(MERCURIAL
                       NAMES hg
                       PATHS [HKEY_LOCAL_MACHINE\\Software\\TortoiseHG]
                       NO_DEFAULT_PATH
                       PATH_SUFFIXES Mercurial)
          find_program(MERCURIAL
                       NAMES hg
                       PATH_SUFFIXES Mercurial)
          mark_as_advanced(MERCURIAL)
          if (MERCURIAL)
            # this probably isn't the last of problems with finding Mercurial so report which one we're using
            message("-- Using Mercurial from " ${MERCURIAL})
            # the added -r . below suppresses the appending of a + to the result if there are uncommitted changes,
            # which in turn appears to cause problems hg_id_result becoming the previous value appended to the current
            # value, resulting in buildids like 4524745335 when the previous was 45247 and it's now 45335
            execute_process(COMMAND ${MERCURIAL} identify --num -r .
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                            RESULT_VARIABLE hg_id_result
                            ERROR_VARIABLE hg_id_error
                            OUTPUT_VARIABLE VIEWER_VERSION_REVISION
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
            if (NOT ${hg_id_result} EQUAL 0)
              message(SEND_ERROR "Revision number generation failed with output:\n${hg_id_error}")
            else (NOT ${hg_id_result} EQUAL 0)
              string(REGEX REPLACE "[^0-9a-f]" "" VIEWER_VERSION_REVISION ${VIEWER_VERSION_REVISION})
            endif (NOT ${hg_id_result} EQUAL 0)
            if ("${VIEWER_VERSION_REVISION}" MATCHES "^[0-9]+$")
              message(STATUS "Revision (from hg) ${VIEWER_VERSION_REVISION}")
            else ("${VIEWER_VERSION_REVISION}" MATCHES "^[0-9]+$")
              message(STATUS "Revision not set (repository not found?); using 0")
              set(VIEWER_VERSION_REVISION 0 )
            endif ("${VIEWER_VERSION_REVISION}" MATCHES "^[0-9]+$")
           else (MERCURIAL)
              message(STATUS "Revision not set: mercurial not found; using 0")
              set(VIEWER_VERSION_REVISION 0)
           endif (MERCURIAL)
        endif (DEFINED ENV{revision})
        message(STATUS "Building '${VIEWER_CHANNEL}' Version ${VIEWER_SHORT_VERSION}.${VIEWER_VERSION_REVISION}")
    else ( EXISTS ${VIEWER_VERSION_BASE_FILE} )
        message(SEND_ERROR "Cannot get viewer version from '${VIEWER_VERSION_BASE_FILE}'") 
    endif ( EXISTS ${VIEWER_VERSION_BASE_FILE} )

    if ("${VIEWER_VERSION_REVISION}" STREQUAL "")
      message(STATUS "Ultimate fallback, revision was blank or not set: will use 0")
      set(VIEWER_VERSION_REVISION 0)
    endif ("${VIEWER_VERSION_REVISION}" STREQUAL "")

    set(VIEWER_CHANNEL_VERSION_DEFINES
        "LL_VIEWER_CHANNEL=${VIEWER_CHANNEL}"
        "LL_VIEWER_VERSION_MAJOR=${VIEWER_VERSION_MAJOR}"
        "LL_VIEWER_VERSION_MINOR=${VIEWER_VERSION_MINOR}"
        "LL_VIEWER_VERSION_PATCH=${VIEWER_VERSION_PATCH}"
        "LL_VIEWER_VERSION_BUILD=${VIEWER_VERSION_REVISION}"
        "LLBUILD_CONFIG=\"${CMAKE_BUILD_TYPE}\""
        )
endif (NOT DEFINED VIEWER_SHORT_VERSION)
