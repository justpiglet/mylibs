//
//  SqliteFunction.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteFunction__
#define _FHKJEF_FDS_CAN_SqliteFunction__

#include "SqliteDatabasePro.h"

class CScalarFunctionContext
{
private:
	void*  m_ctx;
	bool   m_isAggregate;
	int    m_count;
	int    m_argc;
	void** m_argv;
public:
    int GetArgCount();
    int GetArgType(int argIndex);
    bool IsNull(int argIndex);
    int GetInt(int argIndex, int nullValue = 0);
    int64 GetInt64(int argIndex, int64 nullValue = 0);
    double GetDouble(int argIndex, double nullValue = 0);
    const char * GetString(int argIndex, const char * nullValue = NULL);
    const unsigned char* GetBlob(int argIndex, int& len);
    //CStackBuffer& GetBlob(int argIndex, CStackBuffer& buffer);
    void SetResult(int value);
    void SetResult(int64 value);
    void SetResult(double value);
    void SetResult(const char *& value);
    void SetResult(unsigned char* value, int len);
    //void SetResult(CStackBuffer& buffer);
    void SetResultNull();
    void SetResultZeroBlob(int blobSize);
    void SetResultArg(int argIndex);
    void SetResultError(const char * errmsg);
    int GetAggregateCount();
    void* GetAggregateStruct(int len);
    
    static void ExecScalarFunction(void* ctx, int argc, void** argv);
    
    static void ExecAggregateStep(void* ctx, int argc, void** argv);
    
    static void ExecAggregateFinalize(void* ctx);
    
    static int  ExecAuthorizer(void*, int type,
                               const char* arg1, const char* arg2,
                               const char* arg3, const char* arg4);
    
    static int ExecCommitHook(void* hook);
    
    static void ExecRollbackHook(void* hook);
    
    static void ExecUpdateHook(void* hook, int type,
                               const char* database, const char* table,
                               int64 rowid);
    
    static int ExecWriteAheadLogHook(void* hook, void* dbHandle,
                                     const char* database, int numPages);
    
private:
    CScalarFunctionContext(void* ctx, bool isAggregate, int argc = 0, void** argv = NULL);
    
    //CScalarFunctionContext(CScalarFunctionContext& ctx);
    
    
};

class CScalarFunction
{
public:
    CScalarFunction() {}
    
    virtual ~CScalarFunction() {}
    virtual void Execute(CScalarFunctionContext& ctx) = 0;
};

class CAggregateFunction
{
public:
    CAggregateFunction() { m_count = 0; }
    virtual ~CAggregateFunction() {}
    virtual void Aggregate(CScalarFunctionContext& ctx) = 0;
    virtual void Finalize(CScalarFunctionContext& ctx) = 0;
private:
    int    m_count;
    friend class CScalarFunctionContext;
};


class CAuthorizer
{
public:
    enum AuthorizationCode
    {                                    // arg1 =          arg2 =
        SQLITE_AC_COPY                =  0,   // Table Name      File Name
        SQLITE_AC_CREATE_INDEX        =  1,   // Index Name      Table Name
        SQLITE_AC_CREATE_TABLE        =  2,   // Table Name      NULL
        SQLITE_AC_CREATE_TEMP_INDEX   =  3,   // Index Name      Table Name
        SQLITE_AC_CREATE_TEMP_TABLE   =  4,   // Table Name      NULL
        SQLITE_AC_CREATE_TEMP_TRIGGER =  5,   // Trigger Name    Table Name
        SQLITE_AC_CREATE_TEMP_VIEW    =  6,   // View Name       NULL
        SQLITE_AC_CREATE_TRIGGER      =  7,   // Trigger Name    Table Name
        SQLITE_AC_CREATE_VIEW         =  8,   // View Name       NULL
        SQLITE_AC_DELETE              =  9,   // Table Name      NULL
        SQLITE_AC_DROP_INDEX          = 10,   // Index Name      Table Name
        SQLITE_AC_DROP_TABLE          = 11,   // Table Name      NULL
        SQLITE_AC_DROP_TEMP_INDEX     = 12,   // Index Name      Table Name
        SQLITE_AC_DROP_TEMP_TABLE     = 13,   // Table Name      NULL
        SQLITE_AC_DROP_TEMP_TRIGGER   = 14,   // Trigger Name    Table Name
        SQLITE_AC_DROP_TEMP_VIEW      = 15,   // View Name       NULL
        SQLITE_AC_DROP_TRIGGER        = 16,   // Trigger Name    Table Name
        SQLITE_AC_DROP_VIEW           = 17,   // View Name       NULL
        SQLITE_AC_INSERT              = 18,   // Table Name      NULL
        SQLITE_AC_PRAGMA              = 19,   // Pragma Name     1st arg or NULL
        SQLITE_AC_READ                = 20,   // Table Name      Column Name
        SQLITE_AC_SELECT              = 21,   // NULL            NULL
        SQLITE_AC_TRANSACTION         = 22,   // Operation       NULL
        SQLITE_AC_UPDATE              = 23,   // Table Name      Column Name
        SQLITE_AC_ATTACH              = 24,   // Filename        NULL
        SQLITE_AC_DETACH              = 25,   // Database Name   NULL
        SQLITE_AC_ALTER_TABLE         = 26,   // Database Name   Table Name
        SQLITE_AC_REINDEX             = 27,   // Index Name      NULL
        SQLITE_AC_ANALYZE             = 28,   // Table Name      NULL
        SQLITE_AC_CREATE_VTABLE       = 29,   // Table Name      Module Name
        SQLITE_AC_DROP_VTABLE         = 30,   // Table Name      Module Name
        SQLITE_AC_FUNCTION            = 31,   // NULL            Function Name
        SQLITE_AC_SAVEPOINT           = 32,   // Operation       Savepoint Name
        SQLITE_AC_MAX_CODE            = SQLITE_AC_SAVEPOINT
    };
    
    enum AuthorizationResult
    {
        SQLITE_AR_OK     = 0,   // Allow access
        SQLITE_AR_DENY   = 1,   // Abort the SQL statement with an error
        SQLITE_AR_IGNORE = 2    // Don't allow access, but don't generate an error
    };
    
    virtual ~CAuthorizer() {}
    virtual AuthorizationResult Authorize(AuthorizationCode type,
                                          const char * arg1, const char * arg2,
                                          const char * arg3, const char * arg4) = 0;
    
    static const char * AuthorizationCodeToString(CAuthorizer::AuthorizationCode type);
};

class CSqliteDatabase;
class CSqliteDatabaseHook
{
public:
    enum UpdateType
    {
        SQLITE_UT_DELETE              =  9,
        SQLITE_UT_INSERT              = 18,
        SQLITE_UT_UPDATE              = 23
    };
    CSqliteDatabaseHook() : m_db(NULL) {}
    
    virtual ~CSqliteDatabaseHook() {}
    virtual bool CommitCallback() { return false; }
    virtual void RollbackCallback() {}
    virtual void UpdateCallback(UpdateType type,
                                const char * database, const char * table,
                                int64 rowid) {}
    
    virtual int WriteAheadLogCallback(const char * database,
                                      int numPages) { return 0; }
    
    void SetDatabase(CSqliteDatabase* db) { m_db = db; }
    
    
    CSqliteDatabase* GetDatabase() const { return m_db; }
    
private:
    CSqliteDatabase* m_db;
};
#endif /* defined(__TKGame__SqliteFunction__) */
