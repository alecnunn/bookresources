//
// This sample code demonstrates the use of template to implement reflection in C++, namely through the
// use of properties, as is demonstrated in the article "Using Templates For Reflection In C++".
//
// The sample consists of a simple dialog that shows a UI that is dynamically generating by inspecting
// a class's run-time reflection information. As the values on the UI are manipulated, the updated data
// is automatically routed back into the reflective objects.
//
// The first part of the article explains the use of templates for RTTI querying. You will find the relevant
// code in the RTTI folder of this solution.
//
// The second part explains the property system itself, found in the Properties folder.
//
// The sample instantiates the property system, defines some classes with reflective properties, creates
// objects of those types and binds those objects to a dynamic UI system that builds a UI corresponding
// to the properties of that class. The UI allows read/write access.
//
// The UI system uses a commercial property grid control similar to the controls seen in Microsoft's
// Visual Studio .NET. The present sample includes an evaluation version of the library, which explains
// why a popup will appear when the sample is started.
//
// Please direct any inquiries to dfilion@hotmail.com. Have fun!