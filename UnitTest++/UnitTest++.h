#ifndef UNITTEST_PLUS_PLUS_H
#define UNITTEST_PLUS_PLUS_H

// Standard defines documented here: http://predef.sourceforge.net

#if defined(_MSC_VER)
   #pragma warning(disable:4702)// unreachable code
   #pragma warning(disable:4722)// destructor never returns, potential memory leak

   #if (_MSC_VER == 1200)  // VC6
      #define UNITTEST_COMPILER_IS_MSVC6
      #pragma warning(disable:4786)
      #pragma warning(disable:4290)
   #endif

   #ifdef _USRDLL
      #define UNITTEST_WIN32_DLL
   #endif

   #define UNITTEST_WIN32
#endif

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(linux) || \
   defined(__APPLE__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD__) \
   || defined (__HAIKU__)
   #define UNITTEST_POSIX
#endif

#if defined(__MINGW32__)
   #define UNITTEST_MINGW
#endif


// By default, MemoryOutStream is implemented in terms of std::ostringstream.
// This is useful if you are using the CHECK macros on objects that have something like this defined:
// std::ostringstream& operator<<(std::ostringstream& s, const YourObject& value)
//
// On the other hand, it can be more expensive.
// Un-comment this line to use the custom MemoryOutStream (no deps on std::ostringstream).

// #define UNITTEST_USE_CUSTOM_STREAMS

// Developer note: This dual-macro setup is to preserve compatibility with UnitTest++ 1.4 users
// who may have used or defined UNITTEST_USE_CUSTOM_STREAMS outside of this configuration file, as
// well as Google Code HEAD users that may have used or defined
// UNITTEST_MEMORYOUTSTREAM_IS_STD_OSTRINGSTREAM outside of this configuration file.
#ifndef UNITTEST_USE_CUSTOM_STREAMS
   #define UNITTEST_MEMORYOUTSTREAM_IS_STD_OSTRINGSTREAM
#endif

// DeferredTestReporter uses the STL to collect test results for subsequent export by reporters like
// XmlTestReporter.  If you don't want to use this functionality, uncomment this line and no STL
// headers or code will be compiled into UnitTest++

//#define UNITTEST_NO_DEFERRED_REPORTER


// By default, asserts that you report via UnitTest::ReportAssert() abort the current test and
// continue to the next one by throwing an exception, which unwinds the stack naturally, destroying
// all auto variables on its way back down.  If you don't want to (or can't) use exceptions for your
// platform/compiler, uncomment this line.  All exception code will be removed from UnitTest++,
// assert recovery will be done via setjmp/longjmp, and NO correct stack unwinding will happen!

//#define UNITTEST_NO_EXCEPTIONS


// std namespace qualification: used for functions like strcpy that
// may live in std:: namespace (cstring header).
#if defined( UNITTEST_COMPILER_IS_MSVC6 )
   #define UNIITEST_NS_QUAL_STD(x) x
#else
   #define UNIITEST_NS_QUAL_STD(x) ::std::x
#endif

// By default, UnitTest++ will attempt to define "short" macro names like CHECK and  CHECK_EQUAL
// for "public" interface macros etc. Defining UNITTEST_DISABLE_SHORT_MACROS in your project
// will disable this behavior, leaving only the longer macros "namespaced" with the UNITTEST_
// prefix.
//
// "Internal" utility macros will only have the UNITTEST_IMPL_ prefix.

// #define UNITTEST_DISABLE_SHORT_MACROS

#ifndef UNITTEST_NO_EXCEPTIONS
   #define UNITTEST_IMPL_TRY(x) try x
   #define UNITTEST_IMPL_THROW(x) throw x
   #define UNITTEST_IMPL_RETHROW(ExceptionType) catch(ExceptionType&) { throw; }
   #define UNITTEST_IMPL_CATCH(ExceptionType, ExceptionName, CatchBody) catch(ExceptionType& ExceptionName) CatchBody
   #define UNITTEST_IMPL_CATCH_ALL(CatchBody) catch(...) CatchBody
#else
   #define UNITTEST_IMPL_TRY(x) x
   #define UNITTEST_IMPL_THROW(x)
   #define UNITTEST_IMPL_RETHROW(ExceptionType)
   #define UNITTEST_IMPL_CATCH(ExceptionType, ExceptionName, CatchBody)
   #define UNITTEST_IMPL_CATCH_ALL(CatchBody)
#endif

// TODO: THIS SHOULD BE IN THE STANDARD UTPP NAMESPACE
namespace UnitTestSuite
{
   inline char const* GetSuiteName ()
   {
      return "DefaultSuite";
   }
}

#define UNITTEST_MULTILINE_MACRO_BEGIN do {

#if defined(UNITTEST_WIN32) && !defined(UNITTEST_COMPILER_IS_MSVC6)
   #define UNITTEST_MULTILINE_MACRO_END \
      } __pragma(warning(push)) __pragma(warning(disable: 4127)) while (0) __pragma(warning(pop))
#else
   #define UNITTEST_MULTILINE_MACRO_END } while(0)
#endif


#ifdef UNITTEST_WIN32_DLL
   #define UNITTEST_IMPORT __declspec(dllimport)
   #define UNITTEST_EXPORT __declspec(dllexport)

   #ifdef UNITTEST_DLL_EXPORT
      #define UNITTEST_LINKAGE UNITTEST_EXPORT
      #define UNITTEST_IMPEXP_TEMPLATE
   #else
      #define UNITTEST_LINKAGE UNITTEST_IMPORT
      #define UNITTEST_IMPEXP_TEMPLATE extern
   #endif

   #define UNITTEST_STDVECTOR_LINKAGE(T)                                            \
      __pragma(warning(push))                                                       \
      __pragma(warning(disable: 4231))                                              \
      UNITTEST_IMPEXP_TEMPLATE template class UNITTEST_LINKAGE std::allocator< T >; \
      UNITTEST_IMPEXP_TEMPLATE template class UNITTEST_LINKAGE std::vector< T >;    \
      __pragma(warning(pop))
#else
   #define UNITTEST_IMPORT
   #define UNITTEST_EXPORT
   #define UNITTEST_LINKAGE
   #define UNITTEST_IMPEXP_TEMPLATE
   #define UNITTEST_STDVECTOR_LINKAGE(T)
#endif

#ifdef UNITTEST_WIN32
   #define UNITTEST_JMPBUF jmp_buf
   #define UNITTEST_SETJMP setjmp
   #define UNITTEST_LONGJMP longjmp
#elif defined UNITTEST_POSIX
   #define UNITTEST_JMPBUF std::jmp_buf
   #define UNITTEST_SETJMP setjmp
   #define UNITTEST_LONGJMP std::longjmp
#endif

namespace UnitTest {

   class UNITTEST_LINKAGE TestDetails
   {
   public:
      TestDetails(char const* testName, char const* suiteName, char const* filename, int lineNumber);
      TestDetails(const TestDetails& details, int lineNumber);

      char const* const suiteName;
      char const* const testName;
      char const* const filename;
      int const lineNumber;
      mutable bool timeConstraintExempt;

      TestDetails(TestDetails const&); // Why is it public? --> http://gcc.gnu.org/bugs.html#cxx_rvalbind
   private:
      TestDetails& operator=(TestDetails const&);
   };

}

namespace UnitTest {

   class RequiredCheckTestReporter;
   class TestReporter;
   class TestDetails;

   class UNITTEST_LINKAGE TestResults
   {
   public:
      explicit TestResults(TestReporter* reporter = 0);

      void OnTestStart(TestDetails const& test);
      void OnTestFailure(TestDetails const& test, char const* failure);
      void OnTestFinish(TestDetails const& test, float secondsElapsed);

      int GetTotalTestCount() const;
      int GetFailedTestCount() const;
      int GetFailureCount() const;

   private:
      friend class RequiredCheckTestReporter;

      TestReporter* m_testReporter;
      int m_totalTestCount;
      int m_failedTestCount;
      int m_failureCount;

      bool m_currentTestFailed;

      TestResults(TestResults const&);
      TestResults& operator =(TestResults const&);
   };

}

#ifdef UNITTEST_MEMORYOUTSTREAM_IS_STD_OSTRINGSTREAM

#include <sstream>

namespace UnitTest
{

   class UNITTEST_LINKAGE MemoryOutStream : public std::ostringstream
   {
   public:
      MemoryOutStream() {}
      ~MemoryOutStream() {}
      void Clear();
      char const* GetText() const;

   private:
      MemoryOutStream(MemoryOutStream const&);
      void operator =(MemoryOutStream const&);

      mutable std::string m_text;
   };

#ifdef UNITTEST_COMPILER_IS_MSVC6
   std::ostream& operator<<(std::ostream& stream, __int64 const n);
   std::ostream& operator<<(std::ostream& stream, unsigned __int64 const n);
#endif

}

#else

#include <cstddef>

#ifdef UNITTEST_COMPILER_IS_MSVC6
namespace std {}
#endif

namespace UnitTest
{

   class UNITTEST_LINKAGE MemoryOutStream
   {
   public:
      explicit MemoryOutStream(int const size = 256);
      ~MemoryOutStream();

      void Clear();
      char const* GetText() const;

      MemoryOutStream& operator <<(char const* txt);
      MemoryOutStream& operator <<(int n);
      MemoryOutStream& operator <<(long n);
      MemoryOutStream& operator <<(unsigned long n);
#ifdef UNITTEST_COMPILER_IS_MSVC6
      MemoryOutStream& operator <<(__int64 n);
      MemoryOutStream& operator <<(unsigned __int64 n);
#else
      MemoryOutStream& operator <<(long long n);
      MemoryOutStream& operator <<(unsigned long long n);
#endif
      MemoryOutStream& operator <<(float f);
      MemoryOutStream& operator <<(double d);
      MemoryOutStream& operator <<(void const* p);
      MemoryOutStream& operator <<(unsigned int s);

      enum { GROW_CHUNK_SIZE = 32 };
      int GetCapacity() const;

   private:
      void operator= (MemoryOutStream const&);
      void GrowBuffer(int capacity);

      int m_capacity;
      char* m_buffer;
   };

}

#endif

#endif

#include "UnitTestPP.h"
