#include "UITypes.h"

UIEntity::~UIEntity(void)
{
	map<string, UIEvent*>::iterator	theEventIterator;
	
	for (theEventIterator = this->mEventMap.begin(); theEventIterator != this->mEventMap.end(); theEventIterator++)
		delete (*theEventIterator).second;
}

void
UIEntity::AddEvent(IXMLDOMElementPtr inElement, const string& inEventType)
{
	UIEvent*	theEvent;
	_variant_t	theAttribute;

	theAttribute = inElement->getAttribute(inEventType.c_str());
	if (theAttribute.vt != VT_NULL && theAttribute.vt != VT_EMPTY)
	{
		/* create a new event */
		theEvent = new UIEvent;

		/* set the function name we'll look up to execute */
		theEvent->SetCodeMap(string(_bstr_t(theAttribute)));

		/* get the parameters */
		theAttribute = inElement->getAttribute("PARAMETER1");
		if (theAttribute.vt != VT_NULL)
			theEvent->SetParam1(string(_bstr_t(theAttribute)));
		theAttribute = inElement->getAttribute("PARAMETER2");
		if (theAttribute.vt != VT_NULL)
			theEvent->SetParam2(string(_bstr_t(theAttribute)));
		theAttribute = inElement->getAttribute("PARAMETER3");
		if (theAttribute.vt != VT_NULL)
			theEvent->SetParam3(string(_bstr_t(theAttribute)));
		theAttribute = inElement->getAttribute("PARAMETER4");
		if (theAttribute.vt != VT_NULL)
			theEvent->SetParam4(string(_bstr_t(theAttribute)));

		/* add the event to the event map */
		this->mEventMap[inEventType] = theEvent;
	}
}