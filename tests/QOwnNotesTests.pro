TEMPLATE = subdirs
SUBDIRS = unit_tests \

CONFIG += c++20
# For old qmake versions
QMAKE_CXXFLAGS += -std=c++20