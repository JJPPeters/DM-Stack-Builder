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
	ON_BN_CLICKED(IDC_BTN_ALIGN, &CDMDialog::OnBnClickedBtnGpa)
	//ON_BN_CLICKED(IDC_RDO_XCF, &CDMDialog::OnBnClickedBtnGpa)
	//ON_BN_CLICKED(IDC_RDO_PCF, &CDMDialog::OnBnClickedBtnGpa)
	//ON_CBN_SELCHANGE(IDC_CMB_DEVICES, &CDMDialog::OnCbnSelchangeCmbDevices)
END_MESSAGE_MAP()

CDMDialog::CDMDialog(CWnd* pParent) : CDialog(CDMDialog::IDD, pParent), dialogmtx(new boost::mutex), s_bfactor("150"), s_thresh("5")
{
	builder = boost::make_shared<StackBuilder>(StackBuilder(this, dialogmtx));
}

CDMDialog::~CDMDialog(){}

void CDMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_DEVICES, combo_CLdev);
	DDX_Control(pDX, IDC_CMB_METHOD, combo_method);
	DDX_Control(pDX, IDC_RDO_XCF, chk_XCF);
	DDX_Control(pDX, IDC_RDO_PCF, chk_PCF);
	DDX_Control(pDX, IDC_PROGRESS, progressBar);
	DDX_Control(pDX, IDC_TXT_BFACT, txt_bfactor);
	DDX_Text(pDX, IDC_TXT_BFACT, s_bfactor);
	DDX_Control(pDX, IDC_TXT_THRESH, txt_thresh);
	DDX_Text(pDX, IDC_TXT_THRESH, s_thresh);
}

BOOL CDMDialog::Create(UINT templateID, CWnd* pParentWnd){return CDialog::Create(IDD, pParentWnd);}

void CDMDialog::OnPaint(){CDialog::OnPaint();}

BOOL CDMDialog::OnEraseBkgnd(CDC* pDC){return CDialog::OnEraseBkgnd(pDC);}

BOOL CDMDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;

	i = combo_method.AddString("Normal");
	combo_method.SetItemData(i, (DWORD)0);
	i = combo_method.AddString("Overdetermined");
	combo_method.SetItemData(i, (DWORD)1);
	combo_method.SetCurSel(0);
	combo_method.EnableWindow(true);

	chk_XCF.SetCheck(1);

	return true;
}

void CDMDialog::OnBnClickedBtnGpa()
{
	builder->Process();
}