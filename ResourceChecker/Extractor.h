#pragma once

#include <fstream>
#include <iostream>
#include <cstdio>
#include <map>

#define MAX_LEN 1024
#define BLOCKS CArray<CStringA, CStringA&>

// CResourceExtractorDlg 대화 상자
struct RCPlace
{
	int nString;
	int nRCPlace;
	int nTotal;

	BOOL operator==(const RCPlace& RC)
	{
		BOOL b1 = this->nString == RC.nString;
		BOOL b2 = this->nRCPlace == RC.nRCPlace;
		BOOL b3 = this->nTotal == RC.nTotal;

		return b1 && b2 && b3;
	}
};

typedef struct DialogData
{
	CMap<CStringA, LPCSTR, int, int&> *m_pUsedID;
	BLOCKS *m_pDialog;
}RCDATA;

enum ERR_CODE { ERR_NO, ERR_FIND_ID, ERR_SIZE, ERR_ORDER, ERR_EXIST };
enum RC_TYPE { RC_DIALOG_TPYE = 1, RC_MENU_TPYE, RC_OTHER_TPYE };

class CExtractor
{
public:
	CExtractor(BOOL bDialog);

public:
	void Extract(CString strRCFile, CString strOutputPath);

private:
	void InitCtrlType();
	int MakeDialogItem(std::ifstream &openFile);

	void ExtractResourceString_DLG(FILE** outputFile);
	void ExtractResourceString_MENU(FILE** outputFile);
	void RemakeResourceID();
	void PrintingDialogResource(CString strRcFile, std::ofstream& fOut);

	BOOL Parse(CString rStr, CArray<CString, CString> &rParsedString);            //프로젝트 기본(현재는 유니코드) 따라감
	BOOL Parse(CStringA rStr, CArray<CStringA, CStringA> &rParsedString);         //무조건 멀티바이트 따라감
	wchar_t *m2w(char* pstr, int nStrLen);

private:
	CMap<CStringA, LPCSTR, int, int> m_mUsedRC;
	CMap<CStringA, LPCSTR, RCPlace, RCPlace> m_aCtrlType;
	CMap<int, int, CStringA, LPCSTR> m_aResData;

	CArray<CString, CString> m_aFilePath;
	std::map<CStringA, RCDATA> m_mDialogBlock;
	std::map<CStringA, RCDATA> m_mMenuBlock;
	
	BLOCKS m_aDialogBlock;

	BOOL m_bDialog;
};

