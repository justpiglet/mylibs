//
//  SqliteDatabasePro.h
//  TKGame
//
//  Created by Hotman on 13-3-18.
//  Copyright (c) 2013年 Hotman. All rights reserved.
//

#ifndef _FHKJEF_FDS_CAN_SqliteDatabasePro_h
#define _FHKJEF_FDS_CAN_SqliteDatabasePro_h

#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include <string>
#include <vector>
#include <stdlib.h>

#define DATABASE_OPEN_READONLY         0x00000001
#define DATABASE_OPEN_READWRITE        0x00000002
#define DATABASE_OPEN_CREATE           0x00000004
#define DATABASE_OPEN_NOMUTEX          0x00008000
#define DATABASE_OPEN_FULLMUTEX        0x00010000
#define DATABASE_OPEN_SHAREDCACHE      0x00020000
#define DATABASE_OPEN_PRIVATECACHE     0x00040000

#define DATABASE_ERROR 1000


enum DatabaseTransactionType
{
	DATABASE_TRANSACTION_DEFAULT,
	DATABASE_TRANSACTION_DEFERRED,
	DATABASE_TRANSACTION_IMMEDIATE,
	DATABASE_TRANSACTION_EXCLUSIVE
};

#define DATABASE_CHECKPOINT_PASSIVE 0
#define DATABASE_CHECKPOINT_FULL    1
#define DATABASE_CHECKPOINT_RESTART 2

enum DatabaseLimitType
{
	DATABASE_LIMIT_LENGTH              = 0,
	DATABASE_LIMIT_SQL_LENGTH          = 1,
	DATABASE_LIMIT_COLUMN              = 2,
	DATABASE_LIMIT_EXPR_DEPTH          = 3,
	DATABASE_LIMIT_COMPOUND_SELECT     = 4,
	DATABASE_LIMIT_VDBE_OP             = 5,
	DATABASE_LIMIT_FUNCTION_ARG        = 6,
	DATABASE_LIMIT_ATTACHED            = 7,
	DATABASE_LIMIT_LIKE_PATTERN_LENGTH = 8,
	DATABASE_LIMIT_VARIABLE_NUMBER     = 9,
	DATABASE_LIMIT_TRIGGER_DEPTH       = 10
};

#define USE_SQLITE_V2 1
#ifndef SQLITE_HAS_CODEC
#define SQLITE_HAS_CODEC
#endif
#ifndef SQLITE_ENABLE_COLUMN_METADATA
#define SQLITE_ENABLE_COLUMN_METADATA
#endif

#ifndef SQLITE_OUTSIDE
typedef unsigned	char				uint8;
typedef signed		char				int8;
typedef	unsigned	short				uint16;
typedef	signed		short				int16;
typedef	unsigned	long				uint32;
typedef	signed		long				int32;
#define COMMON_VARIABLE
#if defined(_MSC_VER) && !defined(__MWRKS__) && defined(_M_IX86)
typedef __int64							int64;
typedef unsigned __int64				uint64;
#else
typedef signed	long long				int64;
typedef unsigned long long				uint64;
#endif
#endif


//#include "sqlite/sqlite3.h"

//#include "SqliteError.h"
//#include "SqliteReference.h"
//#include "SqliteFunction.h"
//#include "SqliteBackupProgress.h"
//#include "StatementBuffer.h"
//#include "SqliteRecord.h"
//#include "SqliteRecordSet.h"
//#include "SqliteStatement.h"
//#include "SqliteTable.h"
//#include "SqliteBlob.h"

//#include "SqliteDatabase.h"
//#include "SqliteTrackReport.h"
#endif
