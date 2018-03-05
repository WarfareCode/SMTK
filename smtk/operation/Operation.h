//=========================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//=========================================================================
#ifndef __smtk_operation_Operation_h
#define __smtk_operation_Operation_h

#include "smtk/CoreExports.h"
#include "smtk/PublicPointerDefs.h"
#include "smtk/SharedFromThis.h"

#include <string>
#include <typeindex>
#include <utility>

namespace smtk
{
namespace attribute
{
class Attribute;
class Collection;
}
namespace io
{
class Logger;
}
namespace operation
{
class ImportPythonOperation;
class Manager;
class Metadata;

/// Operation is a base class for all SMTK operations. SMTK operations are
/// essentially functors that operate on SMTK resources and resource components.
/// Their input parameters and output results are described by an SMTK attribute
/// resource that is constructed in derived operation classes via the method
/// createSpecification(). There should be exactly one attribute definition that
/// derives from "operation" and one definition that derives from "result". The
/// creation, destruction and manipulation of SMTK resources and components
/// occurs in the method operateInternal(). Operations are designed to work in
/// conjunction with an smtk::operation::Manager, but can also be used as
/// standalone functors.
class SMTKCORE_EXPORT Operation : smtkEnableSharedPtr(Operation)
{
public:
  smtkTypeMacroBase(Operation);

  // A hash value uniquely representing the operation.
  typedef std::size_t Index;

  // An attribute describing the operation's input.
  typedef std::shared_ptr<smtk::attribute::Attribute> Parameters;

  // An attribute containing the operation's result.
  typedef std::shared_ptr<smtk::attribute::Attribute> Result;

  // An attribute collection containing the operation's execution definition
  // result definition.
  typedef std::shared_ptr<smtk::attribute::Collection> Specification;

  // These values are taken on by the "outcome" item of every Operation Result.
  enum class Outcome
  {
    UNABLE_TO_OPERATE, //!< The operation is misconfigured.
    CANCELED,          //!< An observer requested the operation be canceled. And it was.
    FAILED,            //!< The operation attempted to execute but encountered a problem.
    SUCCEEDED,         //!< The operation succeeded.
    UNKNOWN = -1       //!< The operation has not been run or the outcome is uninitialized.
  };

  friend class Manager;
  friend ImportPythonOperation;

  // Return a unique string that describes this class. Registered operations
  // return their registered unique name; unregistered operations simply return
  // this->classname().
  std::string uniqueName() const;

  // Index is a compile-time intrinsic of the derived operation; as such, it
  // cannot be set. It is virtual so that derived operations can assign their
  // own index (as is necessary for python operations that would otherwise all
  // resolve to the same index).
  virtual Index index() const { return std::type_index(typeid(*this)).hash_code(); }

  // Check if the operation's attribute collection is valid. Derived operations
  // may implement more task-specific checks to ensure that the operation is in
  // a valid state.
  virtual bool ableToOperate();

  // Execute the operation, log its outcome and return its results. This method
  // calls operateInternal() and handles additional bookkeeping.
  Result operate();

  // Retrieve the operation's logger. By default, we use the singleton logger.
  // Derived classes can reimplement this method if an alternative logging
  // system is needed.
  virtual smtk::io::Logger& log() const;

  // This accessor facilitates the lazy construction of the specification,
  // allowing for derived implementations of its creation. More sophisticated
  // operations may contain additional attributes as input parameters; they can
  // be accessed through the specification.
  Specification specification();

  // Access the operation's input parameters, constructing them if necessary.
  // The parameters attribute is distinguished by its derivation from the
  // "operation" attribute.
  Parameters parameters();

  // Create an attribute representing this operation's result type. The result
  // attribute is distinguished by its derivation from the "result" attribute.
  Result createResult(Outcome);

  virtual ~Operation();

protected:
  Operation();

  // Perform the actual operation and construct the result.
  virtual Result operateInternal() = 0;

  // Apply post-processing to the result object. This method should not modify
  // the modeling kernel but may change string/float/int properties stored on
  // entities.
  virtual void postProcessResult(Result&) {}

  // Append an output summary string to the output result. Derived classes can
  // reimplement this method to send custom summary strings to the logger.
  virtual void generateSummary(Result&);

  // Construct the operation's base specification. This is done by reading
  // an attribute .sbt file.
  Specification createBaseSpecification() const;

  int m_debugLevel;

private:
  typedef std::shared_ptr<smtk::attribute::Definition> Definition;

  // Construct the operation's specification. This is typically done by reading
  // an attribute .sbt file, but can be done by first constructing a base
  // specification and then augmenting the specification to include the derived
  // operation's input and output attributes.
  virtual Specification createSpecification() = 0;

  Specification m_specification;
  Parameters m_parameters;
  Definition m_resultDefinition;
  std::vector<Result> m_results;
  Manager* m_manager;
};
}
}

#endif // __smtk_operation_Operation_h