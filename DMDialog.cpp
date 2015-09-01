#include "stdafx.h"
#include "DialogPlugIn.h"
#include "DMPluginStubs.h"
#include "DMDialog.h"
#include "DMWrapper/DMout.h"

#include"boost/lexical_cast.hpp"

IMPLEMENT_DYNCREATE(CDMDialog, CDialog)

BEGIN_MESSAGE_MAP(CDMDialog, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_START, &CDMDialog::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, &CDMDialog::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDMDialog::OnBnClickedBtnPause)
	ON_EN_CHANGE(IDC_EDIT_EXPOSURE, &CDMDialog::OnEdtChangedExposure)
END_MESSAGE_MAP()

CDMDialog::CDMDialog(CWnd* pParent) : CDialog(CDMDialog::IDD, pParent), dialogmtx(new boost::mutex), s_Exposure("0.005")
{
	Start();
}

CDMDialog::~CDMDialog(){}

void CDMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RDO_STEM, chk_STEM);
	DDX_Control(pDX, IDC_RDO_TEM, chk_TEM);
	DDX_Control(pDX, IDC_EDIT_EXPOSURE, edt_Exposure);
	DDX_Text(pDX, IDC_EDIT_EXPOSURE, s_Exposure);
}

BOOL CDMDialog::Create(UINT templateID, CWnd* pParentWnd){return CDialog::Create(IDD, pParentWnd);}

void CDMDialog::OnPaint(){CDialog::OnPaint();}

BOOL CDMDialog::OnEraseBkgnd(CDC* pDC){return CDialog::OnEraseBkgnd(pDC);}

BOOL CDMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	chk_STEM.SetCheck(1);

	return true;
}

void CDMDialog::OnBnClickedBtnStart()
{
	//chk_STEM.EnableWindow(FALSE);
	//chk_TEM.EnableWindow(FALSE);

	DMImage front = DMImage();
	front.fromFront();

	addBuilder(StackBuilder(this, dialogmtx, front));
	//builders.push_back( boost::make_shared<StackBuilder>(StackBuilder(this, dialogmtx, front)) );

	//builders.back()->Process();
}

void CDMDialog::OnBnClickedBtnStop()
{
	//DMImage selected = DMImage();
	//selected.fromFront();

	//for (int pos = 0; pos < builders.size(); ++pos)
	//	if (selected == builders[pos]->getImage())
	//	{
	//		builders[pos]->Stop();
	//		break;
	//	}
	//chk_STEM.EnableWindow(TRUE);
	//chk_TEM.EnableWindow(TRUE);
}

void CDMDialog::OnBnClickedBtnPause()
{
	//DMImage selected = DMImage();
	//selected.fromFront();

	//for (int pos = 0; pos < builders.size(); ++pos)
	//	if (selected == builders[pos]->getImage())
	//	{
	//		builders[pos]->Pause();
	//		break;
	//	}
}

void CDMDialog::OnEdtChangedExposure()
{

}