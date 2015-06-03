//#include "stdafx.h"
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"

#include <EAF\EAFUtilities.h>
#include <MFCTools\VersionInfo.h>

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(UINT nResourceID,UINT nIDTemplate,CWnd* pParent /*=NULL*/)
: CDialog(nIDTemplate == 0 ? IDD_ABOUTBOX : nIDTemplate, pParent)
{
   m_ResourceID = nResourceID;
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
   DDX_Control(pDX,IDC_WSDOT,m_WSDOT);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // put the icon in the dialog
   CStatic* pIcon = (CStatic*)GetDlgItem(IDC_APPICON);

   CWinApp* pApp = AfxGetApp();
   HICON hIcon = pApp->LoadIcon(m_ResourceID);
   if ( hIcon )
   {
      pIcon->SetIcon( hIcon );
   }
   else
   {
      CWnd* pWnd = EAFGetMainFrame();
      hIcon = pWnd->GetIcon(TRUE);
      if ( hIcon )
      {
         pIcon->SetIcon( hIcon );
      }
      else
      {
         pIcon->ShowWindow(SW_HIDE);
      }
   }

   // Get the version information and update the version # and copyright
   CString docString;
   docString.LoadString(m_ResourceID);
   CString strAppName;
   AfxExtractSubString(strAppName, docString, 1);

   CString strTitle;
   strTitle.Format(_T("About %s"),strAppName);
   SetWindowText( strTitle );

   CString strExe = AfxGetApp()->m_pszExeName;
   strExe += _T(".dll");

   CVersionInfo verInfo;
   verInfo.Load(strExe);
   CString strVersion = verInfo.GetFileVersionAsString();
   CString strCopyright = verInfo.GetLegalCopyright();

#if defined _WIN64
   CString strPlatform(_T("x64"));
#else
   CString strPlatform(_T("x86"));
#endif
   CString str;
   str.Format(_T("Version %s (%s)"),strVersion,strPlatform);
   GetDlgItem(IDC_VERSION)->SetWindowText(str);
   GetDlgItem(IDC_COPYRIGHT)->SetWindowText(strCopyright);


   m_WSDOT.SetURL(_T("http://www.wsdot.wa.gov"));
   m_WSDOT.SetTooltip(_T("http://www.wsdot.wa.gov"));
   m_WSDOT.SizeToContent();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
