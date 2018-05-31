// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

/**
 * Server/client environment: argument handling, config file parsing, logging,
 * thread wrappers.
 */
#ifndef BITCOIN_UTIL_H
#define BITCOIN_UTIL_H

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "compat.h"
#include "sync.h"
#include "tinyformat.h"
#include "utiltime.h"

#include <atomic>
#include <cstdint>
#include <exception>
#include <map>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread/exceptions.hpp>

static const bool DEFAULT_LOGTIMEMICROS = false;
static const bool DEFAULT_LOGIPS = false;
static const bool DEFAULT_LOGTIMESTAMPS = true;

/** Signals for translation. */
class CTranslationInterface {
public:
    /** Translate a message to the native language of the user. */
    boost::signals2::signal<std::string(const char *psz)> Translate;
};

extern bool fDebug;
extern bool fPrintToConsole;
extern bool fPrintToDebugLog;

extern bool fLogTimestamps;
extern bool fLogTimeMicros;
extern bool fLogIPs;
extern std::atomic<bool> fReopenDebugLog;
extern CTranslationInterface translationInterface;

extern const char *const CORE_CONF_FILENAME;
extern const char *const CORE_PID_FILENAME;

extern std::atomic<uint32_t> logCategories;

/**
 * Translation function: Call Translate signal on UI interface, which returns a
 * boost::optional result. If no translation slot is registered, nothing is
 * returned, and simply return the input.
 */
inline std::string _(const char *psz) {
    boost::optional<std::string> rv = translationInterface.Translate(psz);
    return rv ? (*rv) : psz;
}

void SetupEnvironment();
bool SetupNetworking();

namespace BCLog {
enum LogFlags : uint32_t {
    NONE = 0,
    NET = (1 << 0),
    TOR = (1 << 1),
    MEMPOOL = (1 << 2),
    HTTP = (1 << 3),
    BENCH = (1 << 4),
    ZMQ = (1 << 5),
    DB = (1 << 6),
    RPC = (1 << 7),
    ESTIMATEFEE = (1 << 8),
    ADDRMAN = (1 << 9),
    SELECTCOINS = (1 << 10),
    REINDEX = (1 << 11),
    CMPCTBLOCK = (1 << 12),
    RAND = (1 << 13),
    PRUNE = (1 << 14),
    PROXY = (1 << 15),
    MEMPOOLREJ = (1 << 16),
    LIBEVENT = (1 << 17),
    COINDB = (1 << 18),
    QT = (1 << 19),
    LEVELDB = (1 << 20),
    ALL = ~uint32_t(0),
};
}

/** Return true if log accepts specified category */
static inline bool LogAcceptCategory(uint32_t category) {
    return (logCategories.load(std::memory_order_relaxed) & category) != 0;
}

/** Returns a string with the supported log categories */
std::string ListLogCategories();

/** Return true if str parses as a log category and set the flags in f */
bool GetLogCategory(uint32_t *f, const std::string *str);

/** Send a string to the log output */
int LogPrintStr(const std::string &str);

#define LogPrint(category, ...)                                                \
    do {                                                                       \
        if (LogAcceptCategory((category))) {                                   \
            LogPrintStr(tfm::format(__VA_ARGS__));                             \
        }                                                                      \
    } while (0)

#define LogPrintf(...)                                                         \
    do {                                                                       \
        LogPrintStr(tfm::format(__VA_ARGS__));                                 \
    } while (0)

template <typename... Args> bool error(const char *fmt, const Args &... args) {
    LogPrintStr("ERROR: " + tfm::format(fmt, args...) + "\n");
    return false;
}

void PrintExceptionContinue(const std::exception *pex, const char *pszThread);
void FileCommit(FILE *file);
bool TruncateFile(FILE *file, unsigned int length);
int RaiseFileDescriptorLimit(int nMinFD);
void AllocateFileRange(FILE *file, unsigned int offset, unsigned int length);
bool RenameOver(boost::filesystem::path src, boost::filesystem::path dest);
bool TryCreateDirectory(const boost::filesystem::path &p);
boost::filesystem::path GetDefaultDataDir();
const boost::filesystem::path &GetDataDir(bool fNetSpecific = true);
void ClearDatadirCache();
boost::filesystem::path GetConfigFile(const std::string &confPath);
#ifndef WIN32
boost::filesystem::path GetPidFile();
void CreatePidFile(const boost::filesystem::path &path, pid_t pid);
#endif
#ifdef WIN32
boost::filesystem::path GetSpecialFolderPath(int nFolder, bool fCreate = true);
#endif
void OpenDebugLog();
void ShrinkDebugFile();
void runCommand(const std::string &strCommand);

inline bool IsSwitchChar(char c) {
#ifdef WIN32
    return c == '-' || c == '/';
#else
    return c == '-';
#endif
}

class ArgsManager {
protected:
    CCriticalSection cs_args;
    std::map<std::string, std::string> mapArgs;
    std::map<std::string, std::vector<std::string>> mapMultiArgs;

public:
    void ParseParameters(int argc, const char *const argv[]);
    void ReadConfigFile(const std::string &confPath);
    std::vector<std::string> GetArgs(const std::string &strArg);

    /**
     * Return true if the given argument has been manually set.
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @return true if the argument has been set
     */
    bool IsArgSet(const std::string &strArg);

    /**
     * Return string argument or default value.
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param default (e.g. "1")
     * @return command-line argument or default value
     */
    std::string GetArg(const std::string &strArg,
                        const std::string &strDefault);

    /**
     * Return integer argument or default value.
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param default (e.g. 1)
     * @return command-line argument (0 if invalid number) or default value
     */
    int64_t GetArg(const std::string &strArg, int64_t nDefault);

    /**
     * Return boolean argument or default value.
     *
     * @param strArg Argument to get (e.g. "-foo")
     * @param default (true or false)
     * @return command-line argument or default value
     */
    bool GetBoolArg(const std::string &strArg, bool fDefault);

    /**
     * Set an argument if it doesn't already have a value.
     *
     * @param strArg Argument to set (e.g. "-foo")
     * @param strValue Value (e.g. "1")
     * @return true if argument gets set, false if it already had a value
     */
    bool SoftSetArg(const std::string &strArg, const std::string &strValue);

    /**
     * Set a boolean argument if it doesn't already have a value.
     *
     * @param strArg Argument to set (e.g. "-foo")
     * @param fValue Value (e.g. false)
     * @return true if argument gets set, false if it already had a value
     */
    bool SoftSetBoolArg(const std::string &strArg, bool fValue);

    // Forces a arg setting, used only in testing
    void ForceSetArg(const std::string &strArg, const std::string &strValue);

    // Forces a multi arg setting, used only in testing
    void ForceSetMultiArg(const std::string &strArg,
                          const std::string &strValue);

    // Remove an arg setting, used only in testing
    void ClearArg(const std::string &strArg);
};

extern ArgsManager gArgs;

// wrappers using the global ArgsManager:
static inline void ParseParameters(int argc, const char *const argv[]) {
    gArgs.ParseParameters(argc, argv);
}

static inline void ReadConfigFile(const std::string &confPath) {
    gArgs.ReadConfigFile(confPath);
}

static inline bool SoftSetArg(const std::string &strArg,
                              const std::string &strValue) {
    return gArgs.SoftSetArg(strArg, strValue);
}

static inline void ForceSetArg(const std::string &strArg,
                               const std::string &strValue) {
    gArgs.ForceSetArg(strArg, strValue);
}

static inline void ForceSetMultiArg(const std::string &strArg,
                                    const std::string &strValue) {
    gArgs.ForceSetMultiArg(strArg, strValue);
}

static inline void ClearArg(const std::string &strArg) {
    gArgs.ClearArg(strArg);
}

static inline bool IsArgSet(const std::string &strArg) {
    return gArgs.IsArgSet(strArg);
}

static inline std::string GetArg(const std::string &strArg,
                                 const std::string &strDefault) {
    return gArgs.GetArg(strArg, strDefault);
}

static inline int64_t GetArg(const std::string &strArg, int64_t nDefault) {
    return gArgs.GetArg(strArg, nDefault);
}

static inline bool GetBoolArg(const std::string &strArg, bool fDefault) {
    return gArgs.GetBoolArg(strArg, fDefault);
}

static inline bool SoftSetBoolArg(const std::string &strArg, bool fValue) {
    return gArgs.SoftSetBoolArg(strArg, fValue);
}
/**
 * Format a string to be used as group of options in help messages.
 *
 * @param message Group name (e.g. "RPC server options:")
 * @return the formatted string
 */
std::string HelpMessageGroup(const std::string &message);

/**
 * Format a string to be used as option description in help messages.
 *
 * @param option Option message (e.g. "-rpcuser=<user>")
 * @param message Option description (e.g. "Username for JSON-RPC connections")
 * @return the formatted string
 */
std::string HelpMessageOpt(const std::string &option,
                           const std::string &message);

/**
 * Return the number of physical cores available on the current system.
 * @note This does not count virtual cores, such as those provided by
 * HyperThreading when boost is newer than 1.56.
 */
int GetNumCores();

void RenameThread(const char *name);

/**
 * .. and a wrapper that just calls func once
 */
template <typename Callable> void TraceThread(const char *name, Callable func) {
    std::string s = strprintf("bitcoin-%s", name);
    RenameThread(s.c_str());
    try {
        LogPrintf("%s thread start\n", name);
        func();
        LogPrintf("%s thread exit\n", name);
    } catch (const boost::thread_interrupted &) {
        LogPrintf("%s thread interrupt\n", name);
        throw;
    } catch (const std::exception &e) {
        PrintExceptionContinue(&e, name);
        throw;
    } catch (...) {
        PrintExceptionContinue(nullptr, name);
        throw;
    }
}

std::string CopyrightHolders(const std::string &strPrefix);

#endif // BITCOIN_UTIL_H
