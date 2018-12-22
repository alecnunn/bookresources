//-----------------------------------------------------------------------------
// Menu.h implementation.
// Refer to the Menu.h interface for more details.
//
// Programming a Multiplayer First Person Shooter in DirectX
// Copyright (c) 2004 Vaughan Young
//-----------------------------------------------------------------------------
#include "Main.h"

//-----------------------------------------------------------------------------
// Allows the menu dialog to update the sessions list.
//-----------------------------------------------------------------------------
void UpdateSessionsList( HWND window )
{
	// Enumerate the current sessions on the network.
	g_engine->GetNetwork()->EnumerateSessions();

	// Try to keep the same session selected, if it still exists.
	SessionInfo *selectedSession = NULL;
	int selected = (int)SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_GETCURSEL, 0, 0 );
	if( selected != LB_ERR )
		selectedSession = (SessionInfo*)SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_GETITEMDATA, selected, 0 );

	// Tell the sessions list box not to redraw itself since it will change.
	SendMessage( GetDlgItem( window, IDC_SESSIONS ), WM_SETREDRAW, false, 0 );

	// Clear the contents of the sessions list box.
	SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_RESETCONTENT, 0, 0 );

	// Go through the list of sessions found on the local network.
	char name[MAX_PATH];
	SessionInfo *session = g_engine->GetNetwork()->GetNextSession( true );
	while( session != NULL )
	{
		// Convert this session's name into a character string.
		wcstombs( name, session->description.pwszSessionName, MAX_PATH );

		// Add this session to the sessions list box.
		int index = (int)SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_ADDSTRING, 0, (LPARAM)name );
		SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_SETITEMDATA, index, (LPARAM)session );

		// Check if this is the session the was selected before.
		if( selectedSession == session )
			SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_SETCURSEL, index, 0 );

		// Go to the next session.
		session = g_engine->GetNetwork()->GetNextSession();
	}

	// If there was no selected session, then select the first session.
	if( selectedSession == NULL )
		SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_SETCURSEL, 0, 0 );

	// Tell the sessions list box to redraw itself now.
	SendMessage( GetDlgItem( window, IDC_SESSIONS ), WM_SETREDRAW, true, 0 );
	InvalidateRect( GetDlgItem( window, IDC_SESSIONS ), NULL, false );
}

//-----------------------------------------------------------------------------
// Call back function for the menu's dialog.
//-----------------------------------------------------------------------------
int CALLBACK MenuDialogProc( HWND window, UINT msg, WPARAM wparam, LPARAM lparam )
{
	// These are used to keep the text in the edit boxes between state changes.
	static char name[MAX_PATH] = "Unknown Player";
	static char character[MAX_PATH] = "Marine.txt";
	static char map[MAX_PATH] = "Abandoned City.txt";

	switch( msg )
	{
		case WM_INITDIALOG:
		{
			// Show the player's name, selected character, and selected map.
			SetWindowText( GetDlgItem( window, IDC_NAME ), name );
			SetWindowText( GetDlgItem( window, IDC_CHARACTER ), character );
			SetWindowText( GetDlgItem( window, IDC_MAP ), map );

			// Allow the sessions list to update.
			UpdateSessionsList( window );

			return true;
		}

		case WM_COMMAND:
		{
			switch( LOWORD( wparam ) )
			{
				case IDC_HOST:
				{
					// Get the character and map.
					PlayerData data;
					GetWindowText( GetDlgItem( window, IDC_CHARACTER ), character, MAX_PATH );
					GetWindowText( GetDlgItem( window, IDC_MAP ), map, MAX_PATH );
					strcpy( data.character, character );
					strcpy( data.map, map );

					// Get the player's name.
					GetWindowText( GetDlgItem( window, IDC_NAME ), name, MAX_PATH );

					// Create a session name using the player's name.
					char session[MAX_PATH];
					sprintf( session, "%s's Session", name );

					// Host a new session then switch to the game state.
					if( g_engine->GetNetwork()->Host( name, session, 8, &data, sizeof( data ) ) )
					{
						g_engine->ChangeState( STATE_GAME );

						EndDialog( window, true );
					}

					return true;
				}

				case IDC_SESSIONS:
				{
					// Check if the user double clicked on the session list.
					if( HIWORD( wparam ) != LBN_DBLCLK )
						return true;
				} // If so, then fall through to IDC_JOIN.

				case IDC_JOIN:
				{
					// Get the character.
					PlayerData data;
					GetWindowText( GetDlgItem( window, IDC_CHARACTER ), character, MAX_PATH );
					strcpy( data.character, character );

					// Get the player's name.
					GetWindowText( GetDlgItem( window, IDC_NAME ), name, MAX_PATH );

					// Get the selected session.
					int session = (int)SendMessage( GetDlgItem( window, IDC_SESSIONS ), LB_GETCURSEL, 0, 0 );

					// Join the selected session then switch to the game state.
					if( g_engine->GetNetwork()->Join( name, session, &data, sizeof( data ) ) )
					{
						g_engine->ChangeState( STATE_GAME );

						EndDialog( window, true );
					}
					else
					{
						// If the join failed, it may be because the session
						// doesn't exist any more, so refresh the session list.
						UpdateSessionsList( window );
					}

					return true;
				}

				case IDC_REFRESH:
				{
					// Refresh the session list.
					UpdateSessionsList( window );

					return true;
				}

				case IDC_EXIT:
				{
					PostQuitMessage( 0 );

					return true;
				}
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Menu class constructor.
//-----------------------------------------------------------------------------
Menu::Menu() : State( STATE_MENU )
{
	// Does nothing but set's the state's ID.
}

//-----------------------------------------------------------------------------
// Update the menu state.
//-----------------------------------------------------------------------------
void Menu::Update( float elapsed )
{
	// Display the menu dialog. Processing will hang here until this is closed.
	DialogBox( NULL, MAKEINTRESOURCE( IDD_MENU ), g_engine->GetWindow(), MenuDialogProc );
}