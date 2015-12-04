/**
 * @file CDEECo.h
 * \ingroup cdeeco
 * 
 * This file contains definitions that should be visible by the CDEECo++ users.
 * 
 * \date 4. 12. 2015
 * \author Vladimír Matěna <matena@d3s.mff.cuni.cz>
 * 
 */

#include "Console.h"

// Hardware
extern Console console;

/**
 * This method is to be implemented by DEECo application
 * 
 * It is supposed to be setup DEECo application. In order to see how please read documentation and see example
 * application in test directory.
 * 
 */
void cdeecoSetup(const uint32_t uniqId);