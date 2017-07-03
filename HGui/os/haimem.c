//---------------------------------------------------------------------------------
// Copyright (c) Haisoft 2006-9-1
// Copyright jinhailiao 2008-2010
// E-mail:   jinhailiao@163.com
// Project:			HGui
// File:			haimem.c
// Description:		
//-------------------------------------------------------------
// Reversion Histroy:
//-------------------------------------------------------------
// Version		date		operations				by who
// 1.0.0		2006-09-01	Create					Kingsea
// 2.0.0		2006-09-06  update                  Kingsea
// 3.0.0		2008-03-01	update					Kingsea
//---------------------------------------------------------------------------------

#include  "haitype.h"


typedef struct tagMemNode
{
	S_DWORD size;
	struct tagMemNode *Next;
}
S_MEMNODE, *SP_MEMNODE;


#define   NODESIZE				sizeof(S_MEMNODE)
#define   PMEM2PNODE(pMem)		((SP_MEMNODE)(((S_BYTE*)(pMem))-sizeof(S_DWORD)))
#define   PNODE2PMEM(pNode)		((S_VOID*)(((S_BYTE*)(pNode))+sizeof(S_DWORD)))

static S_MEMNODE *pMemHeap = S_NULL;


static S_VOID _JoinMemNode(SP_MEMNODE pPre, SP_MEMNODE pNext)
{
	if ((((S_BYTE*)pPre) + pPre->size + sizeof(S_DWORD)) == ((S_BYTE*)pNext))
	{
		pPre->Next = pNext->Next;
		pPre->size = pPre->size + pNext->size + sizeof(S_DWORD);
	}
}

S_VOID  hai_InitMemHeap(S_VOID *pMem, S_DWORD size)
{
	pMemHeap = S_NULL;

	if (pMem && size>=sizeof(S_MEMNODE))
	{
		pMemHeap = (SP_MEMNODE)pMem;
		pMemHeap->size = (size-sizeof(S_DWORD)) & (~((S_DWORD)(sizeof(S_DWORD)-1)));
		pMemHeap->Next = S_NULL;
	}
}

S_VOID  *hai_MemAlloc(S_DWORD size)
{
	SP_MEMNODE pPre, pCur, pNext;

	if (!size)
		return S_NULL;

	size = (size+3) & (~((S_DWORD)(sizeof(S_DWORD)-1)));
	for (pPre=S_NULL, pCur=pMemHeap; pCur && pCur->size<size; pPre=pCur, pCur=pCur->Next)
		;

	if (pCur == S_NULL)
		return S_NULL;

	if (pCur->size >= size+NODESIZE)
	{
		pNext = (SP_MEMNODE)(((S_BYTE*)pCur)+size+sizeof(S_DWORD));
		pNext->size = pCur->size - size - sizeof(S_DWORD);
		pNext->Next = pCur->Next;
		pCur->size  = size;
		pCur->Next  = pNext;
	}

	if (pPre)
		pPre->Next = pCur->Next;
	else
		pMemHeap = pCur->Next;

	return PNODE2PMEM(pCur);
}

S_BOOL  hai_MemFree(S_VOID *pMem)
{
	SP_MEMNODE pPre, pCur, pNext;

	if (pMem == S_NULL)
		return S_FALSE;

	pCur = PMEM2PNODE(pMem);
	for (pPre=S_NULL, pNext=pMemHeap; pNext && pNext<pCur;pPre=pNext, pNext=pNext->Next)
		;

	if (pCur == pNext)
		return S_FALSE;

	pCur->Next = pNext;
	if (pPre)
		pPre->Next = pCur;
	else
		pMemHeap = pCur;

	if (pNext)
		_JoinMemNode(pCur, pNext);
	if (pPre)
		_JoinMemNode(pPre, pCur);

	return S_TRUE;
}

S_DWORD hai_MemStatus(S_DWORD *pBlkNum, S_DWORD *pMaxBlkSize)
{
	S_DWORD spc,num,size;
	S_MEMNODE *p = pMemHeap;

	spc = 0;
	num = 0;
	size= 0;
	while(p)
	{
		num++;
		if(p->size > size)
			size = p->size;
		spc += p->size+sizeof(S_DWORD);
		p = p->Next;
	}

	/*return results*/
	if(pBlkNum)
		*pBlkNum = num;
	if(pMaxBlkSize)
		*pMaxBlkSize = num?size+sizeof(S_DWORD):0;
	return spc;
}