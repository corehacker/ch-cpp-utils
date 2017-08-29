
#include <sys/types.h>
#include <sys/stat.h>
#include <fts.h>
#include <string>
#include <vector>
#include <set>

using std::string;
using std::vector;
using std::set;

typedef void (*OnFile) (std::string name, std::string ext, std::string path, void *this_);

typedef struct _FtsOptions {
    vector <string> filters;
    bool bIgnoreHiddenFiles;
    bool bIgnoreHiddenDirs;
} FtsOptions;

class Fts {
public:
    Fts();
    Fts(string root);
    Fts(string root, FtsOptions *options);
    ~Fts();
    bool walk (OnFile onFile, void *this_);
private:
    string root;
    FTS *tree;
    set <string> filters;
    bool bIgnoreHiddenFiles;
    bool bIgnoreHiddenDirs;
};
