﻿#define AppMutex "344635E9-9AE4-4E60-B128-D53E25AB70A7"
#define AppName "TranslucentTB"
#define AppPublisher AppName + " Open Source Developers"
#define AppVersion GetFileVersion('..\Release\' + AppName + '.exe')

[Setup]
AllowRootDirectory=yes
AppCopyright=Copyright © 2020 {#AppPublisher}
AppendDefaultDirName=no
AppMutex={#AppMutex}
AppName={#AppName}
AppPublisher={#AppPublisher}
AppPublisherURL=https://github.com/{#AppName}
AppReadmeFile=https://github.com/{#AppName}/{#AppName}/blob/master/Readme.md
AppSupportURL=https://github.com/{#AppName}/{#AppName}/issues/new
AppVersion={#AppVersion}
DefaultDirName={code:GetDefaultDirName}
DisableWelcomePage=no
DisableProgramGroupPage=yes
LicenseFile=LICENSE.md
MinVersion=10
OutputBaseFilename={#AppName}-setup
OutputDir=.
PrivilegesRequired=none
SetupIconFile=DesktopInstaller\setup.ico
SourceDir=..
UninstallDisplayIcon={app}\{#AppName}.exe
UninstallDisplayName={#AppName}
WizardImageFile=DesktopInstaller\sidebar.bmp
WizardSmallImageFile=DesktopInstaller\header.bmp

[Files]
Source: "Release\*"; Excludes: "*.pdb, *.lib, *.exp"; DestDir: "{app}"; Flags: ignoreversion

[Registry]
Root: HKCU; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Run"; ValueName: {#AppName}; Flags: dontcreatekey uninsdeletevalue

[Icons]
Name: "{commonprograms}\{#AppName}"; Filename: "{app}\{#AppName}.exe"; WorkingDir: "{app}"

[Run]
Filename: "{app}\{#AppName}.exe"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall

[CustomMessages]
DependenciesDir=deps

#include "deps\lang\english.iss"
#include "deps\products.iss"
#include "deps\products\stringversion.iss"
#include "deps\products\msiproduct.iss"
#include "deps\products\vcredist2017.iss"

[Code]
function GetDefaultDirName(Param: string): string;
begin
	if IsAdmin then
	begin
		Result := ExpandConstant('{pf}\{#AppName}');
	end
	else
	begin
		Result := ExpandConstant('{localappdata}\{#AppName}');
	end;
end;

function InitializeSetup(): boolean;
begin
	vcredist2017('14');

	Result := true;
end;