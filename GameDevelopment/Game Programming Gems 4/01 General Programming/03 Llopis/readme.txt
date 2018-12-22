
This is a Visual C++ 6.0 workspace. It contains a static library with 
all the classes involved in the clock system described in the gem.
It also contains a project with unit tests for all the functionality of
the clock system.

The test framework for the unit tests is based Michael Feather's 
CppUnitLite. Just build the project and the unit tests will be 
executed automatically as the last step of the build process. If
any of the tests fail, the build will fail and the failed tests will 
be shown in the output window.
