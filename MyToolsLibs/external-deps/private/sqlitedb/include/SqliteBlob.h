//
//  SqliteBlob.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteBlob__
#define _FHKJEF_FDS_CAN_SqliteBlob__

#include "SqliteDatabasePro.h"
#include "SqliteReference.h"

class CSqliteBlob
{
private:
	CSqliteDatabaseReference* m_db;
	CSqliteBlobReference*     m_blob;
	bool  m_writable;
public:
	CSqliteBlob();
	CSqliteBlob( const CSqliteBlob & blob);
	CSqliteBlob& operator=(const CSqliteBlob& blob);
	CSqliteBlob(CSqliteDatabaseReference* db, CSqliteBlobReference* blobHandle, bool writable);
	~CSqliteBlob();
    
	//CStackBuffer & Read(CStackBuffer & blobValue,int length, int offset);
	void Write(char * lpBuffer,int length, int offset);
	bool IsOk();
	bool IsReadOnly();
	int GetSize();
	void Rebind(int64 rowid);
	void Finalize();
private:
	void CheckBlob();
    
	void Finalize(CSqliteDatabaseReference* db, CSqliteBlobReference* blob);
};

#endif /* defined(__TKGame__SqliteBlob__) */
