#ifndef _WINCLASSES_H
#define _WINCLASSES_H

#include <commctrl.h>
#include <windowsx.h>

void ForceForegroundWindow(HWND hwnd);

namespace EngCtrls
{

class CEControl
{
public:
	virtual bool Init(HWND hWndParent, int itemID);
	virtual int GetInt() { return GetDlgItemInt(m_hWndParent, m_dlgItem, NULL, TRUE); }

protected:
	HWND m_hWnd;
	HWND m_hWndParent;
	int m_dlgItem;
};

class CEListBox : public CEControl
{
public:
	HWND GetHandle() { return m_hWnd; }

	inline int GetCurSel() { return (int)SendMessage(m_hWnd, LB_GETCURSEL, NULL, NULL); }
	inline int	GetLineCount() { return (int)SendMessage(m_hWnd, LB_GETCOUNT, 0, 0); }
	inline LRESULT GetItemData(int index) { return SendMessage(m_hWnd, LB_GETITEMDATA, index, NULL); }
	inline LRESULT SetItemData(int index, DWORD data) { return SendMessage(m_hWnd, LB_SETITEMDATA, index, data); }
	int AddString(const char* inputString);
};

class CEListView : public CEControl
{
public:
	HWND GetHandle() { return m_hWnd; }

	inline void DeleteAllItems() { ListView_DeleteAllItems(m_hWnd); }
	inline BOOL DeleteItem(int index) { return ListView_DeleteItem(m_hWnd, index); }
	BOOL DeleteColumn(int columnIndex);
	int GetItemCount() { return ListView_GetItemCount(m_hWnd); }
	int GetNextItem(int startPos, DWORD flags) { return ListView_GetNextItem(m_hWnd, startPos, flags); }
	LPARAM GetItemData(int itemIndex);
	int InsertItem(int itemIndex, char* text, LPARAM lParam);
	int InsertLast(char* text, LPARAM lParam) { return InsertItem(GetItemCount(), text, lParam); }
	void InsertColumn(int columnIndex, char* columnName);
	void InsertColumn(int columnIndex, char* columnName, int alignment);
	void SetColumnWidth(int columnIndex, int width);
	BOOL SetColumnAlignment(int columnIndex, int alignment);
	void SetItemText(int itemIndex, int subitemIndex, char* text);
	void SetExtendedStyle(DWORD style) { ListView_SetExtendedListViewStyle(m_hWnd, style); }
};

class CEEditBox : public CEControl
{
public:
	CEEditBox() : m_maxLines(0) { }

	HWND GetHandle() { return m_hWnd; }

	// Simple, inlined methods
	inline int GetText(char* buffer, int maxLength) const { return Edit_GetText(m_hWnd, buffer, maxLength); }
	inline int GetTextLength() const { return Edit_GetTextLength(m_hWnd); }
	inline bool Enable(bool bEnable) { return (EnableWindow(m_hWnd, (BOOL)bEnable) != 0); }
	inline void ClearText() { SetText(""); }
	inline int SetText(const char* text) { return Edit_SetText(m_hWnd, text); }
	inline void LimitText(int limit) { Edit_LimitText(m_hWnd, limit); }
	inline int GetLimitText() const { return (int)SendMessage(m_hWnd, EM_GETLIMITTEXT, NULL, NULL); }
	inline int GetLineCount() const { return Edit_GetLineCount(m_hWnd); }
	inline int LineIndex(int line) const { return Edit_LineIndex(m_hWnd, line); }
	inline void SetSel(int start, int end) { Edit_SetSel(m_hWnd, start, end); }
	inline void ReplaceSel(const char* str) { 	Edit_ReplaceSel(m_hWnd, str); }
	inline void SetRedraw(bool bRedraw) { SendMessage(m_hWnd, WM_SETREDRAW, (BOOL)bRedraw, NULL); }
	inline void SetHandle(HLOCAL hLoc) { Edit_SetHandle(m_hWnd, hLoc); }

	// New methods
	void CaretToEnd();
	virtual void AppendString(const char* str);
	void SetMaxLines(int maxLines);

protected:
	int m_maxLines;
};

class CEButton : public CEControl
{
public:
	LRESULT GetCheck();
	inline bool Enable(bool bEnable) { return (EnableWindow(m_hWnd, (BOOL)bEnable) != 0); }
	bool IsChecked();
	void SetCheck(bool bChecked);
};

class CEComboBox : public CEControl
{
public:
	int AddString(const char* str);
	int AddString(const char* str, LPARAM data);
	int GetText(char* buffer, int maxLength);
	int GetCurSel();
	int SetCurSel(int index);
	LRESULT GetItemData(int index);
	int SetItemData(int index, LPARAM data);
};

} // EngCtrls

#endif

