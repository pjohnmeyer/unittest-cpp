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

#ifndef UNITTEST_NO_EXCEPTIONS

#include <exception>

namespace UnitTest {

   class UNITTEST_LINKAGE AssertException : public std::exception
   {
   public:
      AssertException();
      virtual ~AssertException() throw();
   };

}

namespace UnitTest {

   class UNITTEST_LINKAGE RequiredCheckException : public std::exception
   {
   public:
      RequiredCheckException();
      virtual ~RequiredCheckException() throw();
   };

}

#endif

namespace UnitTest {

   class TestResults;
   class TestDetails;

   namespace CurrentTest
   {
      UNITTEST_LINKAGE TestResults*& Results();
      UNITTEST_LINKAGE const TestDetails*& Details();
   }

}

#ifdef UNITTEST_NO_EXCEPTIONS
   #include <csetjmp>
#endif

namespace UnitTest {

   class TestResults;
   class TestDetails;

   namespace Detail {

      UNITTEST_LINKAGE void ExpectAssert(bool expected);

      UNITTEST_LINKAGE void ReportAssertEx(TestResults* testResults,
                                           const TestDetails* testDetails,
                                           char const* description,
                                           char const* filename,
                                           int lineNumber);

      UNITTEST_LINKAGE bool AssertExpected();

#ifdef UNITTEST_NO_EXCEPTIONS
      UNITTEST_LINKAGE UNITTEST_JMPBUF* GetAssertJmpBuf();

#ifdef UNITTEST_WIN32
      #define UNITTEST_SET_ASSERT_JUMP_TARGET()                   \
         __pragma(warning(push)) __pragma(warning(disable: 4611)) \
         UNITTEST_SETJMP(*UnitTest::Detail::GetAssertJmpBuf())    \
         __pragma(warning(pop))
#else
      #define UNITTEST_SET_ASSERT_JUMP_TARGET() UNITTEST_SETJMP(*UnitTest::Detail::GetAssertJmpBuf())
#endif

      #define UNITTEST_JUMP_TO_ASSERT_JUMP_TARGET() UNITTEST_LONGJMP(*UnitTest::Detail::GetAssertJmpBuf(), 1)
#endif

   }
}

#ifdef UNITTEST_POSIX
#include <setjmp.h>
#include <signal.h>

namespace UnitTest {

   class SignalTranslator
   {
   public:
      SignalTranslator();
      ~SignalTranslator();

      static sigjmp_buf* s_jumpTarget;

   private:
      sigjmp_buf m_currentJumpTarget;
      sigjmp_buf* m_oldJumpTarget;

      struct sigaction m_old_SIGFPE_action;
      struct sigaction m_old_SIGTRAP_action;
      struct sigaction m_old_SIGSEGV_action;
      struct sigaction m_old_SIGBUS_action;
      struct sigaction m_old_SIGILL_action;
   };

#if !defined (__GNUC__)
   #define UNITTEST_EXTENSION
#else
   #define UNITTEST_EXTENSION __extension__
#endif

   #define UNITTEST_THROW_SIGNALS_POSIX_ONLY                                               \
      UnitTest::SignalTranslator sig;                                                      \
      if (UNITTEST_EXTENSION sigsetjmp(*UnitTest::SignalTranslator::s_jumpTarget, 1) != 0) \
         throw ("Unhandled system exception");

}
#endif

namespace UnitTest {

   template< typename T >
   void ExecuteTest(T& testObject, TestDetails const& details, bool isMockTest)
   {
      if (isMockTest == false)
         CurrentTest::Details() = &details;

#ifdef UNITTEST_NO_EXCEPTIONS
      if (UNITTEST_SET_ASSERT_JUMP_TARGET() == 0)
      {
#endif
#ifndef UNITTEST_POSIX
      UNITTEST_IMPL_TRY({ testObject.RunImpl(); })
#else
      UNITTEST_IMPL_TRY
         ({
         UNITTEST_THROW_SIGNALS_POSIX_ONLY
         testObject.RunImpl();
      })
#endif
      UNITTEST_IMPL_CATCH(RequiredCheckException, e, { (void)e; })
      UNITTEST_IMPL_CATCH(AssertException, e, { (void)e; })
      UNITTEST_IMPL_CATCH(std::exception, e,
      {
         MemoryOutStream stream;
         stream << "Unhandled exception: " << e.what();
         CurrentTest::Results()->OnTestFailure(details, stream.GetText());
      })
      UNITTEST_IMPL_CATCH_ALL
         ({
         CurrentTest::Results()->OnTestFailure(details, "Unhandled exception: test crashed");
      })
#ifdef UNITTEST_NO_EXCEPTIONS
   }
#endif
   }

}

#ifndef UNITTEST_POSIX
#define UNITTEST_THROW_SIGNALS_POSIX_ONLY
#else
#endif

#define UNITTEST_SUITE(Name)                  \
   namespace Suite ## Name {                  \
      namespace UnitTestSuite {               \
         inline char const* GetSuiteName () { \
            return #Name;                     \
         }                                    \
      }                                       \
   }                                          \
   namespace Suite ## Name

#define UNITTEST_IMPL_TEST(Name, List)                                                              \
   class Test ## Name : public UnitTest::Test                                            \
   {                                                                                     \
   public:                                                                               \
      Test ## Name() : Test(#Name, UnitTestSuite::GetSuiteName(), __FILE__, __LINE__) {} \
   private:                                                                              \
      virtual void RunImpl() const;                                                      \
   } test ## Name ## Instance;                                                           \
                                                                                         \
   UnitTest::ListAdder adder ## Name (List, &test ## Name ## Instance);                  \
                                                                                         \
   void Test ## Name::RunImpl() const


#define UNITTEST_TEST(Name) UNITTEST_IMPL_TEST(Name, UnitTest::Test::GetTestList())


#define UNITTEST_IMPL_TEST_FIXTURE(Fixture, Name, List)                                                                             \
   class Fixture ## Name ## Helper : public Fixture                                                                      \
   {                                                                                                                     \
   public:                                                                                                               \
      explicit Fixture ## Name ## Helper(UnitTest::TestDetails const& details) : m_details(details) {}                   \
      void RunImpl();                                                                                                    \
      UnitTest::TestDetails const& m_details;                                                                            \
   private:                                                                                                              \
      Fixture ## Name ## Helper(Fixture ## Name ## Helper const&);                                                       \
      Fixture ## Name ## Helper& operator =(Fixture ## Name ## Helper const&);                                           \
   };                                                                                                                    \
                                                                                                                         \
   class Test ## Fixture ## Name : public UnitTest::Test                                                                 \
   {                                                                                                                     \
   public:                                                                                                               \
      Test ## Fixture ## Name() : Test(#Name, UnitTestSuite::GetSuiteName(), __FILE__, __LINE__) {}                      \
   private:                                                                                                              \
      virtual void RunImpl() const;                                                                                      \
   } test ## Fixture ## Name ## Instance;                                                                                \
                                                                                                                         \
   UnitTest::ListAdder adder ## Fixture ## Name (List, &test ## Fixture ## Name ## Instance);                            \
                                                                                                                         \
   void Test ## Fixture ## Name::RunImpl() const                                                                         \
   {                                                                                                                     \
      volatile bool ctorOk = false;                                                                                      \
      UNITTEST_IMPL_TRY                                                                                                             \
         ({                                                                                                              \
         Fixture ## Name ## Helper fixtureHelper(m_details);                                                             \
         ctorOk = ctorOk || true; /* || prevents unused var warning in no except builds */                               \
         UnitTest::ExecuteTest(fixtureHelper, m_details, false);                                                         \
      })                                                                                                                 \
      UNITTEST_IMPL_CATCH (UnitTest::AssertException, e,                                                                            \
      {                                                                                                                  \
         (void)e;                                                                                                        \
      })                                                                                                                 \
      UNITTEST_IMPL_CATCH (std::exception, e,                                                                                       \
      {                                                                                                                  \
         UnitTest::MemoryOutStream stream;                                                                               \
         stream << "Unhandled exception: " << e.what();                                                                  \
         UnitTest::CurrentTest::Results()->OnTestFailure(m_details, stream.GetText());                                   \
      })                                                                                                                 \
      UNITTEST_IMPL_CATCH_ALL                                                                                                       \
         ({                                                                                                              \
         if (ctorOk)                                                                                                     \
         {                                                                                                               \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(m_details, __LINE__),                  \
                                                            "Unhandled exception while destroying fixture " #Fixture);   \
         }                                                                                                               \
         else                                                                                                            \
         {                                                                                                               \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(m_details, __LINE__),                  \
                                                            "Unhandled exception while constructing fixture " #Fixture); \
         }                                                                                                               \
      })                                                                                                                 \
   }                                                                                                                     \
   void Fixture ## Name ## Helper::RunImpl()

#define UNITTEST_TEST_FIXTURE(Fixture,Name) UNITTEST_IMPL_TEST_FIXTURE(Fixture, Name, UnitTest::Test::GetTestList())

#ifndef UNITTEST_DISABLE_SHORT_MACROS
   #ifdef SUITE
      #error SUITE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_SUITE instead
   #else
      #define SUITE UNITTEST_SUITE
   #endif

   #ifdef TEST
      #error TEST already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_TEST instead
   #else
      #define TEST UNITTEST_TEST
   #endif

   #ifdef TEST_FIXTURE
      #error TEST_FIXTURE already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_TEST_FIXTURE instead
   #else
      #define TEST_FIXTURE UNITTEST_TEST_FIXTURE
   #endif
#endif

#endif

#include "UnitTestPP.h"
