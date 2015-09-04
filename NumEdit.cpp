// NumEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumEdit.h"

#include <errno.h>
#include <string>
#include <cstddef> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

// this is hardcoded custom, only allows integers that are greater than 1
CNumEdit::CNumEdit()
{
	blockneg = true;
	myType = TypeLong;
	myRejectingChange = false;
}

CNumEdit::~CNumEdit()
{
}


BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void CNumEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch (nChar)
	{
	case VK_BACK:
	case VK_RETURN:
	case 0x0A: // Shift+Enter (= linefeed)
	case VK_ESCAPE:
	case VK_TAB:
		break;
	default:
		myLastSel = GetSel();
		GetWindowText(myLastValidValue);
		break;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CNumEdit::OnUpdate() 
{
	if (! myRejectingChange)
	{
		bool isneg = false;
		CString aValue;
		GetWindowText(aValue);

		if (aValue == "")
		{
			aValue = "1";
			//myRejectingChange = true;
			SetWindowText(aValue);
			//myRejectingChange = false;
			SetSel(-1);
			return;
		}
		else if (aValue != "0")
		{
			// remove leading 0
			std::string sValue(aValue);
			int nz = sValue.find_first_not_of('0');
			sValue.erase(0, nz);
			aValue = sValue.c_str();
			if (nz > 0)
			{
				//myRejectingChange = true;
				SetWindowText(aValue);
				//myRejectingChange = false;
				SetSel(myLastSel);
				return;
			}
			
		}

		LPTSTR aEndPtr;
		union
		{
			long aLongValue;
			double aDoubleValue;
		};

		errno = 0;
		switch (myType)
		{
		case TypeLong:
			aLongValue = _tcstol(aValue, &aEndPtr, 10);
			if (aLongValue < 1 && blockneg)
				isneg = true;
			break;
		case TypeDouble:
			aDoubleValue = _tcstod(aValue, &aEndPtr);
			if (aDoubleValue < 1 && blockneg)
				isneg = true;
			break;
		}

		if ((!(*aEndPtr)) && (errno != ERANGE) && (!isneg))
		{
			myLastValidValue = aValue;
		}
		else
		{
			myRejectingChange = true;
			SetWindowText(myLastValidValue);
			myRejectingChange = false;
			SetSel(myLastSel);
		}
	}
}
