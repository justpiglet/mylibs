//
//  SqliteBlob.cpp
//

//#include "SqliteDatabasePro.h"
#include "SqliteBlob.h"
#include "SqliteError.h"
#include "SqliteDatabase.h"

CSqliteBlob::CSqliteBlob()
{
	m_db   = NULL;
	m_blob = NULL;
	m_writable = false;
}

CSqliteBlob::CSqliteBlob( const CSqliteBlob & blob )
{
	m_db   = blob.m_db;
	if (m_db != NULL)
	{
		m_db->IncrementRefCount();
	}
	m_blob = blob.m_blob;
	if (m_blob != NULL)
	{
		m_blob->IncrementRefCount();
	}
	m_writable = blob.m_writable;
}

CSqliteBlob::CSqliteBlob( CSqliteDatabaseReference* db, CSqliteBlobReference* blobHandle, bool writable )
{
	m_db   = db;
	if (m_db != NULL)
	{
		m_db->IncrementRefCount();
	}
	m_blob = blobHandle;
	if (m_blob != NULL)
	{
		m_blob->IncrementRefCount();
	}
	m_writable = writable;
}

CSqliteBlob& CSqliteBlob::operator=( const CSqliteBlob& blob )
{
	if (this != &blob)
	{
		CSqliteDatabaseReference* dbPrev = m_db;
		CSqliteBlobReference* blobPrev = m_blob;
		m_db   = blob.m_db;
		if (m_db != NULL)
		{
			m_db->IncrementRefCount();
		}
		m_blob = blob.m_blob;
		if (m_blob != NULL)
		{
			m_blob->IncrementRefCount();
		}
		m_writable = blob.m_writable;
		if (blobPrev != NULL && blobPrev->DecrementRefCount() == 0)
		{
			Finalize(dbPrev, blobPrev);
			delete blobPrev;
		}
		if (dbPrev != NULL && dbPrev->DecrementRefCount() == 0)
		{
			delete dbPrev;
		}
	}
	return *this;
}

CSqliteBlob::~CSqliteBlob()
{
	if (m_blob != NULL && m_blob->DecrementRefCount() == 0)
	{
		Finalize(m_db, m_blob);
		delete m_blob;
	}
	if (m_db != NULL && m_db->DecrementRefCount() == 0)
	{
		if (m_db->m_isValid)
		{
			sqlite3_close(m_db->m_db);
		}
		delete m_db;
	}
}
/*
CStackBuffer & CSqliteBlob::Read( CStackBuffer & blobValue,int length, int offset )
{
	CheckBlob();
	char* localBuffer = (char*)blobValue.GetBuffer();
	int rc = sqlite3_blob_read(m_blob->m_blob, localBuffer, length, offset);
	
    
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		CSqliteDatabase::error(rc, localError);
	}
	blobValue.SetWritePos(length);
    
	return blobValue;
    
}
*/
void CSqliteBlob::Write( char * lpBuffer,int length, int offset )
{
	CheckBlob();
	if (m_writable)
	{
		int rc = sqlite3_blob_write(m_blob->m_blob,
                                    lpBuffer, length, offset);
        
		if (rc != SQLITE_OK)
		{
			const char* localError = sqlite3_errmsg(m_db->m_db);
			CSqliteDatabase::error(rc, localError);
		}
	}
	else
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_BLOB);
	}
}

bool CSqliteBlob::IsOk()
{
	return (m_blob != NULL && m_blob->m_isValid);
}

bool CSqliteBlob::IsReadOnly()
{
	return !m_writable;
}

int CSqliteBlob::GetSize()
{
	CheckBlob();
	return sqlite3_blob_bytes(m_blob->m_blob);
}

void CSqliteBlob::Rebind( int64 rowid )
{
	CheckBlob();
	int rc = sqlite3_blob_reopen(m_blob->m_blob, rowid);
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		CSqliteDatabase::error(rc, localError);
	}
}

void CSqliteBlob::Finalize()
{
	Finalize(m_db, m_blob);
}

void CSqliteBlob::Finalize( CSqliteDatabaseReference* db, CSqliteBlobReference* blob )
{
	if (blob != NULL && blob->m_isValid)
	{
		int rc = sqlite3_blob_close(blob->m_blob);
		blob->Invalidate();
		if (rc != SQLITE_OK)
		{
			if (db != NULL && db->m_isValid)
			{
				const char* localError = sqlite3_errmsg(db->m_db);
				CSqliteDatabase::error(rc, localError);
			}
			else
			{
				CSqliteDatabase::error(rc, DATABASE_ERRMSG_FINALIZE_FAILED);
			}
		}
	}
}

void CSqliteBlob::CheckBlob()
{
	if (m_db == NULL || !m_db->m_isValid || m_blob == NULL || !m_blob->m_isValid)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_BLOB);
	}
}
