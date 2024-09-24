/*
 * Logger.h
 *
 *  Created on: Mar 7, 2013
 *      Author: gby18020
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <errno.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

extern const char *DOMAIN;
static std::ofstream threadLog;
static std::string logFile;

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
#define MSG_FATAL    "FATAL-----" /* BDW 26/06/03 Agreed term for use with Tivoli error logging */
#define MSG_CRITICAL "CRITICAL--" /* BDW 26/06/03 Agreed term for use with Tivoli error logging */
#define MSG_TRACE    "TRACE-----"  /* For Debugging only */

#define TRUE                 1
#define FALSE                0
#define ON                   1
#define OFF                  0
#define LENGTH_MESSAGE     512

#if TRACE_ON
 #define TRACEMODE ON
#else
 #define TRACEMODE OFF
#endif

#define RETCDE_SUCCESS         0
#define RETCDE_SYSTEMERROR     1  /* problem with system, I/O etc */
#define RETCDE_MALLOC          2  /* could not allocated memory to variable when calling malloc */
#define RETCDE_FILEERR         3  /* problem with system writing/reading a file */
#define RETCDE_ARGERROR        4  /* problem with arguments */
#define RETCDE_IOERROR         5  /* error with scanf or printf */
#define RETCDE_SUCCEXIT        6  /* exit with out error (-h)*/
#define RETCDE_ERROR           7  /* standard error */
#define RETCDE_ORAERR          8  /* oracle error */

class writeLogMessage
{
private:
	const char *filename;
	int line;
	const char *function;
	const char *level;
	std::ostream *outputStream;


public:

	writeLogMessage( const char *file, int line, const char *function, const char *level) :
			filename( file ),
			line( line ),
			function ( function ),
			level ( level )
	{
	}

	void operator()( std::ostream *ostr, const char *szFormat, ... )
	{
		va_list ap;

		outputStream = ostr;
		va_start(ap, szFormat);
		handleLogMessage(szFormat,ap);
		va_end(ap);
	}

	void operator()( const char *szFormat, ... )
	{
		va_list ap;

		outputStream = &std::cout; // default to standard out
		va_start(ap, szFormat);
		handleLogMessage(szFormat,ap);
		va_end(ap);
	}

	void operator()( std::ostream *ostr, int level, const char *szFormat, ... )
	{
		va_list ap;
		char *env_trace_switch;
		char *env_trace_level;
		int trace_level = _OFF;

		if ( (env_trace_switch = getenv("TRACE_ALL")) != NULL )
		{
			if ( strcmp(env_trace_switch, "ON") == 0 )
				trace_level = _STANDARD;
		}

		if ( (env_trace_switch = getenv(DOMAIN)) != NULL )
		{
			if ( strcmp(env_trace_switch, "ON") == 0)
				trace_level = _STANDARD;
		}

		if ( trace_level != 0 )
		{
			if ( (env_trace_level = getenv("TRACE_LEVEL")) != NULL )
			{
				if (strcmp("BASIC", env_trace_level) == 0 )
					trace_level = _BASIC;
				else if (strcmp("STANDARD", env_trace_level) == 0 )
					trace_level = _STANDARD;
				else if (strcmp("DETAILED", env_trace_level) == 0 )
					trace_level = _DETAILED;
				else if (strcmp("FULL", env_trace_level) == 0 )
					trace_level = _FULL;
			}
			else
				trace_level = _FULL;
		}

		if ( trace_level != _OFF && level <= trace_level )
		{
			outputStream = ostr;
			va_start(ap, szFormat);
			handleLogMessage(szFormat,ap);
			va_end(ap);
		}
	}

	void operator()( int level, const char *szFormat, ... )
	{
		va_list ap;
		char *env_trace_switch;
		char *env_trace_level;
		int trace_level = _OFF;

		if ( (env_trace_switch = getenv("TRACE_ALL")) != NULL )
		{
			if ( strcmp(env_trace_switch, "ON") == 0 )
				trace_level = _STANDARD;
		}

		if ( (env_trace_switch = getenv(DOMAIN)) != NULL )
		{
			if ( strcmp(env_trace_switch, "ON") == 0)
				trace_level = _STANDARD;
		}

		if ( trace_level != 0 )
		{
			if ( (env_trace_level = getenv("TRACE_LEVEL")) != NULL )
			{
				if (strcmp("BASIC", env_trace_level) == 0 )
					trace_level = _BASIC;
				else if (strcmp("STANDARD", env_trace_level) == 0 )
					trace_level = _STANDARD;
				else if (strcmp("DETAILED", env_trace_level) == 0 )
					trace_level = _DETAILED;
				else if (strcmp("FULL", env_trace_level) == 0 )
					trace_level = _FULL;
			}
			else
				trace_level = _FULL;
		}

		if ( trace_level != _OFF && level <= trace_level )
		{
			outputStream = &std::cout; // default to standard out
			va_start(ap, szFormat);
			handleLogMessage(szFormat,ap);
			va_end(ap);
		}
	}

	/*void handleLogMessage(const char *szFormat, va_list ap)
	{
		char szMessage[ MAX_LOG_MSG_LEN ] = {'\0'};
		char date[ DATETIME_LEN ] = {'\0'};
		time_t now = time(NULL);

		strftime( date, DATETIME_LEN, "%d/%m/%y %T", localtime(&now) );
		vsnprintf( szMessage, MAX_LOG_MSG_LEN, szFormat, ap );
		(*outputStream) << level << filename << ":" << line << ":" << date << " > (" << function << ") " << szMessage << "\n";
		outputStream->flush();
	}*/

	void handleLogMessage(const char *szFormat, va_list ap)
	{
		char szMessage[ MAX_LOG_MSG_LEN ] = {'\0'};
		char date[ 80 ] = {'\0'};
		timeval curTime;
		char currentTime[84] = "";
		int milli = 0;
		tm newTime;

		gettimeofday(&curTime, NULL);
		milli = curTime.tv_usec / 1000;
		localtime_r(&curTime.tv_sec, &newTime);
		strftime(date, 80, "%d/%m/%y %T", &newTime);
		sprintf(currentTime, "%s.%03d", date, milli);
		vsnprintf( szMessage, MAX_LOG_MSG_LEN, szFormat, ap );
		(*outputStream) << level << filename << ":" << line << ":" << currentTime << " > (" << function << ") " << szMessage << "\n";
		outputStream->flush();
	}

};

#define gl_inform (writeLogMessage( __FILE__, __LINE__, __func__ , MSG_INFO))
#define gl_error (writeLogMessage( __FILE__, __LINE__, __func__ , MSG_ERROR))
#define gl_warn (writeLogMessage( __FILE__, __LINE__, __func__ , MSG_WARN))
#define gl_trace (writeLogMessage( __FILE__, __LINE__, __func__ , MSG_TRACE))
#define gl_fatal (writeLogMessage( __FILE__, __LINE__, __func__ , MSG_FATAL))

class switchLogFile
{

public:

	switchLogFile( const char *pcLogDir, const char *pcLogDate, const char *pcProcessId)
	{
		int iExitCode = 0;
		char errorTxt[512 + 1] = { '\0' };
		std::ostringstream oss;

		errno = 0;

		/* Set log file name */
		oss << pcLogDir << "/" << pcLogDate;

		// it these match there is no switch needed
		if ( logFile == oss.str() )
			return;

		logFile = oss.str();

		try {
			iExitCode = mkdir(logFile.c_str(), 0755);
			if (iExitCode != 0) {
				/* It's not an error if the directory already exists */
				if (errno != EEXIST) {
					strerror_r(errno, errorTxt, 512);
					gl_error("Unable to create logging directory <%s>, error : %s",
							logFile.c_str(), errorTxt);
					iExitCode = -1;
				}
				else
					iExitCode = 0;
			}

			if (iExitCode == 0) {
				oss.str("");
				oss << logFile << "/" << pcProcessId << ".LOG";
				//logFile = oss.str();

				if ( threadLog.is_open() )
					threadLog.close();

				threadLog.open(oss.str().c_str(),
						std::ofstream::out | std::ofstream::app);
				threadLog.exceptions(std::ofstream::failbit | std::ofstream::badbit);
			}
		} catch (std::ofstream::failure const &e) {
			gl_error("Error opening log file %s : %s", oss.str().c_str(), e.what());
			iExitCode = -1;
		}

		if (iExitCode == 0)
			std::cout.rdbuf(threadLog.rdbuf());
	}
};

#endif /* LOGGER_H_ */
