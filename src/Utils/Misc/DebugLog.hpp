#ifndef SRC_UTILS_MISC_DEBUGLOG_HPP_
#define SRC_UTILS_MISC_DEBUGLOG_HPP_

#ifdef NDEBUG
    #define DEBUG_LOG(x)
#else
    #define DEBUG_LOG(x) \
        DebugLog::get() << x << std::endl;

    #include <fstream>
    #include <string>

    /**
     * Utility class for debug logging. Use like a standard output stream (cout).
     *
     * Writes all data to a file called "debug.log".
     */
    class DebugLog : public std::ofstream
    {
    public:

        /**
         * The name of the file the stream writes to.
         */
        static const std::string DEBUG_LOGFILE;

        /**
         * Creates the debug log instance and file if it does not exist yet and returns the stream.
         */
        static DebugLog & get();

    private:

        DebugLog();

        static DebugLog * instance;
    };

#endif /* NDEBUG */

#endif /* SRC_UTILS_MISC_DEBUGLOG_HPP_ */
