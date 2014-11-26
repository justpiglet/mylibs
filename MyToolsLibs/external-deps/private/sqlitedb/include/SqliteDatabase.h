//
//  SqliteDatabase.h
//  
//
//

#ifndef __CANERRW__SqliteDatabase__
#define __CANERRW__SqliteDatabase__

#include "SqliteDatabasePro.h"
#include "SqliteReference.h"
#include "SqliteBackupProgress.h"
#include "StatementBuffer.h"
#include "SqliteTable.h"
#include "SqliteFunction.h"

class CSqliteDatabase {
private:
    static std::string m_lastError;
    CSqliteDatabaseReference* m_db;
    bool  m_isEncrypted;
	bool  m_isOpen;
    
    int   m_busyTimeoutMs;
    
    int   m_lastRollbackRC;
	int   m_backupPageCount;
    
	static bool  ms_sharedCacheEnabled;
    
public:
    CSqliteDatabase();
    ~CSqliteDatabase();
    
    static CSqliteDatabase * Create(const char * lpstrData,const char *lpstrPassword,int flags = DATABASE_OPEN_READWRITE | DATABASE_OPEN_CREATE);
    
    bool Open(const char * lpstrData,const char * lpstrPassword = NULL,
              int flags = DATABASE_OPEN_READWRITE | DATABASE_OPEN_CREATE);
    
    
    bool IsOpen() const;
	void Close();
    
    void Backup(const char *  targetFileName, const char * key = NULL,
                const char * sourceDatabaseName = ("main"));
    
	void Backup(CSqliteBackupProgress* progressCallback,
                const char * targetFileName, const char * key = NULL,
                const char * sourceDatabaseName = ("main"));
    
	void Restore(const char * sourceFileName, const char * key = NULL,
                 const char * targetDatabaseName = ("main"));
	void Restore(CSqliteBackupProgress* progressCallback,
                 const char * sourceFileName, const char * key = NULL,
                 const char * targetDatabaseName = ("main"));

	void SetBackupRestorePageCount(int pageCount);
    
	void Vacuum();
    
	void Begin(DatabaseTransactionType transactionType = DATABASE_TRANSACTION_DEFAULT);
    
	void Commit();
    
	void Rollback(const char * savepointName = NULL);
	bool GetAutoCommit();
    
	int QueryRollbackState();
	void Savepoint(const char * savepointName);
	void ReleaseSavepoint(const char * savepointName);
    
	bool TableExists(const char * tableName, const char * databaseName = NULL);
	bool TableExists(const char * tableName, std::vector<const char *> & databaseNames);
    
	void GetDatabaseList(std::vector<const char *>& databaseNames);
    
	void GetDatabaseList(std::vector<const char *>& databaseNames, std::vector<const char *>& databaseFiles);
    
	const char * GetDatabaseFilename(const char * databaseName);
    
	bool EnableForeignKeySupport(bool enable);
    
	bool IsForeignKeySupportEnabled();
    
	bool CheckSyntax(const char * sql);

	bool CheckSyntax(const CStatementBuffer& sql);
    
	int ExecuteUpdate(const CStatementBuffer& sql);
	int ExecuteUpdate(const char* sql, bool saveRC = false);
    
    
	CSqliteRecordSet ExecuteQuery(const CStatementBuffer& sql);
	CSqliteRecordSet ExecuteQuery(const char* sql);
	CSqliteRecordSet * Execute(const char* sql);

	int ExecuteScalar(const CStatementBuffer& sql);
	int ExecuteScalar(const char* sql);
    
	CSqliteTable GetTable(const CStatementBuffer& sql);
	CSqliteTable GetTable(const char* sql);
    
	CSqliteStatement PrepareStatement(const char * sql);
	CSqliteStatement PrepareStatement(const CStatementBuffer & sql);
    
	int64 GetLastRowId();
    
	CSqliteBlob GetReadOnlyBlob(int64 rowId,
                              const char * columnName,
                              const char * tableName,
                              const char * dbName = NULL);
    
	CSqliteBlob GetWritableBlob(int64 rowId,
                              const char * columnName,
                              const char * tableName,
                              const char * dbName = NULL);
    
	CSqliteBlob GetBlob(int64 rowId,
                      const char * columnName,
                      const char * tableName,
                      const char * dbName,
                      bool writable = true);
    
	std::string GetLastError(){ return m_lastError; }
public:
    void Interrupt();
    void SetBusyTimeout(int milliSeconds);
    
    bool CreateFunction(const char * name, int argCount, CScalarFunction& function);
	bool CreateFunction(const char * name, int argCount, CAggregateFunction& function);
    
	bool SetAuthorizer(CAuthorizer& authorizer);
	void SetCommitHook(CSqliteDatabaseHook* commitHook);
	void SetRollbackHook(CSqliteDatabaseHook* rollbackHook);
	void SetUpdateHook(CSqliteDatabaseHook* updateHook);
	void SetWriteAheadLogHook(CSqliteDatabaseHook* walHook);
    
    void WriteAheadLogCheckpoint(const char * database, int mode = DATABASE_CHECKPOINT_PASSIVE,
                                 int* logFrameCount = NULL, int* ckptFrameCount = NULL);
    
	void AutoWriteAheadLogCheckpoint(int frameCount);
    
	void ReKey(const char * newKey);
	bool IsEncrypted() const { return m_isEncrypted; }
    
    int GetLimit(DatabaseLimitType id);
	int SetLimit(DatabaseLimitType id, int newValue);
	void ReleaseMemory();
    
	static const char * LimitTypeToString(DatabaseLimitType type);
    
	static void Initialize();
	static void Shutdown();
    
	static void SetSharedCache(bool enable);
	static bool IsSharedCacheEnabled() { return ms_sharedCacheEnabled; }
	static const char * GetVersion();
	static const char * GetSourceId();
	static bool CompileOptionUsed(const char * optionName);
	static const char * GetCompileOptionName(int optionIndex);
    static void error(int errorCode,const char * lpstrError);
protected:
	void* GetDatabaseHandle();
private:
	void CheckDatabase();
    void* Prepare(const char* sql);
	void Close(CSqliteDatabaseReference* db);
    
    static const char * ErrorCodeAsString(int errorCode);
};

#endif /* defined(__TKGame__SqliteDatabase__) */
