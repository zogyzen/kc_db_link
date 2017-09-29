#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <functional>
using namespace std;

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
using namespace boost;
using namespace boost::interprocess;

#include "framework/bundle_activator_i.h"
#include "kc_db_link/srv_main_i.h"
using namespace KC;
