# Logger

## Overview
Logger is a C++ logging library designed to provide various logging levels and functionalities. It supports logging messages with different severity levels such as INFO, ERROR, WARN, TRACE, and FATAL. The library also includes functionality to switch log files based on the process ID and log date.

## Features
- Multiple logging levels: INFO, ERROR, WARN, TRACE, FATAL
- Customizable log messages with file name, line number, and function name
- Support for switching log files based on process ID and log date
- Modern C++ practices and RAII principles

## Requirements
- CMake 3.5 or higher
- C++11 or higher
- Google Test (gtest) for unit testing

## Installation
1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/Logger.git
    cd Logger
    ```

2. Create a build directory and navigate to it:
    ```sh
    mkdir build
    cd build
    ```

3. Configure the project using CMake:
    ```sh
    cmake ..
    ```

4. Build the project:
    ```sh
    make
    ```

## Usage
### Including Logger in Your Project
To use the Logger library in your project, include the `Logger.h` header file:
    ```cpp
    #include "Logger.h"
    ```

### Logging Messages

gl_inform("This is an informational message");
gl_error("This is an error message");
gl_warn("This is a warning message");
gl_trace("This is a trace message");
gl_fatal("This is a fatal message");

### Switching Log Files

switchLogFile logSwitcher("/path/to/log/dir", "log_date", "process_id");

## Running Tests
1. Build the Tests
    ```sh
    make TestLogger
    ```
2. Run the tests
    ```sh
    ./TestLogger
    ```

## Project Structure

Logger/
├── CMakeLists.txt
├── inc/
│   └── Logger.h
├── src/
│   └── Logger.cpp
├── test/
│   └── TestLogger.cpp
└── README.md
