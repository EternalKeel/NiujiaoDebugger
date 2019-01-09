#pragma once
/*
��������������ؽṹ�ʹ�����
*/
#include <Windows.h>
#include "ImageInfo.h"
#include "Disasm.h"

static enum {
	BP_System = 0x1,
	BP_ModuleEntry = 0x10,
	BP_WinMain = 0x100,
	BP_CreateThread = 0x1000,
	BP_ExitThread = 0x10000,
	BP_LoadDll = 0x100000,
	BP_UnloadDll = 0x1000000,
	BP_DebugString = 0x10000000,
	BP_BreakPoint = 0x100000000
}BP_TYPE;//�ϵ�����

static enum {
	DBG_NONE,
	DBG_STOP,
	DBG_SUSPEND,
	DBG_RESTART,
	DBG_RUN,
	DBG_STEPINTO,
	DBG_STEPOVER,

	DBG_TRACE,
	DBG_AUTOSTEPINTO,
	DBG_AUTOSTEPOVER,
	DBG_EXETORET,
	DBG_EXETOUSERSPACE,
}DEBUGER_ENVENT;
static enum {
	ST_RUNNING,
	ST_CLOSED,
	ST_BREAKING
}DBG_STATE;//�ϵ��¼�
class Executor
{
private:
	Executor();
	~Executor();
	Executor(const Executor&) {};
	Executor& operator=(const Executor&) const { };
	static Executor *ExeInstance;  //ִ����ʵ��
	SRWLOCK m_srwStateLock; //��д��
	SRWLOCK m_srwCommandLock; //��д��

	DWORD m_debuggerState;  //������״̬  �ɵ����߳�д �����̶߳�
	DWORD m_controlCommand; //��������   �����߳�д  �����߳�ִ����ɺ������״̬

	HANDLE m_hDebuggerThread;

	TCHAR m_fileName[1024];
	TCHAR m_parameter[1024];
	TCHAR m_environment[1024];


	HANDLE m_currentThread; //Ĭ��Ϊ���߳�

	DWORD m_debuggedProcessId;

	DWORD m_currentDebuggedProcessId;
	DWORD m_currentDebuggedThreadId;

	bool m_firstBreak; //�״��ж�


public:
	static Executor* GetInstance();
	bool IsRunning();
	bool DebuggerStart(LPCTSTR FileN = nullptr, LPCTSTR ExecuteP = nullptr, LPCTSTR Env = nullptr);


	void SetDebuggerState(DWORD);  //���õ���������״̬
	DWORD GetDebuggerState();  //��ȡ����������״̬
	void SetControlCommand(DWORD);  //���ÿ�������
	DWORD GetControlCommand();  //��ȡ��������
	bool DebuggerRestart();
	bool DebuggerClose();
	bool DebuggerSuspend();
	bool DebuggerRun();
	bool DebuggerStepInto();
	bool DebuggerStepOver();

	bool SetSoftBreakPoint(DWORD addr);
	static DWORD WINAPI StartDebugThread(LPVOID lParam);

//�����ǵ����߳�ʹ�õĺ���
//�����ǵ����߳�ʹ�õĺ���
	
	void MessageLoop();
	void DbgMessageLoop();
	bool OnExceptionAccessViolationEvent(DEBUG_EVENT*) const;
	bool OnExceptionBreakPointEvent(DEBUG_EVENT*) const;
	bool OnExceptionDataTypeMisalignmentEvent(DEBUG_EVENT*) const;
	bool OnExceptionSingleStepEvent(DEBUG_EVENT*) const;
	bool OnDbgControlCEvent(DEBUG_EVENT*) const;
	bool OnCreateThreadEvent(DEBUG_EVENT*) const;
	bool OnCreateProcessEvent(DEBUG_EVENT*);
	bool OnExitThreadEvent(DEBUG_EVENT*) const;
	bool OnExitProcessEvent(DEBUG_EVENT*) const;
	bool OnLoadDllEvent(DEBUG_EVENT*) const;
	bool OnUnloadDllEvent(DEBUG_EVENT*) const;
	bool OnRipEvent(DEBUG_EVENT*) const;
	

	bool GetFileNameFromHandle(const HANDLE, char*) const;

	bool StopDebugger();
};