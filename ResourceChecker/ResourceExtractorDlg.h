
// ResourceExtractorDlg.h : 헤더 파일
//

#include "afxwin.h"
#include <fstream>
#include <iostream>
#include <cstdio>
#include <map>
#include "Extractor.h"

#pragma once

class CResourceExtractorDlg : public CDialogEx
{

	// 생성입니다.
public:
	CResourceExtractorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_RESOURCEEXTRACTOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	CEdit m_edtRCforExtract;
	CEdit m_edtOutputFilePath;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	BOOL ExtractResource();

	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedBrokenFileExploreButton();
	afx_msg void OnBnClickedOutputFolderExploreButton();

	DECLARE_MESSAGE_MAP()
};
