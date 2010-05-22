/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   | Copyright (c) 1997-2010 The PHP Group                                |
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

#ifndef __EXTPROFILE_CURL_H__
#define __EXTPROFILE_CURL_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <runtime/ext/ext_curl.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

inline Variant x_curl_init(CStrRef url = null_string) {
  FUNCTION_INJECTION_BUILTIN(curl_init);
  return f_curl_init(url);
}

inline Variant x_curl_copy_handle(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_copy_handle);
  return f_curl_copy_handle(ch);
}

inline Variant x_curl_version(int uversion = CURLVERSION_NOW) {
  FUNCTION_INJECTION_BUILTIN(curl_version);
  return f_curl_version(uversion);
}

inline bool x_curl_setopt(CObjRef ch, int option, CVarRef value) {
  FUNCTION_INJECTION_BUILTIN(curl_setopt);
  return f_curl_setopt(ch, option, value);
}

inline bool x_curl_setopt_array(CObjRef ch, CArrRef options) {
  FUNCTION_INJECTION_BUILTIN(curl_setopt_array);
  return f_curl_setopt_array(ch, options);
}

inline Variant x_curl_exec(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_exec);
  return f_curl_exec(ch);
}

inline Variant x_curl_getinfo(CObjRef ch, int opt = 0) {
  FUNCTION_INJECTION_BUILTIN(curl_getinfo);
  return f_curl_getinfo(ch, opt);
}

inline Variant x_curl_errno(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_errno);
  return f_curl_errno(ch);
}

inline Variant x_curl_error(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_error);
  return f_curl_error(ch);
}

inline Variant x_curl_close(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_close);
  return f_curl_close(ch);
}

inline Object x_curl_multi_init() {
  FUNCTION_INJECTION_BUILTIN(curl_multi_init);
  return f_curl_multi_init();
}

inline Variant x_curl_multi_add_handle(CObjRef mh, CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_add_handle);
  return f_curl_multi_add_handle(mh, ch);
}

inline Variant x_curl_multi_remove_handle(CObjRef mh, CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_remove_handle);
  return f_curl_multi_remove_handle(mh, ch);
}

inline Variant x_curl_multi_exec(CObjRef mh, Variant still_running) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_exec);
  return f_curl_multi_exec(mh, ref(still_running));
}

inline Variant x_curl_multi_select(CObjRef mh, double timeout = 1.0) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_select);
  return f_curl_multi_select(mh, timeout);
}

inline Variant x_curl_multi_getcontent(CObjRef ch) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_getcontent);
  return f_curl_multi_getcontent(ch);
}

inline Variant x_curl_multi_info_read(CObjRef mh, Variant msgs_in_queue = null) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_info_read);
  return f_curl_multi_info_read(mh, ref(msgs_in_queue));
}

inline Variant x_curl_multi_close(CObjRef mh) {
  FUNCTION_INJECTION_BUILTIN(curl_multi_close);
  return f_curl_multi_close(mh);
}

inline void x_evhttp_set_cache(CStrRef address, int max_conn, int port = 80) {
  FUNCTION_INJECTION_BUILTIN(evhttp_set_cache);
  f_evhttp_set_cache(address, max_conn, port);
}

inline Variant x_evhttp_get(CStrRef url, CArrRef headers = null_array, int timeout = 5) {
  FUNCTION_INJECTION_BUILTIN(evhttp_get);
  return f_evhttp_get(url, headers, timeout);
}

inline Variant x_evhttp_post(CStrRef url, CStrRef data, CArrRef headers = null_array, int timeout = 5) {
  FUNCTION_INJECTION_BUILTIN(evhttp_post);
  return f_evhttp_post(url, data, headers, timeout);
}

inline Variant x_evhttp_async_get(CStrRef url, CArrRef headers = null_array, int timeout = 5) {
  FUNCTION_INJECTION_BUILTIN(evhttp_async_get);
  return f_evhttp_async_get(url, headers, timeout);
}

inline Variant x_evhttp_async_post(CStrRef url, CStrRef data, CArrRef headers = null_array, int timeout = 5) {
  FUNCTION_INJECTION_BUILTIN(evhttp_async_post);
  return f_evhttp_async_post(url, data, headers, timeout);
}

inline Variant x_evhttp_recv(CObjRef handle) {
  FUNCTION_INJECTION_BUILTIN(evhttp_recv);
  return f_evhttp_recv(handle);
}


///////////////////////////////////////////////////////////////////////////////
}

#endif // __EXTPROFILE_CURL_H__