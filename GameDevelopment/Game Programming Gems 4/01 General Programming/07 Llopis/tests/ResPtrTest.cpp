
#include "TestHarness/TestHarness.h"
#include "TestRes.h"



typedef ResPtr<TestRes> ResPtrTest;



TEST (ResPtrTest, AccessResource)
{
    TestRes * pRes = new TestRes;
    pRes->num = 4312;

    ResPtrTest resPtr(pRes);
    // Read
    CHECK_LONGS_EQUAL (pRes->num, resPtr->num);

    // Write
    resPtr->num = 666;
    CHECK_LONGS_EQUAL (resPtr->num, pRes->num);

    // Function access
    CHECK_LONGS_EQUAL (pRes->num, resPtr->GetNum());

    delete pRes;
}


TEST (ResPtrTest, NULLBehavior)
{
    ResPtrTest resPtr;
    CHECK (resPtr == NULL);
    CHECK (!(resPtr != NULL));

    TestRes * pResource = NULL;
    ResPtrTest resPtr2(pResource);
    CHECK (resPtr2 == NULL);

    TestRes * pResource3 = NULL;
    ResPtrTest resPtr3(pResource3);
    CHECK (resPtr3 == NULL);
    CHECK (resPtr2 == resPtr3);
}


TEST (ResPtrTest, Comparison)
{
    ResPtrTest resPtr1;
    ResPtrTest resPtr2;
    CHECK (resPtr1 == resPtr2);

    TestRes * pResource = NULL;
    ResPtrTest resPtr3(pResource);
    ResPtrTest resPtr4(pResource);
    CHECK (resPtr3 == resPtr4);

    pResource = new TestRes;
    ResPtrTest resPtr5(pResource);
    ResPtrTest resPtr6(resPtr5);
    CHECK (resPtr5 == resPtr6);

    delete pResource;
}


TEST (ResPtrTest, LessThan)
{
    TestRes * pRes1 = new TestRes;
    TestRes * pRes2 = new TestRes;

    ResPtrTest resPtr1(pRes1);
    ResPtrTest resPtr2(pRes2);
    CHECK (resPtr1 < resPtr2 || resPtr2 < resPtr1);

    resPtr1 = resPtr2;
    CHECK (!(resPtr1 < resPtr2) || !(resPtr2 < resPtr1));

    resPtr1 = NULL;
    CHECK (resPtr1 < resPtr2);

    resPtr2 = NULL;
    CHECK (!(resPtr1 < resPtr2) || !(resPtr2 < resPtr1));

    delete pRes1;
    delete pRes2;
}


TEST (ResPtrTest, Assignment)
{
    TestRes * pResource = new TestRes;

    ResPtrTest resPtr1(pResource);
    ResPtrTest resPtr2 = resPtr1;  // This is really a copy constructor 
    CHECK (resPtr1 == resPtr2);

    TestRes * pResource2 = new TestRes;

    ResPtrTest resPtr3(pResource2);
    resPtr1 = resPtr3;
    CHECK (resPtr1 == resPtr3);

    delete pResource;
    delete pResource2;
}



struct ResPtrConstTest
{
    ResPtrConstTest() : p((ResourcePtrHolder *)1313) {}
    ResPtrTest p;
};

TEST (ResPtr, Constness)
{
    const ResPtrConstTest str;
    ResPtrTest p2 = str.p;
    CHECK_LONGS_EQUAL (1313, (int)p2.GetResPtrHolder());
}


class TestRes2 : public TestRes
{
public:
    virtual const char * GetResourceType() const { return "PointerTestResource2"; }
};
typedef ResPtr<TestRes2> ResPtrTest2;


TEST (ResPtrTest, rp_static_cast)
{
    TestRes2 * pRes2 = new TestRes2;
    pRes2->num = 4312;

    ResPtrTest2 resPtr2(pRes2);
    CHECK_LONGS_EQUAL (pRes2->num, resPtr2->num);

    // Cast up
    ResPtrTest resPtr = rp_static_cast<ResPtrTest, ResPtrTest2>(resPtr2);
    CHECK_LONGS_EQUAL (pRes2->num, resPtr->num);

    // Cast down
    ResPtrTest2 resPtr3 = rp_static_cast<ResPtrTest2, ResPtrTest>(resPtr);
    CHECK_LONGS_EQUAL (pRes2->num, resPtr3->num);

    // Const ptrs
    const ResPtrTest2 resPtrConst(resPtr2);
    ResPtrTest resPtrNonConst = rp_static_cast<ResPtrTest, const ResPtrTest2>(resPtrConst);
    CHECK_LONGS_EQUAL (pRes2->num, resPtrNonConst->num);

    delete pRes2;
}



TEST (ResPtrTest, SwapResources)
{
    TestRes * pNullRes = new TestRes;
    pNullRes->num = 0;

    TestRes * pRes1 = new TestRes;
    pRes1->num = 1234;

    TestRes * pRes2 = new TestRes;
    pRes2->num = 5678;

    ResPtrTest resPtr1(pRes1);
    ResPtrTest resPtr2(resPtr1);
    CHECK_LONGS_EQUAL (1234, resPtr1->num);
    CHECK_LONGS_EQUAL (1234, resPtr2->num);

    resPtr1.GetResPtrHolder()->pRes = pRes2;

    CHECK_LONGS_EQUAL (5678, resPtr1->num);
    CHECK_LONGS_EQUAL (5678, resPtr2->num);

    resPtr1.GetResPtrHolder()->pRes = pNullRes;

    CHECK_LONGS_EQUAL (0, resPtr1->num);
    CHECK_LONGS_EQUAL (0, resPtr2->num);

    delete pRes1;
    delete pRes2;
    delete pNullRes;
}
