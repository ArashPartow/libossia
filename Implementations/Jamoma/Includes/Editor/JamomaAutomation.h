/*!
 * \file JamomaScenario.h
 *
 * \brief
 *
 * \details
 *
 * \author Théo de la Hogue
 *
 * \copyright This code is licensed under the terms of the "CeCILL-C"
 * http://www.cecill.info
 */

#pragma once
#include "Editor/Automation.h"
#include "Editor/TimeNode.h"
#include "Editor/TimeValue.h"
#include "Editor/Value.h"
#include "Network/Address.h"

using namespace OSSIA;
using namespace std;

class JamomaAutomation : public Automation<double>
{
  
private:
  
  // Implementation Specific
  shared_ptr<Curve<double>> curve;
  shared_ptr<Address> addr;
  shared_ptr<Address> element;
  
public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaAutomation();
  
  JamomaAutomation(const JamomaAutomation * other);
  
  virtual ~JamomaAutomation();
  
  virtual shared_ptr<Automation> clone() const override;

# pragma mark -
# pragma mark Execution
  
  virtual void play(bool log = false, string name = "") const override;

# pragma mark -
# pragma mark Accessors
  
  virtual Value getStartValue() const override;
  
  virtual void setStartValue(Value) override;
  
  virtual Value getEndValue() const override;
  
  virtual void setEndValue(Value) override;
  
  virtual const shared_ptr<Curve<double>> & getCurve() const override;
  
  virtual void setCurve(shared_ptr<Curve<double>>) override;
  
  virtual const shared_ptr<Address> & getInputAddress() const override;

  virtual void setInputAddress(shared_ptr<Address>) override;
  
  const std::shared_ptr<TimeConstraint> & getParentTimeConstraint() const override;
  
  const std::shared_ptr<State> & getStartState() const override;
  
  const std::shared_ptr<State> & getEndState() const override;
};
