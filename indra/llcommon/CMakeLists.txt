# -*- cmake -*-

project(llcommon)

include(00-Common)
include(LLCommon)
include(bugsplat)
include(Linking)
include(Boost)
include(LLSharedLibs)
include(JsonCpp)
include(Copy3rdPartyLibs)
include(ZLIBNG)
include(URIPARSER)
include(Tracy)

include_directories(
    ${EXPAT_INCLUDE_DIRS}
    ${LLCOMMON_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIR}
    ${ZLIBNG_INCLUDE_DIRS}
    ${URIPARSER_INCLUDE_DIRS}
    ${TRACY_INCLUDE_DIR}
    )
    
include_directories(
    SYSTEM
    ${LLCOMMON_SYSTEM_INCLUDE_DIRS}
    )    

# add_executable(lltreeiterators lltreeiterators.cpp)
# 
# target_link_libraries(lltreeiterators
#     ${LLCOMMON_LIBRARIES})

set(llcommon_SOURCE_FILES
    commoncontrol.cpp
    indra_constants.cpp
    llallocator.cpp
    llallocator_heap_profile.cpp
    llapp.cpp
    llapr.cpp
    llassettype.cpp
    llatomic.cpp
    llbase32.cpp
    llbase64.cpp
    llbitpack.cpp
    llcallbacklist.cpp
    llcallstack.cpp
    llcleanup.cpp
    llcommon.cpp
    llcommonutils.cpp
    llcoros.cpp
    llcrc.cpp
    llcriticaldamp.cpp
    lldate.cpp
    lldeadmantimer.cpp
    lldependencies.cpp
    lldictionary.cpp
    llerror.cpp
    llevent.cpp
    lleventapi.cpp
    lleventcoro.cpp
    lleventdispatcher.cpp
    lleventfilter.cpp
    llevents.cpp
    lleventtimer.cpp
    llexception.cpp
    llfasttimer.cpp
    llfile.cpp
    llfindlocale.cpp
    llfixedbuffer.cpp
    llformat.cpp
    llframetimer.cpp
    llheartbeat.cpp
    llheteromap.cpp
    llinitparam.cpp
    llinitdestroyclass.cpp
    llinstancetracker.cpp
    llkeybind.cpp
    llleap.cpp
    llleaplistener.cpp
    llliveappconfig.cpp
    lllivefile.cpp
    llmd5.cpp
    llmemory.cpp
    llmemorystream.cpp
    llmetrics.cpp
    llmetricperformancetester.cpp
    llmortician.cpp
    llmutex.cpp
    llptrto.cpp 
    llpredicate.cpp
    llprocess.cpp
    llprocessor.cpp
    llprocinfo.cpp
    llqueuedthread.cpp
    llrand.cpp
    llrefcount.cpp
    llrun.cpp
    llsd.cpp
    llsdjson.cpp
    llsdparam.cpp
    llsdserialize.cpp
    llsdserialize_xml.cpp
    llsdutil.cpp
    llsingleton.cpp
    llstacktrace.cpp
    llstreamqueue.cpp
    llstreamtools.cpp
    llstring.cpp
    llstringtable.cpp
    llsys.cpp
    lltempredirect.cpp
    llthread.cpp
    llthreadsafequeue.cpp
    lltimer.cpp
    lltrace.cpp
    lltraceaccumulators.cpp
    lltracerecording.cpp
    lltracethreadrecorder.cpp
    lluri.cpp
    lluriparser.cpp
    lluuid.cpp
    llworkerthread.cpp
    hbxxh.cpp
    u64.cpp
    threadpool.cpp
    workqueue.cpp
    StackWalker.cpp
    )
    
set(llcommon_HEADER_FILES
    CMakeLists.txt

    chrono.h
    commoncontrol.h
    classic_callback.h
    ctype_workaround.h
    fix_macros.h
    indra_constants.h
    linden_common.h
    llalignedarray.h
    llallocator.h
    llallocator_heap_profile.h
    llapp.h
    llapr.h
    llassettype.h
    llatomic.h
    llbase32.h
    llbase64.h
    llbitpack.h
    llbool.h
    llboost.h
    llcallbacklist.h
    llcallstack.h
    llcleanup.h
    llcommon.h
    llcommonutils.h
    llcond.h
    llcoros.h
    llcrc.h
    llcriticaldamp.h
    lldate.h
    lldeadmantimer.h
    lldefs.h
    lldependencies.h
    lldepthstack.h
    lldictionary.h
    lldoubledispatch.h
    llendianswizzle.h
    llerror.h
    llerrorcontrol.h
    llevent.h
    lleventapi.h
    lleventcoro.h
    lleventdispatcher.h
    lleventfilter.h
    llevents.h
    lleventemitter.h
    llexception.h
    llfasttimer.h
    llfile.h
    llfindlocale.h
    llfixedbuffer.h
    llformat.h
    llframetimer.h
    llhandle.h
    llhash.h
    llheartbeat.h
    llheteromap.h
    llindexedvector.h
    llinitdestroyclass.h
    llinitparam.h
    llinstancetracker.h
    llinstancetrackersubclass.h
    llkeybind.h
    llkeythrottle.h
    llleap.h
    llleaplistener.h
    llliveappconfig.h
    lllivefile.h
    llmainthreadtask.h
    llmd5.h
    llmemory.h
    llmemorystream.h
    llmetrics.h
    llmetricperformancetester.h
    llmortician.h
    llmutex.h
    llnametable.h
    llpointer.h
    llprofiler.h
    llprofilercategories.h
    llpounceable.h
    llpredicate.h
    llpreprocessor.h
    llpriqueuemap.h
    llprocess.h
    llprocessor.h
    llprocinfo.h
    llptrto.h
    llqueuedthread.h
    llrand.h
    llrefcount.h
    llregex.h
    llregistry.h
    llrun.h
    llsafehandle.h
    llsd.h
    llsdjson.h
    llsdparam.h
    llsdserialize.h
    llsdserialize_xml.h
    llsdutil.h
    llsimplehash.h
    llsingleton.h
    llstacktrace.h
    llstl.h
    llstreamqueue.h
    llstreamtools.h
    llstrider.h
    llstring.h
    llstringtable.h
    llstaticstringtable.h
    llstatsaccumulator.h
    llsys.h
    lltempredirect.h
    llthread.h
    llthreadlocalstorage.h
    llthreadsafequeue.h
    lltimer.h
    lltrace.h
    lltraceaccumulators.h
    lltracerecording.h
    lltracethreadrecorder.h
    lltreeiterators.h
    llunits.h
    llunittype.h
    lluri.h
    lluriparser.h
    lluuid.h
    llwin32headers.h
    llwin32headerslean.h
    llworkerthread.h
    hbxxh.h
    lockstatic.h
    stdtypes.h
    stringize.h
    threadpool.h
    threadpool_fwd.h
    threadsafeschedule.h
    timer.h
    tuple.h
    u64.h
    workqueue.h
    StackWalker.h
    )

if (DARWIN)
# <AV:CR>
  list(APPEND llcommon_HEADER_FILES llsys_objc.h)
  list(APPEND llcommon_SOURCE_FILES llsys_objc.mm)
# </AV:CR>
endif (DARWIN)

set_source_files_properties(${llcommon_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

if (USE_BUGSPLAT)
  set_source_files_properties(${llcommon_SOURCE_FILES}
    PROPERTIES COMPILE_DEFINITIONS "${BUGSPLAT_DEFINE}")
endif (USE_BUGSPLAT)

list(APPEND llcommon_SOURCE_FILES ${llcommon_HEADER_FILES})

if(LLCOMMON_LINK_SHARED)
  add_library (llcommon SHARED ${llcommon_SOURCE_FILES})
  if(NOT ADDRESS_SIZE EQUAL 32)
    if(WINDOWS)
      ##add_definitions(/FIXED:NO)
    else(WINDOWS) # not windows therefore DARWIN
      add_definitions(-fPIC)
    endif(WINDOWS)
  endif(NOT ADDRESS_SIZE EQUAL 32)
  if(WINDOWS)
    # always generate llcommon.pdb, even for "Release" builds
    set_target_properties(llcommon PROPERTIES LINK_FLAGS "/DEBUG")
  endif(WINDOWS)
  ll_stage_sharedlib(llcommon)
else(LLCOMMON_LINK_SHARED)
    add_library (llcommon ${llcommon_SOURCE_FILES})
endif(LLCOMMON_LINK_SHARED)

target_link_libraries(
    llcommon
    ${APRUTIL_LIBRARIES}
    ${APR_LIBRARIES}
    ${EXPAT_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${ZLIBNG_LIBRARIES}
    ${WINDOWS_LIBRARIES}
    ${BOOST_FIBER_LIBRARY}
    ${BOOST_CONTEXT_LIBRARY}
    ${BOOST_PROGRAM_OPTIONS_LIBRARY}
    ${BOOST_REGEX_LIBRARY}
    ${BOOST_SYSTEM_LIBRARY}
    ${URIPARSER_LIBRARIES}
    ${TRACY_LIBRARY}
    )

if (DARWIN)
  include(CMakeFindFrameworks)
  find_library(CARBON_LIBRARY Carbon)
  target_link_libraries(llcommon ${CARBON_LIBRARY})
endif (DARWIN)

add_dependencies(llcommon stage_third_party_libs)

if (LL_TESTS)
  include(LLAddBuildTest)
  SET(llcommon_TEST_SOURCE_FILES
    # unit-testing llcommon is not possible right now as the test-harness *itself* depends upon llcommon, causing a circular dependency.  Add your 'unit' tests as integration tests for now.
    )
  LL_ADD_PROJECT_UNIT_TESTS(llcommon "${llcommon_TEST_SOURCE_FILES}")

  #set(TEST_DEBUG on)
  set(test_libs llcommon 
      ${LLCOMMON_LIBRARIES} 
      ${WINDOWS_LIBRARIES} 
      ${GOOGLEMOCK_LIBRARIES} 
      ${BOOST_FIBER_LIBRARY} 
      ${BOOST_CONTEXT_LIBRARY} 
      ${BOOST_THREAD_LIBRARY} 
      ${BOOST_SYSTEM_LIBRARY})
  LL_ADD_INTEGRATION_TEST(bitpack "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(classic_callback "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(commonmisc "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llbase64 "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llcond "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lldate "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lldeadmantimer "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lldependencies "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llerror "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lleventcoro "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lleventdispatcher "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lleventfilter "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llframetimer "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llheteromap "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llinstancetracker "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llleap "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llmainthreadtask "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llpounceable "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llprocess "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llprocessor "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llprocinfo "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llrand "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llsdserialize "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llsingleton "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llstreamqueue "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llstring "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lltrace "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lltreeiterators "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(llunits "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(lluri "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(stringize "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(threadsafeschedule "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(tuple "" "${test_libs}")
  LL_ADD_INTEGRATION_TEST(workqueue "" "${test_libs}")

## llexception_test.cpp isn't a regression test, and doesn't need to be run
## every build. It's to help a developer make implementation choices about
## throwing and catching exceptions.
##LL_ADD_INTEGRATION_TEST(llexception "" "${test_libs}")

endif (LL_TESTS)
