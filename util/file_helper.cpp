/************************************************************************
 * Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
 *                                                                       *
 * This software is provided as-is, without any express or implied       *
 * warranty. In no event will the authors be held liable for any         *
 * damages arising from the use of this software.                        *
 *                                                                       *
 * Permission is granted to anyone to use this software for any purpose, *
 * including commercial applications, and to alter it and redistribute   *
 * it freely, subject to the following restrictions:                     *
 *                                                                       *
 * 1. The origin of this software must not be misrepresented; you must   *
 * not claim that you wrote the original software. If you use this       *
 * software in a product, an acknowledgment in the product documentation *
 * would be appreciated but is not required.                             *
 *                                                                       *
 * 2. Altered source versions must be plainly marked as such, and must   *
 * not be misrepresented as being the original software.                 *
 *                                                                       *
 * 3. This notice may not be removed or altered from any source          *
 * distribution.                                                         *
 ************************************************************************/
#include "file_helper.h"

#include "buffercache.h"
#include "log.h"
#include "m_exceptions.h"
#include "string_helpers.h"

#include <cassert>
#include <physfs.h>

#ifndef OGTA_DEFAULT_DATA_PATH
#define OGTA_DEFAULT_DATA_PATH "gtadata.zip"
#endif
#ifndef OGTA_DEFAULT_MOD_PATH
#define OGTA_DEFAULT_MOD_PATH ""
#endif

#ifndef OGTA_DEFAULT_HOME_PATH
//#ifdef LINUX
//#define OGTA_DEFAULT_HOME_PATH PHYSFS_getUserDir()
//#elif WIN32
//#define OGTA_DEFAULT_HOME_PATH "config"
//#endif
#define OGTA_DEFAULT_HOME_PATH PHYSFS_getBaseDir()
#endif

namespace {

std::string getEnvSafe(const char *envname, std::string def_value = "")
{
    const char *value = getenv(envname);
    return (value != nullptr) ? value : def_value;
}

} // namespace

namespace Util {
namespace FileHelper {

const std::string &BaseDataPath()
{
    static std::string value = getEnvSafe("OGTA_DATA", OGTA_DEFAULT_DATA_PATH);
    return value;
}

const std::string &ModDataPath()
{
    static std::string value = getEnvSafe("OGTA_MOD", OGTA_DEFAULT_MOD_PATH);
    return value;
}

const std::string &UserHomeDir()
{
    static std::string value = getEnvSafe("OGTA_HOME", OGTA_DEFAULT_HOME_PATH);
    return value;
}

std::string Lang2MsgFilename(std::string_view l)
{
    static std::map<std::string_view, std::string> langmap {
        { "en", "ENGLISH.FXT" },
        { "de", "GERMAN.FXT" },
        { "fr", "FRENCH.FXT" },
        { "it", "ITALIAN.FXT" }
    };
    if (l.size() > 2)
        l = l.substr(0, 2);
    auto it = langmap.find(l);
    if (it != langmap.end())
        return it->second;
    WARN << "Unknown language: " << l << " - falling back to english"
         << std::endl;
    return "ENGLISH.FXT";
}

PHYSFS_file *OpenReadVFS(const std::string &file)
{
    PHYSFS_file *fd = PHYSFS_openRead(file.c_str());
    if (fd)
        return fd;
    // try lower case
    std::string name2 { string_lower(file) };
    fd = PHYSFS_openRead(name2.c_str());
    if (!fd) // still no joy, give up
        throw E_FILENOTFOUND(
            file + " with error: "
            + PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
    // take this one instead
    return fd;
}

unsigned char *BufferFromVFS(PHYSFS_file *file)
{
    assert(file != nullptr);
    unsigned int size = PHYSFS_fileLength(file);
    unsigned char *buffer = BufferCache::Instance().requestBuffer(size + 1);
    size = PHYSFS_readBytes(file, buffer, size);
    PHYSFS_close(file);
    return buffer;
}

} // namespace FileHelper
} // namespace Util
