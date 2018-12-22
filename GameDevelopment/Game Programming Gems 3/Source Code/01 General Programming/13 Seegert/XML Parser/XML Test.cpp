#include "XML Test.h"
#include "UITypes.h"


UIEntity*
ProcessElement(IXMLDOMElementPtr inElement, UIEntity* inEntity)
{
	IXMLDOMNodeListPtr	theNodeListPtr;
	IXMLDOMElementPtr	theElementPtr;
	UIEntity*			theNextEntity	= NULL;
	_variant_t			theAttribute;
	string				theElementName;
	int					theLoop;

	if (inElement != NULL)
	{
		/* get the element name */
		theElementName = inElement->GettagName();

		/*
		 * Note, you'd probably want to use a more elegant way of 
		 * checking the element and attribute names.  Also, the 
		 * implementation here is closely tied to the DTD.  If 
		 * the DTD changes, this will
		 * need to change as well. 
		 */

		/* construct the appropriate entities based on the element type. */
		if (theElementName == "UIBUTTON")
		{
			/* make sure our parent is a UIScreen */
			if (dynamic_cast<UIScreen*>(inEntity))
			{
				/* create and pass on a new button */
				theNextEntity = new UIButton;
				static_cast<UIScreen*>(inEntity)->AddControl(theNextEntity);
			}
		}
		else
		if (theElementName == "UIEVENT")
		{
			/* make sure our parent is not NULL */
			if (inEntity != NULL)
			{
				/* set the appropriate events */
				
				/* ONCLICK */
				inEntity->AddEvent(inElement, "ONCLICK");
				
				/* ONMOUSEOVER */
				inEntity->AddEvent(inElement, "ONMOUSEOVER");

				/* ONMOUSEOFF */
				inEntity->AddEvent(inElement, "ONMOUSEOFF");
			}
		}
		else
		if (theElementName == "UILISTBOX")
		{
			/* make sure our parent is a UIScreen */
			if (dynamic_cast<UIScreen*>(inEntity))
			{
				/* create and pass on a new button */
				theNextEntity = new UIListBox;
				static_cast<UIScreen*>(inEntity)->AddControl(theNextEntity);
			}
		}
		else
		if (theElementName == "UISCREEN")
		{
			/* TODO - attributes */
			/* make sure we're NULL */
			if (inEntity == NULL)
			{
				/* create a new screen */
				inEntity = new UIScreen;

				/* we'll pass the screen on to the child elements */
				theNextEntity = inEntity;
			}
		}
		else
		if (theElementName == "UITEXT")
		{
			/* make sure our parent is a UIScreen */
			if (dynamic_cast<UIScreen*>(inEntity))
			{
				/* create and pass on a new button */
				theNextEntity = new UITextBox;
				static_cast<UIScreen*>(inEntity)->AddControl(theNextEntity);
			}
		}
		else
		if (theElementName == "UITEXTITEM")
		{
			/* make sure our parent is not NULL */
			if (inEntity != NULL)
			{
				/* append the text in the element to the ui entity */
				string theText = inElement->Gettext();
				inEntity->AppendText(theText);
			}
		}
		else
		{
			::MessageBox(NULL, "Unimplemented XML element.", "Error", MB_OK);			
		}

		/* get the attributes common to all ui entities */
		if (theNextEntity != NULL)
		{
			/* NAME */
			theAttribute = inElement->getAttribute("NAME");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->SetName(string(_bstr_t(theAttribute)));
			}

			/* BACKGROUND */
			theAttribute = inElement->getAttribute("BACKGROUND");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->SetBackgroundTexture(string(_bstr_t(theAttribute)));
			}

			/* LEFT % */
			theAttribute = inElement->getAttribute("LEFT");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->GetRectangle().mLeft = ::atof(string(_bstr_t(theAttribute)).c_str());
			}

			/* RIGHT % */
			theAttribute = inElement->getAttribute("RIGHT");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->GetRectangle().mRight = ::atof(string(_bstr_t(theAttribute)).c_str());
			}

			/* TOP % */
			theAttribute = inElement->getAttribute("TOP");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->GetRectangle().mTop = ::atof(string(_bstr_t(theAttribute)).c_str());
			}

			/* BOTTOM % */
			theAttribute = inElement->getAttribute("BOTTOM");
			if (theAttribute.vt != VT_NULL)
			{
				theNextEntity->GetRectangle().mBottom = ::atof(string(_bstr_t(theAttribute)).c_str());
			}
		}
		
		/* get the child nodes */
		theNodeListPtr = inElement->GetchildNodes();
		
		/* recursively process them */
		for (theLoop = 0; theLoop < theNodeListPtr->Getlength(); theLoop++)
		{
			theElementPtr = theNodeListPtr->Getitem(theLoop);
			::ProcessElement(theElementPtr, theNextEntity);
		}
	}

	/* return the entity */
	return theNextEntity;
}


UIScreen*
BuildUIScreen(string& inFileName)
{
	IXMLDOMDocumentPtr	theDOMPtr;
	_variant_t			theUrl;
	_variant_t			theSuccess(true);
	HRESULT				theResult;

	/* create the document object model */
	theResult = theDOMPtr.CreateInstance("Msxml2.DOMDocument.4.0");

	/* this is the UI Screen we'll create */
	UIEntity* theUIScreen = NULL;

	if (SUCCEEDED(theResult))
	{
		/* the file to parse */
		theUrl.SetString(inFileName.c_str());

		/* validates with dtd */
        theDOMPtr->put_validateOnParse(true);
		
		/* non synchronous */
        theDOMPtr->put_async(false);

		/* load the xml */
		theSuccess = theDOMPtr->load(theUrl);

		if ((bool)theSuccess)
		{
			/* process the elements in the xml document */
			theUIScreen = ::ProcessElement(theDOMPtr->documentElement, NULL);
		}
		else
		{
			/* couldn't find the file, or it wasn't well formed XML. */
			::MessageBox(NULL, "Unable to process the XML document.", "Error", MB_OK);
		}
	}

	/* return the created UIScreen */
	return (dynamic_cast<UIScreen*>(theUIScreen));
}



void main(void)
{
	UIScreen*	theScreen = NULL;
	string		theFileName("uisample.xml");

	/* initialize com */
	::CoInitialize(NULL);

	/* build the screen from the xml file */
	theScreen = ::BuildUIScreen(theFileName);

	/* clean up the screen */
	if (theScreen != NULL)
		delete theScreen;

	/* uninitialize com */
	::CoUninitialize();
}