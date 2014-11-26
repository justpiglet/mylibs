//
//  SqliteRecordSet.cpp
//

#include "SqliteRecordSet.h"
#include "SqliteError.h"
#include "SqliteDatabase.h"

CSqliteRecordSet::CSqliteRecordSet()
{
	m_db = NULL;
	m_stmt = NULL;
	m_eof = true;
	m_first = true;
	m_cols = 0;
}

CSqliteRecordSet::CSqliteRecordSet( const CSqliteRecordSet & resultSet )
{
	m_db = resultSet.m_db;
	if (m_db != NULL)
	{
		m_db->IncrementRefCount();
	}
	m_stmt = resultSet.m_stmt;
	if (m_stmt != NULL)
	{
		m_stmt->IncrementRefCount();
	}
	m_eof = resultSet.m_eof;
	m_first = resultSet.m_first;
	m_cols = resultSet.m_cols;
}

CSqliteRecordSet::CSqliteRecordSet( CSqliteDatabaseReference* db, CSqliteStatementReference* stmt, bool eof, bool first /*= true*/ )
{
	m_db = db;
	if (m_db != NULL)
	{
		m_db->IncrementRefCount();
	}
	m_stmt = stmt;
	if (m_stmt != NULL)
	{
		m_stmt->IncrementRefCount();
	}
	CheckStmt();
	m_eof = eof;
	m_first = first;
	m_cols = sqlite3_column_count(m_stmt->m_stmt);
}

CSqliteRecordSet& CSqliteRecordSet::operator=( const CSqliteRecordSet& resultSet )
{
	if (this != &resultSet)
	{
		CSqliteDatabaseReference* dbPrev = m_db;
		CSqliteStatementReference* stmtPrev = m_stmt;
		m_db = resultSet.m_db;
		if (m_db != NULL)
		{
			m_db->IncrementRefCount();
		}
		m_stmt = resultSet.m_stmt;
		if (m_stmt != NULL)
		{
			m_stmt->IncrementRefCount();
		}
		m_eof = resultSet.m_eof;
		m_first = resultSet.m_first;
		m_cols = resultSet.m_cols;
		if (stmtPrev != NULL && stmtPrev->DecrementRefCount() == 0)
		{
			Finalize(dbPrev, stmtPrev);
			delete stmtPrev;
		}
		if (dbPrev != NULL && dbPrev->DecrementRefCount() == 0)
		{
			delete dbPrev;
		}
	}
	return *this;
}

CSqliteRecordSet::~CSqliteRecordSet()
{
    //CSqliteTrackReport::shared()->RemoveItem(this);
	if (m_stmt != NULL && m_stmt->DecrementRefCount() == 0)
	{
		if (m_stmt->m_isValid)
		{
            Finalize(m_db, m_stmt);
		}
		delete m_stmt;
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
void CSqliteRecordSet::Release()
{
	delete this;
}
int CSqliteRecordSet::GetColumnCount()
{
	CheckStmt();
	return m_cols;
}

int CSqliteRecordSet::FindColumnIndex( const char * columnName )
{
	CheckStmt();
	if (columnName)
	{
		for (int columnIndex = 0; columnIndex < m_cols; columnIndex++)
		{
			const char* temp = sqlite3_column_name(m_stmt->m_stmt, columnIndex);
            
			if (strcmp(columnName, temp) == 0)
			{
				return columnIndex;
			}
		}
	}
    
	CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
    
	return 0;
}

const char * CSqliteRecordSet::GetColumnName( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	return sqlite3_column_name(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetDeclaredColumnType( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	return sqlite3_column_decltype(m_stmt->m_stmt, columnIndex);
}

int CSqliteRecordSet::GetColumnType( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	return sqlite3_column_type(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetDatabaseName( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	return sqlite3_column_database_name(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetTableName( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
	return sqlite3_column_table_name(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetOriginName( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
	return sqlite3_column_origin_name(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetAsString( int columnIndex )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
	return (const char*)sqlite3_column_text(m_stmt->m_stmt, columnIndex);
}

const char * CSqliteRecordSet::GetAsString( const char * columnName )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetAsString(columnIndex);//(const char*) sqlite3_column_text(m_stmt->m_stmt, columnIndex);
}

int CSqliteRecordSet::GetInt( int columnIndex, int nullValue /*= 0*/ )
{
	if (GetColumnType(columnIndex) == SQLITE_NULL)
	{
		return nullValue;
	}
	else
	{
		return sqlite3_column_int(m_stmt->m_stmt, columnIndex);
	}
}

int CSqliteRecordSet::GetInt( const char * columnName, int nullValue /*= 0*/ )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetInt(columnIndex, nullValue);
}

int64 CSqliteRecordSet::GetInt64( int columnIndex, int64 nullValue /*= 0*/ )
{
	if (GetColumnType(columnIndex) == SQLITE_NULL)
	{
		return nullValue;
	}
	else
	{
		return sqlite3_column_int64(m_stmt->m_stmt, columnIndex);
	}
}

int64 CSqliteRecordSet::GetInt64( const char * columnName, int64 nullValue /*= 0*/ )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetInt64(columnIndex, nullValue);
}

double CSqliteRecordSet::GetDouble( int columnIndex, double nullValue /*= 0.0*/ )
{
	if (GetColumnType(columnIndex) == SQLITE_NULL)
	{
		return nullValue;
	}
	else
	{
		return sqlite3_column_double(m_stmt->m_stmt, columnIndex);
	}
}

double CSqliteRecordSet::GetDouble( const char * columnName, double nullValue /*= 0.0*/ )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetDouble(columnIndex, nullValue);
}

const char * CSqliteRecordSet::GetString( int columnIndex, const char * nullValue /*= NULL*/ )
{
	if (GetColumnType(columnIndex) == SQLITE_NULL)
	{
		return nullValue;
	}
	else
	{
		return (const char*) sqlite3_column_text(m_stmt->m_stmt, columnIndex);
	}
}

const char * CSqliteRecordSet::GetString( const char * columnName, const char * nullValue /*= NULL*/ )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetString(columnIndex, nullValue);
}

const unsigned char* CSqliteRecordSet::GetBlob( int columnIndex, int& len )
{
	CheckStmt();
    
	if (columnIndex < 0 || columnIndex > m_cols-1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_INDEX);
	}
    
	len = sqlite3_column_bytes(m_stmt->m_stmt, columnIndex);
	return (const unsigned char*) sqlite3_column_blob(m_stmt->m_stmt, columnIndex);
}

const unsigned char* CSqliteRecordSet::GetBlob( const char * columnName, int& len )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetBlob(columnIndex, len);
}

bool CSqliteRecordSet::GetBool( int columnIndex )
{
	return GetInt(columnIndex) != 0;
}

bool CSqliteRecordSet::GetBool( const char * columnName )
{
	int columnIndex = FindColumnIndex(columnName);
	return GetBool(columnIndex);
}

bool CSqliteRecordSet::IsNull(int columnIndex)
{
	return (GetColumnType(columnIndex) == SQLITE_NULL);
}

bool CSqliteRecordSet::IsNull( const char * columnName )
{
	int columnIndex = FindColumnIndex(columnName);
	return (GetColumnType(columnIndex) == SQLITE_NULL);
}

bool CSqliteRecordSet::Eof()
{
	CheckStmt();
	return m_eof;
}

bool CSqliteRecordSet::CursorMoved()
{
	CheckStmt();
	return !m_first;
}

bool CSqliteRecordSet::NextRow()
{
	CheckStmt();
    
	int rc;
	if (m_first)
	{
		m_first = false;
		rc = (m_eof) ? SQLITE_DONE : SQLITE_ROW;
	}
	else
	{
		rc = sqlite3_step(m_stmt->m_stmt);
	}
    
	if (rc == SQLITE_DONE) // no more rows
	{
		m_eof = true;
		return false;
	}
	else if (rc == SQLITE_ROW) // more rows
	{
		return true;
	}
	else
	{
		rc = sqlite3_finalize(m_stmt->m_stmt);
		m_stmt->Invalidate();
		CSqliteDatabase::error(rc, sqlite3_errmsg(m_db->m_db));
	}
	return false;
}

void CSqliteRecordSet::Finalize()
{
	Finalize(m_db, m_stmt);
	if (m_stmt != NULL && m_stmt->DecrementRefCount() == 0)
	{
		delete m_stmt;
	}
	m_stmt = NULL;
	if (m_db != NULL && m_db->DecrementRefCount() == 0)
	{
		if (m_db->m_isValid)
		{
			sqlite3_close(m_db->m_db);
		}
		delete m_db;
	}
	m_db = NULL;
}

void CSqliteRecordSet::Finalize( CSqliteDatabaseReference* db,CSqliteStatementReference* stmt )
{
	if (stmt != NULL && stmt->m_isValid)
	{
		int rc = sqlite3_finalize(stmt->m_stmt);
		stmt->Invalidate();
		if (rc != SQLITE_OK)
		{
			if (db != NULL && db->m_isValid)
			{
				CSqliteDatabase::error(rc,sqlite3_errmsg(db->m_db));
			}
			else
			{
				CSqliteDatabase::error(rc, DATABASE_ERRMSG_FINALIZE_FAILED);
			}
		}
	}
}

const char * CSqliteRecordSet::GetSQL()
{
	CheckStmt();
	return sqlite3_sql(m_stmt->m_stmt);
}

bool CSqliteRecordSet::IsOk()
{
	return (m_db != NULL) && (m_db->m_isValid) && (m_stmt != NULL) && (m_stmt->m_isValid);
}

void CSqliteRecordSet::CheckStmt()
{
	if (m_stmt == NULL || m_stmt->m_stmt == NULL || !m_stmt->m_isValid)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_NOSTMT);
	}
}
CSqliteRecord CSqliteRecordSet::CreateRecord()
{
    return CSqliteRecord(this);
}