// WORK IN PROGRESS!
//
// class_wrapper_template.cpp
// Example of using a C++ class via a MEX-file
// by Jonathan Chappelow (chappjc)
//
// Design goals:
//   1. Manage multiple persistent instances of a C++ class
//   2. Small consecutive integer handles used in MATLAB (not cast pointers)
//   3. Transparently handle resource management (i.e. MATLAB never 
//      responsible for memory allocated for C++ classes)
//       a. No memory leaked if MATLAB fails to issue "delete" action
//       b. Automatic deallocation if MEX-file prematurely unloaded
//   4. Guard against premature module unloading
//   5. Validity of handles implicitly verified without checking a magic number
//   6. No wrapper class or functions mimicking mexFunction, just an intuitive
//      switch-case block in mexFunction.
//
// Note that these goals should be acheved without regard to any MATLAB class, 
// but which can also help address memory management issues.  As such, the
// resulting MEX-file can safely be used directly (but not too elegantly).
//
// Use:
//   1. Enumerate the different actions (e.g. New, Delete, Insert, etc.) in the
//      Actions enum.  For each enumerated action, specify a string (e.g.
//      "new", "delete", "insert", etc.) to be passed as the first argument to
//      the MEX function in MATLAB.
//   2. Customize the handling for each action in the switch statement in the
//      body of mexFunction (e.g. call the relevant C++ class method).
//  
// Implementation:
//
// For your C++ class, class_type, mexFunction uses static data storage to hold
// a persistent (between calls to mexFunction) table of integer handles and 
// smart pointers to dynamically allocated class instances.  A std::map is used
// for this purpose, which facilitates locating known handles, for which only 
// valid instances of your class are guaranteed to exist:
//
//    typedef unsigned int handle_type;
//    std::map<handle_type, std::shared_ptr<class_type>>
//
// A std::shared_ptr takes care of deallocation when either (1) a table element
// is erased via the "delete" action or (2) the MEX-file is unloaded.
//
// To prevent the MEX-file from unloading while a MATLAB class instances exist,
// mexLock is called each time a new C++ class instance is created, adding to
// the MEX-file's lock count.  Each time a C++ instance is deleted mexUnlock is
// called, removing one lock from the lock count.
//
// Requirements:
//
// A modern compiler with the following C++11 features:
//   - shared_ptr
//   - auto
//   - enum class
//   - initializer_list (for const map initialization)
// (VS2013, recent GCC possibly with -std=c++11, Clang since 3.1)
//
// TODO:
//
// - This example uses a priority queue class of mine, which is far from the
//   simplest example.  Demonstrate with something more basic.
// - Somehow put the relevant parts in a header, OR the other way around -- put
//   user class config in a header and include into the mexFunction's cpp.
//
// 04/25/15 (chappjc) - Initial version.

#include "mex.h"

#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <string>
#include <sstream>

////////////////////////  BEGIN Step 1: Configuration  ////////////////////////

// Include your class declaration
#include "pqheap.hpp"

// Define class_type for your class
typedef double data_type;
typedef PQheap<data_type> class_type;
// typedef MyClass class_type; // basic case

// List actions
enum class Action
{
    // create/destroy instance - REQUIRED
    New,
    Delete,
    // user-specified class functionality
    Init,
    Insert,
    ExtractTop,
    Size,
    Capacity,
    Print
};

// Map string (first input argument to mexFunction) to an Action
const std::map<std::string, Action> actionTypeMap =
{
    { "new",        Action::New },
    { "delete",     Action::Delete },
    { "init",       Action::Init },
    { "insert",     Action::Insert },
    { "extract",    Action::ExtractTop },
    { "size",       Action::Size },
    { "capacity",   Action::Capacity },
    { "print",      Action::Print }
}; // if no initializer list available, put declaration and inserts into mexFunction

/////////////////////////  END Step 1: Configuration  /////////////////////////

typedef unsigned int handle_type;
typedef std::pair<handle_type, std::shared_ptr<class_type>> indPtrPair_type; // or boost::shared_ptr
typedef std::map<indPtrPair_type::first_type, indPtrPair_type::second_type> instanceMap_type;
typedef indPtrPair_type::second_type instPtr_t;

// getHandle pulls the integer handle out of prhs[1]
handle_type getHandle(int nrhs, const mxArray *prhs[]);
// checkHandle gets the position in the instance table
instanceMap_type::const_iterator checkHandle(const instanceMap_type&, handle_type);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    // static storage duration object for table mapping handles to instances
    static instanceMap_type instanceTab;

    if (nrhs < 1 || !mxIsChar(prhs[0]))
        mexErrMsgTxt("First input must be an action string ('new', 'delete', or a method name).");

    char *actionCstr = mxArrayToString(prhs[0]); // convert char16_t to char
    std::string actionStr(actionCstr); mxFree(actionCstr);

    for (auto & c : actionStr) c = ::tolower(c); // remove this for case sensitivity

    if (actionTypeMap.count(actionStr) == 0)
        mexErrMsgTxt(("Unrecognized action (not in actionTypeMap): " + actionStr).c_str());

    // If action is not "new" or "delete" try to locate an existing instance based on input handle
    instPtr_t instance;
    if (actionTypeMap.at(actionStr) != Action::New && actionTypeMap.at(actionStr) != Action::Delete) {
        handle_type h = getHandle(nrhs, prhs);
        instanceMap_type::const_iterator instIt = checkHandle(instanceTab, h);
        instance = instIt->second;
    }

	//////// Step 2: customize the each action in the switch in mexFuction ////////
    switch (actionTypeMap.at(actionStr))
    {
    case Action::New:
    {
        if (nrhs > 1 && mxGetNumberOfElements(prhs[1]) != 1)
            mexErrMsgTxt("Second argument (optional) must be a scalar, N.");

        handle_type newHandle = instanceTab.size() ? (instanceTab.rbegin())->first + 1 : 1;

        std::pair<instanceMap_type::iterator, bool> insResult;
        if (nrhs > 1) {
            unsigned int N = static_cast<unsigned int>(mxGetScalar(prhs[1]));
            insResult = instanceTab.insert(indPtrPair_type(newHandle, std::make_shared<class_type>(N)));
        }
        else
            insResult = instanceTab.insert(indPtrPair_type(newHandle, std::make_shared<class_type>())); // default constructor

        if (!insResult.second) // sanity check
            mexPrintf("Oh, bad news.  Tried to add an existing handle."); // shouldn't ever happen
        else
            mexLock(); // add to the lock count

		// return the handle
        plhs[0] = mxCreateDoubleScalar(insResult.first->first); // == newHandle

        break;
    }
    case Action::Delete:
    {
        instanceMap_type::const_iterator instIt = checkHandle(instanceTab, getHandle(nrhs, prhs));
        instanceTab.erase(instIt);
        mexUnlock();
        plhs[0] = mxCreateLogicalScalar(instanceTab.empty()); // info
        break;
    }
    case Action::Init:
        if (nrhs < 3 || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("Max heap size must be a scalar.");

        plhs[0] = mxCreateDoubleScalar(
            instance->init(static_cast<unsigned int>(mxGetScalar(prhs[2]))));

        break;
		
    case Action::Insert:
    {
        if (nrhs < 3 || mxGetNumberOfElements(prhs[2]) != 1)
            mexErrMsgTxt("Inserted element must be a scalar.");

        int heapPos = instance->insert(static_cast<data_type>(mxGetScalar(prhs[2])));
        plhs[0] = mxCreateDoubleScalar(heapPos);

        break;
    }
    case Action::Capacity:
        plhs[0] = mxCreateDoubleScalar(instance->capacity());
        break;
		
    case Action::Size:
        plhs[0] = mxCreateDoubleScalar(instance->size());
        break;
		
    case Action::ExtractTop:
        plhs[0] = mxCreateDoubleScalar( instance->size() > 0 ?
            instance->extractTop() : mxGetNaN() );
        break;
		
    case Action::Print:
    {
        data_type const *data = instance->data();
        const int len = instance->size();
        for (int j = 1; j <= len; ++j)
            mexPrintf("| %g ", data[j]);
        mexPrintf("|\n");
		break;
    }
    default:
        mexErrMsgTxt(("Unhandled action: " + actionStr).c_str());
        break;
    }
    ////////////////////////////////  DONE!  ////////////////////////////////
}

handle_type getHandle(int nrhs, const mxArray *prhs[])
{
    if (nrhs < 2 || mxGetNumberOfElements(prhs[1]) != 1) // mxIsScalar in R2015a+
        mexErrMsgTxt("Specify an instance with an integer handle.");
    return static_cast<handle_type>(mxGetScalar(prhs[1]));
}

instanceMap_type::const_iterator checkHandle(const instanceMap_type& m, handle_type h)
{
    auto it = m.find(h);

    if (it == m.end()) {
        std::stringstream ss; ss << "No instance corresponding to handle " << h << " found.";
        mexErrMsgTxt(ss.str().c_str());
    }

    return it;
}
