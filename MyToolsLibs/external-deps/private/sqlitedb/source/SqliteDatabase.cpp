//
//  SqliteDatabase.cpp
//  TKGame
//
//  Created by Hotman on 13-3-18.
//  Copyright (c) 2013年 Hotman. All rights reserved.
//

//#include "SqliteDatabasePro.h"
#include "SqliteDatabase.h"
#include "SqliteError.h"
#include "SqliteStatement.h"
#include "SqliteBlob.h"
#include <stdlib.h>

std::string CSqliteDatabase::m_lastError;
bool  CSqliteDatabase::ms_sharedCacheEnabled = false;

CSqliteDatabase::CSqliteDatabase()
{
    m_isEncrypted = false;
    m_busyTimeoutMs = 60000; // 60 seconds
}

CSqliteDatabase::~CSqliteDatabase()
{
    
}

CSqliteDatabase * CSqliteDatabase::Create(const char * lpstrData,const char *lpstrPassword,int flags)
{

    CSqliteDatabase * pDataBase = new CSqliteDatabase();
    if(pDataBase && pDataBase->Open(lpstrData,lpstrPassword,flags) == false)
    {
        delete pDataBase;
        return NULL;
    }
    return pDataBase;
}

bool CSqliteDatabase::Open(const char * lpstrData,const char * lpstrPassword,
          int flags )
{
    sqlite3* db;
#if USE_SQLITE_V2
	int rc = sqlite3_open_v2((const char*) lpstrData, &db, flags, NULL);
#else
	int rc = sqlite3_open((const char*) lpstrData, &db);
#endif
    
    if (rc != SQLITE_OK)
	{
		const char* localError = "Out of memory";
		if (db != NULL)
		{
			localError = sqlite3_errmsg(db);
			sqlite3_close(db);
		}
        error(rc,localError);
        return false;
	}
    
    rc = sqlite3_extended_result_codes(db, 1);
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(db);
		sqlite3_close(db);
		error(rc,localError);
        return false;
	}
    
    if (lpstrPassword && strlen(lpstrPassword))
	{
		rc = sqlite3_key(db, lpstrPassword, (int)strlen(lpstrPassword));
		if (rc != SQLITE_OK)
		{
			const char* localError = sqlite3_errmsg(db);
			sqlite3_close(db);
			error(rc,localError);
            return false;
		}
		m_isEncrypted = true;
	}
    
    m_db = new CSqliteDatabaseReference(db);
    m_isOpen = true;
	SetBusyTimeout(m_busyTimeoutMs);
    return true;
}


bool CSqliteDatabase::IsOpen() const
{
	return (m_db != NULL && m_db->m_isValid && m_isOpen);
}

void CSqliteDatabase::Close()
{
	CheckDatabase();
	Close(m_db);
	delete this;
}

void CSqliteDatabase::Close( CSqliteDatabaseReference* db )
{
	if (db != NULL && db->m_isValid)
	{
		if (db->m_refCount <= 1)
		{
			sqlite3_close(db->m_db);
			db->Invalidate();
			m_isEncrypted = false;
		}
		m_isOpen = false;
	}
}

void* CSqliteDatabase::Prepare( const char* sql )
{
	CheckDatabase();
    
	const char* tail=0;
	sqlite3_stmt* stmt;
#if USE_SQLITE_V2
	int rc = sqlite3_prepare_v2(m_db->m_db, sql, -1, &stmt, &tail);
#else
	int rc = sqlite3_prepare(m_db->m_db, sql, -1, &stmt, &tail);
#endif
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc, localError);
	}
	return stmt;
}

static bool
BackupRestoreCallback(int total, int remaining, CSqliteBackupProgress* progressCallback)
{
	return progressCallback->Progress(total, remaining);
}

void CSqliteDatabase::Backup( const char * targetFileName, const char * key /*= NULL*/, const char * sourceDatabaseName /*= _T("main")*/ )
{
	Backup(NULL,targetFileName,key,sourceDatabaseName);
}

void CSqliteDatabase::Backup( CSqliteBackupProgress* progressCallback, const char * targetFileName, const char * key /*= NULL*/, const char * sourceDatabaseName /*= ("main")*/ )
{
	CheckDatabase();
	sqlite3* pDest;
	sqlite3_backup* pBackup;
	int rc;
	rc = sqlite3_open(targetFileName, &pDest);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(pDest);
		error(rc, DATABASE_ERRMSG_DBOPEN_FAILED);
        return;
	}
    
	if (key && strlen(key))
	{
		rc = sqlite3_key(pDest, key,strlen(key));
		if (rc != SQLITE_OK)
		{
			const char* localError = sqlite3_errmsg(pDest);
			sqlite3_close(pDest);
			error(rc, localError);
            return;
		}
	}
    
	pBackup = sqlite3_backup_init(pDest, "main", m_db->m_db, sourceDatabaseName);
	if (pBackup == 0)
	{
		const char* localError = sqlite3_errmsg(pDest);
		sqlite3_close(pDest);
		error(rc, localError);
        return;
	}
    
	do
	{
		rc = sqlite3_backup_step(pBackup, m_backupPageCount);
		if (progressCallback != NULL)
		{
			if (!BackupRestoreCallback(sqlite3_backup_pagecount(pBackup),
                                       sqlite3_backup_remaining(pBackup),
                                       progressCallback))
			{
				rc = SQLITE_DONE;
			}
		}
		if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
		{
			sqlite3_sleep(250);
		}
	}
	while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    
	sqlite3_backup_finish(pBackup);
	if (rc == SQLITE_DONE)
	{
		sqlite3_close(pDest);
	}
	else
	{
		const char* localError = sqlite3_errmsg(pDest);
		sqlite3_close(pDest);
		error(rc,localError);
	}
    
}


void CSqliteDatabase::Restore( const char * sourceFileName, const char * key /*= NULL*/, const char * targetDatabaseName /*= ("main")*/ )
{
	Restore(NULL,sourceFileName,key,targetDatabaseName);
}

void CSqliteDatabase::Restore( CSqliteBackupProgress* progressCallback, const char * sourceFileName, const char * key /*= NULL*/, const char * targetDatabaseName /*= ("main")*/ )
{
	CheckDatabase();
	sqlite3* pSrc;
	sqlite3_backup* pBackup;
	int rc;
	int nTimeout = 0;
    
	rc = sqlite3_open(sourceFileName, &pSrc);
	if (rc != SQLITE_OK)
	{
		sqlite3_close(pSrc);
		error(rc, DATABASE_ERRMSG_DBOPEN_FAILED);
        return;
	}
	if (key && strlen(key))
	{
		rc = sqlite3_key(pSrc, key,strlen(key));
		if (rc != SQLITE_OK)
		{
			const char* localError = sqlite3_errmsg(pSrc);
			sqlite3_close(pSrc);
			error(rc, localError);
            return;
		}
	}
    
	pBackup = sqlite3_backup_init(m_db->m_db, targetDatabaseName, pSrc, "main");
	if (pBackup == 0)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		sqlite3_close(pSrc);
		error(rc, localError);
        return;
	}
    
	do
	{
		rc = sqlite3_backup_step(pBackup, m_backupPageCount);
		if (progressCallback != NULL)
		{
			if (!BackupRestoreCallback(sqlite3_backup_pagecount(pBackup),
                                       sqlite3_backup_remaining(pBackup), progressCallback))
			{
				rc = SQLITE_DONE;
			}
		}
		if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
		{
			if (nTimeout++ >= 20) break;
			sqlite3_sleep(250);
		}
		else
		{
			nTimeout = 0;
		}
	}
	while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);
    
	sqlite3_backup_finish(pBackup);
	if (rc == SQLITE_DONE)
	{
		sqlite3_close(pSrc);
	}
	else if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
	{
		sqlite3_close(pSrc);
		error(rc, DATABASE_ERRMSG_SOURCEDB_BUSY);
        return;
	}
	else
	{
		const char* localError = sqlite3_errmsg(pSrc);
		sqlite3_close(pSrc);
		error(rc, localError);
	}
}

void CSqliteDatabase::SetBackupRestorePageCount(int pageCount)
{
	m_backupPageCount = pageCount;
}
void CSqliteDatabase::Vacuum()
{
	ExecuteUpdate("vacuum");
}
void CSqliteDatabase::Begin( DatabaseTransactionType transactionType /*= DATABASE_TRANSACTION_DEFAULT*/ )
{
	std::string sql;
	switch (transactionType)
	{
        case DATABASE_TRANSACTION_DEFERRED:
            sql = ("begin deferred transaction");
            break;
        case DATABASE_TRANSACTION_IMMEDIATE:
            sql = ("begin immediate transaction");
            break;
        case DATABASE_TRANSACTION_EXCLUSIVE:
            sql = ("begin exclusive transaction");
            break;
        default:
            sql = ("begin transaction");
            break;
	}
	ExecuteUpdate(sql.c_str());
}

void CSqliteDatabase::Commit()
{
	ExecuteUpdate("commit transaction");
}

void CSqliteDatabase::Rollback( const char * savepointName /*= NULL*/ )
{
    
	if (!savepointName)
	{
		ExecuteUpdate("rollback transaction");
	}
	else
	{
        std::string sql = "rollback transaction to savepoint ";
		sql.append(savepointName);
		ExecuteUpdate(sql.c_str());
	}
    
}

bool CSqliteDatabase::TableExists( const char * tableName, const char * databaseName /*= NULL*/ )
{
    std::string sql;
	if(!databaseName)
		sql = ("select count(*) from sqlite_master where type='table' and name like ?");
	else
	{
		sql = "select count(*) from ";
		sql+=databaseName;
		sql+=".sqlite_master where type='table' and name like ?";
	}
	CSqliteStatement stmt = PrepareStatement(sql.c_str());
	stmt.Bind(1, tableName);
	CSqliteRecordSet resultSet = stmt.ExecuteQuery();
    
	long value = atol(resultSet.GetAsString(0));
	return (value > 0);
}

bool CSqliteDatabase::TableExists( const char * tableName, std::vector<const char *> & databaseNames)
{
    std::vector<const char *> databaseList;
	GetDatabaseList(databaseList);
	bool found = false;
	size_t count = databaseList.size();
	if (count > 0)
	{
		size_t j;
		for (j = 0; j < count; j++)
		{
			if (TableExists(tableName, databaseList[j]))
			{
				found = true;
				databaseNames.push_back(databaseList[j]);
			}
		}
	}
	return found;
}


CSqliteStatement CSqliteDatabase::PrepareStatement( const char * sql )
{
	CheckDatabase();
	sqlite3_stmt* stmt = (sqlite3_stmt*) Prepare(sql);
	CSqliteStatementReference* stmtRef = new CSqliteStatementReference(stmt);
	return CSqliteStatement(m_db, stmtRef);
}

CSqliteStatement CSqliteDatabase::PrepareStatement( const CStatementBuffer & sql )
{
	return PrepareStatement((const char*) sql);
}

void CSqliteDatabase::GetDatabaseList( std::vector<const char *>& databaseNames )
{
	databaseNames.empty();
	CSqliteRecordSet resultSet = ExecuteQuery("PRAGMA database_list;");
	while (resultSet.NextRow())
	{
		databaseNames.push_back(resultSet.GetString(1));
	}
}

void CSqliteDatabase::GetDatabaseList( std::vector<const char *>& databaseNames, std::vector<const char *>& databaseFiles )
{
	databaseNames.empty();
	databaseFiles.empty();
    
	CSqliteRecordSet resultSet = ExecuteQuery("PRAGMA database_list;");
	while (resultSet.NextRow())
	{
		databaseNames.push_back(resultSet.GetString(1));
		databaseFiles.push_back(resultSet.GetString(2));
	}
}

const char * CSqliteDatabase::GetDatabaseFilename( const char * databaseName )
{
	CheckDatabase();
	return sqlite3_db_filename(m_db->m_db, databaseName);
}

bool CSqliteDatabase::EnableForeignKeySupport( bool enable )
{
	if (enable)
	{
		ExecuteUpdate("PRAGMA foreign_keys=ON;");
	}
	else
	{
		ExecuteUpdate("PRAGMA foreign_keys=OFF;");
	}
	bool enabled = IsForeignKeySupportEnabled();
	return (enable && enabled) || (!enable && !enabled);
}

bool CSqliteDatabase::IsForeignKeySupportEnabled()
{
	bool enabled = false;
	CSqliteRecordSet resultSet = ExecuteQuery("PRAGMA foreign_keys;");
	if (resultSet.NextRow())
	{
		enabled = (resultSet.GetInt(0) == 1);
	}
	return enabled;
}

bool CSqliteDatabase::CheckSyntax( const char * sql )
{
	return sqlite3_complete(sql) != 0;
}

bool CSqliteDatabase::CheckSyntax( const CStatementBuffer& sql )
{
	return CheckSyntax((const char*) sql);
}


int CSqliteDatabase::ExecuteUpdate( const CStatementBuffer& sql )
{
	return ExecuteUpdate((const char*) sql);
}

int CSqliteDatabase::ExecuteUpdate( const char* sql, bool saveRC /*= false*/ )
{
	CheckDatabase();
    
	char* localError=0;
    
	int rc = sqlite3_exec(m_db->m_db, sql, 0, 0, &localError);
	if (saveRC)
	{
		if (strncmp(sql, "rollback transaction", 20) == 0)
		{
			m_lastRollbackRC = rc;
		}
	}
    
	if (rc == SQLITE_OK)
	{
		return sqlite3_changes(m_db->m_db);
	}
	else
	{
		error(rc, localError);
	}
	return -1;
}

CSqliteRecordSet CSqliteDatabase::ExecuteQuery( const CStatementBuffer& sql )
{
	return ExecuteQuery((const char*) sql);
}

CSqliteRecordSet CSqliteDatabase::ExecuteQuery( const char* sql )
{
	CheckDatabase();
    
	sqlite3_stmt* stmt = (sqlite3_stmt*) Prepare(sql);
    
	int rc = sqlite3_step(stmt);
    
	if (rc == SQLITE_DONE) // no rows
	{
		CSqliteStatementReference* stmtRef = new CSqliteStatementReference(stmt);
		return CSqliteRecordSet(m_db, stmtRef, true /* eof */);
	}
	else if (rc == SQLITE_ROW) // one or more rows
	{
		CSqliteStatementReference* stmtRef = new CSqliteStatementReference(stmt);
		return CSqliteRecordSet(m_db, stmtRef, false /* eof */);
	}
	else
	{
		rc = sqlite3_finalize(stmt);
		const char* localError= sqlite3_errmsg(m_db->m_db);
		error(rc,localError);
	}
	return CSqliteRecordSet(NULL,NULL,false);
}
CSqliteRecordSet * CSqliteDatabase::Execute(const char* sql)
{
	CheckDatabase();
	sqlite3_stmt* stmt = (sqlite3_stmt*) Prepare(sql);
	int rc = sqlite3_step(stmt);
	if (rc == SQLITE_DONE) // no rows
	{
		CSqliteStatementReference* stmtRef = new CSqliteStatementReference(stmt);
        CSqliteRecordSet * ptr = new CSqliteRecordSet(m_db, stmtRef, true /* eof */);
        //CSqliteTrackReport::shared()->AddItem(ptr, sql);
		return ptr;
	}
	else if (rc == SQLITE_ROW) // one or more rows
	{
		CSqliteStatementReference* stmtRef = new CSqliteStatementReference(stmt);
		CSqliteRecordSet * ptr = new CSqliteRecordSet(m_db, stmtRef, false /* eof */);
        //CSqliteTrackReport::shared()->AddItem(ptr, sql);
		return ptr;
	}
	else
	{
		rc = sqlite3_finalize(stmt);
		const char* localError= sqlite3_errmsg(m_db->m_db);
		error(rc,localError);
	}
	return NULL;
}
int CSqliteDatabase::ExecuteScalar( const CStatementBuffer& sql )
{
	return ExecuteScalar((const char*) sql);
}

int CSqliteDatabase::ExecuteScalar( const char* sql )
{
	CSqliteRecordSet resultSet = ExecuteQuery(sql);
    
	if (resultSet.Eof() || resultSet.GetColumnCount() < 1)
	{
		error(DATABASE_ERROR, DATABASE_ERRMSG_INVALID_QUERY);
	}
    
	long value = atol(resultSet.GetAsString(0));
	return (int) value;
}

bool CSqliteDatabase::GetAutoCommit()
{
	CheckDatabase();
	return sqlite3_get_autocommit(m_db->m_db) != 0;
}

int CSqliteDatabase::QueryRollbackState()
{
	return m_lastRollbackRC;
}

void CSqliteDatabase::Savepoint( const char * savepointName )
{
    std::string sql = "savepoint ";
	sql.append(savepointName);
	ExecuteUpdate(sql.c_str());
}

void CSqliteDatabase::ReleaseSavepoint( const char * savepointName )
{
    std::string sql = "release savepoint ";
	sql.append(savepointName);
	ExecuteUpdate(sql.c_str());
}

CSqliteTable CSqliteDatabase::GetTable( const CStatementBuffer& sql )
{
	return GetTable((const char*) sql);
}

CSqliteTable CSqliteDatabase::GetTable( const char* sql )
{
	CheckDatabase();
    
	char* localError=0;
	char** results=0;
	int rc;
	int rows(0);
	int cols(0);
    
	rc = sqlite3_get_table(m_db->m_db, sql, &results, &rows, &cols, &localError);
    
	if (rc == SQLITE_OK)
	{
		return CSqliteTable(results, rows, cols);
	}
	else
	{
		error(rc, localError);
	}
	return CSqliteTable(0, 0, 0);
}

int64 CSqliteDatabase::GetLastRowId()
{
	CheckDatabase();
	return sqlite3_last_insert_rowid(m_db->m_db);
}

CSqliteBlob CSqliteDatabase::GetReadOnlyBlob( int64 rowId, const char * columnName, const char * tableName, const char * dbName /*= NULL*/ )
{
	return GetBlob(rowId, columnName, tableName, dbName, false);
}

CSqliteBlob CSqliteDatabase::GetWritableBlob( int64 rowId, const char * columnName, const char * tableName, const char * dbName /*= NULL*/ )
{
	return GetBlob(rowId, columnName, tableName, dbName, true);
}

CSqliteBlob CSqliteDatabase::GetBlob( int64 rowId, const char * columnName, const char * tableName, const char * dbName, bool writable /*= true*/ )
{
    
	int flags = (writable) ? 1 : 0;
	sqlite3_blob* blobHandle;
	CheckDatabase();
	int rc = sqlite3_blob_open(m_db->m_db, dbName, tableName, columnName, rowId, flags, &blobHandle);
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc,localError);
	}
	CSqliteBlobReference* blobRef = new CSqliteBlobReference(blobHandle);
	return CSqliteBlob(m_db, blobRef, writable);
}

void CSqliteDatabase::Interrupt()
{
	CheckDatabase();
	sqlite3_interrupt(m_db->m_db);
}
void CSqliteDatabase::SetBusyTimeout( int milliSeconds )
{
	CheckDatabase();
	m_busyTimeoutMs = milliSeconds;
	sqlite3_busy_timeout(m_db->m_db, m_busyTimeoutMs);
}
bool CSqliteDatabase::CreateFunction(const char * name, int argCount, CScalarFunction& function)
{
    CheckDatabase();
	int rc = sqlite3_create_function(m_db->m_db, name, argCount,
                                     SQLITE_UTF8, &function,
                                     NULL,
                                     (void (*)(sqlite3_context*,int,sqlite3_value**)) CScalarFunctionContext::ExecAggregateStep,
                                     (void (*)(sqlite3_context*)) CScalarFunctionContext::ExecAggregateFinalize);
	return rc == SQLITE_OK;
}
bool CSqliteDatabase::CreateFunction(const char * name, int argCount, CAggregateFunction& function)
{
    CheckDatabase();
	int rc = sqlite3_create_function(m_db->m_db, name, argCount,
                                     SQLITE_UTF8, &function,
                                     (void (*)(sqlite3_context*,int,sqlite3_value**)) CScalarFunctionContext::ExecScalarFunction, NULL, NULL);
	return rc == SQLITE_OK;
}
bool CSqliteDatabase::SetAuthorizer(CAuthorizer& authorizer)
{
    CheckDatabase();
	int rc = sqlite3_set_authorizer(m_db->m_db, CScalarFunctionContext::ExecAuthorizer, &authorizer);
	return rc == SQLITE_OK;
}
void CSqliteDatabase::SetCommitHook(CSqliteDatabaseHook* commitHook)
{
    CheckDatabase();
	if (commitHook)
	{
		sqlite3_commit_hook(m_db->m_db, (int(*)(void*)) CScalarFunctionContext::ExecCommitHook, commitHook);
	}
	else
	{
		sqlite3_commit_hook(m_db->m_db, (int(*)(void*)) NULL, NULL);
	}
}
void CSqliteDatabase::SetRollbackHook(CSqliteDatabaseHook* rollbackHook)
{
    CheckDatabase();
	if (rollbackHook)
	{
		sqlite3_rollback_hook(m_db->m_db, (void(*)(void*)) CScalarFunctionContext::ExecRollbackHook, rollbackHook);
	}
	else
	{
		sqlite3_rollback_hook(m_db->m_db, (void(*)(void*)) NULL, NULL);
	}
}
void CSqliteDatabase::SetUpdateHook(CSqliteDatabaseHook* updateHook)
{
    CheckDatabase();
	if (updateHook)
	{
		sqlite3_update_hook(m_db->m_db, (void(*)(void*,int,const char*,const char*, int64)) CScalarFunctionContext::ExecUpdateHook, updateHook);
	}
	else
	{
		sqlite3_update_hook(m_db->m_db, (void(*)(void*,int,const char*,const char*, int64)) NULL, NULL);
	}
}
void CSqliteDatabase::SetWriteAheadLogHook(CSqliteDatabaseHook* walHook)
{
    CheckDatabase();
	if (walHook)
	{
		walHook->SetDatabase(this);
		sqlite3_wal_hook(m_db->m_db, (int(*)(void *,sqlite3*,const char*,int)) CScalarFunctionContext::ExecWriteAheadLogHook, walHook);
	}
	else
	{
		sqlite3_wal_hook(m_db->m_db, (int(*)(void *,sqlite3*,const char*,int)) NULL, NULL);
	}
}


void CSqliteDatabase::WriteAheadLogCheckpoint( const char * database, int mode /*= DATABASE_CHECKPOINT_PASSIVE*/, int* logFrameCount /*= NULL*/, int* ckptFrameCount /*= NULL*/ )
{
	CheckDatabase();
#if USE_SQLITE_V2
	int rc = sqlite3_wal_checkpoint_v2(m_db->m_db, database, mode, logFrameCount, ckptFrameCount);
#else
	int rc = sqlite3_wal_checkpoint(m_db->m_db, database);
#endif
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc, localError);
	}
}

void CSqliteDatabase::AutoWriteAheadLogCheckpoint( int frameCount )
{
	CheckDatabase();
	int rc = sqlite3_wal_autocheckpoint(m_db->m_db, frameCount);
    
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc, localError);
	}
}

void CSqliteDatabase::ReKey( const char * newKey )
{
	int rc = sqlite3_rekey(m_db->m_db, newKey, strlen(newKey));
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc,localError);
	}
}

int CSqliteDatabase::GetLimit( DatabaseLimitType id )
{
	int value = -1;
	CheckDatabase();
	if (id >= DATABASE_LIMIT_LENGTH && id <= DATABASE_LIMIT_VARIABLE_NUMBER)
	{
		value = sqlite3_limit(m_db->m_db, id, -1);
	}
	return value;
}

int CSqliteDatabase::SetLimit( DatabaseLimitType id, int newValue )
{
	int value = -1;
	CheckDatabase();
	if (id >= DATABASE_LIMIT_LENGTH && id <= DATABASE_LIMIT_VARIABLE_NUMBER)
	{
		value = sqlite3_limit(m_db->m_db, id, newValue);
	}
	return value;
}

void CSqliteDatabase::ReleaseMemory()
{
	CheckDatabase();
	int rc = sqlite3_db_release_memory(m_db->m_db);
	if (rc != SQLITE_OK)
	{
		const char* localError = sqlite3_errmsg(m_db->m_db);
		error(rc, localError);
	}
}


static const char* limitCodeString[] =
{ ("SQLITE_LIMIT_LENGTH"),              ("SQLITE_LIMIT_SQL_LENGTH"),
    ("SQLITE_LIMIT_COLUMN"),              ("SQLITE_LIMIT_EXPR_DEPTH"),
    ("SQLITE_LIMIT_COMPOUND_SELECT"),     ("SQLITE_LIMIT_VDBE_OP"),
    ("SQLITE_LIMIT_FUNCTION_ARG"),        ("SQLITE_LIMIT_ATTACHED"),
    ("SQLITE_LIMIT_LIKE_PATTERN_LENGTH"), ("SQLITE_LIMIT_VARIABLE_NUMBER"),
    ("SQLITE_LIMIT_TRIGGER_DEPTH")
};

const char * CSqliteDatabase::LimitTypeToString( DatabaseLimitType type )
{
	const char * limitString = ("Unknown");
	if (type >= DATABASE_LIMIT_LENGTH && type <= DATABASE_LIMIT_VARIABLE_NUMBER)
	{
		limitString = limitCodeString[type];
	}
	return limitString;
}

void CSqliteDatabase::Initialize()
{
	int rc = sqlite3_initialize();
	if (rc != SQLITE_OK)
	{
		error(rc,DATABASE_ERRMSG_INITIALIZE);
	}
}

void CSqliteDatabase::Shutdown()
{
	int rc = sqlite3_shutdown();
	if (rc != SQLITE_OK)
	{
		error(rc, DATABASE_ERRMSG_SHUTDOWN);
	}
}

void CSqliteDatabase::SetSharedCache( bool enable )
{
	int flag = (enable) ? 1 : 0;
	int rc = sqlite3_enable_shared_cache(flag);
	if (rc != SQLITE_OK)
	{
		error(rc, DATABASE_ERRMSG_SHARED_CACHE);
	}
	ms_sharedCacheEnabled = enable;
}

const char * CSqliteDatabase::GetVersion()
{
	return sqlite3_libversion();
}

const char * CSqliteDatabase::GetSourceId()
{
	return sqlite3_sourceid();
}

bool CSqliteDatabase::CompileOptionUsed( const char * optionName )
{
	return sqlite3_compileoption_used(optionName) == 1;
}

const char * CSqliteDatabase::GetCompileOptionName( int optionIndex )
{
	return sqlite3_compileoption_get(optionIndex);
}

void* CSqliteDatabase::GetDatabaseHandle()
{
	return m_db->m_db;
}


void CSqliteDatabase::CheckDatabase()
{
    if (m_db == NULL || m_db->m_db == NULL || !m_db->m_isValid || !m_isOpen)
	{
		error(DATABASE_ERROR, DATABASE_ERRMSG_NODB);
	}
}
void CSqliteDatabase::error(int errorCode,const char * lpstrError)
{
    std::string strError = ErrorCodeAsString(errorCode);
    if(lpstrError)
        strError.append(lpstrError);
    
    m_lastError = strError;
    
    //handle error code
    
    
}

const char * CSqliteDatabase::ErrorCodeAsString(int errorCode)
{
    switch (errorCode)
	{
        case SQLITE_OK          : return ("SQLITE_OK");
        case SQLITE_ERROR       : return ("SQLITE_ERROR");
        case SQLITE_INTERNAL    : return ("SQLITE_INTERNAL");
        case SQLITE_PERM        : return ("SQLITE_PERM");
        case SQLITE_ABORT       : return ("SQLITE_ABORT");
        case SQLITE_BUSY        : return ("SQLITE_BUSY");
        case SQLITE_LOCKED      : return ("SQLITE_LOCKED");
        case SQLITE_NOMEM       : return ("SQLITE_NOMEM");
        case SQLITE_READONLY    : return ("SQLITE_READONLY");
        case SQLITE_INTERRUPT   : return ("SQLITE_INTERRUPT");
        case SQLITE_IOERR       : return ("SQLITE_IOERR");
        case SQLITE_CORRUPT     : return  ("SQLITE_CORRUPT");
        case SQLITE_NOTFOUND    : return  ("SQLITE_NOTFOUND");
        case SQLITE_FULL        : return  ("SQLITE_FULL");
        case SQLITE_CANTOPEN    : return  ("SQLITE_CANTOPEN");
        case SQLITE_PROTOCOL    : return  ("SQLITE_PROTOCOL");
        case SQLITE_EMPTY       : return  ("SQLITE_EMPTY");
        case SQLITE_SCHEMA      : return  ("SQLITE_SCHEMA");
        case SQLITE_TOOBIG      : return  ("SQLITE OOBIG");
        case SQLITE_CONSTRAINT  : return  ("SQLITE_CONSTRAINT");
        case SQLITE_MISMATCH    : return  ("SQLITE_MISMATCH");
        case SQLITE_MISUSE      : return  ("SQLITE_MISUSE");
        case SQLITE_NOLFS       : return  ("SQLITE_NOLFS");
        case SQLITE_AUTH        : return  ("SQLITE_AUTH");
        case SQLITE_FORMAT      : return  ("SQLITE_FORMAT");
        case SQLITE_RANGE       : return  ("SQLITE_RANGE");
        case SQLITE_NOTADB      : return  ("SQLITE_NOTADB");
        case SQLITE_ROW         : return  ("SQLITE_ROW");
        case SQLITE_DONE        : return  ("SQLITE_DONE");
            // Extended error codes
        case SQLITE_IOERR_READ       : return  ("SQLITE_IOERR_READ");
        case SQLITE_IOERR_SHORT_READ : return  ("SQLITE_IOERR_SHORT_READ");
        case SQLITE_IOERR_WRITE      : return  ("SQLITE_IOERR_WRITE");
        case SQLITE_IOERR_FSYNC      : return  ("SQLITE_IOERR_FSYNC");
        case SQLITE_IOERR_DIR_FSYNC  : return  ("SQLITE_IOERR_DIR_FSYNC");
        case SQLITE_IOERR_TRUNCATE   : return  ("SQLITE_IOERR_TRUNCATE");
        case SQLITE_IOERR_FSTAT      : return  ("SQLITE_IOERR_FSTAT");
        case SQLITE_IOERR_UNLOCK     : return  ("SQLITE_IOERR_UNLOCK");
        case SQLITE_IOERR_RDLOCK     : return  ("SQLITE_IOERR_RDLOCK");
        case SQLITE_IOERR_DELETE     : return  ("SQLITE_IOERR_DELETE");
#if SQLITE_VERSION_NUMBER >= 3004000
        case SQLITE_IOERR_BLOCKED    : return  ("SQLITE_IOERR_BLOCKED");
#endif
#if SQLITE_VERSION_NUMBER >= 3005001
        case SQLITE_IOERR_NOMEM      : return  ("SQLITE_IOERR_NOMEM");
#endif
#if SQLITE_VERSION_NUMBER >= 3006000
        case SQLITE_IOERR_ACCESS     : return  ("SQLITE_IOERR_ACCESS");
        case SQLITE_IOERR_CHECKRESERVEDLOCK : return  ("SQLITE_IOERR_CHECKRESERVEDLOCK");
#endif
#if SQLITE_VERSION_NUMBER >= 3006002
        case SQLITE_IOERR_LOCK       : return  ("SQLITE_IOERR_LOCK");
#endif
#if SQLITE_VERSION_NUMBER >= 3006007
        case SQLITE_IOERR_CLOSE      : return  ("SQLITE_IOERR_CLOSE");
        case SQLITE_IOERR_DIR_CLOSE  : return  ("SQLITE_IOERR_DIR_CLOSE");
#endif
#if SQLITE_VERSION_NUMBER >= 3007000
        case SQLITE_IOERR_SHMOPEN      : return  ("SQLITE_IOERR_SHMOPEN");
        case SQLITE_IOERR_SHMSIZE      : return  ("SQLITE_IOERR_SHMSIZE");
        case SQLITE_IOERR_SHMLOCK      : return  ("SQLITE_IOERR_SHMLOCK");
        case SQLITE_LOCKED_SHAREDCACHE : return  ("SQLITE_LOCKED_SHAREDCACHE");
        case SQLITE_BUSY_RECOVERY      : return  ("SQLITE_BUSY_RECOVERY");
        case SQLITE_CANTOPEN_NOTEMPDIR : return  ("SQLITE_CANTOPEN_NOTEMPDIR");
#endif
#if SQLITE_VERSION_NUMBER >= 3007007
        case SQLITE_CORRUPT_VTAB       : return  ("SQLITE_CORRUPT_VTAB");
        case SQLITE_READONLY_RECOVERY  : return  ("SQLITE_READONLY_RECOVERY");
        case SQLITE_READONLY_CANTLOCK  : return  ("SQLITE_READONLY_CANTLOCK");
#endif
            
        case DATABASE_ERROR     : return  ("DATABASE_ERROR");
        default                 : return  ("UNKNOWN_ERROR");
	}
}