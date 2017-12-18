/*
*
*	ATLMap �� �̿��Ͽ� �����͸� �����ϴ� Hash Map Ŭ������ �����Ѵ�.
*
*	Copyright (c) 2008 Hong Sung Pyo [dbongman@gmail.com]
*
*	BaseClasses is free software; you can redistribute it and/or
*	modify it under the terms of the GNU Lesser General Public
*	License as published by the Free Software Foundation; either
*	version 2.1 of the License, or (at your option) any later version.
*
*	BaseClasses is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*	Lesser General Public License for more details.
*
*	You should have received a copy of the GNU Lesser General Public
*	License along with this library; if not, write to the Free Software
*	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
*/


#pragma once

#include "atlcoll.h"
using namespace ATL;

/**
* ���� �˻� �ӵ��� �����ϴ� ������ ��ü�� �����ϴ� Map Ŭ������ �����Ѵ�.
*
* @author	dbongman
*/
template< class KEY, class PTRTYPE >
class CPtrMap
{
public:
	CPtrMap(void)
	{
	}
	~CPtrMap(void)
	{
		RemoveAll ();
		m_pMutex->release();
		delete m_pMutex;
	}

	bool	IsEmpty(void)
	{
		return m_map.IsEmpty();
	}

	long	Size(void)
	{
		return m_map.GetCount();
	}

	/**
	* ù ��° Item�� �����´�.
	*/
	PTRTYPE	PopItem(void)
	{
		POSITION	pos = m_map.GetStartPosition ();
		PTRTYPE		pValue = NULL;

		if( pos )
		{
			pValue = m_map.GetValueAt( pos );
			m_map.RemoveAtPos( pos );
		}

		return pValue;
	}

	/**
	* ������ Ű�� �ش��ϴ� ��ü �����͸� �����´�.
	*/
	PTRTYPE	PopItem(KEY itemKey)
	{
		CAtlMap< KEY, PTRTYPE >::CPair	*pVal = m_map.Lookup( itemKey );
		PTRTYPE							pValue = NULL;

		if( pVal )
		{
			pValue = pVal->m_value;
			m_map.RemoveKey( itemKey );
		}

		return pValue;
	}

	bool	AddItem(KEY itemKey, PTRTYPE itemValue, Thread* pThread = 0)
	{
		bool ret = false;
		if( pThread ) 
			pThread->wait("MyMutex");
		
		ret = ( m_map.SetAt( itemKey, itemValue ) != NULL );
			
		if( pThread )
			pThread->release("MyMutex");
		
		return ret;
	}

	bool	DelItem(KEY itemKey, Thread* pThread = 0)
	{
		CAtlMap< KEY, PTRTYPE >::CPair	*pVal = m_map.Lookup( itemKey );

		if( pVal == NULL)
			return false;

		if( pThread )
		{
			pThread->wait("MyMutex");

			delete pVal->m_value;
			m_map.RemoveKey( itemKey );

			pThread->release("MyMutex");
		}
		

		return true;
	}

	/**
	* Ű�� �ش��ϴ� ��ü �����͸� �˻��Ѵ�.
	* Hash Map �� ����� ���ؼ� ���� �˻��� �����Ѵ�.
	*
	* @return �˻��� ������ ��� NULL.
	*/
	PTRTYPE	FindItem(KEY itemKey, Thread* pThread = 0)
	{
		if( pThread )
			pThread->wait("MyMutex");

		CAtlMap< KEY, PTRTYPE >::CPair	*pVal = m_map.Lookup( itemKey );

		if( pThread )
			pThread->release("MyMutex");

		return ( pVal ? pVal->m_value : NULL );
	}

	void	RemoveAll(void)
	{
		if( m_map.IsEmpty() )
			return;

		POSITION	pos = m_map.GetStartPosition ();
		PTRTYPE		pValue = NULL;

		do
		{
			pValue = m_map.GetNextValue( pos );
			delete pValue;
		}
		while( pos != NULL );

		m_map.RemoveAll();
	}

	void	InitHashTable(UINT tSize) 
	{
		m_pMutex = new Mutex("MyMutex");
		m_map.InitHashTable(tSize);	
	}

	POSITION	GetStartPosition()	
	{
		return m_map.GetStartPosition();
	}

	PTRTYPE		GetNext(POSITION& pos)
	{
		return m_map.GetNext(pos)->m_value;
	}
	CAtlMap< KEY, PTRTYPE >& GetATLMap() { return m_map; }

protected:
	Mutex*								m_pMutex;
	CAtlMap< KEY, PTRTYPE >				m_map;
};
