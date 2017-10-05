#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <functional>
#include <atomic>
using namespace std;

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
using namespace boost;
using namespace boost::interprocess;

#include "db_drv_i.h"
using namespace KC;
