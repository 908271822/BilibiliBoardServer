
/******************************************************************************
����ģ��ͻ�
******************************************************************************/



#pragma  once


#include "NetworkServer.h"



class XClass NetworkClient :public ReceivePacker ,public CEventManager
{
public:

	NetworkClient();


	virtual ~NetworkClient();


	/**����һ������
	*/
	void addLister(NetWorkLister* pLister);


	/**�Ƴ�һ������
	*/
	void removeLister(NetWorkLister* pLister);


	/**���ӵ�Զ�̷�����
	*/
	bool connect(const xstring& serverip,uint32 portnumber);

	bool connect(const xstring& ipport);


	/**�ر�����
	*/
	bool closeConnect();
	/**֪ͨ�����ӹر�
	 */
	 void notifyCloseConnect(const xstring& ip);

	void setUserData(uint32 data);
	/**��������
	*/
	bool send(uint32 messageid, const char* pdata,uint32 length,bool isencrypt,uint32 roleid=0,uint32 serverid=0);
#ifndef LIBSY_NOUSEPROTOBUFF
	bool sendProtoBuf(uint32 mesageid ,const  ::google::protobuf::Message & pdata,bool isencrypt,uint32 roleID=0,uint32 serverid=0)
	{
		std::string buf;
		pdata.SerializePartialToString(&buf);
		return send(mesageid,buf.c_str(),(uint32)buf.length(),isencrypt,roleID,serverid);
	}
#endif

	/**ÿ֡����
	*/
	void update();


	bool isConnect()const {return m_isConnect;}
	
	bool isFinishConnect()const {return m_isFinishConnect;}

protected:


	///����asio::io_server����
	boost::asio::io_service& getIOServer(){return m_IOServerPool.get_io_service();}

	
	 void handleConnect(ConnectionPtr pConnect,const boost::system::error_code& e);


protected:

	io_service_pool                m_IOServerPool;///�����̳߳�

	ConnectionPtr              m_pConnect;///Զ������

	NetWorkListerVector        m_Listeners;///��������

	bool                       m_isConnect ;

	bool							m_isFinishConnect;

	uint32						m_userData;
	
	typedef std::deque<xstring> ConnectDeque;
	///���Ե�������
	ConnectDeque        m_NewConnectCollect;
	///�����Ӽ�����
	boost::mutex       m_NewConnectCollect_Mutex;
	///���жϿ�����
	ConnectDeque        m_CloseConnectCollect;
	///�Ͽ�������
	boost::mutex       m_CloseConnectCollect_Mutex;


};