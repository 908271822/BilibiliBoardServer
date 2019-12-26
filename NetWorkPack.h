/*********************************************************************************************
���ݰ�����
��ͷ(���ֽ�)+(�汾1�ֽ�)+����Ϣ������4�ֽ�,�������ȣ��Ӱ�ͷ�汾��)+(��Ϣid4���ֽ�)+(�û�����4�ֽ�)+(��Ϣ��)
***********************************************************************************************/







#pragma once
#include "Helper.h"
///�����ͷ��ǰ������
#define PACKHEAD0  0x08
#define PACKHEADENCRYPT	0x09
#define PACKHEAD1  8


///���İ汾��
#define PACKVERSION 1


///�����ͷƫ����
#define  PACK_HEAD_OFFSET        0
#define  PACK_VERSION_OFFSET     2
#define  PACK_LENGTH_OFFSET      3
#define  PACK_MESSSAGEID_OFFSET  7
#define  PACK_USERDATA_OFFSET	11
#define	 PACK_SERVERID_OFFSET	15		///������id;
#define  PACK_MESSAGE_OFFSET     19		///��������;

#define  PACK_HEAD_SIZE         19

///���İ���С
//#define  MAXPACKSIZE 65536
//#define  MAXMESSAGESIZE  65536 - 9





class XClass NetWorkPack
{

public:
	NetWorkPack();


	/**���캯��
	*@param ModeID ģ��id
	*@param MessageID ��Ϣid
	*@param pdata ������
	*@param leght pdata�ĳ���
	*/
	NetWorkPack(uint32 MessageID,const void* pdata,uint32 length,bool encrypt,uint32 userdata=0,int32 serverId = 0);

	/**���·������С
	*/
	bool  resize(uint32 messageid,uint32 userdata,int32 serverId, uint32 messageLength);


	/**
	*/
	virtual ~NetWorkPack();

	/**���ð�
	*/
	void  reset();


	/**��ȡ���ݰ�(��Ϣ��)�Ĵ�С
	*/
	uint32  getBodyLength();


	/**��ȡ��Ϣ�汾*/
	uint8 getVersion()const;
	
	/**��ȡ��Ϣid
	*/
	uint32 getMessagID()const ;

	/**��ȡ���û�����
	*/
	uint32 getUserData()const;

	/**�����û�����*/
	void setUserData(uint32 userdata);

	/**��ȡ���еķ�����id;*/
	int32 getServerId() const;


	/**��ȡ������С��������ͷ
	*/
	uint32  getSize()const ;



	/**��ȡ��Ϣ������
	*/
	const char* getData() ; 

		/**��ȡ��Ϣ������
	*/
	char* getRawData()
	{
		if(m_pbuffer==NULL)
		{
			return NULL;
		}
		return 	m_pbuffer+PACK_MESSAGE_OFFSET;
	}
	

	bool getIsEncrypt()
	{
		return m_encryptpack;
	}

	void setEncrpyt()
	{
		m_encryptpack=true;
	}


	/**��ȡ�����ݣ���������ͷ*/
	const char* getBuffer()const {return m_pbuffer;}


	/**�ж�phead�Ƿ�����ȷ�İ�ͷ��Ϣ
	*/
	static bool isPackHead(const char* pHead,uint16 length,bool &isencrypt);
	


	/**��ȡ���ķ����ߵ�ip
	*/
	const xstring& getIP()const {return m_IP;}

	/**��ȡ�����ߵĶ˿ں�*/
	const uint32 getPort()const
	{
		for(uint32 r=0;r<m_IP.length();r++)
		{
			if(m_IP[r]==':')
			{
				return Helper::StringToInt(m_IP.c_str()+r+1);
			}
		}
		return 0;
	}

	void setIP(const xstring& ip){m_IP=ip;}

protected:

	char*  m_pbuffer;///���ݿ�

	char *	m_pbufferDec;

	uint32 m_Size;///m_pbuffer����


	xstring m_IP;
	
	bool m_encryptpack;
};


typedef boost::shared_ptr<NetWorkPack> NetWorkPackPtr;