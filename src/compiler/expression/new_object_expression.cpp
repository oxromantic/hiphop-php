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

#include <compiler/expression/new_object_expression.h>
#include <compiler/expression/scalar_expression.h>
#include <compiler/expression/expression_list.h>
#include <compiler/analysis/code_error.h>
#include <compiler/analysis/class_scope.h>
#include <compiler/analysis/function_scope.h>
#include <compiler/option.h>
#include <compiler/analysis/variable_table.h>

using namespace HPHP;
using namespace std;
using namespace boost;

///////////////////////////////////////////////////////////////////////////////
// constructors/destructors

NewObjectExpression::NewObjectExpression
(EXPRESSION_CONSTRUCTOR_PARAMETERS,
 ExpressionPtr variable, ExpressionListPtr params)
  : FunctionCall(EXPRESSION_CONSTRUCTOR_PARAMETER_VALUES,
                 variable, "", params, ExpressionPtr()),
    m_redeclared(false), m_dynamic(false) {
}

ExpressionPtr NewObjectExpression::clone() {
  NewObjectExpressionPtr exp(new NewObjectExpression(*this));
  FunctionCall::deepCopy(exp);
  return exp;
}

///////////////////////////////////////////////////////////////////////////////
// parser functions

///////////////////////////////////////////////////////////////////////////////
// static analysis functions

void NewObjectExpression::analyzeProgram(AnalysisResultPtr ar) {
  FunctionScopePtr func;
  if (!m_name.empty()) {
    addUserClass(ar, m_name);
    if (ClassScopePtr cls = ar->resolveClass(shared_from_this(), m_name)) {
      if (!cls->isRedeclaring()) {
        func = cls->findConstructor(ar, true);
      }
    }
  }

  m_nameExp->analyzeProgram(ar);
  if (m_params) {
    m_params->analyzeProgram(ar);
    markRefParams(func, "", false);
  }
}

TypePtr NewObjectExpression::inferTypes(AnalysisResultPtr ar, TypePtr type,
                                        bool coerce) {
  reset();
  ConstructPtr self = shared_from_this();
  if (!m_name.empty()) {
    ClassScopePtr cls = ar->resolveClass(self, m_name);
    if (cls) {
      m_name = cls->getName();
    }
    if (!cls || cls->isRedeclaring()) {
      if (cls) {
        m_redeclared = true;
        getScope()->getVariables()->
          setAttribute(VariableTable::NeedGlobalPointer);
      }
      if (!cls && getScope()->isFirstPass()) {
        Compiler::Error(Compiler::UnknownClass, self);
      }
      if (m_params) m_params->inferAndCheck(ar, Type::Any, false);
      return Type::Object;
    }
    if (cls->isVolatile()) {
      getScope()->getVariables()->
        setAttribute(VariableTable::NeedGlobalPointer);
    }
    m_dynamic = cls->derivesFromRedeclaring();
    m_validClass = true;
    FunctionScopePtr func = cls->findConstructor(ar, true);
    if (!func) {
      if (m_params) {
        if (!m_dynamic && m_params->getCount()) {
          if (getScope()->isFirstPass()) {
            Compiler::Error(Compiler::BadConstructorCall, self);
          }
          m_params->setOutputCount(0);
        }
        m_params->inferAndCheck(ar, Type::Some, false);
      }
    } else {
      m_extraArg = func->inferParamTypes(ar, self, m_params,
                                         m_validClass);
      m_variableArgument = func->isVariableArgument();
    }
    if (!m_validClass || m_dynamic) {
      m_implementedType = Type::Object;
    } else {
      m_implementedType.reset();
    }
    return Type::CreateObjectType(m_name);
  } else {
    ar->containsDynamicClass();
    if (m_params) {
      m_params->markParams(false);
    }
  }

  m_implementedType.reset();
  m_nameExp->inferAndCheck(ar, Type::String, false);
  if (m_params) m_params->inferAndCheck(ar, Type::Any, false);
  return Type::Object;
}

///////////////////////////////////////////////////////////////////////////////
// code generation functions

void NewObjectExpression::outputPHP(CodeGenerator &cg, AnalysisResultPtr ar) {
  outputLineMap(cg, ar);

  cg_printf("new ");
  m_nameExp->outputPHP(cg, ar);
  cg_printf("(");
  if (m_params) m_params->outputPHP(cg, ar);
  cg_printf(")");
}

void NewObjectExpression::preOutputStash(CodeGenerator &cg,
                                         AnalysisResultPtr ar, int state) {
  if (!m_receiverTemp.empty()) {
    TypePtr e = getExpectedType();
    if (!e || !Type::IsCastNeeded(ar, getActualType(), e)) {
      bool unused = isUnused();
      setUnused(true);
      outputCPPImpl(cg, ar);
      setUnused(unused);
      setCPPTemp(m_receiverTemp);
      cg_printf(";\n");
      return;
    }
  }
  Expression::preOutputStash(cg, ar, state);
}

void NewObjectExpression::outputCPPImpl(CodeGenerator &cg,
                                        AnalysisResultPtr ar) {
  string &cname = (m_origName == "self" || m_origName == "parent") ?
    m_name : m_origName;
  bool outsideClass = !ar->checkClassPresent(shared_from_this(), m_origName);
  if (!m_name.empty() && !m_redeclared && m_validClass && !m_dynamic) {
    ClassScopePtr cls = ar->resolveClass(shared_from_this(), m_name);
    ASSERT(cls);
    if (m_receiverTemp.empty()) {
      if (outsideClass) {
        cls->outputVolatileCheckBegin(cg, ar, getScope(), cname);
      }
      cg_printf("%s%s((NEWOBJ(%s%s)())->create(",
                Option::SmartPtrPrefix, cls->getId(cg).c_str(),
                Option::ClassPrefix, cls->getId(cg).c_str());
    } else {
      cg_printf("(%s->create(", m_receiverTemp.c_str());
    }

    FunctionScope::outputCPPArguments(m_params, cg, ar, m_extraArg,
                                      m_variableArgument, m_argArrayId,
                                      m_argArrayHash, m_argArrayIndex);
    if (m_receiverTemp.empty()) {
      cg_printf("))");
      if (outsideClass) {
        cls->outputVolatileCheckEnd(cg);
      }
    } else {
      cg_printf(")");
      if (!isUnused()) {
        cg_printf(", %s", m_receiverTemp.c_str());
      }
      cg_printf(")");
    }
  } else {
    bool wrap = false;
    wrap = m_actualType && m_actualType->is(Type::KindOfVariant) &&
      !m_expectedType && !m_implementedType;
    if (wrap) {
      cg_printf("((Variant)");
    }
    cg_printf("id(obj%d)", m_objectTemp);
    if (wrap) {
      cg_printf(")");
    }
  }
}

bool NewObjectExpression::preOutputCPP(CodeGenerator &cg, AnalysisResultPtr ar,
                                       int state) {
  string &cname = (m_origName == "self" || m_origName == "parent") ?
    m_name : m_origName;
  if (m_name.empty() || m_redeclared || !m_validClass || m_dynamic) {
    // Short circuit out if inExpression() returns false
    if (!ar->inExpression()) return true;

    if (m_nameExp) m_nameExp->preOutputCPP(cg, ar, state);
    ar->wrapExpressionBegin(cg);
    m_ciTemp = cg.createNewId(shared_from_this());
    m_objectTemp = cg.createNewId(shared_from_this());
    cg_printf("Object obj%d(", m_objectTemp);
    if (m_redeclared) {
      bool outsideClass = !ar->checkClassPresent(shared_from_this(),
                                                 m_origName);
      ClassScopePtr cls = ar->resolveClass(shared_from_this(), m_name);
      ASSERT(cls);
      if (outsideClass) {
        cls->outputVolatileCheckBegin(cg, ar, getScope(), cname);
      }
      cg_printf("g->%s%s->createOnly()", Option::ClassStaticsObjectPrefix,
                m_name.c_str());
      if (outsideClass) {
        cls->outputVolatileCheckEnd(cg);
      }
    } else {
      cg_printf("create_object_only(");
      if (!cname.empty()) {
        cg_printf("\"%s\"", cname.c_str());
      } else if (m_nameExp->is(Expression::KindOfSimpleVariable)) {
        m_nameExp->outputCPP(cg, ar);
      } else {
        cg_printf("(");
        m_nameExp->outputCPP(cg, ar);
        cg_printf(")");
      }
      cg_printf(")");
    }
    cg_printf(");\n");
    cg_printf("MethodCallPackage mcp%d;\n", m_ciTemp,
        m_objectTemp);
    cg_printf("mcp%d.construct(obj%d);\n", m_ciTemp, m_objectTemp);
    cg_printf("const CallInfo *cit%d = mcp%d.ci;\n", m_ciTemp, m_ciTemp);

    if (m_params && m_params->getCount() > 0) {
      ar->pushCallInfo(m_ciTemp);
      m_params->preOutputCPP(cg, ar, state);
      ar->popCallInfo();
    }
    cg_printf("(cit%d->getMeth())(mcp%d, ", m_ciTemp, m_ciTemp);
    if (m_params && m_params->getOutputCount()) {
      ar->pushCallInfo(m_ciTemp);
      FunctionScope::outputCPPArguments(m_params, cg, ar, -1, false);
      ar->popCallInfo();
    } else {
      cg_printf("Array()");
    }
    cg_printf(");\n");

    if (state & FixOrder) {
      ar->pushCallInfo(m_ciTemp);
      preOutputStash(cg, ar, state);
      ar->popCallInfo();
    }

    if (hasCPPTemp() && !(state & FixOrder)) {
      cg_printf("id(%s);\n", cppTemp().c_str());
    }
    return true;
  } else {
    bool tempRcvr = true;

    bool paramEffect = false;
    if (m_params && m_params->getCount() > 0) {
      for (int i = m_params->getCount(); i--; ) {
        if (!(*m_params)[i]->isScalar()) {
          paramEffect = true;
          break;
        }
      }
    }

    if (!paramEffect) {
      tempRcvr = false;
    }

    if (tempRcvr && ar->inExpression()) {
      bool outsideClass = !ar->checkClassPresent(shared_from_this(),
                                                 m_origName);
      ClassScopePtr cls = ar->resolveClass(shared_from_this(), m_name);
      ASSERT(cls);
      ar->wrapExpressionBegin(cg);
      m_receiverTemp = genCPPTemp(cg, ar);
      cg_printf("%s%s %s = ", Option::SmartPtrPrefix, cls->getId(cg).c_str(),
          m_receiverTemp.c_str());
      if (outsideClass) {
        cls->outputVolatileCheckBegin(cg, ar, getScope(), cname);
      }
      cg_printf("NEWOBJ(%s%s)()", Option::ClassPrefix, cls->getId(cg).c_str());
      if (outsideClass) {
        cls->outputVolatileCheckEnd(cg);
      }
      cg_printf(";\n");
    }

    bool tempParams = FunctionCall::preOutputCPP(cg, ar, state);
    return tempRcvr || tempParams;
  }
}
