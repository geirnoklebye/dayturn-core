# -*- cmake -*-
include(Prebuilt)

# There are three possible solutions to provide the llphysicsextensions:
# - The full source package, selected by -DHAVOK:BOOL=ON
# - The stub source package, selected by -DHAVOK:BOOL=OFF 
# - The prebuilt package available to those with sublicenses, selected by -DHAVOK_TPV:BOOL=ON

if (INSTALL_PROPRIETARY)
   set(HAVOK ON CACHE BOOL "Use Havok physics library")
endif (INSTALL_PROPRIETARY)


# Note that the use_prebuilt_binary macros below do not in fact include binaries;
# the llphysicsextensions_* packages are source only and are built here.
# The source package and the stub package both build libraries of the same name.

   use_prebuilt_binary( ndPhysicsStub )

# <FS:ND> Don't set this variable, there is no need to build any stub source if using ndPhysicsStub
#   set(LLPHYSICSEXTENSIONS_SRC_DIR ${LIBS_PREBUILT_DIR}/llphysicsextensions/stub)
# </FS:ND>

   set(LLPHYSICSEXTENSIONS_LIBRARIES nd_hacdConvexDecomposition hacd nd_Pathing )

   # <FS:ND> include paths for LLs version and ours are different.
   set(LLPHYSICSEXTENSIONS_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/ )
   # </FS:ND>

# <FS:ND> include paths for LLs version and ours are different.
#set(LLPHYSICSEXTENSIONS_INCLUDE_DIRS ${LIBS_PREBUILT_DIR}/include/llphysicsextensions) 
# </FS:ND>