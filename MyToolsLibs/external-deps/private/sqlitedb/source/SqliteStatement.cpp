//
//  SqliteStatement.cpp
//

#include "SqliteStatement.h"
#include "SqliteError.h"
#include "SqliteDatabase.h"


CSqliteStatement::CSqliteStatement()
{
	m_db = 0;
	m_stmt = 0;
}

CSqliteStatement::CSqliteStatement( const CSqliteStatement & statement )
{
	m_db = statement.m_db;
	if (m_db != NULL)
	{
		m_db->IncrementRefCount();
	}
	m_stmt = statement.m_stmt;
	if (m_stmt != NULL)
	{
		m_stmt->IncrementRefCount();
	}
}

CSqliteStatement::CSqliteStatement( CSqliteDatabaseReference* db, CSqliteStatementReference* stmt )
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
}

CSqliteStatement & CSqliteStatement::operator=( const CSqliteStatement & statement )
{
	if (this != &statement)
	{
		CSqliteDatabaseReference* dbPrev = m_db;
		CSqliteStatementReference* stmtPrev = m_stmt;
		m_db = statement.m_db;
		if (m_db != NULL)
		{
			m_db->IncrementRefCount();
		}
		m_stmt = statement.m_stmt;
		if (m_stmt != NULL)
		{
			m_stmt->IncrementRefCount();
		}
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

CSqliteStatement::~CSqliteStatement()
{
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
int CSqliteStatement::ExecuteUpdate()
{
	CheckDatabase();
	CheckStmt();
    
	int rc = sqlite3_step(m_stmt->m_stmt);
    
	if (rc == SQLITE_DONE)
	{
		int rowsChanged = sqlite3_changes(m_db->m_db);
        
		rc = sqlite3_reset(m_stmt->m_stmt);
        
		if (rc != SQLITE_OK)
		{
            CSqliteDatabase::error(rc,sqlite3_errmsg(m_db->m_db));
		}
        
		return rowsChanged;
	}
	else
	{
		rc = sqlite3_reset(m_stmt->m_stmt);
		CSqliteDatabase::error(rc,sqlite3_errmsg(m_db->m_db));
	}
	return -1;
}

CSqliteRecordSet CSqliteStatement::ExecuteQuery()
{
	CheckDatabase();
	CheckStmt();
    
	int rc = sqlite3_step(m_stmt->m_stmt);
    
	if (rc == SQLITE_DONE)  // no more rows
	{
		return CSqliteRecordSet(m_db, m_stmt, true/*eof*/, true/*first*/);
	}
	else if (rc == SQLITE_ROW)  // one or more rows
	{
		return CSqliteRecordSet(m_db, m_stmt, false/*eof*/, true/*first*/);
	}
	else
	{
		rc = sqlite3_reset(m_stmt->m_stmt);
		CSqliteDatabase::error(rc, sqlite3_errmsg(m_db->m_db));
	}
	return CSqliteRecordSet(0,0,0);
}

int CSqliteStatement::ExecuteScalar()
{
	CSqliteRecordSet resultSet = ExecuteQuery();
    
	if (resultSet.Eof() || resultSet.GetColumnCount() < 1)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_QUERY);
	}
    
	long value = atol(resultSet.GetAsString(0));
	//resultSet.GetAsString(0).ToLong(&value);
	return (int) value;
}

int CSqliteStatement::GetParamCount()
{
	CheckStmt();
	return sqlite3_bind_parameter_count(m_stmt->m_stmt);
}

int CSqliteStatement::GetParamIndex( const char * paramName )
{
	CheckStmt();
	return sqlite3_bind_parameter_index(m_stmt->m_stmt, paramName);
}

const char * CSqliteStatement::GetParamName( int paramIndex )
{
	CheckStmt();
	return sqlite3_bind_parameter_name(m_stmt->m_stmt, paramIndex);
}

void CSqliteStatement::Bind( int paramIndex, const char * stringValue )
{
	CheckStmt();
	int rc = sqlite3_bind_text(m_stmt->m_stmt, paramIndex, stringValue, -1, SQLITE_TRANSIENT);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_STR);
	}
}


void CSqliteStatement::Bind(int paramIndex, int intValue)
{
	CheckStmt();
	int rc = sqlite3_bind_int(m_stmt->m_stmt, paramIndex, intValue);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_INT);
	}
}

void CSqliteStatement::Bind(int paramIndex, int64 int64Value)
{
	CheckStmt();
	int rc = sqlite3_bind_int64(m_stmt->m_stmt, paramIndex, int64Value);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_INT64);
	}
}

void CSqliteStatement::Bind(int paramIndex, double doubleValue)
{
	CheckStmt();
	int rc = sqlite3_bind_double(m_stmt->m_stmt, paramIndex, doubleValue);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_DBL);
	}
}

void CSqliteStatement::Bind(int paramIndex, const unsigned char* blobValue, int blobLen)
{
	CheckStmt();
	int rc = sqlite3_bind_blob(m_stmt->m_stmt, paramIndex,
                               (const void*)blobValue, blobLen, SQLITE_TRANSIENT);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_BLOB);
	}
}


void CSqliteStatement::BindBool(int paramIndex, bool value)
{
	Bind(paramIndex, value ? 1 : 0);
}

void CSqliteStatement::BindNull(int paramIndex)
{
	CheckStmt();
	int rc = sqlite3_bind_null(m_stmt->m_stmt, paramIndex);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_NULL);
	}
}


void CSqliteStatement::BindZeroBlob(int paramIndex, int blobSize)
{
	CheckStmt();
	int rc = sqlite3_bind_zeroblob(m_stmt->m_stmt, paramIndex, blobSize);
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_ZEROBLOB);
	}
    
}

void CSqliteStatement::ClearBindings()
{
	CheckStmt();
#if 0
	int rc = sqlite3_clear_bindings(m_stmt->m_stmt);
    
	if (rc != SQLITE_OK)
	{
		CSqliteDatabase::error(rc, DATABASE_ERRMSG_BIND_CLEAR);
	}
#else
	for (int paramIndex = 1; paramIndex <= GetParamCount(); paramIndex++)
	{
		BindNull(paramIndex);
	}
#endif
}

const char * CSqliteStatement::GetSQL()
{
	CheckStmt();
	return sqlite3_sql(m_stmt->m_stmt);
}

void CSqliteStatement::Reset()
{
	if (m_stmt != NULL && m_stmt->m_isValid)
	{
		int rc = sqlite3_reset(m_stmt->m_stmt);
		if (rc != SQLITE_OK)
		{
			CSqliteDatabase::error(rc, sqlite3_errmsg(m_db->m_db));
		}
	}
}

bool CSqliteStatement::IsReadOnly()
{
	CheckStmt();
	return sqlite3_stmt_readonly(m_stmt->m_stmt) != 0;
}

void CSqliteStatement::Finalize()
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

void CSqliteStatement::Finalize(CSqliteDatabaseReference* db, CSqliteStatementReference* stmt)
{
	if (stmt != NULL && stmt->m_isValid)
	{
		int rc = sqlite3_finalize(stmt->m_stmt);
		stmt->Invalidate();
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


bool CSqliteStatement::IsOk()
{
	return (m_db != 0) && (m_stmt != 0);
}

bool CSqliteStatement::IsBusy()
{
#if SQLITE_VERSION_NUMBER >= 3007010
	CheckStmt();
	int rc = sqlite3_stmt_busy(m_stmt->m_stmt);
	return (rc != 0);
#else
	return false;
#endif
}

void CSqliteStatement::CheckDatabase()
{
	if (m_db == NULL || m_db->m_db == NULL || !m_db->m_isValid)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_NODB);
	}
}

void CSqliteStatement::CheckStmt()
{
	if (m_stmt == NULL || m_stmt->m_stmt == NULL || !m_stmt->m_isValid)
	{
		CSqliteDatabase::error(DATABASE_ERROR, DATABASE_ERRMSG_NOSTMT);
	}
}
