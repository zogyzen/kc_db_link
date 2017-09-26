#pragma once

constexpr int g_ConnCount = 8;

#include "stdsrv.h"
#include "db_helper_i.h"
using namespace kc_db;

class TParms;
class TRecodeSet;
class Db_helper : public IDbHelper
{
public:
    Db_helper();

    bool Init(IParms&) override;
    IParms& CreateParm(void) override;
    int Exec(const char* sSql, IParms*) override;
    IRecodeSet& Query(const char* sSql, IParms*) override;
    void Release(IRecodeSet&) override;
    void Release(IParms&) override;

protected:
    QSqlDatabase* m_dbConn[g_ConnCount] = { nullptr };
    map<int, std::shared_ptr<TParms>> m_parms;
    map<int, std::shared_ptr<TRecodeSet>> m_rsets;
};
