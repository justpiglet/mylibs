//
//  SqliteStatement.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteStatement__
#define _FHKJEF_FDS_CAN_SqliteStatement__

#include "SqliteDatabasePro.h"
#include "SqliteReference.h"
#include "SqliteRecordSet.h"

class CSqliteStatement {
private:
	CSqliteDatabaseReference*  m_db;
	CSqliteStatementReference* m_stmt;
public:
    CSqliteStatement();
	CSqliteStatement(const CSqliteStatement & statement);
	CSqliteStatement & operator = (const CSqliteStatement & statement);
	CSqliteStatement(CSqliteDatabaseReference* db, CSqliteStatementReference* stmt);
	~CSqliteStatement();
    
	int ExecuteUpdate();
	CSqliteRecordSet ExecuteQuery();
	int ExecuteScalar();
	int GetParamCount();
	int GetParamIndex(const char * paramName);
	const char * GetParamName(int paramIndex);
	void Bind(int paramIndex, const char * stringValue);
	void Bind(int paramIndex, int intValue);
	void Bind(int paramIndex, int64 int64Value);
	void Bind(int paramIndex, double doubleValue);
	//void Bind(int paramIndex, const char* charValue);
	void Bind(int paramIndex, const unsigned char* blobValue, int blobLen);
	void BindBool(int paramIndex, bool value);
	void BindNull(int paramIndex);
	void BindZeroBlob(int paramIndex, int blobSize);
	void ClearBindings();
	const char * GetSQL();
	void Reset();
	bool IsReadOnly();
	void Finalize();
	bool IsOk();
	bool IsBusy();
    
private:
	void CheckDatabase();
	void CheckStmt();
	void Finalize(CSqliteDatabaseReference* db,CSqliteStatementReference* stmt);
};

#endif /* defined(__TKGame__SqliteStatement__) */
