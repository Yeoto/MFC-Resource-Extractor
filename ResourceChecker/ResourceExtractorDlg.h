
// ResourceExtractorDlg.h : ��� ����
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

	// �����Դϴ�.
public:
	CResourceExtractorDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_RESOURCEEXTRACTOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	HICON m_hIcon;

	CEdit m_edtRCforExtract;
	CEdit m_edtOutputFilePath;

	// ������ �޽��� �� �Լ�
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
