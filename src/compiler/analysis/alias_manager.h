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

#ifndef __ALIAS_MANAGER_H__
#define __ALIAS_MANAGER_H__

#include <compiler/expression/expression.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

DECLARE_BOOST_TYPES(MethodStatement);

class BucketMapEntry {
 public:
  BucketMapEntry() : m_num(0) {}
 public:
  ExpressionPtr find(ExpressionPtr e);
  void add(ExpressionPtr e);
  void clear();
  void beginScope();
  void endScope();
  void resetScope();
  void pop_back() { m_exprs.pop_back(); }
  ExpressionPtrList::iterator begin() { return m_exprs.begin(); }
  ExpressionPtrList::iterator end() { return m_exprs.end(); }
  ExpressionPtrList::reverse_iterator rbegin() { return m_exprs.rbegin(); }
  ExpressionPtrList::reverse_iterator rend() { return m_exprs.rend(); }
  bool isLast(ExpressionPtr e) { return *rbegin() == e; }
  bool isSubLast(ExpressionPtr e);
  size_t size() { return m_num; }
  void stash(size_t from, ExpressionPtrList &to);
  void import(ExpressionPtrList &from);
  void erase(ExpressionPtrList::reverse_iterator rit,
             ExpressionPtrList::reverse_iterator &end) {
    // the base of a reverse iterator points one beyond
    // the element the reverse iterator points to, so
    // decrement it...
    ExpressionPtrList::iterator it(--rit.base());
    // erasing an element /can/ invalidate the end
    // reverse_iterator, so fix it if necessary
    if (end.base() == it) end = rit;
    m_exprs.erase(it);
    // rit is still valid, and now (magically) points at
    // the next element... no need to return a new iterator.
    m_num--;
  }
 private:
  ExpressionPtrList     m_exprs;
  std::vector<size_t>   m_stack;
  size_t                m_num;
};

class AliasInfo {
 public:
  AliasInfo() : m_refLevels(0), m_isRefTo(false),
    m_isParam(false), m_isGlobal(false) {}
 public:
  void setRefLevels(unsigned rl) { m_refLevels = rl; }
  void clearRefLevel(int l) { m_refLevels &= ~(1u << l); }
  void addRefLevel(int l) { m_refLevels |= 1u << l; }
  unsigned getRefLevels() { return m_refLevels; }
  bool checkRefLevel(int l) { return (m_refLevels & (1u << l)) != 0; }

  bool getIsRefTo() const { return m_isRefTo; }
  void setIsRefTo(bool ref = true) { m_isRefTo = ref; }

  bool getIsParam() const { return m_isParam; }
  void setIsParam(bool p = true) { m_isParam = p; }

  bool getIsGlobal() const { return m_isGlobal; }
  void setIsGlobal(bool g = true) { m_isGlobal = g; }
 private:
  unsigned m_refLevels;
  bool m_isRefTo;
  bool m_isParam;
  bool m_isGlobal;
};

class AliasManager {
 public:
  AliasManager();
 public:
  void clear();
  void beginScope();
  void endScope();
  void resetScope();
  ExpressionPtr getCanonical(ExpressionPtr e);

  int optimize(AnalysisResultPtr ar, MethodStatementPtr s);
  void setChanged() { m_changes++; }

  static bool parseOptimizations(const std::string &optimizations,
                                 std::string &errs);
 private:
  enum { MaxBuckets = 0x10000 };
  enum { FallThrough, CondBranch, Branch, Converge };
  enum { SameAccess, InterfAccess, DisjointAccess, NotAccess };
  struct CondStackElem {
    CondStackElem(size_t s = 0) : m_size(s), m_exprs() {}
    size_t              m_size;
    ExpressionPtrList   m_exprs;
  };

  typedef std::set<std::string> StringSet;

  class LoopInfo {
  public:
    LoopInfo(StatementPtr s);

    StatementPtr m_stmt;
    StatementPtrVec m_inner;
    bool m_valid;
    StringSet m_candidates;
    StringSet m_excluded;
  };

  typedef std::map<unsigned, BucketMapEntry> BucketMap;
  typedef std::map<std::string, AliasInfo> AliasInfoMap;
  typedef std::vector<CondStackElem> CondStack;
  typedef std::vector<LoopInfo> LoopInfoVec;

  void mergeScope();

  static void clearHelper(BucketMap::value_type &it);
  static void beginScopeHelper(BucketMap::value_type &it);
  static void endScopeHelper(BucketMap::value_type &it);
  static void resetScopeHelper(BucketMap::value_type &it);

  void add(BucketMapEntry &em, ExpressionPtr e);

  int testAccesses(ExpressionPtr e1, ExpressionPtr e2);
  void cleanInterf(ExpressionPtr rv,
                   ExpressionPtrList::reverse_iterator it,
                   ExpressionPtrList::reverse_iterator &end,
                   int depth);
  void killLocals();
  bool okToKill(ExpressionPtr ep, bool killRef);
  int checkInterf(ExpressionPtr rv, ExpressionPtr e, bool &isLoad,
                  int &depth, int &effects);
  int findInterf(ExpressionPtr rv, bool isLoad, ExpressionPtr &rep);
  void applyAssign(ExpressionPtr lhs, ExpressionPtr rhs);

  void canonicalizeKid(ConstructPtr e, ExpressionPtr kid, int i);
  ExpressionPtr canonicalizeNode(ExpressionPtr e);
  ExpressionPtr canonicalizeNonNull(ExpressionPtr e);
  ExpressionPtr canonicalizeRecurNonNull(ExpressionPtr e);
  ExpressionPtr canonicalizeRecur(ExpressionPtr e);
  int canonicalizeRecur(StatementPtr e);

  void collectAliasInfoRecur(ConstructPtr cs, bool unused);
  void pushStringScope(StatementPtr s);
  void popStringScope(StatementPtr s);
  void stringOptsRecur(StatementPtr s);
  void stringOptsRecur(ExpressionPtr s, bool ok);

  BucketMap             m_bucketMap;
  CondStack             m_stack;

  unsigned              m_nextID;

  int                   m_changes;
  int                   m_replaced;
  bool                  m_wildRefs;
  AliasInfoMap          m_aliasInfo;

  AnalysisResultPtr     m_arp;
  VariableTablePtr      m_variables;

  LoopInfoVec           m_loopInfo;

  std::string           m_returnVar;
  int                   m_nrvoFix;

  bool                  m_inlineAsExpr;
  bool                  m_noAdd;
};

///////////////////////////////////////////////////////////////////////////////
}
#endif // __ALIAS_MANAGER_H__
