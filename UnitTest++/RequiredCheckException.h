#ifndef UNITTEST_REQUIREDCHECKEXCEPTION_H
#define UNITTEST_REQUIREDCHECKEXCEPTION_H

#ifndef UNITTEST_NO_EXCEPTIONS

#include <exception>

namespace UnitTest {

   class UNITTEST_LINKAGE RequiredCheckException : public std::exception
   {
   public:
      RequiredCheckException();
      virtual ~RequiredCheckException() throw();
   };

}

#endif

#endif
