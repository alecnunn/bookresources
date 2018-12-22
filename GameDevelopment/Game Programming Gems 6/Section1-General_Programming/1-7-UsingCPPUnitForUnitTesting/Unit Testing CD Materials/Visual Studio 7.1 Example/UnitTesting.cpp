// UnitTesting example
#include "stdafx.h"
#include "allocator.h"
#include "model.h"

//Unit test for loading a model file
class LoadModelTest : public CppUnit::TestFixture
    {
public:
    CPPUNIT_TEST_SUITE(LoadModelTest);
        CPPUNIT_TEST(TestInvalidHeaderNullValue);
        CPPUNIT_TEST(TestInvalidHeaderIllFormatted);
        CPPUNIT_TEST(TestInvalidHeaderInvalidVersionSection);
        CPPUNIT_TEST(TestInvalidValidHeader);
    CPPUNIT_TEST_SUITE_END();

    void TestInvalidHeaderNullValue()
        {
        model my_model;
        CPPUNIT_ASSERT_THROW(my_model.parse_header_section(NULL),
                             model_invalid_header);
        }

    void TestInvalidHeaderIllFormatted()
        {
        model my_model;
        CPPUNIT_ASSERT_THROW(my_model.parse_header_section(
            "<HEADER"
            "<VERSION>1.1</VERSION>"
            "<TYPE>WorldLevel</TYPE>"
            "<NAME>Character Select Gallery</NAME>"
            "<AUTHOR>Blake Madden</AUTHOR>"),
            model_invalid_header);
        }

    void TestInvalidHeaderInvalidVersionSection()
        {
        model my_model;
        //Version section is missing the version number
        CPPUNIT_ASSERT_THROW(my_model.parse_header_section(
            "<HEADER>"
            "<VERSION></VERSION>"
            "<TYPE>WorldLevel</TYPE>"
            "<NAME>Character Select Gallery</NAME>"
            "<AUTHOR>Blake Madden</AUTHOR>"
            "</HEADER>"),
            model_invalid_version);
        //Version section is missing
        CPPUNIT_ASSERT_THROW(my_model.parse_header_section(
            "<HEADER>"
            "<TYPE>WorldLevel</TYPE>"
            "<NAME>Character Select Gallery</NAME>"
            "<AUTHOR>Blake Madden</AUTHOR>"
            "</HEADER>"),
            model_invalid_version);
        //Ill-formatted version section
        CPPUNIT_ASSERT_THROW(my_model.parse_header_section(
            "<HEADER>"
            "<VERSION>1.2"
            "<TYPE>WorldLevel</TYPE>"
            "<NAME>Character Select Gallery</NAME>"
            "<AUTHOR>Blake Madden</AUTHOR>"
            "</HEADER>"),
            model_invalid_version);
        }

    void TestInvalidValidHeader()
        {
        model my_model;
        //Perfectly valid header
        my_model.parse_header_section(
            "<HEADER>"
            "<VERSION>1.2</VERSION>"
            "<TYPE>WorldLevel</TYPE>"
            "<NAME>Character Select Gallery</NAME>"
            "<AUTHOR>Blake Madden</AUTHOR>"
            "</HEADER>");
        CPPUNIT_ASSERT(my_model.get_version() == 1.2);
        CPPUNIT_ASSERT(my_model.get_type() == world_level);
        CPPUNIT_ASSERT(strcmp(my_model.get_name(), "Character Select Gallery") == 0);
        CPPUNIT_ASSERT(strcmp(my_model.get_author(), "Blake Madden") == 0);
        //Author and name are not included
        my_model.parse_header_section(
            "<HEADER>"
            "<VERSION>1.2</VERSION>"
            "<TYPE>WorldLevel</TYPE>"
            "</HEADER>");
        CPPUNIT_ASSERT(my_model.get_version() == 1.2);
        CPPUNIT_ASSERT(my_model.get_type() == world_level);
        CPPUNIT_ASSERT(strcmp(my_model.get_name(), "") == 0);
        CPPUNIT_ASSERT(strcmp(my_model.get_author(), "") == 0);
        }
    };

//Unit test memory allocation manager
class HeapAllocatorTest : public CppUnit::TestFixture
    {
public:
    CPPUNIT_TEST_SUITE(HeapAllocatorTest);
        CPPUNIT_TEST(TestAllocation);
        /*Unfortunately, the allocation test case fails so badly that it corrupts
        the memory where "TestDellocationSimple" goes and can't be ran.  The test
        results for "TestAllocation" verbosely indicates how allocations sometimes
        overlap and cause corruption.*/
        //CPPUNIT_TEST(TestDellocationSimple);
        CPPUNIT_TEST(TestDellocationFromFreeStorage);
        CPPUNIT_TEST(TestDellocationFromUnmanagedPointer);
    CPPUNIT_TEST_SUITE_END();

public:
    void tearDown()
        {
        m_memory_manager.clear();
        }

    void TestAllocation()
        {
        //Data that we will copy into our allocated arrays
        char data1[FIRST_ALLOCATION_SIZE];
        std::memset(data1, 'a', FIRST_ALLOCATION_SIZE);
        char data2[SECOND_ALLOCATION_SIZE];
        std::memset(data2, 'b', SECOND_ALLOCATION_SIZE);
        char data3[THIRD_ALLOCATION_SIZE];
        std::memset(data3, 'c', THIRD_ALLOCATION_SIZE);

        /*Allocate arrays, verify that the allocations worked
        and fill with data*/
        char* array1 =
             m_memory_manager.allocate(FIRST_ALLOCATION_SIZE);
        CPPUNIT_ASSERT_MESSAGE("First allocation returned NULL",
             array1);
        std::memcpy(array1, data1, FIRST_ALLOCATION_SIZE);
        char* array2 =
             m_memory_manager.allocate(SECOND_ALLOCATION_SIZE);
        CPPUNIT_ASSERT_MESSAGE("Second allocation returned NULL",
             array2);
        std::memcpy(array2, data2, SECOND_ALLOCATION_SIZE);
        //This request will be too big for the allocator's fragmented heap
        char* array3 =
             m_memory_manager.allocate(THIRD_ALLOCATION_SIZE);
        CPPUNIT_ASSERT_MESSAGE(
             "Requesting too much memory returned NULL", array3);
        std::memcpy(array3, data3, THIRD_ALLOCATION_SIZE);

        /*Try to allocate a zero size-size array and
        verify it throws a bad_alloc exception*/
        CPPUNIT_ASSERT_THROW(m_memory_manager.allocate(0),
             std::bad_alloc);

        /*Test the integrity of the allocated memory to make sure that the
        returned pointers don't overlap*/
        CPPUNIT_ASSERT_MESSAGE(
             "First allocation's data is corrupt."
             "Allocations may overlap.",
             std::memcmp(array1, data1, FIRST_ALLOCATION_SIZE) == 0);
        CPPUNIT_ASSERT_MESSAGE(
             "Second allocation's data is corrupt."
             "Allocations may overlap.",
             std::memcmp(array2, data2, SECOND_ALLOCATION_SIZE) == 0);
        CPPUNIT_ASSERT_MESSAGE(
             "Third allocation's data is corrupt."
             "Allocations may overlap.",
             std::memcmp(array3, data3, THIRD_ALLOCATION_SIZE) == 0);

        //Free everything
        CPPUNIT_ASSERT(m_memory_manager.deallocate(array1,
             FIRST_ALLOCATION_SIZE));
        CPPUNIT_ASSERT(m_memory_manager.deallocate(array2,
             SECOND_ALLOCATION_SIZE));
        CPPUNIT_ASSERT(m_memory_manager.deallocate(array3,
             THIRD_ALLOCATION_SIZE));
        }

    void TestDellocationSimple()
        {
        //Allocate arrays
        char* array1 = m_memory_manager.allocate(FIRST_ALLOCATION_SIZE);

        //Free it
        CPPUNIT_ASSERT(m_memory_manager.deallocate(array1, FIRST_ALLOCATION_SIZE));
        CPPUNIT_ASSERT_MESSAGE("Allocation deallocated a NULL pointer.",
            m_memory_manager.deallocate(NULL, 1) == false);
        }

    void TestDellocationFromFreeStorage()
        {        
        //Allocate array outside of the memory manager
        char* array1 = new char[FIRST_ALLOCATION_SIZE];

        //Allocator should not free this because it did not allocate it
        CPPUNIT_ASSERT_MESSAGE(
            "Allocator deallocated a pointer that it did not originally allocate.",
            m_memory_manager.deallocate(array1, FIRST_ALLOCATION_SIZE) == false);
        delete [] array1;
        }

    void TestDellocationFromUnmanagedPointer()
        {
        //This request will be bigger than the allocator's managed heap
        char* array1 = m_memory_manager.allocate(TOO_BIG_ALLOCATION_SIZE);

        /*Although allocator creates this from outside its own heap, it should still
        be able to deallocate it*/
        CPPUNIT_ASSERT_MESSAGE("Allocation larger than managed heap wasn't deallocated.",
            m_memory_manager.deallocate(array1, TOO_BIG_ALLOCATION_SIZE));
        }
private:
    //Memory manager used for all test cases (max heap size is 1000 bytes)
    heap_allocator<char, 1000> m_memory_manager;
    //Allocation sizes
    static const size_t FIRST_ALLOCATION_SIZE = 500;
    static const size_t SECOND_ALLOCATION_SIZE = 400;
    static const size_t THIRD_ALLOCATION_SIZE = 300;
    static const size_t TOO_BIG_ALLOCATION_SIZE = 1100;
    };

int _tmain(int argc, _TCHAR* argv[])
    {
    CppUnit::TextTestRunner runner; 
    std::ofstream ofs("tests.xml");
    CppUnit::XmlOutputter* xml = new CppUnit::XmlOutputter(&runner.result(), ofs);
    xml->setStyleSheet("report.xsl");
    runner.setOutputter(xml);

    runner.addTest(LoadModelTest::suite() );
    runner.addTest(HeapAllocatorTest::suite() );
    runner.run();
    ofs.close();
    return 0;
    }
