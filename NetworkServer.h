

#pragma once
#include "Singleton.h"
#include "Connection.h"
#include "NetWorkPack.h"
#include "io_service_pool.h"
#include "EventManager.h"
#ifndef LIBSY_NOUSEPROTOBUFF
#include <google/protobuf/message.h>
#endif

typedef boost::function<void (NetWorkPackPtr  pPack  )> NetworkFun;
///���������
class NetWorkLister
{
public:

	NetWorkLister(){}


	virtual void  onConnect(const xstring& ip)=0;

	virtual void  onCloseConnect(const xstring& ip)=0;

	virtual void  onMessage(NetWorkPackPtr pack)=0;


};

typedef std::vector<NetWorkLister*> NetWorkListerVector;






class Connection;


///���ջذ�����
class ReceivePacker
{
public:

	ReceivePacker(){}


	virtual ~ReceivePacker(){}


	/**����һ�������
	*/
	void     addPack(const NetWorkPackPtr& pack)
	{
		if(pack.use_count()==0)
		{
			return ;
		}
		boost::mutex::scoped_lock lock(m_ReceivePackCollect_Mutex);
		m_ReceivePackCollect.push_back(pack);
	}


protected:


	typedef std::deque<NetWorkPackPtr> NetWorkPackCollect;

	NetWorkPackCollect                 m_ReceivePackCollect;

	boost::mutex                       m_ReceivePackCollect_Mutex;///��������


};





///



class XClass NetworkServer  :public Singleton<NetworkServer> ,public  ReceivePacker ,public CEventManager
{


	 friend class Connection;

public:


	/**���캯��
	*@param IOthreadNumber io�߳�����
	*@param workThreadNumber �����߳�����
	*/
	NetworkServer(uint8 IOthreadNumber=4);


	virtual ~NetworkServer();


	/**����������
	*@param portnumber listen network portnumber
	*@return return true if sucess else return false
	*/
	bool start(uint32 portnumber,uint32 maxConnect=65320);


	/**�رշ�����
	*/
	void stop();


	int32 getLastMsgRole()
	{
		return m_LastMsgRole;
	}


	int32 getlastMsgID()
	{
		return m_LastMsgID;
	}


	/**����ÿ֡�հ������㲥�հ���Ϣ���㲥���ӺͶϿ���Ϣ
	*
	*/
	void update();

	/**ˢ֡����;*/
	//void netUpdate();

	/**������Ϣ
	*@param messageid ��Ϣid
	*@param pdata ��Ϣ����
	*@param ip     Ŀ��ip
	*@param �첽����,����true ��ʾ���͵����Ͷ��е���
	*/
	bool send(uint32 messageid,const char* pdata,uint32 length,const xstring& ip,bool isencrypt,uint32 userdata = 0,int32 serverId = 0);


	template<typename T>
	bool send(uint32 mesageid ,const T& t,const xstring& ip,bool isencrypt, uint32 userdata = 0,int32 serverId = 0)
	{
		return send(mesageid,(const char *)&t,sizeof(T),ip,isencrypt,userdata,serverId);
	}
#ifndef LIBSY_NOUSEPROTOBUFF

	bool sendProtoBuf(uint32 mesageid ,const  ::google::protobuf::Message & pdata,const xstring& ip,bool isencrypt)
	{
		std::string buf;
		pdata.SerializePartialToString(&buf);
		return send(mesageid,buf.c_str(),(uint32)buf.length(),ip,isencrypt,0,0);
	}
#endif
	/*template<typename T>
	bool sendProtoBuf(uint32 mesageid ,const  T & pdata,const xstring& ip,uint32 userdata = 0,int32 serverId = 0)
	{
		std::string buf;
		pdata.SerializePartialToString(&buf);
		return send(mesageid,buf.c_str(),(uint32)buf.length(),ip,userdata,serverId);
	}
*/

	/**ͨ��ip:port����ȡָ��������
	*���Ƕ�̰�ȫ����
	*@return ��Ҫ�ж��Ƿ���ȷ��ȡ������
	*/
	ConnectionPtr getConnectByIP(const xstring& ip);


	/**���ӵ�Զ�̻���
	*/
	bool connect(const xstring& ip ,uint16 portnumber);

	/**�Ͽ�һ������
	*/
	bool closeConnect(const xstring& ip);

	/**����һ������
	*/
	void addLister(NetWorkLister* pLister);


	/**�Ƴ�һ������
	*/
	void removeLister(NetWorkLister* pLister);

	/**�����հ���������;*/
	void setRecivePackFunc(NetworkFun func)	{m_RecivePackFunc = func;}

private:

	void start(xstring hostname,uint32 portnumber,uint32 maxConnect);

protected:


	///����asio::io_server����
	 boost::asio::io_service& getIOServer(){return m_IOServerPool.get_io_service();}


	 /**�ȴ����Ӳ���ִ��
	 */
	 void waitConnect(uint32 index);



	 ///�пͻ������ӵĻص�����,����ִ�С�
	 void handleConnect(ConnectionPare pConnectPare,const boost::system::error_code& e);



public:
	 /**֪ͨ�����ӹر�
	 */
	 void notifyCloseConnect(const xstring& ip);


	 /**֪ͨ�����ӽ���
	 */
	 void notifyNewConnect(const xstring&   ip);







protected:



	io_service_pool                m_IOServerPool;///�����̳߳�


	std::vector<boost::asio::ip::tcp::acceptor*> m_pAcceptorCollects;


	///�������Ӽ���
	//typedef std::list<ConnectionPtr> ConnectList;
	typedef xHashMap<xstring,ConnectionPtr>ConnectMap;


	///�������ӵļ���
	ConnectMap         m_ConnectCollect;
	boost::mutex       m_ConnectCollect_Mutex;



	typedef std::deque<xstring> ConnectDeque;
	///���Ե�������
	ConnectDeque        m_NewConnectCollect;
	///�����Ӽ�����
	boost::mutex       m_NewConnectCollect_Mutex;


	///���жϿ�����
	ConnectDeque        m_CloseConnectCollect;
	///�Ͽ�������
	boost::mutex       m_CloseConnectCollect_Mutex;




	////���������
	NetWorkListerVector   m_Listeners;

	NetworkFun m_RecivePackFunc;				///��ȡ�����������ݰ���������;

	int32 m_LastMsgRole;				//�������Ϣ�Ľ�ɫ
	int32 m_LastMsgID;					//����͵���ϢID
};
