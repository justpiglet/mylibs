//
//  SqliteReference.cpp
//

#include "SqliteReference.h"


CSqliteDatabaseReference::CSqliteDatabaseReference( sqlite3* db /*= NULL*/ )
: m_db(db)
{
	m_db = db;
	if (m_db != NULL)
	{
		m_isValid = true;
		m_refCount = 1;
	}
	else
	{
		m_isValid = false;
		m_refCount = 0;
	}
}

CSqliteDatabaseReference::~CSqliteDatabaseReference()
{
    
}

int CSqliteDatabaseReference::IncrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csDatabase);
#endif
	return ++m_refCount;
}

void CSqliteDatabaseReference::Invalidate()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csDatabase);
#endif
	m_isValid = false;
}

int CSqliteDatabaseReference::DecrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csDatabase);
#endif
	if (m_refCount > 0) --m_refCount;
	return m_refCount;
}


CSqliteStatementReference::CSqliteStatementReference( sqlite3_stmt* stmt /*= NULL*/ )
: m_stmt(stmt)
{
	m_stmt = stmt;
	if (m_stmt != NULL)
	{
		m_isValid = true;
		m_refCount = 0;
	}
	else
	{
		m_isValid = false;
		m_refCount = 0;
	}
}

CSqliteStatementReference::~CSqliteStatementReference()
{
    
}

int CSqliteStatementReference::IncrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csStatment);
#endif
	return ++m_refCount;
}

int CSqliteStatementReference::DecrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csStatment);
#endif
	if (m_refCount > 0) --m_refCount;
	return m_refCount;
}

void CSqliteStatementReference::Invalidate()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csStatment);
#endif
	m_isValid = false;
}

CSqliteBlobReference::CSqliteBlobReference( sqlite3_blob* blob /*= NULL*/ )
: m_blob(blob)
{
	m_blob = blob;
	if (m_blob != NULL)
	{
		m_isValid = true;
		m_refCount = 0;
	}
	else
	{
		m_isValid = false;
		m_refCount = 0;
	}
}

CSqliteBlobReference::~CSqliteBlobReference()
{
    
}

int CSqliteBlobReference::IncrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csBlob);
#endif
	return ++m_refCount;
}

int CSqliteBlobReference::DecrementRefCount()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csBlob);
#endif
	if (m_refCount > 0) --m_refCount;
	return m_refCount;
}

void CSqliteBlobReference::Invalidate()
{
#if USE_THREADS
	//CSharedLock locker(&gs_csBlob);
#endif
	m_isValid = false;
}