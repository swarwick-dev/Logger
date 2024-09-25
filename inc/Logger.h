#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ctime>
#include <memory>

#define _BASIC    1
#define _STANDARD 2
#define _DETAILED 4
#define _FULL     8
#define _OFF      0

#define MAX_LOG_MSG_LEN 4096
#define DATETIME_LEN 19+1

#define MSG_ERROR    "ERROR-----"
#define MSG_INFO     "INFORM----"
#define MSG_WARN     "WARN------"
#define MSG_FATAL    "FATAL-----"
#define MSG_CRITICAL "CRITICAL--"
#define MSG_TRACE    "TRACE-----"

#define TRUE  1
#define FALSE 0
#define ON    1
#define OFF   0

#define LENGTH_MESSAGE 512

#define gl_inform (writeLogMessage(__FILE__, __LINE__, __func__, MSG_INFO))
#define gl_error (writeLogMessage(__FILE__, __LINE__, __func__, MSG_ERROR))
#define gl_warn (writeLogMessage(__FILE__, __LINE__, __func__, MSG_WARN))
#define gl_trace (writeLogMessage(__FILE__, __LINE__, __func__, MSG_TRACE))
#define gl_fatal (writeLogMessage(__FILE__, __LINE__, __func__, MSG_FATAL))

class writeLogMessage {
private:
    const char *filename;
    int line;
    const char *function;
    const char *level;
    mutable std::ostream *outputStream;

public:
    writeLogMessage(const char *file, int line, const char *function, const char *level)
        : filename(file), line(line), function(function), level(level), outputStream(&std::cout) {}

    void operator()(std::ostream *ostr, const char *szFormat, ...) const {
        va_list ap;
        outputStream = ostr;
        va_start(ap, szFormat);
        handleLogMessage(szFormat, ap);
        va_end(ap);
    }

    void operator()(const char *szFormat, ...) const {
        va_list ap;
        va_start(ap, szFormat);
        handleLogMessage(szFormat, ap);
        va_end(ap);
    }

    void operator()(std::ostream *ostr, int level, const char *szFormat, ...) const {
        if (shouldLog(level)) {
            va_list ap;
            outputStream = ostr;
            va_start(ap, szFormat);
            handleLogMessage(szFormat, ap);
            va_end(ap);
        }
    }

    void operator()(int level, const char *szFormat, ...) const {
        if (shouldLog(level)) {
            va_list ap;
            va_start(ap, szFormat);
            handleLogMessage(szFormat, ap);
            va_end(ap);
        }
    }

private:
    bool shouldLog(int level) const {
        int trace_level = _OFF;
		char *env_trace_level = getenv("TRACE_LEVEL");
		
		if (env_trace_level) {
			if (strcmp("BASIC", env_trace_level) == 0) trace_level = _BASIC;
			else if (strcmp("STANDARD", env_trace_level) == 0) trace_level = _STANDARD;
			else if (strcmp("DETAILED", env_trace_level) == 0) trace_level = _DETAILED;
			else if (strcmp("FULL", env_trace_level) == 0) trace_level = _FULL;
		} else {
			trace_level = _FULL;
		}

        return trace_level != _OFF && level <= trace_level;
    }

    void handleLogMessage(const char *szFormat, va_list ap) const {
        char szMessage[MAX_LOG_MSG_LEN] = {'\0'};
        char date[80] = {'\0'};
        timeval curTime;
        char currentTime[84] = "";
        int milli = 0;
        tm newTime;

        gettimeofday(&curTime, NULL);
        milli = curTime.tv_usec / 1000;
        localtime_r(&curTime.tv_sec, &newTime);
        strftime(date, 80, "%d/%m/%y %T", &newTime);
        sprintf(currentTime, "%s.%03d", date, milli);
        vsnprintf(szMessage, MAX_LOG_MSG_LEN, szFormat, ap);
        (*outputStream) << level << filename << ":" << line << ":" << currentTime << " > (" << function << ") " << szMessage << "\n";
        outputStream->flush();
    }
};


class switchLogFile {
private:
    std::ofstream threadLog;
    std::string logFile;

public:
    switchLogFile(const char *pcLogDir, const char *pcLogDate, const char *pcProcessId) {
        std::ostringstream oss;
        oss << pcLogDir << "/" << pcLogDate;

        if (logFile == oss.str()) return;

        logFile = oss.str();
        createLogDirectory();
        openLogFile(pcProcessId);
    }

private:
    void createLogDirectory() {
        int iExitCode = mkdir(logFile.c_str(), 0755);
        if (iExitCode != 0 && errno != EEXIST) {
            char errorTxt[512 + 1] = {'\0'};
            strerror_r(errno, errorTxt, 512);
            gl_error("Unable to create logging directory <%s>, error : %s", logFile.c_str(), errorTxt);
            throw std::runtime_error("Failed to create log directory");
        }
    }

    void openLogFile(const char *pcProcessId) {
        std::ostringstream oss;
        oss << logFile << "/" << pcProcessId << ".LOG";

        if (threadLog.is_open()) {
            threadLog.close();
        }

        threadLog.open(oss.str().c_str(), std::ofstream::out | std::ofstream::app);
        threadLog.exceptions(std::ofstream::failbit | std::ofstream::badbit);

        std::cout.rdbuf(threadLog.rdbuf());
    }
};