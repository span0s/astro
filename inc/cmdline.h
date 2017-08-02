// -*-c++-*-
#pragma once

#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <set>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <terminal.h>

// utility for parsing command line options, as well as printing informative help messages
// with user feedback.
//
// cmdline tries to handle as much of the standard boilerplate for program option parsing as possible
//     * it automatically handles the --help/-h options and prints program usage.
//     * it checks for proper number of position arguments being passed to program.
//     * it warns when an unknown option is given on the command line
//     * it warns if an option is set twice on the command line
//     * it throws an error if an option requires a value and none is given
//     * any options that are specified, but never queried generate a warning on program exit
//
// general usage:
//    cmdline args(argc, argv, spec);
//
// builds a cmdline instance called args.  The string variable spec contains a specification
// for the commandline of the program.  By default, lines are treated as raw text, and are
// replicated in the help message as-is.
//
//  Lines beggining with '_' are parsed as header names.
//
//  Lines beginning with '<' are parsed as argument specifications.  An argument name can have
// any character except '+' or '>' in it.  The format is:
//
//        <name> - description.
//
// if '+' is the last character of the name, this argument is interpreted as a vector argument
// that can take an array of values from the command line.  These are interpreted as everything
// that's "left" over after parsing other arguments.  As such, only one is allowed per program.
//
// The description starts with a '-' and extends to the end of the line.
//
//  Lines begining with a '-' are parsed as option specifications.  An option name can have
// alphanumeric characters and '-'.  The format is:
//
//        --option[/-o[=]] - description
// or:     -o[=]           - description
//
// Options can be a short name only, or a long name with an optional short name separated by '/'
// when suffixed by an '=', the option takes a value.
//
//
// Example usage:
//
//     cmdline args(argc, argv,
//         "This is an example program showing how to use cmdline.h.  It's just a theoretical program\n"
//         "that multiplexes multiple files into one file.\n"
//         "The program spec is specified as a string containing information about arguments and options\n\n"
//
//         "_Parameters\n"
//         "  <output>  - output filename to write\n"
//         "  <inputs+> - input filenames to merge\n\n"
//
//         "_Options\n"
//         "  --progress/-p - print progress to stderr\n"
//         "  --limit/-l=   - limit total number of lines in output\n"
//     );
//
//

namespace command_line {
    using std::set;
    using std::string;
    using std::vector;

    /*******************************************************************************
     * utility functions
     *******************************************************************************/

    // split string into tokens
    static vector<string> split(const char* str, char delimiter) {
        vector<string> ans;
        while (str && *str) {
            const char* beg = str;

            // seek until we hit a delimiter
            while (*str && *str != delimiter) {
                str++;
            }

            ans.push_back(string(beg, (size_t)(str-beg)));
            if (*str) str++; // consume delimiter
        }
        return ans;
    }


    // trim whitespace from boths ends of a string
    static string trim(string str) {
        size_t beg=0, len=str.size(), end=len-1;

        while (beg < end && isspace(str[beg])) beg++;
        while (end > beg && isspace(str[end])) end--;

        return str.substr(beg, end-beg+1);
    }


    /*******************************************************************************
     * types to hold arguments, options, and text lines in a single struct
     *******************************************************************************/

    // argument type, holds information about position arguments
    struct argument {
        string name;           // argument name
        string desc;           // argument description
        vector<string> values; // value(s) the argument holds
        size_t desc_col;       // column that description starts on
        bool   multival;       // indicates argument has multiple entries

        // print argument with standard format
        void print(FILE* fd=stdout) {
            size_t col=0;

            fputs("  <", fd);        col += 3;
            fputs(name.c_str(), fd); col += name.size();

            if (multival) {
                term_mode(TERM_FG_RED, TERM_BOLD, fd);
                fputc('+', fd);      col += 1;
                term_reset(fd);
            }

            fputs(">", fd);          col += 1;

            if (!desc.empty()) {
                while (col < desc_col-1) {
                    fputc(' ', fd);
                    col++;
                }
                fputs("- ", fd);
                fputs(desc.c_str(), fd);
            }
            fputc('\n', fd);
        }
    };

    
    // option type, holds information about switched arguments
    struct option {
        string long_name;   // long option name (eg: --verbose)
        string short_name;  // short option name (eg: -v)
        string desc;        // description
        string value;       // raw text value of option
        size_t desc_col;    // column that description starts on
        bool   takes_value; // true if option takes a value (ie '=' is set)
        bool   set;         // is the value set?
        bool   viewed;      // has the option been viewed?
        
        // return true if option has a long name or short name
        bool has_long_name()  { return long_name.size()  > 0; }
        bool has_short_name() { return short_name.size() > 0; }

        // return canonical long/short name if it exist
        string long_form()    { return has_long_name()  ? "--" + long_name  : ""; }
        string short_form()   { return has_short_name() ?  "-" + short_name : ""; }

        // return combo name, including long and short name if possible
        string full_name() {
            if (has_long_name()) {
                if (has_short_name()) {
                    return long_form() + "/" + short_form();
                }
                return long_form();
            }
            return short_form();
        }

        // print the option in standard format
        void print(FILE *fd=stdout) {
            size_t col=0;

            if (long_name.size() > 0) {
                fputs("  --", fd);             col += 4;

                term_mode(TERM_FG_CYAN, fd);
                fputs(long_name.c_str(), fd);  col += long_name.size();
                term_reset(fd);

                if (short_name.length() != 0) {
                    fputs("/-", fd);           col += 2;
                    term_mode(TERM_FG_GREEN, fd);
                    fputc(short_name[0], fd);  col += 1;
                    term_reset(fd);
                }
            } else {
                fputs("   -", fd);             col += 4;
                term_mode(TERM_FG_GREEN, fd);
                fputc(short_name[0], fd);      col += 1;
                term_reset(fd);
            }

            if (takes_value) {
                term_mode(TERM_FG_RED, TERM_BOLD, fd);
                fputc('=', fd);                col += 1;
                term_reset(fd);
            }

            while (col < desc_col-1) {
                fputc(' ', fd);
                col++;
            }

            fprintf(fd, "- %s\n", desc.c_str());
        }
    };

    
    // specline, just a tagged union type
    struct specline {
        typedef enum {
            ARGUMENT,
            OPTION,
            HEADER,
            TEXT,
            NONE
        } type_t;

        specline()                       : type(NONE)               {}
        specline(argument arg)           : type(ARGUMENT), arg(arg) {}
        specline(option   opt)           : type(OPTION),   opt(opt) {}
        specline(string   txt)           : type(TEXT),     txt(txt) {}
        specline(string   txt, type_t t) : type(t),        txt(txt) {}

        void print(FILE* fd=stdout) {
            switch (type) {
                case ARGUMENT: arg.print(fd);                    break;
                case OPTION:   opt.print(fd);                    break;
                case TEXT:     fprintf(fd, "%s\n", txt.c_str()); break;

                case HEADER: {
                    term_mode(TERM_UNDERLINE, TERM_BOLD, fd);
                    fputs(txt.c_str(), fd);
                    term_reset(fd);
                    fputc('\n', fd);
                    break;
                }

                case NONE: {
                    fprintf(stderr, "Saw a NONE type when printing specline, should never happen\n");
                    exit(EXIT_FAILURE);
                    break;
                }
            }
        }

        // only one of these is valid according to type above
        type_t   type;
        argument arg;
        option   opt;
        string   txt;
    };


    /*******************************************************************************
     * tokenizer to be used with the parser
     *******************************************************************************/

    // used to catch parsing errors without having to propagate error codes
    struct parse_error : std::exception {
         parse_error(ssize_t line, ssize_t col, const char* msg)
             : line(line), col(col), msg(msg) {}

        const char* what() const throw() { return msg.c_str(); }

        ssize_t line;
        ssize_t col;
    private:
        string msg;
    };

    
    // tokenizer provides a nice way to dice up a character stream to match our grammar
    struct tokenizer {
        tokenizer()
            : str_(NULL), line_(0), col_(0) { errmsg_[0] = 0; }

        tokenizer(const char* str)
            : str_(str), line_(0), col_(0) {

            errmsg_[0] = 0;
        }

        // return true if we're at the end of our buffer
        bool done() {
            return (str_ == 0) || (*str_ == 0);
        }

        // return true if next token is the one given, does not consume token
        bool next_is(const char* token) {
            return strncmp(skipws(str_), token, strlen(token)) == 0;
        }

        // allow a particular token at the current position in the stream
        bool allow(const char* token, bool allow_space=true) {
            if (allow_space) skipws();

            size_t toklen = strlen(token);
            if (str_ && strncmp(str_, token, toklen) == 0) {
                munch(toklen);
                return true;
            }
            return false;
        }

        // expect a particular token at the current position in the stream
        void expect(const char* token, const char* msg=NULL, bool allow_space=true) {
            if (!allow(token, allow_space)) {
                if (msg) {
                    snprintf(errmsg_, sizeof(errmsg_), "error: expected '%s' here, %s", token, msg);
                } else {
                    snprintf(errmsg_, sizeof(errmsg_), "error: expected '%s' here", token);
                }
                throw parse_error(line_, col_, errmsg_);
            }
        }

        // expect a character that satisfies a predicate
        string expect_char(int (*pred)(int c), const char* msg) {
            if (str_ && *str_ && pred(*str_)) {
                char ans = *str_;
                munch();
                return string(1, ans);
            }
            snprintf(errmsg_, sizeof(errmsg_), "error: %s", msg);
            throw parse_error(line_, col_, errmsg_);
        }

        // skip whitespace in str_
        void skipws() {
            while (str_ && isspace(*str_) && (*str_ != '\n')) {
                munch();
            }                
        }

        // grab text to end of line, doesn't include newline
        string grab_to_eol() {
            const char* beg = str_;
            while (str_ && *str_ && *str_ != '\n') {
                munch();
            }

            const char* end = str_;
            while (end > beg+1 && isspace(*end) && isspace(*(end-1))) end--;
            string ans(beg, end-beg);

            if (*str_ == '\n') {
                munch(); // consume newline
            }
            return ans;
        }

        // grab text until any of the given characters is seen. trailing whitespace is trimmed before returning
        // if predicate evaluates to false before hitting one of the given tokens, throw a parse error.
        string grab_to_char(const char* tokens, int (*pred)(int c)=NULL) {
            const char* beg = str_;
            while (str_ && *str_ && (strchr(tokens, *str_) == NULL) && (*str_ != '\n')) {
                if (pred && !pred(*str_)) {
                    snprintf(errmsg_, sizeof(errmsg_), "error: unexpected character '%c'", *str_);
                    throw parse_error(line_, col_, errmsg_);
                }
                munch();
            }

            // trim whitespace
            const char* end = str_;
            while (end > beg+1 && isspace(*end) && isspace(*(end-1))) end--;
            string ans(beg, end-beg);

            return ans;
        }

        ssize_t line()   { return line_; }
        ssize_t column() { return col_;  }
    private:
        const char*  str_;    // string we're tokenizing
        ssize_t      line_;   // current line in the text
        ssize_t      col_;    // current column in the text
        char         errmsg_[1024];

        tokenizer(const tokenizer&) {} // disallow copying

        // consume a character from the stream, handle line and column count when hitting a newline
        void munch(int count=1) {
            for (ssize_t ii=0; ii < count; ii++) {
                if (str_ && *str_) {
                    if (*str_ == '\n') {
                        line_++;
                        col_ = 0;
                    }
                    str_++;
                    col_++;
                }
            }
        }

        // skip whitespace in ptr
        const char* skipws(const char* ptr) {
            while (ptr && *ptr && isspace(*ptr)) ptr++;
            return ptr;
        }
    };


    /*******************************************************************************
     * parser to parse the command line spec
     *******************************************************************************/
    struct cmdline_spec {
        // build a cmdline_spec, takes a string and parses it into a spec.  Prints parsing
        // error and std::terminate if there's an error.
        cmdline_spec(const char* str)
            : lines_(split(str, '\n')), multiarg_seen_(false) {

            // tokenizer for the stream
            tokenizer stream(str);

            try {
                while (!stream.done()) {
                         if (stream.next_is("_")) parse_header(stream);   // parse section header
                    else if (stream.next_is("<")) parse_argument(stream); // parse argument spec
                    else if (stream.next_is("-")) parse_option(stream);   // parse option spec
                    else                          parse_text(stream);     // parse plain text line

                    // manually consume standalone newlines as empty text lines.  This is to prevent
                    // the parser from eating them as whitespace and to preserve user spacing.
                    while (stream.allow("\n", false)) {
                        spec_.push_back(specline(""));
                    }
                }
            } catch (parse_error err) {
                print_error_and_exit(err.line, err.col, err.what());
            }
        }

        // print the spec to a file descriptor
        void print(const char* progname, FILE* fd=stdout) {
            fprintf(fd, "Usage: %s [--help/-h]", progname);

            // print usage string
            vector<specline>::iterator iter;
            for (iter = spec_.begin(); iter != spec_.end(); iter++) {
                if (iter->type == specline::ARGUMENT) {
                    fputs(" <", fd);
                    term_mode(TERM_UNDERLINE, fd);
                    fputs(iter->arg.name.c_str(), fd);
                    term_reset(fd);
                    fputs(">", fd);
                }
            }
            fputs("\n\n", fd);

            // print spec
            for (iter = spec_.begin(); iter != spec_.end(); iter++) {
                iter->print(fd);
            }
        }

        // return a vector of the speclines corresponding to arguments
        vector<argument> arguments() {
            vector<argument> ans;

            vector<specline>::iterator iter;
            for (iter = spec_.begin(); iter != spec_.end(); iter++) {
                if (iter->type == specline::ARGUMENT) {
                    ans.push_back(iter->arg);
                }
            }
            return ans;
        }

        // return a vector of the speclines corresponding to arguments
        vector<option> options() {
            vector<option> ans;

            vector<specline>::iterator iter;
            for (iter = spec_.begin(); iter != spec_.end(); iter++) {
                if (iter->type == specline::OPTION) {
                    ans.push_back(iter->opt);
                }
            }
            return ans;
        }

    private:
        set<string>      long_names_;
        set<string>      short_names_;
        vector<string>   lines_;         // raw input split into lines (only used for printing parse error)
        vector<specline> spec_;          // all spec lines, including raw text
        char             errmsg_[1024];  // buffer for error messages
        bool             multiarg_seen_; // indicates whether a multiarg has been seen already
        
        // predicate to allow alphanumeric or dash in string
        static int isalnumdash(int c) {
            return isalnum(c) || c == '-';
        }

        // parse a textline, just grab the whole thing
        void parse_text(tokenizer &stream) {
            spec_.push_back(
                specline(stream.grab_to_eol())
            );
        }

        // parse a header, simple grabs text and marks it with type
        void parse_header(tokenizer &stream) {
            stream.expect("_");
            spec_.push_back(
                specline(stream.grab_to_eol(), specline::HEADER)
            );
        }

        // parse an argument if possible, add to specline list when done
        void parse_argument(tokenizer &stream) {
            argument arg = argument();

            // skip whitespace and save the start point of the line
            stream.skipws();
            int start_line = stream.line();
            int start_col  = stream.column();

            // grab argument name, any character except "+" and ">" allowed
            stream.expect("<");
            stream.skipws();
            arg.name = trim(stream.grab_to_char("+>"));

            // if + is specified, argument is a multi-arg, have to check that
            // it's the only one so far, else it's an error.
            if (stream.allow("+")) {
                if (multiarg_seen_) {
                    snprintf(errmsg_, sizeof(errmsg_), "error: only one vector argument allowed");
                    throw parse_error(start_line, start_col, errmsg_);
                }

                multiarg_seen_ = true;
                arg.multival   = true;
            }
            stream.expect(">");

            stream.expect("-", "argument description required");
            arg.desc_col = stream.column() - 1;
            stream.skipws();
            arg.desc = stream.grab_to_eol();

            // save to spec
            spec_.push_back(specline(arg));
        }

        // parse an option if possible, add to specline list when done
        void parse_option(tokenizer &stream) {
            option opt = option();

            // skip whitespace and save the start point of the line
            stream.skipws();
            int start_line = stream.line();
            int start_col  = stream.column();

            // handle long-option name first
            if (stream.allow("--")) {
                opt.long_name = stream.grab_to_char("/= ", isalnumdash);
                if (stream.allow("/", false)) {
                    stream.expect("-", "short name required after /", false);
                    opt.short_name = stream.expect_char(isalnum, "expected short option character");
                }
            } else {
                // otherwise expect a short option
                stream.expect("-");
                opt.short_name = stream.expect_char(isalnum, "expected short option name");
            }

            // check for duplicate option definition
            string used_name = "";
            if (short_names_.count(opt.short_form()) > 0) used_name = opt.short_form();
            if (long_names_.count (opt.long_form())  > 0) used_name = opt.long_form();
            
            if (used_name != "") {
                snprintf(errmsg_, sizeof(errmsg_), "error: duplicate option definition (%s was used)", used_name.c_str());
                throw parse_error(start_line, start_col, errmsg_);
            }
            
            opt.takes_value = stream.allow("=", false);

            stream.expect("-", "option description required");
            opt.desc_col = stream.column() - 1; // save column of '-' for printing
            stream.skipws();
            opt.desc     = stream.grab_to_eol();

            // if option is --help or -h, throw an error, as we handle it
            if (opt.long_name == "help" || opt.short_name == "h") {
                snprintf(errmsg_, sizeof(errmsg_), "error: reserved option name");
                throw parse_error(start_line, start_col, errmsg_);
            }

            // save to spec
            spec_.push_back(specline(opt));
            long_names_.insert (opt.long_form());
            short_names_.insert(opt.short_form());
        }

        // print equivalent spacing from a string.  This seeks through to the given
        // position in the string and prints any tabs, etc to align things properly
        void print_equivalent_space(const string& line, ssize_t idx, FILE *fd=stderr) {
            for (ssize_t ii=0; ii < idx; ii++) {
                if (isspace(line[ii])) {
                    putc(line[ii], fd);
                } else {
                    putc(' ',      fd);
                }
            }
        }

        // print spec parsing error to stderr and call exit
        void print_error_and_exit(size_t errline, size_t errcol, const char* msg) {
            term_mode(TERM_FG_WHITE, TERM_BG_RED, stderr);
            fprintf(stderr, "Error parsing command line spec\n\n");
            term_reset(stderr);

            for (size_t ii=0; ii < lines_.size(); ii++) {
                fprintf(stderr, "%s\n", lines_[ii].c_str());

                // print error after noted line, highlight carat to show where it is
                if (ii == errline) {
                    // space over to where error was
                    print_equivalent_space(lines_[ii], errcol-1);

                    // draw highlighted carat
                    term_mode(TERM_FG_WHITE, TERM_BG_RED, TERM_BOLD, stderr);
                    fputs("^", stderr);
                    term_reset(stderr);

                    // print error message
                    term_mode(TERM_FG_RED, TERM_BOLD, stderr);
                    fprintf(stderr, " %s\n", msg);
                    term_reset(stderr);
                }
            }
            exit(EXIT_FAILURE);
        }
    };


    /*******************************************************************************
     * main cmdline functionality
     *******************************************************************************/
    struct cmdline {
        cmdline(int argc, const char* argv[], const char* spec_str)
            : cmdspec_(spec_str), progname_(argv[0]), nargs_(0) {

            // grab argument/option specs from parser
            optspec_ = cmdspec_.options();
            argspec_ = cmdspec_.arguments();

            // vector to hold arguments as we move through command line
            vector<string> args;

            // iterate over command line arguments,
            ssize_t ii;
            for (ii=1; ii < argc; ii++) {
                // "--" by itself stops parsing
                if (strcmp(argv[ii], "--") == 0) {
                    ii++;
                    break;
                }

                // automatically print help if requested
                if (strcmp(argv[ii], "--help") == 0 || strcmp(argv[ii], "-h") == 0) {
                    print_help();
                    exit(EXIT_SUCCESS);
                }

                // parse multi-option (eg: -xvzf)
                if ((strncmp(argv[ii], "-",  1) == 0) &&
                    (strncmp(argv[ii], "--", 2) != 0) &&
                    (strlen (argv[ii])           > 2) &&
                    (strchr (argv[ii], '=')     == NULL)) {

                    // parse each character as a separate option
                    const char* arg = argv[ii]+1;
                    ssize_t  arglen = strlen(arg);

                    for (ssize_t ii=0; ii < arglen; ii++) {
                        option* opt = find_option("-" + string(arg+ii, 1));
                        if (!opt) { 
                            warning("unknown option '-%c' given", *(arg+ii));
                            continue;
                        }

                        if (opt->takes_value) {
                            error("'-%c' takes a value, cannot be part of a compound option", *(arg+ii));
                            exit(EXIT_FAILURE);
                        }
                        
                        opt->set = true;
                    }

                    continue;
                }

                // parse as regular option (eg: --progress or -p)
                if ((strchr(argv[ii], '-') == argv[ii])) {
                    tokenizer stream(argv[ii]);

                    string opt_name = "";
                    string opt_val  = "";

                    // grab long/short option names
                    stream.expect("-");
                    if (stream.allow("-")) opt_name = "--" + stream.grab_to_char("=");
                    else                   opt_name = "-"  + stream.expect_char(isalnum, "expected short option character");

                    // look up option in the command spec
                    option* opt = find_option(opt_name);
                    if (opt) {
                        // option is one of the ones we know how to deal with, so now handle when it's of the
                        // form --option=value, or -o=value, handle grabbing the value here
                        if (opt->takes_value) {
                            if (stream.allow("=")) {
                                opt_val = stream.grab_to_eol();
                            } else if (ii < argc-1) {
                                opt_val = argv[ii+1];
                                ii++;
                            } else {
                                error("option '%s' requires a value, but none given", opt_name.c_str());
                                exit(EXIT_FAILURE);
                            }

                            // check that it hasn't been set before, issue warning if it has
                            if (opt->set) {
                                char other_str[256];
                                if (opt_name == opt->short_form()) {
                                    snprintf(other_str, sizeof(other_str), "(possibly via %s)", opt->long_form().c_str());
                                } else {
                                    snprintf(other_str, sizeof(other_str), "(possibly via %s)", opt->short_form().c_str());
                                }

                                warning("setting value for '%s', which was previously set on command line %s", opt_name.c_str(), other_str);
                            }
                        }

                        // finally we can set the value
                        opt->set   = true;
                        opt->value = opt_val;
                    } else {
                        warning("unknown option '%s' given", opt_name.c_str());
                    }

                    continue;
                }

                // if we get here, then parameter is just an argument
                args.push_back(string(argv[ii]));
                
                // swap argument to head of argv
                nargs_++;
                std::swap(*(argv+nargs_), *(argv+ii));
            }

            // finish parsing the rest as arguments in case we got a "--" that terminated loop
            for (; ii < argc; ii++) {
                args.push_back(string(argv[ii]));
                nargs_++;
                std::swap(*(argv+nargs_), *(argv+ii));
            }

            if (args.size() < argspec_.size()) {
                error("too few arguments, expected %i, saw %i", (int)argspec_.size(), (int)args.size());
                printf("\n");
                print_help(stderr);
                exit(EXIT_FAILURE);
            }

            // assign argument values where they belong.
            // figure out how many normal (non-multiarg) arguments are at the front and end of the spec
            // list.  Assign arguments to those, everything else goes into the multiarg (if any)
            ssize_t fidx = 0; // front argument index
            for (fidx=0; fidx < (ssize_t)argspec_.size(); fidx++) {
                if (argspec_[fidx].multival) {
                    break;
                }
                argspec_[fidx].values.push_back(args[fidx]);
            }

            ssize_t ridx    = 0; // rear argument index
            ssize_t specidx = argspec_.size()-1;
            for (ridx=(ssize_t)args.size()-1; ridx > 0; ridx--, specidx--) {
                if (argspec_[specidx].multival) {
                    break;
                }
                argspec_[specidx].values.push_back(args[ridx]);
            }

            // save arguments to multiarg
            for (ssize_t ii=fidx; ii <= ridx; ii++) {
                argspec_[fidx].values.push_back(args[ii]);
            }
        }

        // destructor, we'll just print some cleanup error messages if there are options we never queried during
        // the program run
        ~cmdline() {
            bool first=true;
            for (size_t ii=0; ii < optspec_.size(); ii++) {
                if (!optspec_[ii].viewed) {
                    if (first) {
                        first = false;
                        fputc('\n', stderr);
                    }
                    warning("option '%s' was specified, but never queried in program", optspec_[ii].full_name().c_str());
                }
            }
        }
        
        // print the standard help banner.  This just program usage line and command spec
        void print_help(FILE *fd=stdout) {
            cmdspec_.print(progname_, fd);
        }

        // return number of arguments left after parsing options out
        size_t num_args() {
            return nargs_;
        }
        
        // return true if option is set, false otherwise
        bool optset(string name) {
            option* opt = find_option(name, true);
            opt->viewed = true;
            return opt->set;
        }

        // return raw string value of option, return def if not set
        string optval(string name, string def) {
            option* opt = find_option(name, true);
            opt->viewed = true;
            if (opt->set) {
                return opt->value;
            }
            return def;
        }

        // return the option as a double value.  Error if not convertible.  Return def if not set
        double optflt(string name, double def) {
            option* opt = find_option(name, true);
            opt->viewed = true;
            if (!opt->set) {
                return def;
            }

            char *end;
            double val = strtold(opt->value.c_str(), &end);
            if (end == opt->value.c_str()) {
                error("unable to convert '%s' to double for '%s'", opt->value.c_str(), name.c_str());
                exit(EXIT_FAILURE);
            }
            return val;
        }

        // return the option as a signed int64_t value.  Error if not convertible.  Return def if not set
        int64_t optint(string name, int64_t def) {
            option* opt = find_option(name, true);
            opt->viewed = true;
            if (!opt->set) {
                return def;
            }

            char *end;
            int64_t val = strtoll(opt->value.c_str(), &end, 0);
            if (end == opt->value.c_str()) {
                error("unable to convert '%s' to integer for '%s'", opt->value.c_str(), name.c_str());
                exit(EXIT_FAILURE);
            }
            return val;
        }

    private:
        cmdline_spec     cmdspec_;  // parsed command line spec
        const char*      progname_; // name program was executed with

        vector<option>   optspec_;  // option specification
        vector<argument> argspec_;  // argument specification
        size_t           nargs_;    // number of arguments left after parsing out options
        
        // search optspec_ for a given option name and return it, return NULL if not found
        // we just keep a vector of options, so we'll do the O(N) thing and scan through it
        // if fatal is set, failure to find the option generates a fatal error
        option* find_option(string name, bool fatal=false) {
            vector<option>::iterator iter;
            for (iter=optspec_.begin(); iter != optspec_.end(); iter++) {
                if (iter->long_form() == name || iter->short_form() == name) {
                    return &*iter;
                }
            }
            if (fatal) {
                error("unkown option '%s'", name.c_str());
                exit(EXIT_FAILURE);
            }
            return NULL;
        }
        
        // print a warning with message
        __attribute__ ((format (printf, 1, 2)))
        static void warning(const char* fmt, ...) {
            term_mode(TERM_FG_YELLOW, TERM_UNDERLINE, stderr);
            fputs("warning", stderr);
            term_reset(stderr);
            fputs(" - ", stderr);

            va_list args;
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
            fputs("\n", stderr);
        }


        // print an error message
        __attribute__ ((format (printf, 1, 2)))
        static void error(const char* fmt, ...) {
            term_mode(TERM_FG_RED, TERM_BOLD, TERM_UNDERLINE, stderr);
            fputs("error", stderr);
            term_reset(stderr);
            fputs(" - ", stderr);

            va_list args;
            va_start(args, fmt);
            vfprintf(stderr, fmt, args);
            va_end(args);
            fputs("\n", stderr);
        }
    }; // cmdline
}

using command_line::cmdline;
