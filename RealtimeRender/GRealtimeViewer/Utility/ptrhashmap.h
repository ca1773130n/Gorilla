/*
*
*	ATLMap 을 이용하여 포인터를 보관하는 Hash Map 클래스를 구현한다.
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
* 빠른 검색 속도를 제공하는 포인터 객체를 보관하는 Map 클래스를 구현한다.
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
	* 첫 번째 Item을 꺼내온다.
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
	* 지정된 키에 해당하는 객체 포인터를 꺼내온다.
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
	* 키에 해당하는 객체 포인터를 검색한다.
	* Hash Map 의 기능을 통해서 빠른 검색을 수행한다.
	*
	* @return 검색이 실패한 경우 NULL.
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
