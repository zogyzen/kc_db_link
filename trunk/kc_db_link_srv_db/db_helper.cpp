#include "db_helper.h"

std::atomic_int g_connID(0);
const char* g_connName[g_ConnCount] = {"aiting_1", "aiting_2", "aiting_3", "aiting_4", "aiting_5", "aiting_6", "aiting_7", "aiting_8"};

extern "C"
{
    IDbHelper& DbHelper(void)
    {
        static Db_helper db;
        return db;
    }
}

class TVal : public IVal
{
public:
    TVal(int id) : m_id(id) {}

    int GetID(void) override
    {
        return m_id;
    }

    IVal& Set(int v) override
    {
        m_val = v;
        m_type = evtInt;
        return *this;
    }
    IVal& Set(double v) override
    {
        m_val = v;
        m_type = evtFloat;
        return *this;
    }
    IVal& Set(const char* v) override
    {
        m_val = string(v);
        m_type = evtString;
        return *this;
    }
    int AsInt(void) override
    {
        if (evtInt == m_type)
            return any_cast<int>(m_val);
        else if (evtFloat == m_type)
            return any_cast<double>(m_val);
        else if (evtString == m_type)
            return lexical_cast<int>(any_cast<string>(m_val));
        return 0;
    }
    double AsFloat(void) override
    {
        if (evtInt == m_type)
            return any_cast<int>(m_val);
        else if (evtFloat == m_type)
            return any_cast<double>(m_val);
        else if (evtString == m_type)
            return lexical_cast<double>(any_cast<string>(m_val));
        return 0;
    }
    const char* AsStr(void) override
    {
        static thread_local string s_val;
        if (evtInt == m_type)
            s_val = lexical_cast<string>(any_cast<int>(m_val));
        else if (evtFloat == m_type)
            s_val = lexical_cast<string>(any_cast<double>(m_val));
        else if (evtString == m_type)
            s_val = any_cast<string>(m_val);
        return s_val.c_str();
    }
    EValType GetType(void) override
    {
        return m_type;
    }

public:
    EValType m_type;
    any m_val;

protected:
    int m_id = 0;
};

class TParms : public IParms
{
public:
    TParms(int id) : m_id(id) {}

    int GetID(void) override
    {
        return m_id;
    }

    IVal& NewVal(void) override
    {
        int iMax = 0;
        if (!m_vals.empty())
            iMax = m_vals.rbegin()->first + 1;
        TVal* val = new TVal(iMax);
        std::shared_ptr<TVal> ptr(val);
        m_vals.insert(make_pair(iMax, ptr));
        return *val;
    }
    void Add(const char* sName, IVal& v) override
    {
        auto it = m_vals.find(v.GetID());
        if (m_vals.end() == it)
            throw std::runtime_error("Not exists value");
        m_parms.insert(make_pair(string(sName), it->second));
    }
    IVal& Get(const char* sName) override
    {
        auto it = m_parms.find(sName);
        if (m_parms.end() == it)
            throw std::runtime_error("Unknown parameter");
        return *(it->second.get());
    }
    int GetCount(void) override
    {
        return (int)m_parms.size();
    }
    const char* GetParmName(int i) override
    {
        if (GetCount() <= i) return "";
        auto it = m_parms.begin();
        for (; i > 0; --i, ++it);
        return it->first.c_str();
    }

public:
    map<int, std::shared_ptr<TVal>> m_vals;
    map<string, std::shared_ptr<TVal>> m_parms;

protected:
    int m_id = 0;
};

class TRecodeSet : public IRecodeSet
{
public:
    TRecodeSet(int id) : query(QSqlDatabase::database(g_connName[g_connID++ % g_ConnCount])), m_id(id) {}

    int GetID(void) override
    {
        return m_id;
    }

    int FieldCount(void) override
    {
        return query.record().count();
    }
    const char* FieldName(int i) override
    {
        static thread_local string sFieldName;
        sFieldName = query.record().fieldName(i).toStdString();
        return sFieldName.c_str();
    }
    int FieldType(int i) override
    {
        return query.record().field(i).type();
    }
    IVal& Value(int i) override
    {
        IVal& val = NewVal();
        val.Set(query.value(i).toString().toStdString().c_str());
        return val;
    }
    IVal& Value(const char* n) override
    {
        IVal& val = NewVal();
        val.Set(query.value(n).toString().toStdString().c_str());
        return val;
    }
    bool Next(void) override
    {
        return query.next();
    }

public:
    QSqlQuery query;
    map<int, std::shared_ptr<TVal>> m_vals;

    IVal& NewVal(void)
    {
        int iMax = 0;
        if (!m_vals.empty())
            iMax = m_vals.rbegin()->first + 1;
        TVal* val = new TVal(iMax);
        std::shared_ptr<TVal> ptr(val);
        m_vals.insert(make_pair(iMax, ptr));
        return *val;
    }

protected:
    int m_id = 0;
};

Db_helper::Db_helper()
{
}

bool Db_helper::Init(IParms& parm)
{
    QDir dir;
    QCoreApplication::addLibraryPath(dir.currentPath());
    //QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    qDebug() << QSqlDatabase::drivers();
    qDebug() << QCoreApplication::libraryPaths();
    for (auto l: QCoreApplication::libraryPaths())
        cout << l.toStdString() << endl;

    bool bResult = true;
    int iConnName = 0;
    for (QSqlDatabase*& dbConn: m_dbConn)
    {
        if (nullptr == dbConn)
            dbConn = new QSqlDatabase(QSqlDatabase::addDatabase(parm.Get("DBDriver").AsStr(), g_connName[iConnName++]));
        dbConn->setHostName(parm.Get("SrvIP").AsStr());
        dbConn->setDatabaseName(parm.Get("DBName").AsStr());
        dbConn->setUserName(parm.Get("UserName").AsStr());
        dbConn->setPassword(parm.Get("Password").AsStr());
        if (!dbConn->open())
        {
            qCritical() << "(" << __FILE__ << " : " << __FUNCTION__ << " @ " << __LINE__ << ")" << endl
                        << "打开数据库失败：" << dbConn->lastError().text();
            cout << "link db failure: " << dbConn->lastError().text().toStdString() << endl;
            bResult = false;
            break;
        }
    }
    if (bResult)
    {
        qInfo() << "(" << __FILE__ << " : " << __FUNCTION__ << " @ " << __LINE__ << ")" << endl
                << "打开数据库成功！";
        cout << "link db success" << endl;
    }
    return bResult;
}

IParms& Db_helper::CreateParm(void)
{
    int iMax = 0;
    if (!m_parms.empty())
        iMax = m_parms.rbegin()->first + 1;
    TParms* parm = new TParms(iMax);
    std::shared_ptr<TParms> ptr(parm);
    m_parms.insert(make_pair(iMax, ptr));
    return *parm;
}

int Db_helper::Exec(const char* sSql, IParms* parm)
{
    QSqlQuery query(QSqlDatabase::database(g_connName[g_connID++ % g_ConnCount]));
    query.prepare(sSql);
    TParms* prms = dynamic_cast<TParms*>(parm);
    if (nullptr != prms)
        for (auto p: prms->m_parms)
            query.bindValue(p.first.c_str(), p.second->AsStr());
    if (query.exec())
        return query.numRowsAffected();
    return 0;
}

IRecodeSet& Db_helper::Query(const char* sSql, IParms* parm)
{
    int iMax = 0;
    if (!m_rsets.empty())
        iMax = m_rsets.rbegin()->first + 1;
    TRecodeSet* rset = new TRecodeSet(iMax);
    std::shared_ptr<TRecodeSet> ptr(rset);
    m_rsets.insert(make_pair(iMax, ptr));
    rset->query.prepare(sSql);
    TParms* prms = dynamic_cast<TParms*>(parm);
    if (nullptr != prms)
        for (auto p: prms->m_parms)
            rset->query.bindValue(p.first.c_str(), p.second->AsStr());
    if (!rset->query.exec())
        throw std::runtime_error("SQL error" + rset->query.lastError().text().toStdString());
    return *rset;
}

void Db_helper::Release(IRecodeSet& rs)
{
    auto it = m_rsets.find(rs.GetID());
    if (m_rsets.end() != it)
        m_rsets.erase(it);
}

void Db_helper::Release(IParms& parm)
{
    auto it = m_parms.find(parm.GetID());
    if (m_parms.end() != it)
        m_parms.erase(it);
}
