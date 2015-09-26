//
#ifndef TELLDUS_CORE_SERVICE_PROTOCOLCRX500_H_
#define TELLDUS_CORE_SERVICE_PROTOCOLCRX500_H_

#include <string>
#include "service/Protocol.h"
#include "service/ControllerMessage.h"

class ProtocolCRX500 : public Protocol {
public:
	static std::string decodeData(const ControllerMessage &dataMsg);
};

#endif  // TELLDUS_CORE_SERVICE_PROTOCOLCRX500_H_
