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

int main(int argc, char *argv[])
{
	string start_ip, end_ip;
	if (argc < 3) {
		start_ip.assign("0.0.0.0");
		end_ip.assign("255.255.255.255");
	} else {
		start_ip.assign(argv[1]);
		end_ip.assign(argv[2]);
	}
	int sa,sb,sc,sd, ea, eb, ec, ed;

	sa = atoi(start_ip.substr(0,start_ip.find(".")).c_str());
	start_ip = start_ip.substr(start_ip.find(".") + 1);

	sb = atoi(start_ip.substr(0,start_ip.find(".")).c_str());
	start_ip = start_ip.substr(start_ip.find(".") + 1);

	sc = atoi(start_ip.substr(0,start_ip.find(".")).c_str());
	start_ip = start_ip.substr(start_ip.find(".") + 1);

	sd = atoi(start_ip.c_str());

	ea = atoi(end_ip.substr(0,end_ip.find(".")).c_str());
	end_ip = end_ip.substr(end_ip.find(".") + 1);

	eb = atoi(end_ip.substr(0,end_ip.find(".")).c_str());
	end_ip = end_ip.substr(end_ip.find(".") + 1);

	ec = atoi(end_ip.substr(0,end_ip.find(".")).c_str());
	end_ip = end_ip.substr(end_ip.find(".") + 1);

	ed = atoi(end_ip.c_str());

	NET_DEVICEINFO deviceInfo ={0};
	unsigned long m_LoginID;
	
	BOOL ret = CLIENT_Init(DisConnectFunc,0); 

	if(ret) {	

		CLIENT_SetSnapRevCallBack(SnapPicRet,0);	

		cout << "Init OK." << endl;
	} else {
		cout << "Init Fail." << endl;
	}

	for (int a = sa; a <= ea; a++){
		for( int b = sb; b <= eb; b++){
			for( int c = sc; c <= ec; c++){
				for( int d = sd; d <= ed; d++){

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
						Sleep(3000);
					}
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

