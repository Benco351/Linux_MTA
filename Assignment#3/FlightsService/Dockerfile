# Use a base image with CMake and necessary dependencies
FROM ubuntu:latest
SHELL ["/bin/bash","-c"]
# Set the working directory
WORKDIR /app
# Copy the necessary files
COPY CMakeLists.txt .
ADD build /app/build
# Set the working directory to the build directory
WORKDIR /app/build
# Set the entry point to run the compiled executable
CMD ["./FlightService"]
