//
//  SqliteReference.h
//  
//
//

#ifndef _FHKJEF_FDS_CAN_SqliteReference__
#define _FHKJEF_FDS_CAN_SqliteReference__

#include "SqliteDatabasePro.h"
#include "sqlite/sqlite3.h"

class CSqliteDatabaseReference
{
public:
	CSqliteDatabaseReference(sqlite3* db = NULL);
    
	virtual ~CSqliteDatabaseReference();
    
private:
	int IncrementRefCount();
	void Invalidate();
	int DecrementRefCount();
private:
	sqlite3* m_db;
	int      m_refCount;
	bool     m_isValid;
    
	friend class CSqliteDatabase;
	friend class CSqliteRecordSet;
	friend class CSqliteStatement;
	friend class CSqliteBlob;
};


class CSqliteStatementReference
{
public:
	CSqliteStatementReference(sqlite3_stmt* stmt = NULL);
	virtual ~CSqliteStatementReference();
    
private:
	int IncrementRefCount();
    
	int DecrementRefCount();
    
    
	void Invalidate();
    
private:
	sqlite3_stmt* m_stmt;
	int           m_refCount;
	bool          m_isValid;
    
	friend class CSqliteRecordSet;
	friend class CSqliteStatement;
};



class CSqliteBlobReference
{
private:
    
	sqlite3_blob* m_blob;
	int           m_refCount;
	bool          m_isValid;
    
	friend class CSqliteBlob;
public:
	CSqliteBlobReference(sqlite3_blob* blob = NULL);
	virtual ~CSqliteBlobReference();
private:
	int IncrementRefCount();
	int DecrementRefCount();
	void Invalidate();
};


#endif /* defined(__TKGame__SqliteReference__) */
