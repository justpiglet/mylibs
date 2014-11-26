//
//  DatabaseBackupProgress.h
//

#ifndef _FHKJEF_FDS_CAN_DatabaseBackupProgress__
#define _FHKJEF_FDS_CAN_DatabaseBackupProgress__

class CSqliteBackupProgress
{
public:
    CSqliteBackupProgress() {}
    virtual ~CSqliteBackupProgress() {}
    virtual bool Progress(int totalPages, int remainingPages) { return true; }
};

#endif /* defined(__TKGame__DatabaseBackupProgress__) */
