//
//  SqliteError.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteError_h
#define _FHKJEF_FDS_CAN_SqliteError_h



#define DATABASE_ERRMSG_NODB            ("No Database opened")
#define DATABASE_ERRMSG_NOSTMT          ("Statement not accessible")
#define DATABASE_ERRMSG_NOMEM           ("Out of memory")
#define DATABASE_ERRMSG_DECODE          ("Cannot decode binary")
#define DATABASE_ERRMSG_INVALID_INDEX   ("Invalid field index")
#define DATABASE_ERRMSG_INVALID_NAME    ("Invalid field name")
#define DATABASE_ERRMSG_INVALID_ROW     ("Invalid row index")
#define DATABASE_ERRMSG_INVALID_QUERY   ("Invalid scalar query")
#define DATABASE_ERRMSG_INVALID_BLOB    ("Invalid BLOB handle")

#define DATABASE_ERRMSG_NORESULT        ("Null Results pointer")
#define DATABASE_ERRMSG_BIND_STR        ("Error binding string param")
#define DATABASE_ERRMSG_BIND_INT        ("Error binding int param")
#define DATABASE_ERRMSG_BIND_INT64      ("Error binding int64 param")
#define DATABASE_ERRMSG_BIND_DBL        ("Error binding double param")
#define DATABASE_ERRMSG_BIND_BLOB       ("Error binding blob param")
#define DATABASE_ERRMSG_BIND_DATETIME   ("Error binding date/time param")
#define DATABASE_ERRMSG_BIND_NULL       ("Error binding NULL param")
#define DATABASE_ERRMSG_BIND_ZEROBLOB   ("Error binding zero blob param")
#define DATABASE_ERRMSG_BIND_CLEAR      ("Error clearing bindings")

#define DATABASE_ERRMSG_NOMETADATA      ("Meta data support not available")
#define DATABASE_ERRMSG_NOCODEC         ("Encryption support not available")
#define DATABASE_ERRMSG_NOLOADEXT       ("Loadable extension support not available")
#define DATABASE_ERRMSG_NOINCBLOB       ("Incremental BLOB support not available")
#define DATABASE_ERRMSG_NOBLOBREBIND    ("Rebind BLOB support not available")
#define DATABASE_ERRMSG_NOSAVEPOINT     ("Savepoint support not available")
#define DATABASE_ERRMSG_NOBACKUP        ("Backup/restore support not available")
#define DATABASE_ERRMSG_NOWAL           ("Write Ahead Log support not available")
#define DATABASE_ERRMSG_NOCOLLECTIONS   ("Named collection support not available")

#define DATABASE_ERRMSG_SHARED_CACHE    ("Setting SQLite shared cache mode failed")

#define DATABASE_ERRMSG_INITIALIZE      ("Initialization of SQLite failed")
#define DATABASE_ERRMSG_SHUTDOWN        ("Shutdown of SQLite failed")

#define DATABASE_ERRMSG_SOURCEDB_BUSY     ("Source database is busy")
#define DATABASE_ERRMSG_DBOPEN_FAILED     ("Database open failed")
#define DATABASE_ERRMSG_DBCLOSE_FAILED     ("Database close failed")
#define DATABASE_ERRMSG_DBASSIGN_FAILED   ("Database assignment failed")
#define DATABASE_ERRMSG_FINALIZE_FAILED   ("Finalize failed")
#define DATABASE_ERRMSG_FIELD_ERROR        ("Field Error")

#endif
