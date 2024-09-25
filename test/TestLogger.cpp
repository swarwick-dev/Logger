#include <gtest/gtest.h>
#include <sstream>
#include "../inc/Logger.h"

const char *LOG_DOMAIN = "TEST";

char *CompileDate(void) {
	struct tm result;
	static char cDate[11];
	strptime(__DATE__, "%b %d %Y", &result);
	strftime(cDate, sizeof(cDate), "%d-%m-%Y", &result);
	return cDate;
}

TEST(LoggerTest, BasicFunctionality) {
	std::ostringstream oss;
	gl_inform(&oss, "Test Logger Starting ");
	EXPECT_NE(oss.str().find("INFORM----"), std::string::npos);
	EXPECT_NE(oss.str().find("Test Logger Starting"), std::string::npos);
}

TEST(LoggerTest, CompileDate) {
	std::ostringstream oss;
	gl_inform(&oss, "Test Logger Compiled on %s ", CompileDate());
	EXPECT_NE(oss.str().find("INFORM----"), std::string::npos);
	EXPECT_NE(oss.str().find("Test Logger Compiled on"), std::string::npos);
	EXPECT_NE(oss.str().find(CompileDate()), std::string::npos);
}

TEST(LoggerTest, ErrorLevel) {
	std::ostringstream oss;
	gl_error(&oss, "This is an error message");
	EXPECT_NE(oss.str().find("ERROR-----"), std::string::npos);
	EXPECT_NE(oss.str().find("This is an error message"), std::string::npos);
}

TEST(LoggerTest, WarningLevel) {
	std::ostringstream oss;
	gl_warn(&oss, "This is a warning message");
	EXPECT_NE(oss.str().find("WARN------"), std::string::npos);
	EXPECT_NE(oss.str().find("This is a warning message"), std::string::npos);
}

TEST(LoggerTest, TraceLevel) {
	std::ostringstream oss;
	gl_trace(&oss, "This is a trace message");
	EXPECT_NE(oss.str().find("TRACE-----"), std::string::npos);
	EXPECT_NE(oss.str().find("This is a trace message"), std::string::npos);
}

TEST(LoggerTest, FatalLevel) {
	std::ostringstream oss;
	gl_fatal(&oss, "This is a fatal message");
	EXPECT_NE(oss.str().find("FATAL-----"), std::string::npos);
	EXPECT_NE(oss.str().find("This is a fatal message"), std::string::npos);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	gl_inform("Test Logger Starting ");
	gl_inform("Test Logger Compiled on %s ", CompileDate());
	return RUN_ALL_TESTS();
}