//
//  SqliteRecordSet.h.
//

#ifndef _FHKJEF_FDS_CAN_SqliteRecordSet__
#define _FHKJEF_FDS_CAN_SqliteRecordSet__

#include "SqliteDatabasePro.h"
#include "SqliteReference.h"
#include "SqliteRecord.h"

class CSqliteRecordSet {
private:
	CSqliteDatabaseReference*  m_db;
	CSqliteStatementReference* m_stmt;
	bool  m_eof;
	bool  m_first;
	int   m_cols;
public:
	CSqliteRecordSet();
	CSqliteRecordSet(const CSqliteRecordSet & resultSet);
	CSqliteRecordSet(CSqliteDatabaseReference* db,
               CSqliteStatementReference* stmt,
               bool eof, bool first = true);
    
	CSqliteRecordSet& operator=(const CSqliteRecordSet& resultSet);
	~CSqliteRecordSet();
    void Release();
	int GetColumnCount();
	int FindColumnIndex(const char * columnName);
	const char * GetColumnName(int columnIndex);
	const char * GetDeclaredColumnType(int columnIndex);
	int GetColumnType(int columnIndex);
	const char * GetDatabaseName(int columnIndex);
	const char * GetTableName(int columnIndex);
	const char * GetOriginName(int columnIndex);
    
	const char * GetAsString(int columnIndex);
	const char * GetAsString(const char * columnName);
	int GetInt(int columnIndex, int nullValue = 0);
	int GetInt(const char * columnName, int nullValue = 0);
	int64 GetInt64(int columnIndex, int64 nullValue = 0);
	int64 GetInt64(const char * columnName, int64 nullValue = 0);
	double GetDouble(int columnIndex, double nullValue = 0.0);
	double GetDouble(const char * columnName, double nullValue = 0.0);
	const char * GetString(int columnIndex, const char * nullValue = NULL);
	const char * GetString(const char * columnName, const char * nullValue = NULL);
	const unsigned char* GetBlob(int columnIndex, int& len);
	const unsigned char* GetBlob(const char * columnName, int& len);
	bool GetBool(int columnIndex);
	bool GetBool(const char * columnName);
	bool IsNull(int columnIndex);
	bool IsNull(const char * columnName);
    
	bool Eof();
	bool CursorMoved();
	bool NextRow();
    
	void Finalize();
    
	const char * GetSQL();
	bool IsOk();
    
    CSqliteRecord CreateRecord();
private:
	void CheckStmt();
	void Finalize(CSqliteDatabaseReference* db,CSqliteStatementReference* stmt);
};

#endif /* defined(__TKGame__SqliteRecordSet__) */
