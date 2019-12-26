

#include "stdafx.h"
#include "Connection.h"
#include "NetworkServer.h"
#include "NetWorkClient.h"
#include "Helper.h"
#include "NetWorkPack.h"
#include "xLogManager.h"
#define MAX_SEND_LEN 10240


//----------------------------------------------------------------------------------
Connection::Connection(boost::asio::io_service& ioserver,ReceivePacker* pReceivePack)
	:m_Socket(ioserver),m_Sending(false),m_ReceivePacker(pReceivePack), m_isClose(false)
{
	
	m_pSendBuff = NULL;
	m_iBuffSize = 0;
}

//----------------------------------------------------------------------------------
Connection::~Connection()
{
	if(m_Socket.is_open())
	{
		m_Socket.close();
	}

	boost::system::error_code ignored_ec;
	m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

	SafeDelete(m_pSendBuff);
	m_iBuffSize = 0;

}



//----------------------------------------------------------------------------------
bool Connection::startRead()
{
	
	if(m_Socket.is_open()==false)
	{
	  return  false;
	}

	try
	{
		xstring ipaddr=m_Socket.remote_endpoint().address().to_string();
		uint16 port=m_Socket.remote_endpoint().port();
		///��ȡip
		m_IP=ipaddr+_TEXT(":")+Helper::IntToString(port);
	}
	catch( boost::system::system_error* e)
	{
		if(e!=NULL)
		{
			//remote_endpoint
			xLogMessager::getSingleton().logMessage(e->what(),Log_ErrorLevel);
		}

	}
	catch (...)
	{

		return  true;


	}

	memset(m_MessageHead,0,sizeof(m_MessageHead));

	boost::asio::async_read(m_Socket,
		boost::asio::buffer(m_MessageHead,sizeof(m_MessageHead)),
		bind(
		&Connection::handReadHead,
		shared_from_this(),
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
		)
		);
	/*m_Socket.async_read(boost::asio::buffer(m_MessageHead,sizeof(m_MessageHead)),
		bind(
		&Connection::handReadHead,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred
		)
		);*/


	

	

	return   true	;

}

//----------------------------------------------------------------------------------
void Connection::close()
{


	 if( m_isClose==true)
	 { 
		 return ;
	 }

	  m_isClose=true;
	if(getIP().length()>0)
	{
		NetworkServer* pServer=dynamic_cast<NetworkServer*>(m_ReceivePacker);
		if(pServer!=NULL)
		{
			pServer->notifyCloseConnect(getIP());
		}
		else
		{
			NetworkClient *pClinet =dynamic_cast<NetworkClient*>(m_ReceivePacker);
			if(pClinet!=NULL)
			{
				pClinet->notifyCloseConnect(getIP());
			}
		}
	}
	if(m_Socket.is_open())
	{
		m_Socket.close();
		boost::system::error_code ignored_ec;
		m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec); 
	}

	
}

//----------------------------------------------------------------------------------
const xstring& Connection::getIP()const 
{
	return m_IP;
}


//----------------------------------------------------------------------------------
bool Connection::send(uint32 messageid,const char* pdata,uint32	lenght,bool isencrypt,uint32 roleid/*=0*/,int32 serverId /*= 0*/)
{
	if(pdata==NULL||lenght==0)
	{
		return false;
	}

	NetWorkPackPtr pPack(new NetWorkPack(messageid,pdata,lenght,isencrypt,roleid,serverId));
	///��סѹ�뵽���Ͷ��е���ȥ
	boost::mutex::scoped_lock lock(m_SendPackCollect_Mutex);
	m_SendPackCollect.push_back(pPack);


	doSend();

	///ִ����ʵ�ķ��Ͷ���
	return true;


}



//----------------------------------------------------------------------------------
bool  Connection::doSend()
{
	///������ڷ����о�ֱ�ӷ���
	if(m_Sending==true)
	{
		return false;
	}


	///�Ӷ�����ȡ��������������оͷ��͵�һ��
	if(m_SendPackCollect.empty())
	{
		return false;
	}

	m_Sending=true;
/*
	NetWorkPackPtr pPack=m_SendPackCollect[0];

	const char* pdata = pPack->getBuffer();
	uint32 size = pPack->getSize();
	boost::asio::async_write(m_Socket, boost::asio::buffer(pdata,size),boost::bind(&Connection::handWrite, shared_from_this(),boost::asio::placeholders::error));

	

#ifdef WIN32
	{
		uint32 id = pPack->getMessagID();
		int32 roleid = pPack->getUserData();

		const char* pdata = pPack->getBuffer();
		uint32 size = pPack->getSize();


		xstring strProcessName;
		xAppliction* pApplication = xAppliction::getSingletonPtr();
		if(pApplication!=NULL)
			strProcessName = pApplication->getApplicationName();

		Helper::dbgTrace("taitan %s send: role=%d, sender=%s, sendlen=%d, msgid=%d\n",strProcessName.c_str(),roleid,m_IP.c_str(),size,id);
	}
#endif
*/

	uint32 nFullSize = 0;
	//��¼���е�
	m_iSendPackCount = 0;
	for(m_iSendPackCount=0; m_iSendPackCount<(int32)m_SendPackCollect.size(); m_iSendPackCount++)
	{
		NetWorkPackPtr pPack=m_SendPackCollect[m_iSendPackCount];
		uint32  size=pPack->getSize();

		nFullSize += size;

		if(nFullSize>MAX_SEND_LEN)
		{
			if(m_iSendPackCount==0)
				++m_iSendPackCount;	//ֻ����һ����
			else	//��ǰ���жϵİ�������
				nFullSize-=size;
			break;
		}
	}

	int32 roleid=0;
	int32 serverid=0;
	xstring strMsgID;

	if(m_iSendPackCount>1)
	{
		//�����Ͱ�����1ʱ�����仺��ռ�
		if(m_pSendBuff==NULL)
		{
			m_iBuffSize = nFullSize;
			if(nFullSize<MAX_SEND_LEN)
				m_iBuffSize = MAX_SEND_LEN;

			m_pSendBuff = new char[m_iBuffSize];
			CHECKERRORANDRETURNRESULT(m_pSendBuff!=NULL);
		}
		else
		{
			if(m_iBuffSize<nFullSize)
			{
				SafeDelete(m_pSendBuff);
				m_pSendBuff = new char[nFullSize];
				CHECKERRORANDRETURNRESULT(m_pSendBuff!=NULL);
				m_iBuffSize = nFullSize;
			}
		}

		uint32 iCurrPos = 0;
		for(int32 i=0; i<m_iSendPackCount; i++)
		{
			NetWorkPackPtr pPack=m_SendPackCollect[i];
			const char* pdata = pPack->getBuffer();
			uint32 size = pPack->getSize();
			memcpy(m_pSendBuff+iCurrPos,pdata,size);
			iCurrPos += size;

			roleid = pPack->getUserData();
			serverid = pPack->getServerId();
			strMsgID += Helper::IntToString(pPack->getMessagID());
			strMsgID += ",";
		}

		boost::asio::async_write(m_Socket, boost::asio::buffer(m_pSendBuff,nFullSize),boost::bind(&Connection::handWrite, shared_from_this(),boost::asio::placeholders::error));
		m_SendPackCollect.erase(m_SendPackCollect.begin(),m_SendPackCollect.begin()+m_iSendPackCount);
	}
	else
	{//ֻ��һ����
		NetWorkPackPtr pPack=m_SendPackCollect[0];

		const char* pdata = pPack->getBuffer();
		uint32 size = pPack->getSize();
		boost::asio::async_write(m_Socket, boost::asio::buffer(pdata,size),boost::bind(&Connection::handWrite, shared_from_this(),boost::asio::placeholders::error));

		roleid = pPack->getUserData();
		serverid = pPack->getServerId();
		strMsgID += Helper::IntToString(pPack->getMessagID());
	}

	//std::cout << " send to" << m_IP << ": role=" << roleid << ",serverID=" << serverid << ",count=" << m_iSendPackCount << ",sendlen=" <<nFullSize << ", msgid=" << strMsgID << std::endl;


	return true;
}

//----------------------------------------------------------------------------------
void  Connection::handWrite(const  boost::system::error_code& e)
{
	if(!e)
	{
		boost::mutex::scoped_lock lock(m_SendPackCollect_Mutex);

		m_Sending=false;
		///�������ڷ��͵Ķ���
		if(m_SendPackCollect.empty()==false)
		{
			//m_SendPackCollect.pop();
			if(m_iSendPackCount==1)	//ֻ��һ���������
				m_SendPackCollect.erase(m_SendPackCollect.begin());
		}

		///��������л��С����Ͷ������
		doSend();



	}
	//else
	//{
		//close();
	//}

}


//----------------------------------------------------------------------------------
void  Connection::handReadHead(const boost::system::error_code& error, size_t len)
{
	///���Ӻ�ĵ�һ����Ҫ����������֤���ӵİ�ȫ�ԣ�
	///��ȫ������ͨ���󣬰��յ���������ת�ɰ����ݣ��ٷŵ�networkserver������

	if(!error)
	{
		bool isencrypt=false;
		///����յ���ȷ�İ�ͷ���ͼ���������
		if(NetWorkPack::isPackHead(m_MessageHead,sizeof(m_MessageHead),isencrypt))
		{
			
			NetWorkPackPtr pPack(new NetWorkPack());
			m_ReceivePack=pPack;
			uint32* pMessageid=(uint32*)(m_MessageHead+PACK_MESSSAGEID_OFFSET);
			uint32* plength=(uint32*)(m_MessageHead+PACK_LENGTH_OFFSET);
			uint32 temleng=(*plength);
			temleng=temleng-PACK_HEAD_SIZE;
			uint32* userdata=(uint32*)(m_MessageHead+PACK_USERDATA_OFFSET);
			int32* serverId = (int32*)(m_MessageHead+PACK_SERVERID_OFFSET);
			if(isencrypt)
			{
				m_ReceivePack->setEncrpyt();
			}
			m_ReceivePack->resize(*pMessageid,*userdata,*serverId,temleng);
			///���̶�����
			boost::asio::async_read
				(m_Socket,
				boost::asio::buffer(m_ReceivePack->getRawData(),m_ReceivePack->getBodyLength()),
				bind(
				&Connection::handReadBody,
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
				)
				);

			/*m_Socket.async_read(boost::asio::buffer(m_ReceivePack->getData(),m_ReceivePack->getBodyLength()),
				bind(
				&Connection::handReadBody,
				this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
				)
				);
*/
		}
		
	}else 
	{
		close();

	}

	

}


//----------------------------------------------------------------------------------
void Connection::handReadBody(const boost::system::error_code& error, size_t len)
{
	if(!error)
	{
		
		if(m_ReceivePacker!=NULL&&m_ReceivePack.use_count()!=0)
		{

			m_ReceivePack->setIP(getIP());
			m_ReceivePacker->addPack(m_ReceivePack);
			///�ѻ�ȡ�İ�����ȥ
			//std::cout << "receive from " << m_IP << ": userData=" << m_ReceivePack->getUserData() << ", serverid=" << m_ReceivePack->getServerId() << ", sendlen=" << m_ReceivePack->getBodyLength() << ", msgid=" << m_ReceivePack->getMessagID() << std::endl;

		}
		m_ReceivePack.reset();

		

		///���̶���ͷ
		startRead();

	}else///�����ȡ���� ���رն�ȡ
	{
		close();
	}


}