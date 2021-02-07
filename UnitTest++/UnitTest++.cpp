#include "UnitTest++.h"

#ifndef UNITTEST_NO_EXCEPTIONS

namespace UnitTest {

   AssertException::AssertException()
   {}

   AssertException::~AssertException() throw()
   {}

}

#endif
#include "Checks.h"
#include <cstring>

namespace UnitTest {

   namespace {

      void CheckStringsEqual(TestResults& results, char const* expected, char const* actual,
                             TestDetails const& details)
      {
         using namespace std;

         if ((expected && actual) ? strcmp(expected, actual) : (expected || actual))
         {
            UnitTest::MemoryOutStream stream;
            stream << "Expected " << (expected ? expected : "<NULLPTR>") << " but was " << (actual ? actual : "<NULLPTR>");

            results.OnTestFailure(details, stream.GetText());
         }
      }

   }


   void CheckEqual(TestResults& results, char const* expected, char const* actual,
                   TestDetails const& details)
   {
      CheckStringsEqual(results, expected, actual, details);
   }

   void CheckEqual(TestResults& results, char* expected, char* actual,
                   TestDetails const& details)
   {
      CheckStringsEqual(results, expected, actual, details);
   }

   void CheckEqual(TestResults& results, char* expected, char const* actual,
                   TestDetails const& details)
   {
      CheckStringsEqual(results, expected, actual, details);
   }

   void CheckEqual(TestResults& results, char const* expected, char* actual,
                   TestDetails const& details)
   {
      CheckStringsEqual(results, expected, actual, details);
   }


}
#include "CompositeTestReporter.h"
#include <cstddef>

namespace UnitTest {

   CompositeTestReporter::CompositeTestReporter()
      : m_reporterCount(0)
   {}

   int CompositeTestReporter::GetReporterCount() const
   {
      return m_reporterCount;
   }

   bool CompositeTestReporter::AddReporter(TestReporter* reporter)
   {
      if (m_reporterCount == kMaxReporters)
         return false;

      m_reporters[m_reporterCount++] = reporter;
      return true;
   }

   bool CompositeTestReporter::RemoveReporter(TestReporter* reporter)
   {
      for (int index = 0; index < m_reporterCount; ++index)
      {
         if (m_reporters[index] == reporter)
         {
            m_reporters[index] = m_reporters[m_reporterCount - 1];
            --m_reporterCount;
            return true;
         }
      }

      return false;
   }

   void CompositeTestReporter::ReportFailure(TestDetails const& details, char const* failure)
   {
      for (int index = 0; index < m_reporterCount; ++index)
         m_reporters[index]->ReportFailure(details, failure);
   }

   void CompositeTestReporter::ReportTestStart(TestDetails const& test)
   {
      for (int index = 0; index < m_reporterCount; ++index)
         m_reporters[index]->ReportTestStart(test);
   }

   void CompositeTestReporter::ReportTestFinish(TestDetails const& test, float secondsElapsed)
   {
      for (int index = 0; index < m_reporterCount; ++index)
         m_reporters[index]->ReportTestFinish(test, secondsElapsed);
   }

   void CompositeTestReporter::ReportSummary(int totalTestCount,
                                             int failedTestCount,
                                             int failureCount,
                                             float secondsElapsed)
   {
      for (int index = 0; index < m_reporterCount; ++index)
         m_reporters[index]->ReportSummary(totalTestCount, failedTestCount, failureCount, secondsElapsed);
   }

}
#include <cstddef>

namespace UnitTest {

   UNITTEST_LINKAGE TestResults*& CurrentTest::Results()
   {
      static TestResults* testResults = NULL;
      return testResults;
   }

   UNITTEST_LINKAGE const TestDetails*& CurrentTest::Details()
   {
      static const TestDetails* testDetails = NULL;
      return testDetails;
   }

}
#ifndef UNITTEST_NO_DEFERRED_REPORTER

#include "DeferredTestReporter.h"

using namespace UnitTest;

void DeferredTestReporter::ReportTestStart(TestDetails const& details)
{
   m_results.push_back(DeferredTestResult(details.suiteName, details.testName));
}

void DeferredTestReporter::ReportFailure(TestDetails const& details, char const* failure)
{
   DeferredTestResult& r = m_results.back();
   r.failed = true;
   r.failures.push_back(DeferredTestFailure(details.lineNumber, failure));
   r.failureFile = details.filename;
}

void DeferredTestReporter::ReportTestFinish(TestDetails const&, float secondsElapsed)
{
   DeferredTestResult& r = m_results.back();
   r.timeElapsed = secondsElapsed;
}

DeferredTestReporter::DeferredTestResultList& DeferredTestReporter::GetResults()
{
   return m_results;
}

#endif

#ifndef UNITTEST_NO_DEFERRED_REPORTER

#include "DeferredTestResult.h"
#include <cstring>

namespace UnitTest
{

   DeferredTestFailure::DeferredTestFailure()
      : lineNumber(-1)
   {
      failureStr[0] = '\0';
   }

   DeferredTestFailure::DeferredTestFailure(int lineNumber_, const char* failureStr_)
      : lineNumber(lineNumber_)
   {
      UNIITEST_NS_QUAL_STD(strcpy)(failureStr, failureStr_);
   }

   DeferredTestResult::DeferredTestResult()
      : suiteName("")
      , testName("")
      , failureFile("")
      , timeElapsed(0.0f)
      , failed(false)
   {}

   DeferredTestResult::DeferredTestResult(char const* suite, char const* test)
      : suiteName(suite)
      , testName(test)
      , failureFile("")
      , timeElapsed(0.0f)
      , failed(false)
   {}

   DeferredTestResult::~DeferredTestResult()
   {}

}

#endif

#ifdef UNITTEST_MEMORYOUTSTREAM_IS_STD_OSTRINGSTREAM

namespace UnitTest {

   char const* MemoryOutStream::GetText() const
   {
      m_text = this->str();
      return m_text.c_str();
   }

   void MemoryOutStream::Clear()
   {
      this->str(std::string());
      m_text = this->str();
   }

#ifdef UNITTEST_COMPILER_IS_MSVC6

   #define snprintf _snprintf

   template<typename ValueType>
   std::ostream& FormatToStream(std::ostream& stream, char const* format, ValueType const& value)
   {
      using namespace std;

      const size_t BUFFER_SIZE=32;
      char txt[BUFFER_SIZE];
      snprintf(txt, BUFFER_SIZE, format, value);
      return stream << txt;
   }

   std::ostream& operator<<(std::ostream& stream, __int64 const n)
   {
      return FormatToStream(stream, "%I64d", n);
   }

   std::ostream& operator<<(std::ostream& stream, unsigned __int64 const n)
   {
      return FormatToStream(stream, "%I64u", n);
   }

#endif

}

#else

#include <cstring>
#include <cstdio>

#if _MSC_VER
#define snprintf _snprintf
#endif

namespace UnitTest {

   namespace {

      template<typename ValueType>
      void FormatToStream(MemoryOutStream& stream, char const* format, ValueType const& value)
      {
         using namespace std;

         const size_t BUFFER_SIZE=32;
         char txt[BUFFER_SIZE];
         snprintf(txt, BUFFER_SIZE, format, value);
         stream << txt;
      }

      int RoundUpToMultipleOfPow2Number (int n, int pow2Number)
      {
         return (n + (pow2Number - 1)) & ~(pow2Number - 1);
      }

   }


   MemoryOutStream::MemoryOutStream(int const size)
      : m_capacity (0)
      , m_buffer (0)

   {
      GrowBuffer(size);
   }

   MemoryOutStream::~MemoryOutStream()
   {
      delete [] m_buffer;
   }

   void MemoryOutStream::Clear()
   {
      m_buffer[0] = '\0';
   }

   char const* MemoryOutStream::GetText() const
   {
      return m_buffer;
   }

   MemoryOutStream& MemoryOutStream::operator <<(char const* txt)
   {
      using namespace std;

      int const bytesLeft = m_capacity - (int)strlen(m_buffer);
      int const bytesRequired = (int)strlen(txt) + 1;

      if (bytesRequired > bytesLeft)
      {
         int const requiredCapacity = bytesRequired + m_capacity - bytesLeft;
         GrowBuffer(requiredCapacity);
      }

      strcat(m_buffer, txt);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(int const n)
   {
      FormatToStream(*this, "%i", n);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(long const n)
   {
      FormatToStream(*this, "%li", n);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(unsigned long const n)
   {
      FormatToStream(*this, "%lu", n);
      return *this;
   }

#ifdef UNITTEST_COMPILER_IS_MSVC6
   MemoryOutStream& MemoryOutStream::operator <<(__int64 const n)
#else
   MemoryOutStream& MemoryOutStream::operator <<(long long const n)
#endif
   {
#ifdef UNITTEST_WIN32
      FormatToStream(*this, "%I64d", n);
#else
      FormatToStream(*this, "%lld", n);
#endif

      return *this;
   }

#ifdef UNITTEST_COMPILER_IS_MSVC6
   MemoryOutStream& MemoryOutStream::operator <<(unsigned __int64 const n)
#else
   MemoryOutStream& MemoryOutStream::operator <<(unsigned long long const n)
#endif
   {
#ifdef UNITTEST_WIN32
      FormatToStream(*this, "%I64u", n);
#else
      FormatToStream(*this, "%llu", n);
#endif

      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(float const f)
   {
      FormatToStream(*this, "%0.6f", f);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(void const* p)
   {
      FormatToStream(*this, "%p", p);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(unsigned int const s)
   {
      FormatToStream(*this, "%u", s);
      return *this;
   }

   MemoryOutStream& MemoryOutStream::operator <<(double const d)
   {
      FormatToStream(*this, "%0.6f", d);
      return *this;
   }

   int MemoryOutStream::GetCapacity() const
   {
      return m_capacity;
   }


   void MemoryOutStream::GrowBuffer(int const desiredCapacity)
   {
      int const newCapacity = RoundUpToMultipleOfPow2Number(desiredCapacity, GROW_CHUNK_SIZE);

      using namespace std;

      char* buffer = new char[newCapacity];
      if (m_buffer)
         strcpy(buffer, m_buffer);
      else
         strcpy(buffer, "");

      delete [] m_buffer;
      m_buffer = buffer;
      m_capacity = newCapacity;
   }

}


#endif
#include "ReportAssert.h"
#include "ReportAssertImpl.h"

#ifdef UNITTEST_NO_EXCEPTIONS
   #include "ReportAssertImpl.h"
#endif

namespace UnitTest {

   namespace
   {
      bool& AssertExpectedFlag()
      {
         static bool s_assertExpected = false;
         return s_assertExpected;
      }
   }

   UNITTEST_LINKAGE void ReportAssert(char const* description, char const* filename, int lineNumber)
   {
      Detail::ReportAssertEx(CurrentTest::Results(), CurrentTest::Details(),
                             description, filename, lineNumber);
   }

   namespace Detail {

#ifdef UNITTEST_NO_EXCEPTIONS
      UNITTEST_JMPBUF* GetAssertJmpBuf()
      {
         static UNITTEST_JMPBUF s_jmpBuf;
         return &s_jmpBuf;
      }
#endif

      UNITTEST_LINKAGE void ReportAssertEx(TestResults* testResults,
                                           const TestDetails* testDetails,
                                           char const* description,
                                           char const* filename,
                                           int lineNumber)
      {
         if (AssertExpectedFlag() == false)
         {
            TestDetails assertDetails(testDetails->testName, testDetails->suiteName, filename, lineNumber);
            testResults->OnTestFailure(assertDetails, description);
         }

         ExpectAssert(false);

#ifndef UNITTEST_NO_EXCEPTIONS
         throw AssertException();
#else
         UNITTEST_JUMP_TO_ASSERT_JUMP_TARGET();
#endif
      }

      UNITTEST_LINKAGE void ExpectAssert(bool expected)
      {
         AssertExpectedFlag() = expected;
      }

      UNITTEST_LINKAGE bool AssertExpected()
      {
         return AssertExpectedFlag();
      }

   }
}

#ifndef UNITTEST_NO_EXCEPTIONS

namespace UnitTest {

   RequiredCheckException::RequiredCheckException()
   {
   }

   RequiredCheckException::~RequiredCheckException() throw()
   {
   }

}

#endif
#include "RequiredCheckTestReporter.h"


namespace UnitTest {

   RequiredCheckTestReporter::RequiredCheckTestReporter(TestResults& results)
      : m_results(results)
      , m_originalTestReporter(results.m_testReporter)
      , m_throwingReporter(results.m_testReporter)
      , m_continue(0)
   {
      m_results.m_testReporter = &m_throwingReporter;
   }

   RequiredCheckTestReporter::~RequiredCheckTestReporter()
   {
      m_results.m_testReporter = m_originalTestReporter;
   }

   bool RequiredCheckTestReporter::Next()
   {
      return m_continue++ == 0;
   }
}

#include "Test.h"
#include "TestList.h"
#include "ExecuteTest.h"

#ifdef UNITTEST_POSIX
#include "Posix/SignalTranslator.h"
#endif

namespace UnitTest {

   TestList& Test::GetTestList()
   {
      static TestList s_list;
      return s_list;
   }

   Test::Test(char const* testName, char const* suiteName, char const* filename, int lineNumber)
      : m_details(testName, suiteName, filename, lineNumber)
      , m_nextTest(0)
      , m_isMockTest(false)
   {}

   Test::~Test()
   {}

   void Test::Run()
   {
      ExecuteTest(*this, m_details, m_isMockTest);
   }

   void Test::RunImpl() const
   {}

}

namespace UnitTest {

   TestDetails::TestDetails(char const* testName_, char const* suiteName_, char const* filename_, int lineNumber_)
      : suiteName(suiteName_)
      , testName(testName_)
      , filename(filename_)
      , lineNumber(lineNumber_)
      , timeConstraintExempt(false)
   {}

   TestDetails::TestDetails(const TestDetails& details, int lineNumber_)
      : suiteName(details.suiteName)
      , testName(details.testName)
      , filename(details.filename)
      , lineNumber(lineNumber_)
      , timeConstraintExempt(details.timeConstraintExempt)
   {}


}
#include "TestList.h"
#include "Test.h"

#include <cassert>

namespace UnitTest {

   TestList::TestList()
      : m_head(0)
      , m_tail(0)
   {}

   void TestList::Add(Test* test)
   {
      if (m_tail == 0)
      {
         assert(m_head == 0);
         m_head = test;
         m_tail = test;
      }
      else
      {
         m_tail->m_nextTest = test;
         m_tail = test;
      }
   }

   Test* TestList::GetHead() const
   {
      return m_head;
   }

   ListAdder::ListAdder(TestList& list, Test* test)
   {
      list.Add(test);
   }

}
#include "TestReporter.h"

namespace UnitTest {

   TestReporter::~TestReporter()
   {}

}
#include "TestReporterStdout.h"
#include <cstdio>


// cstdio doesn't pull in namespace std on VC6, so we do it here.
#if defined(UNITTEST_WIN32) && (_MSC_VER == 1200)
namespace std {}
#endif

namespace UnitTest {

   void TestReporterStdout::ReportFailure(TestDetails const& details, char const* failure)
   {
      using namespace std;
#if defined(__APPLE__) || defined(__GNUG__)
      char const* const errorFormat = "%s:%d:%d: error: Failure in %s: %s\n";
      fprintf(stderr, errorFormat, details.filename, details.lineNumber, 1, details.testName, failure);
#else
      char const* const errorFormat = "%s(%d): error: Failure in %s: %s\n";
      fprintf(stderr, errorFormat, details.filename, details.lineNumber, details.testName, failure);
#endif
   }

   void TestReporterStdout::ReportTestStart(TestDetails const& /*test*/)
   {}

   void TestReporterStdout::ReportTestFinish(TestDetails const& /*test*/, float)
   {}

   void TestReporterStdout::ReportSummary(int const totalTestCount, int const failedTestCount,
                                          int const failureCount, float const secondsElapsed)
   {
      using namespace std;

      if (failureCount > 0)
         printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
      else
         printf("Success: %d tests passed.\n", totalTestCount);

      printf("Test time: %.2f seconds.\n", secondsElapsed);
   }

}
#include "TestReporter.h"


namespace UnitTest {

   TestResults::TestResults(TestReporter* testReporter)
      : m_testReporter(testReporter)
      , m_totalTestCount(0)
      , m_failedTestCount(0)
      , m_failureCount(0)
      , m_currentTestFailed(false)
   {}

   void TestResults::OnTestStart(TestDetails const& test)
   {
      ++m_totalTestCount;
      m_currentTestFailed = false;
      if (m_testReporter)
         m_testReporter->ReportTestStart(test);
   }

   void TestResults::OnTestFailure(TestDetails const& test, char const* failure)
   {
      ++m_failureCount;
      if (!m_currentTestFailed)
      {
         ++m_failedTestCount;
         m_currentTestFailed = true;
      }

      if (m_testReporter)
         m_testReporter->ReportFailure(test, failure);
   }

   void TestResults::OnTestFinish(TestDetails const& test, float secondsElapsed)
   {
      if (m_testReporter)
         m_testReporter->ReportTestFinish(test, secondsElapsed);
   }

   int TestResults::GetTotalTestCount() const
   {
      return m_totalTestCount;
   }

   int TestResults::GetFailedTestCount() const
   {
      return m_failedTestCount;
   }

   int TestResults::GetFailureCount() const
   {
      return m_failureCount;
   }


}
#include "TestRunner.h"
#include "TestReporter.h"
#include "TestReporterStdout.h"
#include "TimeHelpers.h"

#include <cstring>


namespace UnitTest {

   int RunAllTests()
   {
      TestReporterStdout reporter;
      TestRunner runner(reporter);
      return runner.RunTestsIf(Test::GetTestList(), NULL, True(), 0);
   }


   TestRunner::TestRunner(TestReporter& reporter)
      : m_reporter(&reporter)
      , m_result(new TestResults(&reporter))
      , m_timer(new Timer)
   {
      m_timer->Start();
   }

   TestRunner::~TestRunner()
   {
      delete m_result;
      delete m_timer;
   }

   TestResults* TestRunner::GetTestResults()
   {
      return m_result;
   }

   int TestRunner::Finish() const
   {
      float const secondsElapsed = static_cast<float>(m_timer->GetTimeInMs() / 1000.0);
      m_reporter->ReportSummary(m_result->GetTotalTestCount(),
                                m_result->GetFailedTestCount(),
                                m_result->GetFailureCount(),
                                secondsElapsed);

      return m_result->GetFailureCount();
   }

   bool TestRunner::IsTestInSuite(const Test* const curTest, char const* suiteName) const
   {
      using namespace std;
      return (suiteName == NULL) || !strcmp(curTest->m_details.suiteName, suiteName);
   }

   void TestRunner::RunTest(TestResults* const result, Test* const curTest, int const maxTestTimeInMs) const
   {
      if (curTest->m_isMockTest == false)
         CurrentTest::Results() = result;

      Timer testTimer;
      testTimer.Start();

      result->OnTestStart(curTest->m_details);

      curTest->Run();

      double const testTimeInMs = testTimer.GetTimeInMs();
      if (maxTestTimeInMs > 0 && testTimeInMs > maxTestTimeInMs && !curTest->m_details.timeConstraintExempt)
      {
         MemoryOutStream stream;
         stream << "Global time constraint failed. Expected under " << maxTestTimeInMs <<
            "ms but took " << testTimeInMs << "ms.";

         result->OnTestFailure(curTest->m_details, stream.GetText());
      }

      result->OnTestFinish(curTest->m_details, static_cast< float >(testTimeInMs / 1000.0));
   }

}
#include "ThrowingTestReporter.h"

#ifdef UNITTEST_NO_EXCEPTIONS
#include "ReportAssertImpl.h"
#endif

namespace UnitTest {

   ThrowingTestReporter::ThrowingTestReporter(TestReporter* decoratedReporter)
      : m_decoratedReporter(decoratedReporter)
   {}

   //virtual
   ThrowingTestReporter::~ThrowingTestReporter()
   {}

   //virtual
   void ThrowingTestReporter::ReportTestStart(TestDetails const& test)
   {
      if(m_decoratedReporter)
      {
         m_decoratedReporter->ReportTestStart(test);
      }
   }

   //virtual
   void ThrowingTestReporter::ReportFailure(TestDetails const& test, char const* failure)
   {
      if(m_decoratedReporter)
      {
         m_decoratedReporter->ReportFailure(test, failure);
      }

      #ifndef UNITTEST_NO_EXCEPTIONS
         throw RequiredCheckException();
      #else
         static const int stopTest = 1;
         UNITTEST_LONGJMP(*UnitTest::Detail::GetAssertJmpBuf(), stopTest);
      #endif
   }

   //virtual
   void ThrowingTestReporter::ReportTestFinish(TestDetails const& test, float secondsElapsed)
   {
      if(m_decoratedReporter)
      {
         m_decoratedReporter->ReportTestFinish(test, secondsElapsed);
      }
   }

   //virtual
   void ThrowingTestReporter::ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
   {
      if(m_decoratedReporter)
      {
         m_decoratedReporter->ReportSummary(totalTestCount, failedTestCount, failureCount, secondsElapsed);
      }
   }

}
#include "TimeConstraint.h"

namespace UnitTest {


   TimeConstraint::TimeConstraint(int ms, TestDetails const& details, int lineNumber)
      : m_details(details, lineNumber)
      , m_maxMs(ms)
   {
      m_timer.Start();
   }

   TimeConstraint::~TimeConstraint()
   {
      double const totalTimeInMs = m_timer.GetTimeInMs();
      if (totalTimeInMs > m_maxMs)
      {
         MemoryOutStream stream;
         stream << "Time constraint failed. Expected to run test under " << m_maxMs <<
            "ms but took " << totalTimeInMs << "ms.";

         CurrentTest::Results()->OnTestFailure(m_details, stream.GetText());
      }
   }

}
#ifndef UNITTEST_NO_DEFERRED_REPORTER

#include "XmlTestReporter.h"

#include <iostream>
#include <sstream>
#include <string>

using std::string;
using std::ostringstream;
using std::ostream;

namespace {

   void ReplaceChar(string& str, char c, string const& replacement)
   {
      for (size_t pos = str.find(c); pos != string::npos; pos = str.find(c, pos + 1))
         str.replace(pos, 1, replacement);
   }

   string XmlEscape(string const& value)
   {
      string escaped = value;

      ReplaceChar(escaped, '&', "&amp;");
      ReplaceChar(escaped, '<', "&lt;");
      ReplaceChar(escaped, '>', "&gt;");
      ReplaceChar(escaped, '\'', "&apos;");
      ReplaceChar(escaped, '\"', "&quot;");

      return escaped;
   }

   string BuildFailureMessage(string const& file, int line, string const& message)
   {
      ostringstream failureMessage;
      failureMessage << file << "(" << line << ") : " << message;
      return failureMessage.str();
   }

}

namespace UnitTest {

   XmlTestReporter::XmlTestReporter(ostream& ostream)
      : m_ostream(ostream)
   {}

   void XmlTestReporter::ReportSummary(int totalTestCount, int failedTestCount,
                                       int failureCount, float secondsElapsed)
   {
      AddXmlElement(m_ostream, NULL);

      BeginResults(m_ostream, totalTestCount, failedTestCount, failureCount, secondsElapsed);

      DeferredTestResultList const& results = GetResults();
      for (DeferredTestResultList::const_iterator i = results.begin(); i != results.end(); ++i)
      {
         BeginTest(m_ostream, *i);

         if (i->failed)
            AddFailure(m_ostream, *i);

         EndTest(m_ostream, *i);
      }

      EndResults(m_ostream);
   }

   void XmlTestReporter::AddXmlElement(ostream& os, char const* encoding)
   {
      os << "<?xml version=\"1.0\"";

      if (encoding != NULL)
         os << " encoding=\"" << encoding << "\"";

      os << "?>";
   }

   void XmlTestReporter::BeginResults(std::ostream& os, int totalTestCount, int failedTestCount,
                                      int failureCount, float secondsElapsed)
   {
      os << "<unittest-results"
         << " tests=\"" << totalTestCount << "\""
         << " failedtests=\"" << failedTestCount << "\""
         << " failures=\"" << failureCount << "\""
         << " time=\"" << secondsElapsed << "\""
         << ">";
   }

   void XmlTestReporter::EndResults(std::ostream& os)
   {
      os << "</unittest-results>";
   }

   void XmlTestReporter::BeginTest(std::ostream& os, DeferredTestResult const& result)
   {
      os << "<test"
         << " suite=\"" << result.suiteName << "\""
         << " name=\"" << result.testName << "\""
         << " time=\"" << result.timeElapsed << "\"";
   }

   void XmlTestReporter::EndTest(std::ostream& os, DeferredTestResult const& result)
   {
      if (result.failed)
         os << "</test>";
      else
         os << "/>";
   }

   void XmlTestReporter::AddFailure(std::ostream& os, DeferredTestResult const& result)
   {
      os << ">"; // close <test> element

      for (DeferredTestResult::FailureVec::const_iterator it = result.failures.begin();
           it != result.failures.end();
           ++it)
      {
         string const escapedMessage = XmlEscape(std::string(it->failureStr));
         string const message = BuildFailureMessage(result.failureFile, it->lineNumber, escapedMessage);

         os << "<failure" << " message=\"" << message << "\"" << "/>";
      }
   }

}

#endif

#ifdef UNITTEST_POSIX
#include "Posix/SignalTranslator.h"

namespace UnitTest {

   sigjmp_buf* SignalTranslator::s_jumpTarget = 0;

   namespace {

      void SignalHandler(int sig)
      {
         siglongjmp(*SignalTranslator::s_jumpTarget, sig );
      }

   }


   SignalTranslator::SignalTranslator()
   {
      m_oldJumpTarget = s_jumpTarget;
      s_jumpTarget = &m_currentJumpTarget;

      struct sigaction action;
      action.sa_flags = 0;
      action.sa_handler = SignalHandler;
      sigemptyset( &action.sa_mask );

      sigaction( SIGSEGV, &action, &m_old_SIGSEGV_action );
      sigaction( SIGFPE, &action, &m_old_SIGFPE_action  );
      sigaction( SIGTRAP, &action, &m_old_SIGTRAP_action );
      sigaction( SIGBUS, &action, &m_old_SIGBUS_action  );
      sigaction( SIGILL, &action, &m_old_SIGILL_action  );
   }

   SignalTranslator::~SignalTranslator()
   {
      sigaction( SIGILL, &m_old_SIGILL_action, 0 );
      sigaction( SIGBUS, &m_old_SIGBUS_action, 0 );
      sigaction( SIGTRAP, &m_old_SIGTRAP_action, 0 );
      sigaction( SIGFPE, &m_old_SIGFPE_action, 0 );
      sigaction( SIGSEGV, &m_old_SIGSEGV_action, 0 );

      s_jumpTarget = m_oldJumpTarget;
   }
}

#include "Posix/TimeHelpers.h"
#include <unistd.h>

namespace UnitTest {

   Timer::Timer()
   {
      m_startTime.tv_sec = 0;
      m_startTime.tv_usec = 0;
   }

   void Timer::Start()
   {
      gettimeofday(&m_startTime, 0);
   }

   double Timer::GetTimeInMs() const
   {
      struct timeval currentTime;
      gettimeofday(&currentTime, 0);

      double const dsecs = currentTime.tv_sec - m_startTime.tv_sec;
      double const dus = currentTime.tv_usec - m_startTime.tv_usec;

      return (dsecs * 1000.0) + (dus / 1000.0);
   }

   void TimeHelpers::SleepMs(int ms)
   {
      usleep(static_cast<useconds_t>(ms * 1000));
   }

}
#endif

#ifdef UNITTEST_WIN32

#include "Win32/TimeHelpers.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace UnitTest {

   Timer::Timer()
      : m_threadHandle(::GetCurrentThread())
      , m_startTime(0)
   {
#if defined(UNITTEST_WIN32) && (_MSC_VER == 1200) // VC6 doesn't have DWORD_PTR
      typedef unsigned long DWORD_PTR;
#endif

      DWORD_PTR systemMask;
      ::GetProcessAffinityMask(GetCurrentProcess(), &m_processAffinityMask, &systemMask);
      ::SetThreadAffinityMask(m_threadHandle, 1);
      ::QueryPerformanceFrequency(reinterpret_cast< LARGE_INTEGER* >(&m_frequency));
      ::SetThreadAffinityMask(m_threadHandle, m_processAffinityMask);
   }

   void Timer::Start()
   {
      m_startTime = GetTime();
   }

   double Timer::GetTimeInMs() const
   {
      __int64 const elapsedTime = GetTime() - m_startTime;
      double const seconds = double(elapsedTime) / double(m_frequency);
      return seconds * 1000.0;
   }

   __int64 Timer::GetTime() const
   {
      LARGE_INTEGER curTime;
      ::SetThreadAffinityMask(m_threadHandle, 1);
      ::QueryPerformanceCounter(&curTime);
      ::SetThreadAffinityMask(m_threadHandle, m_processAffinityMask);
      return curTime.QuadPart;
   }

   void TimeHelpers::SleepMs(int ms)
   {
      ::Sleep(ms);
   }

}

#endif
