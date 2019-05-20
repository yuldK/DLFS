#ifndef __DL_FS_FILE__
#define __DL_FS_FILE__
#pragma once

#include "fs_type.h"
#include <mutex>

namespace dl {
    namespace filesystem {
        
        class file {
        public:

//      private:
            string filename;
            size_t size;
            size_t pos;
            FILE* handler;
//          std::mutex cs;
        };

    }
}

#endif
