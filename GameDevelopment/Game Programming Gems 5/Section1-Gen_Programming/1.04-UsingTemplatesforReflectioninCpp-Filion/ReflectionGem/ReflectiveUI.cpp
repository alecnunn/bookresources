#include "stdafx.h"
#include "ReflectiveUI.h"
#include "BaseObject.h"

CReflectiveUI::CReflectiveUI()
{
	m_pActiveObject = NULL;
}

void CReflectiveUI::BuildUIForObject( CBaseObject* pObject, const char* szName )
{
	m_pActiveObject = pObject;
    CXTPPropertyGridItem* pRoot        = m_pGrid->AddCategory( szName );
	
	vector<CAbstractProperty*> Properties;
	pObject->GetRTTI()->EnumProperties( Properties );

	for ( DWORD i = 0; i < Properties.size(); i++ )
	{
		switch ( Properties[i]->GetType() )
		{
		case eptBOOL :
			{
				CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
				CXTPPropertyGridItem* pItem = pRoot->AddChildItem( new CXTPPropertyGridItemBool(
					pTypedProperty->GetName(), pTypedProperty->GetValue( pObject ) ) );
				break;
			}
		case eptDWORD :
			{
				CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
				CXTPPropertyGridItem* pItem = pRoot->AddChildItem( new CXTPPropertyGridItemNumber(
					pTypedProperty->GetName(), pTypedProperty->GetValue( pObject ) ) );
				break;
			}
		case eptINT :
			{
				CTypedProperty<int>* pTypedProperty = (CTypedProperty<int>*)Properties[i];
				CXTPPropertyGridItem* pItem = pRoot->AddChildItem( new CXTPPropertyGridItemNumber(
					pTypedProperty->GetName(), pTypedProperty->GetValue( pObject ) ) );
				break;
			}
		case eptFLOAT :
			{
				CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
				CXTPPropertyGridItem* pItem = pRoot->AddChildItem( new CXTPPropertyGridItemDouble(
					pTypedProperty->GetName(), pTypedProperty->GetValue( pObject ) ) );
				break;
			}
		case eptSTRING :
			{
				CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
				CXTPPropertyGridItem* pItem = pRoot->AddChildItem( new CXTPPropertyGridItem(
					pTypedProperty->GetName(), pTypedProperty->GetValue( pObject ) ) );
				break;
			}
		case eptPTR :
			{
				CTypedProperty<CBaseObject*>* pTypedProperty = (CTypedProperty<CBaseObject*>*)Properties[i];
				BuildUIForObject( pTypedProperty->GetValue( pObject ), pTypedProperty->GetName() );
				break;
			}
		};
	}
	pRoot->Expand();
	pRoot->Select();
}

LRESULT CReflectiveUI::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

		vector<CAbstractProperty*> Properties;
		m_pActiveObject->GetRTTI()->EnumProperties( Properties );

		for ( DWORD i = 0; i < Properties.size(); i++ )
		{
			if ( strcmp( Properties[i]->GetName(), pItem->GetCaption() ) == 0 )
			{
				switch ( Properties[i]->GetType() )
				{
				case eptBOOL :
					{
						CXTPPropertyGridItemBool* pTypedItem = (CXTPPropertyGridItemBool*)pItem;
						CTypedProperty<bool>* pTypedProperty = (CTypedProperty<bool>*)Properties[i];
						pTypedProperty->SetValue( m_pActiveObject, pTypedItem->GetBool() != 0 );
						break;
					}
				case eptDWORD :
					{
						CXTPPropertyGridItemNumber* pTypedItem = (CXTPPropertyGridItemNumber*)pItem;
						CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
						pTypedProperty->SetValue( m_pActiveObject, pTypedItem->GetNumber() );
						break;
					}
				case eptINT :
					{
						CXTPPropertyGridItemNumber* pTypedItem = (CXTPPropertyGridItemNumber*)pItem;
						CTypedProperty<DWORD>* pTypedProperty = (CTypedProperty<DWORD>*)Properties[i];
						pTypedProperty->SetValue( m_pActiveObject, pTypedItem->GetNumber() );
						break;
					}
				case eptFLOAT :
					{
						CXTPPropertyGridItemDouble* pTypedItem = (CXTPPropertyGridItemDouble*)pItem;
						CTypedProperty<float>* pTypedProperty = (CTypedProperty<float>*)Properties[i];
						pTypedProperty->SetValue( m_pActiveObject, (float)pTypedItem->GetDouble() );
						break;
					}
				case eptSTRING :
					{
						CTypedProperty<char*>* pTypedProperty = (CTypedProperty<char*>*)Properties[i];
						pTypedProperty->SetValue( m_pActiveObject, (LPTSTR)(LPCTSTR)pItem->GetValue() );
						break;
					}
				};
			}
		}
	}
	return 0;
}