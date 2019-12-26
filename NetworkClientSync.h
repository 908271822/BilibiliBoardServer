#pragma  once

#include "Helper.h"
#include "NetworkServer.h"
class XClass NetworkClientSync
{
public:
	NetworkClientSync();
	NetworkClientSync(xstring &ipport);

	~NetworkClientSync();

	
	/**���ӵ�Զ�̷�����,�Ƕ�����,�˺�������ִ������,�ڷ��͵�ʱ������,���Ͳ��յ��ذ���ɺ�Ͽ�
	*/
	void setaddress(const xstring& serverip,uint32 portnumber);

	//������д˺���,������������.
	bool connect();

	bool closeconnect();

	void setUserData(uint32 userdata)
	{
		m_userdata=userdata;
	}
	void setTimeOutVal(uint32 val)
	{
		m_timeoutval=val;
	}

		/**��������
	*/
	bool send(uint32 messageid, const char* pdata,uint32 length,uint32 retmessageId,char **retData,uint32 &retlength,bool noneedret=false);

	bool send(uint32 messageid, const char* pdata,uint32 length,uint32 retmessageId);
#ifndef LIBSY_NOUSEPROTOBUFF
	template<typename T>
	bool sendProtoBuf(uint32 mesageid ,const  ::google::protobuf::Message & pdata,uint32 retmessageId, T & retdata)
	{
		std::string buf;
		pdata.SerializePartialToString(&buf);
		char *retDataBuff=NULL;
		uint32 retlength=0;
		bool ret= send(mesageid,buf.c_str(),(uint32)buf.length(),retmessageId,&retDataBuff,retlength);
		if(ret==false)
		{
			return false;
		}
		if(retDataBuff==NULL||retlength==0)
		{
			//MessageBox(NULL,"socket���ؿ�����","",MB_OK);
			return false;
		}
		if(!retdata.ParseFromArray(retDataBuff,retlength))
		{
			//MessageBox(NULL,"���ݽ�������",Helper::IntToString(mesageid).c_str(),MB_OK);
			SafeDeleteArray(retDataBuff);
			return false;
		}
		SafeDeleteArray(retDataBuff);
		return true;
	}
#endif
private:

	void initvalue();


	xstring m_ip;
	uint32 m_port;
	uint32 m_userdata;
	uint32 m_timeoutval;
	// ����asio�඼��Ҫio_service����
	boost::asio::io_service iosev;
	// socket����
	boost::asio::ip::tcp::socket *socket;
};