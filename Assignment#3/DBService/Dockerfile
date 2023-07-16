# Use a base image with CMake and necessary dependencies
FROM ubuntu:latest
RUN apt-get update && apt-get install -y cmake curl make gcc libzip-dev
SHELL ["/bin/bash","-c"]
# Set the working directory
WORKDIR /app
# Copy the necessary files
COPY flightScanner.sh .
COPY flightsDB.zip .
ADD build /app/build
# Set the working directory to the build directory
WORKDIR /app/build
# Set the entry point to run the compiled executable
CMD ["./DBService"]