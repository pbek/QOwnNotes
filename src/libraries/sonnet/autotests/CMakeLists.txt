
include(ECMMarkAsTest)
include(ECMAddTests)

find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

########### unittests ###############

ecm_add_tests(
 test_filter
 test_core
 test_suggest
 test_settings
 NAME_PREFIX "sonnet-"
 LINK_LIBRARIES KF5::SonnetCore Qt5::Test
)

ecm_add_tests(
 test_highlighter
 NAME_PREFIX "sonnet-"
 LINK_LIBRARIES KF5::SonnetUi KF5::SonnetCore Qt5::Test
)
