/******************************************************************************
 * Copyright (c) 2000-2021 Ericsson Telecom AB
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * which accompanies this distribution, and is available at
 * https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
 *
 * Contributors:
 *   Baji, Laszlo
 *   Balasko, Jeno
 *   Raduly, Csaba
 *
 ******************************************************************************/
#include "error.h"
#include "CompilerError.hh"
#include "../common/memory.h"
#include "../common/path.h"
#include "main.hh"
#include "Setting.hh"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#ifndef MINGW
# include <sys/wait.h>
#endif

unsigned verb_level=0x0007; /* default value */

const char *argv0; /* the programname :) */

enum stderr_color {
	COLOR_ERROR,
	COLOR_WARNING,
	COLOR_NOTE,
	COLOR_NOTIFY,
	COLOR_DEBUG,
};

/* might be interesting to provide a way to override these from the environment, similar to GCC_COLORS,
 * or even to fall back to GCC_COLORS if it is set? */
static const char *color_error = "01;31";
static const char *color_warning = "01;35";
static const char *color_note = "01;36";
static const char *color_notify = "";
static const char *color_debug = "";

static const char *get_color(enum stderr_color color)
{
  switch (color) {
  case COLOR_ERROR:
    return color_error;
  case COLOR_WARNING:
    return color_warning;
  case COLOR_NOTE:
    return color_note;
  case COLOR_NOTIFY:
    return color_notify;
  case COLOR_DEBUG:
    return color_debug;
  default:
    return NULL;
  }
}

/* should we colorize the output? */
static bool should_colorize(void)
{
  char const *t = getenv("TERM");
  if (!t)
    return false;
  if (!strcmp(t, "dumb"))
    return false;
  if (isatty(STDERR_FILENO))
    return true;
  return false;
}

static void begin_colorize(enum stderr_color color)
{
  const char *colstr;
  if (!should_colorize())
    return;
  colstr = get_color(color);
  if (colstr)
    fprintf(stderr, "\033[1;%sm", colstr);
}

static void end_colorize()
{
  if (!should_colorize())
    return;
  fputs("\033[0;m", stderr);
}

void fatal_error(const char *filename, int lineno, const char *fmt, ...)
{
  va_list parameters;
#ifdef FATAL_DEBUG
  va_start(parameters, fmt);
  Common::Location loc(filename, lineno);
  Common::Error_Context::report_error(&loc, fmt, parameters);
  va_end(parameters);
#else
  begin_colorize(COLOR_ERROR);
  fprintf(stderr, "FATAL ERROR: %s: In line %d of %s: ",
          argv0, lineno, filename);
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  end_colorize();
#endif
  fflush(stderr);
  abort();
}

void ERROR(const char *fmt, ...)
{
  begin_colorize(COLOR_ERROR);
  fprintf(stderr, "%s: error: ", argv0);
  va_list parameters;
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  end_colorize();
  fflush(stderr);
  Common::Error_Context::increment_error_count();
}

void WARNING(const char *fmt, ...)
{
  if(!(verb_level & 2)) return;
  begin_colorize(COLOR_WARNING);
  fprintf(stderr, "%s: warning: ", argv0);
  va_list parameters;
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  end_colorize();
  fflush(stderr);
  Common::Error_Context::increment_warning_count();
}

void NOTSUPP(const char *fmt, ...)
{
  if(!(verb_level & 1)) return;
  fprintf(stderr, "%s: warning: not supported: ", argv0);
  va_list parameters;
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  fflush(stderr);
  Common::Error_Context::increment_warning_count();
}

void NOTIFY(const char *fmt, ...)
{
  if(!(verb_level & 4)) return;
  begin_colorize(COLOR_NOTIFY);
  fprintf(stderr, "Notify: ");
  va_list parameters;
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  end_colorize();
  fflush(stderr);
}

void DEBUG(unsigned level, const char *fmt, ...)
{
  if((level>7?7:level)>((verb_level>>3)&0x07)) return;
  begin_colorize(COLOR_DEBUG);
  fprintf(stderr, "%*sDebug: ", level, "");
  va_list parameters;
  va_start(parameters, fmt);
  vfprintf(stderr, fmt, parameters);
  va_end(parameters);
  putc('\n', stderr);
  end_colorize();
  fflush(stderr);
}

unsigned int get_error_count(void)
{
  return Common::Error_Context::get_error_count();
}

unsigned int get_warning_count(void)
{
  return Common::Error_Context::get_warning_count();
}

void path_error(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char *err_msg = mprintf_va_list(fmt, ap);
  va_end(ap);
  ERROR("%s", err_msg);
  Free(err_msg);
}

namespace Common {

  unsigned int Error_Context::error_count = 0, Error_Context::warning_count = 0;
  unsigned int Error_Context::max_errors = (unsigned)-1;

  bool Error_Context::chain_printed = false;

  Error_Context *Error_Context::head = 0, *Error_Context::tail = 0;

  void Error_Context::print_context(FILE *fp)
  {
    int level = 0;
    for (Error_Context *ptr = head; ptr; ptr = ptr->next) {
      if (ptr->is_restorer) FATAL_ERROR("Error_Context::print()");
      else if (!ptr->str) continue;
      else if (!ptr->is_printed) {
	for (int i = 0; i < level; i++) putc(' ', fp);
	if (ptr->location) ptr->location->print_location(fp);
	if (gcc_compat) fputs("note: ", fp); // CDT ignores "note"
	fputs(ptr->str, fp);
	fputs(":\n", fp);
	ptr->is_printed = true;
      }
      level++;
    }
    for (int i = 0; i < level; i++) putc(' ', fp);
    chain_printed = true;
  }

  Error_Context::Error_Context(size_t n_keep)
  : prev(0), next(0), location(0), str(0), is_printed(false),
    is_restorer(true), outer_printed(false)
  {
    Error_Context *begin = head;
    for (size_t i = 0; i < n_keep; i++) {
      if (!begin) break;
      begin = begin->next;
    }
    if (begin == head) {
      // complete backup
      next = head;
      head = 0;
      prev = tail;
      tail = 0;
    } else {
      // partial backup (only elements before begin are kept)
      next = begin;
      if (begin) {
        prev = tail;
	tail = begin->prev;
        tail->next = 0;
        begin->prev = 0;
      } else prev = 0;
    }
    outer_printed = chain_printed;
    chain_printed = false;
  }

  Error_Context::Error_Context(const Location *p_location)
  : prev(0), next(0), location(p_location), str(0), is_printed(false),
    is_restorer(false), outer_printed(false)
  {
    if (!head) head = this;
    if (tail) tail->next = this;
    prev = tail;
    next = 0;
    tail = this;
  }

  Error_Context::Error_Context(const Location *p_location,
    const char *p_fmt, ...)
    : prev(0), next(0), location(p_location), str(0), is_printed(false),
      is_restorer(false), outer_printed(false)
  {
    va_list args;
    va_start(args, p_fmt);
    str = mprintf_va_list(p_fmt, args);
    va_end(args);

    if (!head) head = this;
    if (tail) tail->next = this;
    prev = tail;
    next = 0;
    tail = this;
  }

  Error_Context::~Error_Context()
  {
    if (is_restorer) {
      if (chain_printed) {
	for (Error_Context *ptr = next; ptr; ptr = ptr->next)
	  ptr->is_printed = false;
      } else chain_printed = outer_printed;
      if (head) {
        // partial restoration
        if (next) {
	  tail->next = next;
	  next->prev = tail;
	  tail = prev;
	}
      } else {
        // full restoration
	head = next;
	tail = prev;
      }
    } else {
      Free(str);
      if (tail != this) FATAL_ERROR("Error_Context::~Error_Context()");
      if (prev) prev->next = 0;
      else head = 0;
      tail = prev;
    }
  }

  void Error_Context::set_message(const char *p_fmt, ...)
  {
    if (is_restorer) FATAL_ERROR("Error_Context::set_message()");
    Free(str);
    va_list args;
    va_start(args, p_fmt);
    str = mprintf_va_list(p_fmt, args);
    va_end(args);
    is_printed = false;
  }

  void Error_Context::report_error(const Location *loc, const char *fmt,
    va_list args)
  {
    if (!suppress_context) print_context(stderr);
    if (tail != 0 && loc && loc->get_filename() == 0) {
      // borrow location information from the innermost context
      Location my_location;
      my_location.set_location( *(tail->location) );
      my_location.print_location(stderr);
    } else if (loc) {
      loc->print_location(stderr);
    }
    begin_colorize(COLOR_ERROR);
    fputs("error: ", stderr);
    end_colorize();
    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    fflush(stderr);
    increment_error_count();
  }

  void Error_Context::report_warning(const Location *loc, const char *fmt,
    va_list args)
  {
    if(!(verb_level & 2)) return;
    if (!suppress_context) print_context(stderr);
    if (loc) loc->print_location(stderr);
    begin_colorize(COLOR_WARNING);
    fputs("warning: ", stderr);
    end_colorize();
    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    fflush(stderr);
    increment_warning_count();
  }

  void Error_Context::report_note(const Location *loc, const char *fmt,
    va_list args)
  {
    if (!suppress_context) print_context(stderr);
    if (loc) loc->print_location(stderr);
    begin_colorize(COLOR_NOTE);
    fputs("note: ", stderr);
    end_colorize();
    vfprintf(stderr, fmt, args);
    putc('\n', stderr);
    fflush(stderr);
  }

  void Error_Context::increment_error_count()
  {
    if (++error_count >= max_errors) {
      fputs("Maximum number of errors reached, aborting.\n", stderr);
      fflush(stderr);
      exit(EXIT_FAILURE);
    }
  }

  void Error_Context::increment_warning_count()
  {
    warning_count++;
  }

  void Error_Context::print_error_statistics()
  {
    if (error_count == 0) {
      if (warning_count == 0) NOTIFY("No errors or warnings were detected.");
      else NOTIFY("No errors and %u warning%s were detected.",
	 warning_count, warning_count > 1 ? "s" : "");
    } else {
      if (warning_count == 0) NOTIFY("%u error%s and no warnings were "
	"detected.", error_count, error_count > 1 ? "s" : "");
      else NOTIFY("%u error%s and %u warning%s were detected.",
	error_count, error_count > 1 ? "s" : "",
	warning_count, warning_count > 1 ? "s" : "");
    }
  }

} // namespace Common
