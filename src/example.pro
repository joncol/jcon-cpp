QT += core network

TEMPLATE = app

# The lib jcon is built before the example in jcon-cpp.pro
LIBS += -Ljcon -ljcon

HEADERS = *.h
SOURCES = *.cpp

OTHER_FILES = CMakeLists.txt


