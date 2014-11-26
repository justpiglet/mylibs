//
//  SqliteFunction.cpp
//

#include "SqliteFunction.h"
#include "sqlite/sqlite3.h"

CScalarFunctionContext::CScalarFunctionContext( void* ctx, bool isAggregate, int argc /*= 0*/, void** argv /*= NULL*/ )
: m_ctx(ctx), m_isAggregate(isAggregate), m_count(0), m_argc(argc), m_argv(argv)
{
    
}

int CScalarFunctionContext::GetArgCount()
{
	return m_argc;
}

int CScalarFunctionContext::GetArgType( int argIndex )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		return sqlite3_value_type((sqlite3_value*) m_argv[argIndex]);
	}
	else
	{
		return SQLITE_NULL;
	}
}

bool CScalarFunctionContext::IsNull( int argIndex )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		return sqlite3_value_type((sqlite3_value*) m_argv[argIndex]) == SQLITE_NULL;
	}
	else
	{
		return true;
	}
}

int CScalarFunctionContext::GetInt( int argIndex, int nullValue /*= 0*/ )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			return sqlite3_value_int((sqlite3_value*) m_argv[argIndex]);
		}
		else
		{
			return nullValue;
		}
	}
	else
	{
		return nullValue;
	}
}

int64 CScalarFunctionContext::GetInt64( int argIndex, int64 nullValue /*= 0*/ )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			return int64(sqlite3_value_int64((sqlite3_value*) m_argv[argIndex]));
		}
		else
		{
			return nullValue;
		}
	}
	else
	{
		return nullValue;
	}
}

double CScalarFunctionContext::GetDouble( int argIndex, double nullValue /*= 0*/ )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			return sqlite3_value_double((sqlite3_value*) m_argv[argIndex]);
		}
		else
		{
			return nullValue;
		}
	}
	else
	{
		return nullValue;
	}
}

const char * CScalarFunctionContext::GetString( int argIndex, const char * nullValue /*= NULL*/ )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			return (const char*) sqlite3_value_text((sqlite3_value*) m_argv[argIndex]);
		}
		else
		{
			return nullValue;
		}
	}
	else
	{
		return nullValue;
	}
}

const unsigned char* CScalarFunctionContext::GetBlob( int argIndex, int& len )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			len= sqlite3_value_bytes((sqlite3_value*) m_argv[argIndex]);
            
			const void* blob = sqlite3_value_blob((sqlite3_value*) m_argv[argIndex]);
			return (const unsigned char*)blob;
		}
	}
	return NULL;
}
/*
CStackBuffer& CScalarFunctionContext::GetBlob( int argIndex, CStackBuffer& buffer )
{
	if (argIndex >= 0 && argIndex < m_argc)
	{
		if (!IsNull(argIndex))
		{
			int len = sqlite3_value_bytes((sqlite3_value*) m_argv[argIndex]);
			buffer.InitBuffer(len);
            
			const void* blob = sqlite3_value_blob((sqlite3_value*) m_argv[argIndex]);
			buffer.Write((uint8 *)blob,len);
		}
	}
	return buffer;
}*/

void CScalarFunctionContext::SetResult(int value)
{
	sqlite3_result_int((sqlite3_context*) m_ctx, value);
}

void CScalarFunctionContext::SetResult(int64 value)
{
	sqlite3_result_int64((sqlite3_context*) m_ctx, value);
}

void CScalarFunctionContext::SetResult(double value)
{
	sqlite3_result_double((sqlite3_context*) m_ctx, value);
}

void CScalarFunctionContext::SetResult( const char *& value )
{
	sqlite3_result_text((sqlite3_context*) m_ctx, value, -1, SQLITE_TRANSIENT);
}

void CScalarFunctionContext::SetResult( unsigned char* value, int len )
{
	sqlite3_result_blob((sqlite3_context*) m_ctx, value, len, SQLITE_TRANSIENT);
}
/*
void CScalarFunctionContext::SetResult( CStackBuffer& buffer )
{
	sqlite3_result_blob((sqlite3_context*) m_ctx, buffer.GetBuffer(), (int) buffer.GetBufferSize(), SQLITE_TRANSIENT);
}
*/
void CScalarFunctionContext::SetResultNull()
{
	sqlite3_result_null((sqlite3_context*) m_ctx);
}

void CScalarFunctionContext::SetResultZeroBlob(int blobSize)
{
	sqlite3_result_zeroblob((sqlite3_context*) m_ctx, blobSize);
}

void CScalarFunctionContext::SetResultArg(int argIndex)
{
	if (argIndex >= 0 && argIndex < m_argc) {
		sqlite3_result_value((sqlite3_context*) m_ctx, (sqlite3_value*) m_argv[argIndex]);
	} else {
		sqlite3_result_null((sqlite3_context*) m_ctx);
	}
}

void CScalarFunctionContext::SetResultError( const char * errmsg )
{
	sqlite3_result_error((sqlite3_context*) m_ctx, errmsg, -1);
}

int CScalarFunctionContext::GetAggregateCount()
{
	if (m_isAggregate)
	{
		return m_count;
	}
	else
	{
		return 0;
	}
}

void* CScalarFunctionContext::GetAggregateStruct(int len)
{
	if (m_isAggregate)
	{
		return sqlite3_aggregate_context((sqlite3_context*) m_ctx, len);
	}
	else
	{
		return NULL;
	}
}

/* static */
void CScalarFunctionContext::ExecScalarFunction(void* ctx, int argc, void** argv)
{
	CScalarFunctionContext context(ctx, false, argc, argv);
	CScalarFunction* func = (CScalarFunction*) sqlite3_user_data((sqlite3_context*) ctx);
	func->Execute(context);
}

/* static */
void CScalarFunctionContext::ExecAggregateStep(void* ctx, int argc, void** argv)
{
	CScalarFunctionContext context(ctx, true, argc, argv);
	CAggregateFunction* func = (CAggregateFunction*) sqlite3_user_data((sqlite3_context*) ctx);
	func->m_count++;
	context.m_count = func->m_count;
	func->Aggregate(context);
}

/* static */
void CScalarFunctionContext::ExecAggregateFinalize(void* ctx)
{
	CScalarFunctionContext context(ctx, true, 0, NULL);
	CAggregateFunction* func = (CAggregateFunction*) sqlite3_user_data((sqlite3_context*) ctx);
	context.m_count = func->m_count;
	func->Finalize(context);
}

/* static */
int CScalarFunctionContext::ExecAuthorizer(void* func, int type,
                                           const char* arg1, const char* arg2,
                                           const char* arg3, const char* arg4)
{
	CAuthorizer::AuthorizationCode localType = (CAuthorizer::AuthorizationCode) type;
	return (int) ((CAuthorizer*) func)->Authorize(localType, arg1, arg2, arg3, arg4);
}

/* static */
int CScalarFunctionContext::ExecCommitHook(void* hook)
{
	return (int) ((CSqliteDatabaseHook*) hook)->CommitCallback();
}

/* static */
void CScalarFunctionContext::ExecRollbackHook(void* hook)
{
	((CSqliteDatabaseHook*) hook)->RollbackCallback();
}

/* static */
void CScalarFunctionContext::ExecUpdateHook(void* hook, int type,
                                            const char* database, const char* table,
                                            int64 rowid)
{
    
	CSqliteDatabaseHook::UpdateType locType = (CSqliteDatabaseHook::UpdateType) type;
	int64 locRowid = rowid;
	((CSqliteDatabaseHook*) hook)->UpdateCallback(locType, database, table, locRowid);
}


/* static */
int CScalarFunctionContext::ExecWriteAheadLogHook(void* hook, void* dbHandle,
                                                  const char* database, int numPages)
{
    
	return (int) ((CSqliteDatabaseHook*) hook)->WriteAheadLogCallback(database, numPages);
}

static const char* authCodeString[] =
{ ("SQLITE_COPY"),              ("SQLITE_CREATE_INDEX"),      ("SQLITE_CREATE_TABLE"),
    ("SQLITE_CREATE_TEMP_INDEX"), ("SQLITE_CREATE_TEMP_TABLE"), ("SQLITE_CREATE_TEMP_TRIGGER"),
    ("SQLITE_CREATE_TEMP_VIEW"),  ("SQLITE_CREATE_TRIGGER"),    ("SQLITE_CREATE_VIEW"),
    ("SQLITE_DELETE"),            ("SQLITE_DROP_INDEX"),        ("SQLITE_DROP_TABLE"),
    ("SQLITE_DROP_TEMP_INDEX"),   ("SQLITE_DROP_TEMP_TABLE"),   ("SQLITE_DROP_TEMP_TRIGGER"),
    ("SQLITE_DROP_TEMP_VIEW"),    ("SQLITE_DROP_TRIGGER"),      ("SQLITE_DROP_VIEW"),
    ("SQLITE_INSERT"),            ("SQLITE_PRAGMA"),            ("SQLITE_READ"),
    ("SQLITE_SELECT"),            ("SQLITE_TRANSACTION"),       ("SQLITE_UPDATE"),
    ("SQLITE_ATTACH"),            ("SQLITE_DETACH"),            ("SQLITE_ALTER_TABLE"),
    ("SQLITE_REINDEX"),           ("SQLITE_ANALYZE"),           ("SQLITE_CREATE_VTABLE"),
    ("SQLITE_DROP_VTABLE"),       ("SQLITE_FUNCTION"),          ("SQLITE_SAVEPOINT")
};

/* static */
const char * CAuthorizer::AuthorizationCodeToString(CAuthorizer::AuthorizationCode type)
{
	const char* authString = ("Unknown");
	if (type >= SQLITE_AC_COPY && type <= SQLITE_AC_MAX_CODE)
	{
		authString = authCodeString[type];
	}
	return authString;
}