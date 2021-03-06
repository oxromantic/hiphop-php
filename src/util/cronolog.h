/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef __CRONOLOG_H__
#define __CRONOLOG_H__

#include <string>
#include <util/cronoutils.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

class Cronolog {
public:
  Cronolog() :
    m_periodicity(UNKNOWN),
    m_periodDelayUnits(UNKNOWN),
    m_periodMultiple(1),
    m_periodDelay(0),
    m_useAmericanDateFormats(0),
    m_startTime(NULL),
    m_prevLinkName(NULL),
    m_timeOffset(0),
    m_nextPeriod(0),
    m_file(NULL),
    m_bytesWritten(0),
    m_prevBytesWritten(0) {}
  ~Cronolog() {
    if (m_file) fclose(m_file);
  }
  void setPeriodicity();
  FILE *getOutputFile();
public:
  PERIODICITY m_periodicity;
  PERIODICITY m_periodDelayUnits;
  int m_periodMultiple;
  int m_periodDelay;
  int m_useAmericanDateFormats;
  char m_fileName[PATH_MAX];
  char *m_startTime;
  std::string m_template;
  std::string m_linkName;
  char *m_prevLinkName;
  time_t m_timeOffset;
  time_t m_nextPeriod;
  FILE *m_file;
  int m_bytesWritten;
  int m_prevBytesWritten;
};

///////////////////////////////////////////////////////////////////////////////
}

#endif // __CRONOLOG_H__
