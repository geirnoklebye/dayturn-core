# -*- cmake -*-

# The copy_win_libs folder contains file lists and a script used to
# copy dlls, exes and such needed to run the Dayturn.sln from within
# VisualStudio.

include(CMakeCopyIfDifferent)
include(Linking)

# When we copy our dependent libraries, we almost always want to copy them to
# both the Release and the RelWithDebInfo staging directories. This has
# resulted in duplicate (or worse, erroneous attempted duplicate)
# copy_if_different commands. Encapsulate that usage.
# Pass FROM_DIR, TARGETS and the files to copy. TO_DIR is implicit.
# to_staging_dirs diverges from copy_if_different in that it appends to TARGETS.
MACRO(to_staging_dirs from_dir targets)
  foreach(staging_dir
          "${SHARED_LIB_STAGING_DIR_RELEASE}"
          "${SHARED_LIB_STAGING_DIR_RELWITHDEBINFO}")
    copy_if_different("${from_dir}" "${staging_dir}" out_targets ${ARGN})
    list(APPEND "${targets}" "${out_targets}")
  endforeach()
ENDMACRO(to_staging_dirs from_dir to_dir targets)

###################################################################
# set up platform specific lists of files that need to be copied
###################################################################
if(WINDOWS)
    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}/Debug")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}/RelWithDebInfo")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}/Release")

    #*******************************
    # VIVOX - *NOTE: no debug version
    set(vivox_lib_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(slvoice_src_dir "${ARCH_PREBUILT_BIN_RELEASE}")    
    set(slvoice_files SLVoice.exe )
    if (ADDRESS_SIZE EQUAL 64)
        list(APPEND vivox_libs
            vivoxsdk_x64.dll
            ortp_x64.dll
            )
    else (ADDRESS_SIZE EQUAL 64)
        list(APPEND vivox_libs
        vivoxsdk.dll
        ortp.dll
        )
    endif (ADDRESS_SIZE EQUAL 64)

    #*******************************
    # Misc shared libs 

    set(debug_src_dir "${ARCH_PREBUILT_DIRS_DEBUG}")
    set(debug_files
        libapr-1.dll
        libaprutil-1.dll
        libapriconv-1.dll
        ssleay32.dll
        libeay32.dll
        glod.dll
        libhunspell.dll
        )



    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(release_files
        openjpeg.dll
        libapr-1.dll
        libaprutil-1.dll
        libapriconv-1.dll
        nghttp2.dll
        libhunspell.dll
        uriparser.dll
        )

    # OpenSSL
    if(ADDRESS_SIZE EQUAL 64)
        set(release_files ${release_files} libcrypto-1_1-x64.dll)
        set(release_files ${release_files} libssl-1_1-x64.dll)
    else(ADDRESS_SIZE EQUAL 64)
        set(release_files ${release_files} libcrypto-1_1.dll)
        set(release_files ${release_files} libssl-1_1.dll)
    endif(ADDRESS_SIZE EQUAL 64)

    # Filenames are different for 32/64 bit BugSplat file and we don't
    # have any control over them so need to branch.
    if (USE_BUGSPLAT)
      if(ADDRESS_SIZE EQUAL 32)
        set(release_files ${release_files} BugSplat.dll)
        set(release_files ${release_files} BugSplatRc.dll)
        set(release_files ${release_files} BsSndRpt.exe)
      else(ADDRESS_SIZE EQUAL 32)
        set(release_files ${release_files} BugSplat64.dll)
        set(release_files ${release_files} BugSplatRc64.dll)
        set(release_files ${release_files} BsSndRpt64.exe)
      endif(ADDRESS_SIZE EQUAL 32)
    endif (USE_BUGSPLAT)

    if (FMODSTUDIO)
       set(debug_files ${debug_files} fmodL.dll)
       set(release_files ${release_files} fmod.dll)
    endif (FMODSTUDIO)
 
    if (OPENAL)
        list(APPEND release_files openal32.dll alut.dll)
    endif (OPENAL)
   
    #*******************************
    # Find directory to copy MS C runtime DLLs from using the registry

    if(ADDRESS_SIZE EQUAL 32)
        # this folder contains the 32bit DLLs.. (yes really!)
        set(registry_find_path "[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Windows;Directory]/SysWOW64")
    else(ADDRESS_SIZE EQUAL 32)
        # this folder contains the 64bit DLLs.. (yes really!)
        set(registry_find_path "[HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Windows;Directory]/System32")
    endif(ADDRESS_SIZE EQUAL 32)

    # Having a string containing the system registry path is a start, but to
    # get CMake to actually read the registry, we must engage some other
    # operation.
    get_filename_component(registry_path "${registry_find_path}" ABSOLUTE)


    #*******************************
    # Copy MS C runtime dlls, required for packaging.

    # default values
    set(MSVC_VER 150)
    set(MSVC_VER_RUNTIME 140)
    
    # Update MSVC_VER_RUNTIME based on active version of Vsiaul Studio. Note that
    # these are all identical for the moment and whilst that is historically valid
    # for VS 2017, one day, it may not be true for VS 2019 and beyond so I think it makes
    # send to maintain the flexibility to change easily.
    # Note: we also capture the MSVC_VER (mirrors AUTOBUILD_VSVER) and display it in the log
    # to confirm to the user what is happening but do not (currently) use it for anything
    if (MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920) # Visual Studio 2017
        set(MSVC_VER 150)
        set(MSVC_VER_RUNTIME 140)
    elseif (MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930) # Visual Studio 2019
        set(MSVC_VER 160)
        set(MSVC_VER_RUNTIME 140)
    elseif (MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1939) # Visual Studio 2022
        set(MSVC_VER 170)
        set(MSVC_VER_RUNTIME 140)
    else (MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920) 
        MESSAGE(WARNING "New MSVC_VERSION ${MSVC_VERSION} of MSVC: adapt Copy3rdPartyLibs.cmake")
    endif (MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920) 
    
    MESSAGE(STATUS "Building projects/solution for MSVC version ${MSVC_VER} and using version ${MSVC_VER_RUNTIME} as the basis for copying runtime DLLs")

    # These are candidate DLL names. Empirically, VS versions including 
    # VS 2017 and later require msvcp*.dll and vcruntime*.dll
    foreach(release_msvc_file
            msvcp${MSVC_VER_RUNTIME}.dll
            vcruntime${MSVC_VER_RUNTIME}.dll
            vcruntime140_1.dll
            )
        if(EXISTS "${registry_path}/${release_msvc_file}")
            to_staging_dirs(
                ${registry_path}
                third_party_targets
                ${release_msvc_file})
        else()
            # This isn't a WARNING because, as noted above, every VS version
            # we've observed has only a subset of the specified DLL names.
            MESSAGE(STATUS "Redist lib ${release_msvc_file} not found")
        endif()
    endforeach()
    MESSAGE(STATUS "Will copy redist files for MSVC ${MSVC_VER_RUNTIME}:")
    foreach(target ${third_party_targets})
        MESSAGE(STATUS "${target}")
    endforeach()


elseif(DARWIN)
    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}/Debug/Resources")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}/RelWithDebInfo/Resources")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}/Release/Resources")

    set(vivox_lib_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(slvoice_files SLVoice)
    set(vivox_libs
        libortp.dylib
        libvivoxsdk.dylib
       )
    set(debug_src_dir "${ARCH_PREBUILT_DIRS_DEBUG}")
    set(debug_files
       )
    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(release_files
        libapr-1.0.dylib
        libapr-1.dylib
        libaprutil-1.0.dylib
        libaprutil-1.dylib
        ${EXPAT_COPY}
        libhunspell-1.3.0.dylib
        libndofdev.dylib
        libnghttp2.dylib
        libnghttp2.14.dylib
        libnghttp2.14.19.0.dylib
        liburiparser.dylib
        liburiparser.1.dylib
        liburiparser.1.0.27.dylib
       )
    if (FMODSTUDIO)
      set(debug_files ${debug_files} libfmodL.dylib)
      set(release_files ${release_files} libfmod.dylib)
    endif (FMODSTUDIO)

elseif(LINUX)
    # linux is weird, multiple side by side configurations aren't supported
    # and we don't seem to have any debug shared libs built yet anyways...
    set(SHARED_LIB_STAGING_DIR_DEBUG            "${SHARED_LIB_STAGING_DIR}")
    set(SHARED_LIB_STAGING_DIR_RELWITHDEBINFO   "${SHARED_LIB_STAGING_DIR}")
    set(SHARED_LIB_STAGING_DIR_RELEASE          "${SHARED_LIB_STAGING_DIR}")

    set(vivox_lib_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
    set(vivox_libs
        libsndfile.so.1
        libortp.so
        libvivoxoal.so.1
        libvivoxsdk.so
        )
    set(slvoice_files SLVoice)

    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS

    set(debug_src_dir "${ARCH_PREBUILT_DIRS_DEBUG}")
    set(debug_files
       )
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS

    set(release_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")

    # *FIX - figure out what to do with duplicate libalut.so here -brad

    if(${ARCH} STREQUAL "x86_64")
      set(vivox_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
      set(vivox_files
          libortp.so
          libvivoxsdk.so
          SLVoice
        )
      set(release_files
          libapr-1.so.0
          libaprutil-1.so.0
          libatk-1.0.so.0 
          libdb-5.1.so
          libexpat.so
          libexpat.so.1
          libgmodule-2.0.so.0 
          libgobject-2.0.so 
          libopenjpeg.so
          libopenjpeg.so.1.4.0
         )
    else(${ARCH} STREQUAL "x86_64")
      set(vivox_src_dir "${ARCH_PREBUILT_DIRS_RELEASE}")
      set(vivox_files
          libsndfile.so.1
          libortp.so
          libvivoxoal.so.1
          libvivoxplatform.so
          libvivoxsdk.so
          SLVoice
          )
    set(release_files
        libapr-1.so.0
        libaprutil-1.so.0
        libatk-1.0.so
        libdb-5.1.so
        ${EXPAT_COPY}
        libfreetype.so.6.6.2
        libgmodule-2.0.so
        libgobject-2.0.so
        libhunspell-1.3.so.0.0.0
        libopenjpeg.so
        libuuid.so.16
        libuuid.so.16.0.22
        libfontconfig.so.1.8.0
        libfontconfig.so.1
        )
    if (OPENAL)
           set(release_files ${release_files} "libopenal.so")
    endif (OPENAL)
    endif(${ARCH} STREQUAL "x86_64")
    if (FMODSTUDIO)
        set(debug_files ${debug_files} "libfmodstudioL.so")
        set(release_files ${release_files} "libfmodstudio.so")
    endif (FMODSTUDIO)

else(WINDOWS)
    message(STATUS "WARNING: unrecognized platform for staging 3rd party libs, skipping...")
    set(vivox_lib_dir "${CMAKE_SOURCE_DIR}/newview/vivox-runtime/i686-linux")
    set(vivox_libs "")
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(debug_src_dir "${CMAKE_SOURCE_DIR}/../libraries/${ARCH}-linux/lib/debug")
    set(debug_files "")
    # *TODO - update this to use LIBS_PREBUILT_DIR and LL_ARCH_DIR variables
    # or ARCH_PREBUILT_DIRS
    set(release_src_dir "${CMAKE_SOURCE_DIR}/../libraries/${ARCH}-linux/lib/release")
    set(release_files "")

    set(debug_llkdu_src "")
    set(debug_llkdu_dst "")
    set(release_llkdu_src "")
    set(release_llkdu_dst "")
    set(relwithdebinfo_llkdu_dst "")
endif(WINDOWS)


################################################################
# Done building the file lists, now set up the copy commands.
################################################################

# Curiously, slvoice_files are only copied to SHARED_LIB_STAGING_DIR_RELEASE.
# It's unclear whether this is oversight or intentional, but anyway leave the
# single copy_if_different command rather than using to_staging_dirs.
copy_if_different(
    ${slvoice_src_dir}
    "${SHARED_LIB_STAGING_DIR_RELEASE}"
    out_targets
    ${slvoice_files}
    )
list(APPEND third_party_targets ${out_targets})

to_staging_dirs(
    ${vivox_lib_dir}
    third_party_targets
    ${vivox_libs}
    )

to_staging_dirs(
    ${release_src_dir}
    third_party_targets
    ${release_files}
    )

if(NOT USESYSTEMLIBS)
  add_custom_target(
      stage_third_party_libs ALL
      DEPENDS ${third_party_targets}
      )
endif(NOT USESYSTEMLIBS)
