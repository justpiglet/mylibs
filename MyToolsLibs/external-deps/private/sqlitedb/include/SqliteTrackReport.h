//
//  SqliteTaskReport.h
//

#ifndef _FHKJEF_FDS_CAN_SqliteTrackReport__
#define _FHKJEF_FDS_CAN_SqliteTrackReport__

#include "SqliteRecordSet.h"
#include <map>

class CSqliteTrackReport
{
    struct TrackNode
    {
        CSqliteRecordSet * m_ptr;
        std::string m_path;
    };
    
    typedef std::map<CSqliteRecordSet*,TrackNode*> TrackNodeMap;
    
    TrackNodeMap m_TrackNodeMap;
    
    static CSqliteTrackReport * g_pSelf;
public:
    CSqliteTrackReport();
    ~CSqliteTrackReport();
    
    static CSqliteTrackReport * shared();
    
    void AddItem(CSqliteRecordSet * ptr,const char * lpszPath);
    void RemoveItem(CSqliteRecordSet * ptr);
    
    void ReportTrack();
};

inline CSqliteTrackReport * CSqliteTrackReport::shared()
{
    if(!g_pSelf)
        g_pSelf = new CSqliteTrackReport();
    
    return g_pSelf;
}

#endif /* defined(__TKGame__SqliteTrackReport__) */
