//==============================================================
// MessageHandler.h
// Coded by: Trent Polack (trent@codershq.com)
//==============================================================
// The message handler base class for a demo framework.
//==============================================================
#ifndef __DemoFramework_MessageHandler_H__
#define __DemoFramework_MessageHandler_H__

namespace DemoFramework {
	class MessageHandler {

	public:

		//functions to set and get the MessageHandler parent
		void SetMessageParent( MessageHandler *pParent ) { m_pParent= pParent; }
		MessageHandler *GetMessageParent( void ) { return m_pParent; }

		//handles messages
		bool HandleMessage( int messageID, int argc, void **argv ) {
			//attempt to handle message
			if( OnMessage( messageID, argc, argv) )	return true ;
			else {
				//the message was not handled, look for a parent to pass it to
				if( GetMessageParent( ) )	return( GetMessageParent( )->HandleMessage( messageID, argc, argv ) );
				else return false;
			}
		}

		//triggered when a message occurs
		virtual bool OnMessage( int messageID, int argc, void **argv ) {
			return false;
		}

		MessageHandler( void ) { }
		virtual ~MessageHandler( void ) { }
	};
}


#endif	//__DemoFramework_MessageHandler_H__