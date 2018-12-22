
#include "TestHarness/TestHarness.h"
#include "TestRes.h"


typedef RCResPtr<TestRes> RCResPtrTest;


TEST (RCResPtrTest, Destruction)
{
    // Getting out of this block, the smart pointer should decrease the ref count
    // and delete the resource
    TestRes::s_nDeleted = 0;
    {
        TestRes * pRes = new TestRes;
        RCResPtrTest resPtr(pRes);
    }
    CHECK_LONGS_EQUAL (1, TestRes::s_nDeleted);

    // If a pointer is still valid, the resource should not be deleted
    TestRes::s_nDeleted = 0;
    {
        TestRes * pRes = new TestRes;
        RCResPtrTest resPtr(pRes);
        {
            RCResPtrTest resPtr2(resPtr);
        }
        CHECK_LONGS_EQUAL (0, TestRes::s_nDeleted);
    }
    CHECK_LONGS_EQUAL (1, TestRes::s_nDeleted);
}


TEST (RCResPtrTest, AssignmentAndDestruction)
{
    TestRes::s_nDeleted = 0;
    {
        TestRes * pRes1 = new TestRes;
        RCResPtrTest resPtr(pRes1);

        TestRes * pRes2 = new TestRes;
        RCResPtrTest resPtr2(pRes2);

        resPtr = resPtr2;
        CHECK_LONGS_EQUAL (1, TestRes::s_nDeleted);
    }
    CHECK_LONGS_EQUAL (2, TestRes::s_nDeleted);


    TestRes::s_nDeleted = 0;
    {
        TestRes * pRes1 = new TestRes;
        RCResPtrTest resPtr(pRes1);
        RCResPtrTest resPtr3(resPtr);

        TestRes * pRes2 = new TestRes;
        RCResPtrTest resPtr2(pRes2);

        resPtr = resPtr2;
        CHECK_LONGS_EQUAL (0, TestRes::s_nDeleted);
    }
    CHECK_LONGS_EQUAL (2, TestRes::s_nDeleted);


    TestRes::s_nDeleted = 0;
    TestRes * pRes1 = new TestRes;
    RCResPtrTest resPtr(pRes1);
    resPtr = NULL;
    CHECK_LONGS_EQUAL (1, TestRes::s_nDeleted);
}



TEST (RCResPtrTest, AccessResource)
{
    TestRes * pRes = new TestRes;
    pRes->num = 4312;

    RCResPtrTest resPtr(pRes);
    // Read
    CHECK_LONGS_EQUAL (pRes->num, resPtr->num);

    // Write
    resPtr->num = 666;
    CHECK_LONGS_EQUAL (resPtr->num, pRes->num);

    // Function access
    CHECK_LONGS_EQUAL (pRes->num, resPtr->GetNum());
}


TEST (RCResPtrTest, NULLBehavior)
{
    RCResPtrTest resPtr;
    CHECK (resPtr == NULL);
    CHECK (!(resPtr != NULL));

    TestRes * pResource = NULL;
    RCResPtrTest resPtr2(pResource);
    CHECK (resPtr2 == NULL);

    TestRes * pResource3 = NULL;
    RCResPtrTest resPtr3(pResource3);
    CHECK (resPtr3 == NULL);
    CHECK (resPtr2 == resPtr3);
}


TEST (RCResPtrTest, Comparison)
{
    RCResPtrTest resPtr1;
    RCResPtrTest resPtr2;
    CHECK (resPtr1 == resPtr2);

    TestRes * pResource = NULL;
    RCResPtrTest resPtr3(pResource);
    RCResPtrTest resPtr4(pResource);
    CHECK (resPtr3 == resPtr4);

    pResource = new TestRes;
    RCResPtrTest resPtr5(pResource);
    RCResPtrTest resPtr6(resPtr5);
    CHECK (resPtr5 == resPtr6);
}


TEST (RCResPtrTest, LessThan)
{
    TestRes * pRes1 = new TestRes;
    TestRes * pRes2 = new TestRes;

    RCResPtrTest resPtr1(pRes1);
    RCResPtrTest resPtr2(pRes2);
    CHECK (resPtr1 < resPtr2 || resPtr2 < resPtr1);

    resPtr1 = resPtr2;
    CHECK (!(resPtr1 < resPtr2) || !(resPtr2 < resPtr1));

    resPtr1 = NULL;
    CHECK (resPtr1 < resPtr2);

    resPtr2 = NULL;
    CHECK (!(resPtr1 < resPtr2) || !(resPtr2 < resPtr1));
}


TEST (RCResPtrTest, Assignment)
{
    TestRes * pResource = new TestRes;

    RCResPtrTest resPtr1(pResource);
    RCResPtrTest resPtr2 = resPtr1;  // This is really a copy constructor 
    CHECK (resPtr1 == resPtr2);

    TestRes * pResource2 = new TestRes;

    RCResPtrTest resPtr3(pResource2);
    resPtr1 = resPtr3;
    CHECK (resPtr1 == resPtr3);
}



