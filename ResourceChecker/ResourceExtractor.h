
// ResourceExtractor.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CResourceExtractorApp:
// �� Ŭ������ ������ ���ؼ��� ResourceExtractor.cpp�� �����Ͻʽÿ�.
//
class CExtractor;

class CResourceExtractorApp : public CWinApp
{
public:
	CResourceExtractorApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CResourceExtractorApp theApp;