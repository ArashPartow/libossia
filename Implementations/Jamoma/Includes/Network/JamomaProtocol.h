/*!
 * \file JamomaProtocol.h
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

#include "Network/Protocol.h"

#include "Network/JamomaAddress.h"

using namespace OSSIA;
using namespace std;

class JamomaProtocol : public virtual Protocol
{

public:
  
# pragma mark -
# pragma mark Life cycle
  
  JamomaProtocol();
};
