
// MFCApplication1Dlg.h : 헤더 파일
//

#pragma once

#include<mmsystem.h>
#pragma comment (lib, "winmm.lib")
#define MAX_QUEUE_NUMBER   3   //버퍼의 개수
#define MAX_QUEUE_SIZE    4800 // 버퍼의 크기, 0.3초간 록음을 하는 크기

// CMFCApplication1Dlg 대화 상자
class CMFCApplication1Dlg : public CDialogEx
{
private:
	// 록음과 재생에 사용할 공통 설정정보(16bit,8khz, mono)
	WAVEFORMATEX m_wave_format; 

	// 록음장치핸들
	HWAVEIN mh_wave_in;
	
	// 록음에 사용할 데이터 블럭에 대한 정보(Wave data block handler)
	WAVEHDR *mp_in_block[MAX_QUEUE_NUMBER];

	// 재생장치핸들
	HWAVEOUT mh_wave_out;

	// 재생에 사용할 데이터 블럭에 대한 정보(Wave data block handler)
	WAVEHDR *mp_out_block[MAX_QUEUE_NUMBER];

	// 재생할 데이터를 저장할 위치
	int m_play_index;

// 생성입니다.
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartBtn();
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
