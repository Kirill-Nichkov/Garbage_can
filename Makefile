CXX := g++
PKG_CONFIG := pkg-config
TARGET := garbage-panel

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:.cpp=.o)

QT_CFLAGS := $(shell $(PKG_CONFIG) --cflags Qt5Widgets)
QT_LIBS := $(shell $(PKG_CONFIG) --libs Qt5Widgets)

CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic -fPIC -Iinclude $(QT_CFLAGS)
LDFLAGS := $(QT_LIBS)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
