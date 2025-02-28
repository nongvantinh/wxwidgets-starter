#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/file.h>

// Helper function: Base64-encode a wxString (assuming UTF-16LE)
wxString Base64Encode(const wxString& input)
{
	const wchar_t* pData = input.wc_str();
	DWORD dataSize = (DWORD)(wcslen(pData) * sizeof(wchar_t));
	DWORD encodedLen = 0;
	if (!CryptBinaryToStringW(reinterpret_cast<const BYTE*>(pData), dataSize,
		CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &encodedLen))
	{
		return "";
	}
	wxString encoded;
	wchar_t* buffer = new wchar_t[encodedLen];
	if (CryptBinaryToStringW(reinterpret_cast<const BYTE*>(pData), dataSize,
		CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, buffer, &encodedLen))
	{
		encoded = wxString(buffer);
	}
	delete[] buffer;
	return encoded;
}

// Executes a PowerShell script using -EncodedCommand without creating temporary files.
// The process is created without showing a window.
wxString ExecutePowershellScriptFromMemory(const wxString& script)
{
	wxString encoded = Base64Encode(script);
	wxString command = "powershell.exe -NoProfile -ExecutionPolicy Bypass -EncodedCommand " + encoded;

	HANDLE hRead = NULL, hWrite = NULL;
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&hRead, &hWrite, &saAttr, 0))
	{
		return "Failed to create pipe";
	}
	SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOW si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.hStdOutput = hWrite;
	si.hStdError = hWrite;
	si.dwFlags |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; // Hide the window

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	wxString commandLine = command;
	wxWCharBuffer wCmd = commandLine.wc_str();

	// Use CREATE_NO_WINDOW to prevent a console window from showing
	BOOL bSuccess = CreateProcessW(NULL, wCmd.data(), NULL, NULL, TRUE,
		CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	CloseHandle(hWrite);

	if (!bSuccess)
	{
		CloseHandle(hRead);
		return "Failed to create process";
	}

	wxString output;
	const int BUFSIZE = 4096;
	char buffer[BUFSIZE];
	DWORD bytesRead;
	while (ReadFile(hRead, buffer, BUFSIZE - 1, &bytesRead, NULL) && bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		output += wxString::FromUTF8(buffer);
	}
	CloseHandle(hRead);

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return output;
}

// Helper: Process the raw output to extract only the table portion.
wxString ProcessOutput(const wxString& rawOutput)
{
	wxString processed;
	int pos = rawOutput.Find("Volumes available to retrieve recovery keys:");
	if (pos != wxNOT_FOUND)
	{
		int start = rawOutput.find("\n", pos);
		if (start != wxNOT_FOUND)
			processed = rawOutput.Mid(start + 1);
		else
			processed = rawOutput;

		int objsPos = processed.Find("<Objs");
		if (objsPos != wxNOT_FOUND)
			processed = processed.Left(objsPos);

		processed = processed.Trim(true).Trim(false);
	}
	else
	{
		processed = rawOutput;
	}
	return processed;
}
class BitlockerDialog : public wxDialog {
public:
	BitlockerDialog(wxWindow* parent, const wxString& keyData)
		: wxDialog(parent, wxID_ANY, "Bitlocker Keys", wxDefaultPosition, wxSize(500, 300)) {

		wxPanel* panel = new wxPanel(this);

		wxStaticText* keyText = new wxStaticText(panel, wxID_ANY, keyData, wxDefaultPosition, wxSize(450, -1), wxALIGN_LEFT);

		wxButton* okButton = new wxButton(panel, wxID_OK, "OK", wxDefaultPosition, wxSize(100, 30));

		wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
		vbox->Add(keyText, 1, wxALL | wxEXPAND, 15);
		vbox->Add(okButton, 0, wxALIGN_CENTER | wxBOTTOM, 10);

		panel->SetSizer(vbox);
	}
};

class MyFrame : public wxFrame {
public:
	MyFrame() : wxFrame(NULL, wxID_ANY, "MetaDefender Drive: Bitlocker Tool - [E:\\tools\\bitlocker]",
		wxDefaultPosition, wxSize(450, 200)) {
		wxPanel* panel = new wxPanel(this);

		//wxStaticText* title = new wxStaticText(panel, wxID_ANY, "MetaDefender Drive: Bitlocker Tool - [E:\\tools\\bitlocker]",
		//	wxPoint(0, 0), wxSize(450, 30), wxALIGN_CENTER_HORIZONTAL);
		//title->SetBackgroundColour(*wxBLUE);
		//title->SetForegroundColour(*wxWHITE);
		//wxFont titleFont = title->GetFont();
		//titleFont.SetWeight(wxFONTWEIGHT_BOLD);
		//title->SetFont(titleFont);

		wxString exePath = wxStandardPaths::Get().GetExecutablePath();
		wxFileName fn(exePath);
		currentDir = fn.GetPath();

		wxString descriptionText = "Bitlocker Tool will generate Bitlocker keys to " + currentDir;
		wxStaticText* description = new wxStaticText(panel, wxID_ANY, descriptionText, wxDefaultPosition, wxSize(400, -1), wxALIGN_CENTER_HORIZONTAL);
		description->Wrap(400);

		wxButton* generateButton = new wxButton(panel, wxID_ANY, "Generate", wxDefaultPosition, wxSize(100, 40));
		wxButton* closeButton = new wxButton(panel, wxID_ANY, "Close", wxDefaultPosition, wxSize(100, 40));

		generateButton->Bind(wxEVT_BUTTON, &MyFrame::OnGenerate, this);
		closeButton->Bind(wxEVT_BUTTON, &MyFrame::OnClose, this);

		wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
		//vbox->Add(title, 0, wxEXPAND | wxALL, 0);
		vbox->AddSpacer(10);
		vbox->Add(description, 0, wxALIGN_CENTER | wxALL, 10);
		vbox->AddSpacer(10);

		wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
		hbox->Add(generateButton, 0, wxRIGHT, 10);
		hbox->Add(closeButton, 0, wxLEFT, 10);

		vbox->Add(hbox, 0, wxALIGN_CENTER | wxBOTTOM, 20);
		panel->SetSizer(vbox);
	}

private:
	wxString currentDir;

	void OnGenerate(wxCommandEvent& event) {
		wxString psScript =
			"if (!(Get-Command Get-BitLockerVolume -errorAction SilentlyContinue)) { \n"
			"    Write-Output \"Get-BitLockerVolume Cmdlet does not exist, please check you have administrator permissions to extract recovery password...\"; \n"
			"    exit 1 \n"
			"}; \n"
			"$volumes = Get-BitLockerVolume | Where-Object { $_.KeyProtector.KeyProtectorType -eq 'RecoveryPassword' } | \n"
			"            Select-Object MountPoint, @{Label='Key'; Expression={\"$($_.KeyProtector.RecoveryPassword)\"}}; \n"
			"Write-Output \"Volumes available to retrieve recovery keys:\"; \n"
			"$volumes | Format-Table -AutoSize | Out-String;";

		wxString rawOutput = ExecutePowershellScriptFromMemory(psScript);
		wxString output = ProcessOutput(rawOutput);

		wxString filePath = currentDir + "/bitlocker.key";
		wxFile file(filePath, wxFile::write);
		if (file.IsOpened()) {
			file.Write(output);
			file.Close();
		}

		BitlockerDialog dlg(this, output);
		dlg.ShowModal();
		wxMessageBox("Bitlocker keys saved to " + filePath, "Success", wxOK | wxICON_INFORMATION);
	}

	void OnClose(wxCommandEvent& event) {
		Close(true);
	}
};

class MyApp : public wxApp {
public:
	virtual bool OnInit() {
		MyFrame* frame = new MyFrame();
		frame->Show(true);
		return true;
	}
};

wxIMPLEMENT_APP(MyApp);
