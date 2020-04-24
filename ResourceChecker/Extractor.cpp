#include "stdafx.h"
#include "Extractor.h"
#include <regex>

CExtractor::CExtractor(BOOL bDialog)
{
	m_bDialog = bDialog;
	m_bJP = FALSE;
}

void CExtractor::InitCtrlType()
{
	CStringA strCtrlName = ("");
	RCPlace RcPlace = {};

	strCtrlName = ("CONTROL");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 9;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("PUSHBUTTON");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("GROUPBOX");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("COMBOBOX");
	RcPlace.nString = -1;
	RcPlace.nRCPlace = 1;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("EDITTEXT");
	RcPlace.nString = -1;
	RcPlace.nRCPlace = 1;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("LTEXT");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("DEFPUSHBUTTON");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("RTEXT");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("CTEXT");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 2;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("CAPTION");
	RcPlace.nString = 1;
	RcPlace.nRCPlace = 0;
	RcPlace.nTotal = 2;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("SCROLLBAR");
	RcPlace.nString = -1;
	RcPlace.nRCPlace = 1;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = ("LISTBOX");
	RcPlace.nString = -1;
	RcPlace.nRCPlace = 1;
	RcPlace.nTotal = 7;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = "POPUP";
	RcPlace.nRCPlace = 1;
	RcPlace.nString = 1;
	RcPlace.nTotal = 2;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);

	strCtrlName = "MENUITEM";
	RcPlace.nRCPlace = 2;
	RcPlace.nString = 1;
	RcPlace.nTotal = 3;
	m_aCtrlType.SetAt(strCtrlName, RcPlace);
}

void CExtractor::Extract(CString strRCFile, CString strOutputPath)
{
	InitCtrlType();

	m_mUsedRC.RemoveAll();

	CString strFileName = PathFindFileName(strRCFile);
	CFileFind fileFinder;
	CString strFileNameWithOutExt;
	if (fileFinder.FindFile(strRCFile))
	{
		fileFinder.FindNextFileW();
		strFileNameWithOutExt = fileFinder.GetFileTitle();
		fileFinder.Close();
	}
	CString strINIFilePath = strOutputPath + L"\\ctrl_" + strFileNameWithOutExt + L".ini";
	CString strRCFileOut = strOutputPath + L"\\" + strFileName;

	std::regex pattern("jp");

	CT2CA pszConvertedAnsiString(strFileNameWithOutExt);
	std::string s(pszConvertedAnsiString);

	if (std::regex_search(s, pattern)) { m_bJP = TRUE; }

	FILE *fOut_INI = NULL;
	_wfopen_s(&fOut_INI, strINIFilePath, _T("wt,ccs=UNICODE"));     //유니코드 방식 저장용

	std::ifstream InRCFile(strRCFile, std::ios::in);
	if (InRCFile == NULL)
	{
		ASSERT(0);
		if (m_bDialog)
			AfxMessageBox(L"파일 경로가 올바르지 않습니다.");
	}
	std::ofstream outRCFile(strRCFileOut);

	MakeDialogItem(InRCFile);
	RemakeResourceID();  

	ExtractResourceString_DLG(&fOut_INI);
	ExtractResourceString_MENU(&fOut_INI);

	PrintingDialogResource(strRCFile, outRCFile);

	fclose(fOut_INI);

	for (std::map<CStringA, RCDATA>::iterator it = m_mDialogBlock.begin(); it != m_mDialogBlock.end(); it++)
	{
		RCDATA Data;
		Data = it->second;
		if (Data.m_pDialog)
		{
			delete Data.m_pDialog;
			Data.m_pDialog = NULL;
		}
		if (Data.m_pUsedID)
		{
			delete Data.m_pUsedID;
			Data.m_pUsedID = NULL;
		}
	}

	for (std::map<CStringA, RCDATA>::iterator it = m_mMenuBlock.begin(); it != m_mMenuBlock.end(); it++)
	{
		RCDATA Data;
		Data = it->second;
		if (Data.m_pDialog)
		{
			delete Data.m_pDialog;
			Data.m_pDialog = NULL;
		}
		if (Data.m_pUsedID)
		{
			delete Data.m_pUsedID;
			Data.m_pUsedID = NULL;
		}
	}

	m_mDialogBlock.clear();
	m_mUsedRC.RemoveAll();
	m_aDialogBlock.RemoveAll();

}

void CExtractor::ExtractResourceString_DLG(FILE** outputFile)
{
	std::map<CStringA, RCDATA> DialogData = m_mDialogBlock;

	for (std::map<CStringA, RCDATA>::iterator it = DialogData.begin(); it != DialogData.end(); it++)
	{
		BLOCKS BlockData;
		BlockData.Copy(*(it->second.m_pDialog));
		for (int i = 0; i < BlockData.GetSize(); i++)
		{
			CArray<CStringA, CStringA> strParsed;
			CStringA strInsert = "";
			Parse(BlockData[i], strParsed);
			strInsert = "IDS_";
			strInsert += it->first + "_";

			RCPlace RCTemp;
			if (m_aCtrlType.Lookup(strParsed[0], RCTemp))
			{
				if (RCTemp.nString < 0) continue;

				if (!strParsed[RCTemp.nRCPlace].CompareNoCase("CAPTION"))
				{
					strInsert += it->first;
				}
				else
				{
					strInsert += strParsed[RCTemp.nRCPlace];
				}
				strInsert += " = " + strParsed[RCTemp.nString] + "\n";
			}
			else continue;
			char* pStr = strInsert.GetBuffer(0);
			int nStrLength = strlen(pStr);
			WCHAR *strWC = m2w(pStr, nStrLength + 1);   //multibyte to wide character
			if (strWC == NULL) continue;

			fputws(strWC, *outputFile);
			delete strWC;
		}
	}
}

void CExtractor::ExtractResourceString_MENU(FILE** outputFile)
{
	std::map<CStringA, RCDATA> *MenuData = &m_mMenuBlock;
	std::map<CStringA, int> mUsedID;
	mUsedID.clear();

	BOOL bMultiLine = FALSE;
	CStringA strPrev = "";
	for (std::map<CStringA, RCDATA>::iterator it = MenuData->begin(); it != MenuData->end(); it++)
	{
		BLOCKS asBlocks;  asBlocks.RemoveAll();
		asBlocks.Copy(*(it->second.m_pDialog));

		for (int i = 0; i < asBlocks.GetSize(); i++)
		{
			CArray<CStringA, CStringA> asParsed;  asParsed.RemoveAll();
			RCPlace RcTemp;
			Parse(asBlocks[i], asParsed);

			if (m_aCtrlType.Lookup(asParsed[0], RcTemp))
			{
				if (asParsed[1] == "SEPARATOR") continue;
				if (RcTemp.nRCPlace > 0)
				{
					CStringA strInsert = "";
					CStringA RCID = asParsed[RcTemp.nRCPlace];
					RCID.Replace(("\""), (""));
					strInsert = "IDS_" + it->first + "_" + RCID + " = " + asParsed[RcTemp.nString] + "\n";

					char* pStr = strInsert.GetBuffer(0);
					int nStrLength = strlen(pStr);
					WCHAR *strWC = m2w(pStr, nStrLength + 1);   //multibyte to wide character
					if (strWC == NULL) continue;

					fputws(strWC, *outputFile);
					delete strWC;
				}
			}
		}
	}
}

void CExtractor::RemakeResourceID()
{
	std::map<CStringA, RCDATA> *DialogData = &m_mDialogBlock;
	for (std::map<CStringA, RCDATA>::iterator it = DialogData->begin(); it != DialogData->end(); it++)
	{
		if (it->first == "") continue;

		CMap<CStringA, LPCSTR, int, int> mUsedID; mUsedID.RemoveAll();
		BLOCKS BlockData;
		BlockData.Copy(*(it->second.m_pDialog));

		POSITION pContent = it->second.m_pUsedID->GetStartPosition();
		while (pContent)
		{
			int thing;
			CStringA key;
			it->second.m_pUsedID->GetNextAssoc(pContent, key, thing);
			mUsedID.SetAt(key, thing);
		}

		for (int i = 0; i < BlockData.GetSize(); i++)
		{
			CArray<CStringA, CStringA> strParsed;
			Parse(BlockData[i], strParsed);

			RCPlace RCTemp;
			if (m_aCtrlType.Lookup(strParsed[0], RCTemp))
			{
				int nCount = -1;

				if (RCTemp.nString < 0) continue;

				CStringA RCID;
				if (strParsed[RCTemp.nRCPlace] == "-1")
					RCID = "IDC_STATIC";
				else
					RCID = strParsed[RCTemp.nRCPlace];

				if (mUsedID.Lookup(strParsed[RCTemp.nRCPlace], nCount))
				{
					CStringA strModify;
					int      nResourceCnt = -1;
					int      nIDCnt = 0;
					strModify = RCID;

					if (nCount == 0) continue;

					if (strModify.CompareNoCase("CAPTION"))
					{
						while (mUsedID.Lookup(strModify, nResourceCnt))
						{
							nIDCnt++;
							strModify.Format("%s%d", RCID, nIDCnt);
						}
					}
					BlockData[i].Replace(strParsed[RCTemp.nRCPlace], strModify);
					mUsedID.SetAt(strModify, nIDCnt);
				}
				else
				{
					ASSERT(0);
				}
			}
			else continue;
		}
		it->second.m_pDialog->Copy(BlockData);
	}
}

//Return Value
//1  : 성공
int CExtractor::MakeDialogItem(std::ifstream &openFile)
{
	char str[1024] = {};
	CStringA strDialogName = "";
	BOOL bStartDialog = FALSE;
	BLOCKS asDIalogBlock;
	BLOCKS asMenuBlock;
	CMap<CStringA, LPCSTR, int, int> mUsedID; mUsedID.RemoveAll();
	CStringA strPrev = "";
	BOOL bMultiLine = FALSE;
	int nResourceType = FALSE;
	int nTabs_MENU = 0;

	CStringA strBackupIDD = ("");
	int nLine = 0;
	while (!openFile.eof())
	{
		nLine++;
		openFile.getline(str, MAX_LEN);

		if (str[0] == '#') continue;

		CStringA ResourceLine = "";
		CArray<CStringA, CStringA> asParsed;
		if (bMultiLine == TRUE)
		{
			ResourceLine = strPrev + "\n" + str;
			bMultiLine = FALSE;
		}
		else
		{
			ResourceLine = str;
		}
		if (!ResourceLine.CompareNoCase("// Dialog"))            nResourceType = RC_DIALOG_TPYE;
		else if (!ResourceLine.CompareNoCase("// Menu"))         nResourceType = RC_MENU_TPYE;
		else if (!ResourceLine.CompareNoCase("// DESIGNINFO"))   nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Cursor"))       nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Accelerator"))  nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Bitmap"))       nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// BINARY"))       nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// TEXTINCLUDE"))  nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Icon"))         nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Dialog Info"))  nResourceType = RC_OTHER_TPYE;
		else if (!ResourceLine.CompareNoCase("// Version"))      nResourceType = RC_OTHER_TPYE;

		if (nResourceType == RC_OTHER_TPYE)
		{
			asDIalogBlock.RemoveAll();
			asMenuBlock.RemoveAll();
			continue;
		}

		Parse(ResourceLine, asParsed);

		if (asParsed.GetSize() <= 0) break;

#pragma region RC_DIALOG_TYPE
		if (nResourceType == RC_DIALOG_TPYE)
		{
			if ((asParsed[0].Left(asParsed[0].Find('_')) == "IDD"))
			{
				std::map<CStringA, RCDATA> mUsedDialog;   mUsedDialog.clear();
				mUsedDialog = m_mDialogBlock;

				strDialogName = asParsed[0];

				std::string str = strDialogName.GetBuffer(0);
				std::regex reg("IDD[\\w|_]+\\$\\(\\w+\\)");
				std::smatch m;

				if (std::regex_match(str, m, reg))
				{
					CStringA temp = strDialogName;
					temp = temp.Left(temp.Find('$'));
					strBackupIDD = temp;
					continue;
				}

				if (strDialogName == strBackupIDD)
				{
					strBackupIDD = "";
					continue;
				}

				if (mUsedDialog.find(strDialogName) != mUsedDialog.end())
				{
					bStartDialog = FALSE;
				}
				else
				{
					bStartDialog = TRUE;
				}
				continue;
			}

			RCPlace RcTemp = {};
			if (m_aCtrlType.Lookup(asParsed[0], RcTemp) && bStartDialog)
			{
				if (asParsed.GetSize() < RcTemp.nTotal)
				{
					strPrev = ResourceLine;
					bMultiLine = TRUE;
					continue;
				}
				else if (RcTemp.nString > 0)
				{
					int nCount = -1;
					CStringA strSetting = "";

					if (asParsed[RcTemp.nRCPlace] == "-1")
					{
						strSetting = "IDC_STATIC";
						ResourceLine.Replace(",-1", ",IDC_STATIC");
					}
					else
					{
						strSetting = asParsed[RcTemp.nRCPlace];
					}
					mUsedID.Lookup(strSetting, nCount);
					nCount++;
					mUsedID.SetAt(strSetting, nCount);
				}
			}

			if (bStartDialog == TRUE)
			{
				if (ResourceLine == "//")
					ASSERT(0);

				asDIalogBlock.Add(ResourceLine);
			}
			if (!asParsed[0].CompareNoCase("END") && bStartDialog == TRUE)
			{
				RCDATA sDialog = {};
				sDialog.m_pUsedID = new CMap<CStringA, LPCSTR, int, int &>;
				sDialog.m_pDialog = new BLOCKS;

				POSITION pContent = mUsedID.GetStartPosition();
				while (pContent)
				{
					int thing;
					CStringA key;
					mUsedID.GetNextAssoc(pContent, key, thing);
					sDialog.m_pUsedID->SetAt(key, thing);
				}
				sDialog.m_pDialog->Copy(asDIalogBlock);
				m_mDialogBlock[strDialogName] = sDialog;

				bStartDialog = FALSE;
				strDialogName = "";
				strBackupIDD = "";
				mUsedID.RemoveAll();
				asDIalogBlock.RemoveAll();
			}
		}
#pragma endregion
#pragma region RC_MENU_TYPE
		if (nResourceType == RC_MENU_TPYE)
		{
			if ((asParsed[0].Left(asParsed[0].Find('_')) == "IDR") && (asParsed[0].Find('$') < 0))
			{
				std::map<CStringA, RCDATA> mUsedDialog;   mUsedDialog.clear();
				std::map<CStringA, RCDATA> TRASH;   TRASH.clear();

				mUsedDialog = m_mMenuBlock;
				strDialogName = asParsed[0];

				if (mUsedDialog.find(strDialogName) != mUsedDialog.end())
					bStartDialog = FALSE;
				else
					bStartDialog = TRUE;

				continue;
			}
			else
			{
				RCPlace RcTemp = {};
				if (m_aCtrlType.Lookup(asParsed[0], RcTemp))
				{
					if (asParsed[1].CompareNoCase("SEPARATOR"))
					{
						if (asParsed.GetSize() < RcTemp.nTotal)
						{
							strPrev = ResourceLine;
							bMultiLine = TRUE;
							continue;
						}
						else if (RcTemp.nRCPlace > 0)
						{
							int nCount = -1;
							CStringA strSetting = asParsed[RcTemp.nRCPlace];
							mUsedID.Lookup(strSetting, nCount);
							nCount++;
							mUsedID.SetAt(strSetting, nCount);
						}
					}
				}
			}

			if (bStartDialog == TRUE)
			{
				if (ResourceLine == "//")
					ASSERT(0);

				asDIalogBlock.Add(ResourceLine);
			}

			if (asParsed[0] == "BEGIN") nTabs_MENU++;
			else if (asParsed[0] == "END")
			{
				nTabs_MENU--;
				if (nTabs_MENU == 0)
				{
					if (strDialogName == "") continue;
					RCDATA sDialog = {};
					sDialog.m_pUsedID = new CMap<CStringA, LPCSTR, int, int &>;
					sDialog.m_pDialog = new BLOCKS;

					POSITION pContent = mUsedID.GetStartPosition();
					while (pContent)
					{
						int thing;
						CStringA key;
						mUsedID.GetNextAssoc(pContent, key, thing);
						sDialog.m_pUsedID->SetAt(key, thing);
					}
					sDialog.m_pDialog->Copy(asDIalogBlock);
					m_mMenuBlock[strDialogName] = sDialog;

					bStartDialog = FALSE;
					strDialogName = "";
					mUsedID.RemoveAll();
					asDIalogBlock.RemoveAll();
				}
			}
		}
#pragma endregion

	}

	return 1;
}

wchar_t *CExtractor::m2w(char* pstr, int nStrLen)
{
	size_t len;
	wchar_t *pwstr = NULL;
	if (!pstr) return NULL;
	char Country[2][10] = {};

	if (m_bJP)
	{
		strcpy_s(Country[1], "Korean");
		strcpy_s(Country[0], "Japanese");
	}
	else {
		strcpy_s(Country[0], "Korean");
		strcpy_s(Country[1], "Japanese");
	}

	setlocale(LC_ALL, Country[0]);
	mbstowcs_s(&len, NULL, 0, pstr, 0);
	if (len == 0)    //일문에서 교체 실패시 한국어로 해봄
	{
		setlocale(LC_ALL, Country[1]);
		mbstowcs_s(&len, NULL, 0, pstr, 0);
	}
	if (len == 0)    //여전히 실패할경우 안함.
	{
		return NULL;
	}
	pwstr = new WCHAR[len + 1];
	if (!pwstr)  return NULL;
	mbstowcs_s(NULL, pwstr, len, pstr, nStrLen);
	return pwstr;
}

void CExtractor::PrintingDialogResource(CString strRcFile, std::ofstream& fOut)
{
	using namespace std;
	char str[MAX_LEN] = {};
	std::map<CStringA, RCDATA> DialogData = m_mDialogBlock;
	std::map<CStringA, int> UsedDialog; UsedDialog.clear();

	BOOL bDialogBlock = FALSE;
	BOOL bStartDialogData = FALSE;
	CStringA strDialogName;
	CStringA strPrev = "";
	BOOL bMultiLine = FALSE;
	int nLine = 0;

	CStringA strBackupIDD = "";
	std::ifstream fIn(strRcFile, std::ios::in);
	while (!fIn.eof())
	{
		fIn.getline(str, MAX_LEN);
		CStringA ResourceLine = str;
		CArray<CStringA, CStringA> asParsed;

		if (bMultiLine == TRUE)
		{
			ResourceLine = strPrev + "\n" + str;
			bMultiLine = FALSE;
		}
		else
		{
			ResourceLine = str;
		}

		Parse(ResourceLine, asParsed);

		RCPlace RcTemp = {};
		if (m_aCtrlType.Lookup(asParsed[0], RcTemp))
		{
			if (asParsed.GetSize() < RcTemp.nTotal)
			{
				strPrev = ResourceLine;
				bMultiLine = TRUE;
				continue;
			}
		}

		if ((asParsed[0].Left(asParsed[0].Find('_')) == "IDD"))
		{
			strDialogName = asParsed[0];

			std::string str = strDialogName.GetBuffer(0);
			std::regex reg("IDD[\\w|_]+\\$\\(\\w+\\)");
			std::smatch m;

			if (std::regex_match(str, m, reg))
			{
				CStringA temp = strDialogName;
				temp = temp.Left(temp.Find('$'));
				strBackupIDD = temp;
				fOut << ResourceLine << endl;
				continue;
			}

			if (strDialogName == strBackupIDD)
			{
				strBackupIDD = "";
				fOut << ResourceLine << endl;
				continue;
			}

			if (DialogData.find(strDialogName) != DialogData.end())
				bDialogBlock = TRUE;
			else
				bDialogBlock = FALSE;

			fOut << ResourceLine << endl;
			continue;
		}

		if (!ResourceLine.CompareNoCase("// Dialog"))
			bStartDialogData = TRUE;
		else if (!ResourceLine.CompareNoCase("// Menu"))         bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// DESIGNINFO"))   bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Cursor"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Accelerator"))  bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Bitmap"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// BINARY"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// TEXTINCLUDE"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Icon"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Dialog Info"))       bStartDialogData = FALSE;
		else if (!ResourceLine.CompareNoCase("// Version"))       bStartDialogData = FALSE;

		if ((!bDialogBlock || !bStartDialogData) || str[0] == '#')
		{
			fOut << ResourceLine << endl;
		}
		else
		{
			if (strDialogName == "")
			{
				VERIFY(0);
				return;
			}
			else if (DialogData.find(strDialogName) == DialogData.end())
			{
				VERIFY(0);
				return;
			}
			else
			{
				fOut << DialogData[strDialogName].m_pDialog->GetAt(nLine++).GetBuffer(0) << endl;
			}
		}

		if (ResourceLine == "END")
		{
			nLine = 0;
			strDialogName = "";
			bDialogBlock = FALSE;
		}
	}
}

BOOL CExtractor::Parse(CString rStr, CArray<CString, CString> &rParsedString)
{
	ASSERT(0);
	rStr.Replace(L" | ", L"|");

	CString temp;
	int rStrLength = rStr.GetLength();
	BOOL bOpen = FALSE;

	for (int i = 0; i < rStrLength; i++)
	{
		if (rStr[i] == _T('"'))
		{
			bOpen = !bOpen;
		}
		if ((rStr[i] == _T(' ') || rStr[i] == _T(',')) && bOpen == FALSE)
		{
			if (!temp.IsEmpty())
			{
				temp.Replace(L"|", L" | ");

				if ( temp != L"\n")
					rParsedString.Add(temp);
			}

			temp.Empty();
		}
		else temp += rStr[i];
	}

	rParsedString.Add(temp);
	return TRUE;
}

BOOL CExtractor::Parse(CStringA rStr, CArray<CStringA, CStringA> &rParsedString)
{
	rStr.Replace(" | ", "|");

	CStringA temp;
	int rStrLength = rStr.GetLength();
	BOOL bOpen = FALSE;

	for (int i = 0; i < rStrLength; i++)
	{
		if (rStr[i] == _T('"'))
		{
			bOpen = !bOpen;
		}
		if ((rStr[i] == _T(' ') || rStr[i] == _T(',')) && bOpen == FALSE)
		{
			if (!temp.IsEmpty())
			{
				temp.Replace("|", " | ");

				if (temp != L"\n")
					rParsedString.Add(temp);
			}

			temp.Empty();
		}
		else temp += rStr[i];
	}

	rParsedString.Add(temp);
	return TRUE;
}