#ifndef UNITTEST_CURRENTTESTRESULTS_H
#define UNITTEST_CURRENTTESTRESULTS_H


namespace UnitTest {

   class TestResults;
   class TestDetails;

   namespace CurrentTest
   {
      UNITTEST_LINKAGE TestResults*& Results();
      UNITTEST_LINKAGE const TestDetails*& Details();
   }

}

#endif
