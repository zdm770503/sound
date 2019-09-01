
// Sound.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "Sound.h"
#include "SoundDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSoundApp

BEGIN_MESSAGE_MAP(CSoundApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSoundApp 생성

CSoundApp::CSoundApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CSoundApp 개체입니다.

CSoundApp theApp;


// CSoundApp 초기화

BOOL CSoundApp::InitInstance()
{
	CWinApp::InitInstance();
	CSoundDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

