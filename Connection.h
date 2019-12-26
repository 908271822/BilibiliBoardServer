

#pragma  once



#include "NetWorkPack.h"
#include <queue>

class ReceivePacker;
class NetworkServer;


class XClass Connection :public  boost::enable_shared_from_this<Connection>
{

	friend  class  NetworkServer;
	friend  class  NetworkClient;

protected:

	

	Connection(boost::asio::io_service& ioserver,ReceivePacker* pReceivePack);

public:

	virtual ~Connection();



	/**��ʼ��ȡ����
	*/
	bool startRead();



	/**��ȡ���ӶԷ���ip��ַ�Ͷ˿ں�
	*@param portnumber true: 127.0.0.1:344  false:127.0.0.1
	*/
	const xstring& getIP()const ;



	/**see getIP
	*/
	void setIP(const xstring& p){m_IP=p;};



public:




	/**������Ϣ���첽���ͻ����͵����Ͷ��С�Ȼ���ٷ���
	*@param message ��Ϣid
	*@param pdata ��Ϣ����
	*@param lenght pdata����
	*/
	bool send(uint32 messageid,const char* pdata,uint32	lenght,bool isencrypt,uint32 roleid=0,int32 serverId = 0);


	template<typename T>
	bool send(uint32 messageid,const T& t)
	{
		return send(messageid,&t,sizeof(T));
	}
	///�ر�����
	void close();


	boost::asio::ip::tcp::socket& getSocket(){return m_Socket;}

protected:



	

	/**��sock�����ݵ���ʱ�Ļص�����,������Ϣ��ͷ
	*@
	*/
	void handReadHead(const boost::system::error_code& error, size_t len);



	/**��������ص�
	*/
	void handReadBody(const boost::system::error_code& error, size_t len);



	/**�ӷ��Ͷ�����ȡ���ݷ���
	*/
	bool doSend();


	/**��ȡд��ص�
	*/
	void handWrite(const boost::system::error_code& e);



protected:

	boost::asio::ip::tcp::socket  m_Socket;

	/////���ջ���
	//std::vector<char>  m_ReceiveBuffer;
	
	xstring           m_IP;
	char              m_MessageHead[PACK_HEAD_SIZE];///����ÿ���յ��İ�ͷ��Ϣ

	NetWorkPackPtr    m_ReceivePack;///���ڶ�ȡ�İ���Ϣ


	typedef std::vector<NetWorkPackPtr> NetWorkPackQueue;
	NetWorkPackQueue   m_SendPackCollect;  ///���еķ��ͻ��弯��
	boost::mutex       m_SendPackCollect_Mutex;///���Ͱ�����


	///���ڷ��͵������
	bool               m_Sending;///�Ƿ����ڷ�����


	ReceivePacker*        m_ReceivePacker;


	bool             m_isClose;


	char* m_pSendBuff;

	uint32 m_iBuffSize;

	//int32 m_lastSendSize;
	int32 m_iSendPackCount;

};


typedef boost::shared_ptr<Connection> ConnectionPtr;

struct ConnectionPare
{
	ConnectionPtr ptr;
	uint32 index;
};