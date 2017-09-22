

#include "ch-cpp-utils/fts.hpp"



Fts::Fts() {
    root = ".";
    tree = NULL;
    bIgnoreHiddenFiles = true;
    bIgnoreHiddenDirs = true;
    bIgnoreRegularFiles = false;
    bIgnoreRegularDirs = false;
}

Fts::Fts(string root) {
   tree = NULL;
   bIgnoreHiddenFiles = true;
   bIgnoreHiddenDirs = true;
   bIgnoreRegularFiles = false;
   bIgnoreRegularDirs = false;
    this->root = ((0 == root.length()) ? "." : root);
}

Fts::Fts(string root, FtsOptions *options) {
   tree = NULL;
   bIgnoreHiddenFiles = true;
   bIgnoreHiddenDirs = true;
    this->root = ((0 == root.length()) ? "." : root);

    if (options) {
        for (uint32_t i = 0; i < options->filters.size(); i++) {
            filters.insert (options->filters.at (i));
        }
        bIgnoreHiddenFiles = options->bIgnoreHiddenFiles;
        bIgnoreHiddenDirs = options->bIgnoreHiddenDirs;
        bIgnoreRegularFiles = options->bIgnoreRegularFiles;
        bIgnoreRegularDirs = options->bIgnoreRegularDirs;
    }
}

Fts::~Fts() {

}


/*
FTS *fts_open(char * const *path_argv, int options,
                     int (*compar)(const FTSENT **, const FTSENT **));

FTSENT *fts_read(FTS *ftsp);

FTSENT *fts_children(FTS *ftsp, int options);

           typedef struct _ftsent {
               unsigned short fts_info;     // flags for FTSENT structure
               char          *fts_accpath;  // access path
               char          *fts_path;     // root path
               short          fts_pathlen;  // strlen(fts_path)
               char          *fts_name;     // filename
               short          fts_namelen;  // strlen(fts_name)
               short          fts_level;    // depth (-1 to N)
               int            fts_errno;    // file errno
               long           fts_number;   // local numeric value
               void          *fts_pointer;  // local address value
               struct ftsent *fts_parent;   // parent directory
               struct ftsent *fts_link;     // next file structure
               struct ftsent *fts_cycle;    // cycle structure
               struct stat   *fts_statp;    // stat(2) information
           } FTSENT;

*/
bool Fts::walk (OnFile onFile, void *this_) {
    char * path [2] {(char *) root.data(), nullptr};
    tree = fts_open(path, FTS_NOCHDIR, 0);
    if (!tree) return false;

    FTSENT *node = nullptr;

    while ((node = fts_read(tree))) {
        if ((node->fts_info & FTS_F) && !bIgnoreRegularFiles) {
            string name = node->fts_name;
            int32_t pos = name.find_last_of('.');
            if (0 == pos) {
                if (!bIgnoreHiddenFiles) {
                    string ext = name.substr (pos + 1);
                    if (filters.empty ()) {
                        onFile (node->fts_name, ext, node->fts_path, this_);
                    } else {
                        if (filters.count (ext) > 0) {
                            onFile (node->fts_name, ext, node->fts_path, this_);
                        }
                    }
                }
            } else {
                string ext = name.substr (pos + 1);
                if (filters.empty ()) {
                    onFile (node->fts_name, ext, node->fts_path, this_);
                } else {
                    if (filters.count (ext) > 0) {
                        onFile (node->fts_name, ext, node->fts_path, this_);
                    }
                }
            }
        }
        if ((node->fts_info & FTS_D) && !bIgnoreRegularDirs) {
           string name = node->fts_name;
           int32_t pos = name.find_last_of('.');
//           printf ("Directory found: %s %s %lu\n", node->fts_name, node->fts_path, name.length());
           if (0 == pos) {
               if (!bIgnoreHiddenDirs) {
                  if (1 == name.length() && 0 == name.compare(".")) {

                  } else {
                     string ext = name.substr (pos + 1);
                     onFile (node->fts_name, ext, node->fts_path, this_);
                  }
               }
           } else {
              string ext = name.substr (pos + 1);
             onFile (node->fts_name, ext, node->fts_path, this_);
           }
        }
    }

    if (errno) {
        return false;
    }

    return (fts_close(tree) ? false : true);
}

bool Fts::walk (string root, OnFile onFile, void *this_) {
   this->root = root;
   return this->walk(onFile, this_);
}

