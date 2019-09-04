
// MFCApplication1Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCApplication1Dlg 대화 상자



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	mh_wave_in = NULL;
	mh_wave_out = NULL;
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_BTN, &CMFCApplication1Dlg::OnBnClickedStartBtn)
	ON_BN_CLICKED(IDC_STOP_BTN, &CMFCApplication1Dlg::OnBnClickedStopBtn)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 메시지 처리기

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// 록음 또는 재생에 사용할 소리정보를 설정한다.
	// 기본구성은 8kkhz, 16bit, mono -> 1초당 16000 Byte(8000 * 2 * 1)
	m_wave_format.wFormatTag = WAVE_FORMAT_PCM; // PCM
	m_wave_format.nChannels = 1;                // 마이크 록음방식; 모노
	m_wave_format.nSamplesPerSec = 80000;       // 셈플링주기: 8Khz
	m_wave_format.wBitsPerSample = 16;          // 샘플링 단위: 16Bits  

	// 한개의 샘플데이터를 몇바이트씩 읽을것인지를 정한다.
	// 샘플당 비트수/8(샘플당 바이트수)* 채널수
	m_wave_format.nBlockAlign = m_wave_format.nChannels*m_wave_format.wBitsPerSample / 8;
	// 1초당 데이터를 입출력하는데에 필요한 바이트수를 구한다.
	// 샘플링 * 샘플당 비트수/8 * 채널수
	m_wave_format.nAvgBytesPerSec = m_wave_format.nSamplesPerSec*m_wave_format.nBlockAlign;
	m_wave_format.cbSize = 0; // WAVEFORMATEX 구조체 정보외에 추가적인 정보가 없다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnBnClickedStartBtn()
{
	// 록음장치를 열었는지 여부가 록음 및 재생이 실행되였는지의 판단기준이 된다.
	if(mh_wave_in == NULL){
		//재생버퍼의 기준 위치를 0으로 설정한다.
		m_play_index = 0;
		//사용할 버퍼의 개수만큼 메모리를 생성한다.
		for(int i = 0;i < MAX_QUEUE_NUMBER; i++){
			//록음버퍼용 메모리를 구성한다.
			mp_in_block[i] = (WAVEHDR *)::HeapAlloc(GetProcessHeap(), 0, sizeof(WAVEHDR));
			mp_in_block[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), 0, MAX_QUEUE_SIZE);
			mp_in_block[i]->dwBufferLength = MAX_QUEUE_SIZE;
			mp_in_block[i]->dwFlags = 0;
			mp_in_block[i]->dwLoops = 0;
			// 재생버퍼용 메모리를 구성한다.
			mp_out_block[i] = (WAVEHDR*)::HeapAlloc(GetProcessHeap(), 0, sizeof(WAVEHDR));
			mp_out_block[i]->lpData = (char*)::HeapAlloc(GetProcessHeap(), 0, MAX_QUEUE_SIZE);
			mp_out_block[i]->dwBufferLength = MAX_QUEUE_SIZE;
			mp_out_block[i]->dwFlags = 0;
			mp_out_block[i]->dwLoops = 0;
		}
	   // 사운드 입력장치를 연다.
	   waveInOpen(&mh_wave_in, WAVE_MAPPER, &m_wave_format, (DWORD)m_hWnd, NULL, CALLBACK_WINDOW);
	   // 사운드 출력장치를 연다.
	   waveOutOpen(&mh_wave_out, NULL, &m_wave_format, (DWORD)m_hWnd, NULL, CALLBACK_WINDOW | WAVE_ALLOWSYNC); // WAVE_ALLOWSYNC항목을 입력하지 않으면 음질이 떨어진다.

	   // 세개의 버퍼(큐)를 모두 록음장치에게 전송한다.
	   for (int i = 0; i < MAX_QUEUE_NUMBER; i++) {
		   // 해당 버퍼가 록음할 준비가 되였음을 록음장치에게 알린다.
		   waveInPrepareHeader(mh_wave_in, mp_in_block[i], sizeof(WAVEHDR));
		   // 록음을 위해 할당된 버퍼를 록음장치에게 전송한다.
		   waveInAddBuffer(mh_wave_in, mp_in_block[i], sizeof(WAVEHDR));
	   }
	     waveInStart(mh_wave_in); // 록음을 시작한다.
	}
}



void CMFCApplication1Dlg::OnBnClickedStopBtn()
{
	if (mh_wave_in != NULL) {
		// 록음을 정지한다.
		waveInStop(mh_wave_in);
		// 록음중이던 버퍼을 비운다.
		waveInReset(mh_wave_in);

		// 재생을 일시 중지한다.
		waveOutPause(mh_wave_out);
		// 재생중이던 버퍼를 비운다.
		waveOutReset(mh_wave_out);

		// 록음 및 재생중이던 버퍼를 모두 반환한다.
		for (int i = 0; i < MAX_QUEUE_NUMBER; i++) {
			waveInUnprepareHeader(mh_wave_in, mp_in_block[i], sizeof(WAVEHDR));
			waveOutUnprepareHeader(mh_wave_out, mp_out_block[i], sizeof(WAVEHDR));
		}
		// 록음 및 재생을 위해 할당된 메모리를 제거한다.
		for (int i = 0; i < MAX_QUEUE_NUMBER; i++) {
			::HeapFree(GetProcessHeap(), 0, mp_in_block[i]->lpData);
			::HeapFree(GetProcessHeap(), 0, mp_in_block[i]);

			::HeapFree(GetProcessHeap(), 0, mp_out_block[i]->lpData);
			::HeapFree(GetProcessHeap(), 0, mp_out_block[i]);
		}

		waveInClose(mh_wave_in); // 사운드 입력장치를 닫는다.
		waveOutClose(mh_wave_out);// 사운드 재생장치를 닫는다.

		// 록음과 재생에 관련된 장치핸들을 초기화한다.
		mh_wave_in = NULL;
		mh_wave_out = NULL;
	}
}


void CMFCApplication1Dlg::OnDestroy()
{
	OnBnClickedStopBtn();
	CDialogEx::OnDestroy();
}



LRESULT CMFCApplication1Dlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == MM_WIM_DATA && mh_wave_in != NULL) {
		// lParam 인자에 WAVEHDR구조체 형태로 록음된 소리정보가 들어있다.
		WAVEHDR *p_wave_header = (WAVEHDR *)lParam;

		// 록음버퍼에 저장된 데이터를 재생버퍼에 복사한다.
		memcpy(mp_out_block[m_play_index]->lpData, p_wave_header->lpData, p_wave_header->dwBytesRecorded);
		mp_out_block[m_play_index]->dwBufferLength = p_wave_header->dwBytesRecorded;

		// 재생버퍼를 출력장치로 전달한다.
		waveOutPrepareHeader(mh_wave_out, mp_out_block[m_play_index], sizeof(WAVEHDR));
		// 재생장치에 재생을 명령한다.
		waveOutWrite(mh_wave_out, mp_out_block[m_play_index], sizeof(WAVEHDR));
		// 재생버퍼의 다음 위치를 계산한다.
		m_play_index = (m_play_index + 1) % MAX_QUEUE_NUMBER;

		// 록음에 사용되였던 버퍼가 록음할 준비가 되였음을 록음장치에게 알린다.
		waveInPrepareHeader(mh_wave_in, p_wave_header, sizeof(WAVEHDR));
		// 록음이 끝나서 사용가능해진 버퍼를 록음장치에게 전송한다.
		waveInAddBuffer(mh_wave_in, p_wave_header, sizeof(WAVEHDR));
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}
