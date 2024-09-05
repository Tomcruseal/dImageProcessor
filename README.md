# dImageProcessor
A peer-to-peer architecture for image sending and processing

## Required dependencies
CMake>=3.19
boost 1.77.0 (already packed in the /include)
C++ compiler clang++15 with C++ standard set to **17**
pthread (should be internally available on most Unix systems)
JDK 11.0.24 Oracle x64
gradle 8.6
package org.json:json:20240303
genome-terminal(otherwise the following `make` could not execute, the user needs to build both peer applications separately)

## Tested environment
RHEL 8.9 on an Intel NUC 10 (i7-10710U, 32GB RAM)

## Test instructions
At the root directory, 
```
make
```
then upon the command line of the `ImageProducer`,
```
localhost 8080 ../../resources/dog3.jpg
```

Or if the user wants to build both peer applications separately,
```
cd imageConsumer
gradle build
gradle run
```
The imageConsumer should run and listen to port 8080.
Then similarly,
```
cd imageProducer
cmake .
make
./ImageProducer
```
The imageProducer should run and ready to accept the parameters.


## Directory Structure

- `dImageProcessor/`
  - `imageProducer/`
    - `build/`
    - `imageProducer.cpp`
    - `CMakeLists.txt`
  - `imageConsumer/`
    - `build/`
    - `src/`
      - `main/`
        - `java/`
          - `ImageConsumer.java`
    - `build.gradle`
  - `include/`
  - `Makefile`
  - `resources/`
  - `README.md`

+ Directory imageProducer: C++ based imageProducer peer to send image and receive the results
+ Directory imageConsumer: Java based imageConsumer to calculate the most common pixel colour per image row for the received image
+ Directory include: boost library (if needed)
+ Directory resources: images for tests
+ Makefile: the Makefile to build and run both peer applications at once
+ README.md this file



