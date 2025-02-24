# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -lhiredis

# Target executable
TARGET = redisProj

# Source files
SRCS = main.cpp redis.cpp logger.cpp  # ✅ Add logger.cpp back

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(CXXFLAGS)

# Compile the source files into object files
%.o: %.cpp
	$(CXX) -c $< -o $@

# Clean up the build files
clean:
	rm -f $(OBJS) $(TARGET)
