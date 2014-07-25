/**
 * \ingroup cdeeco
 * @file KnowledgeFragment.h
 *
 * Definition of the CDEECo++ knowledge fragment
 *
 * \date 28. 4. 2014
 * \author Vladimír Matěna <vlada@mattty.cz>
 */

#ifndef KNOWLEDGE_FRAGMENT_H
#define KNOWLEDGE_FRAGMENT_H

#include <cstdint>
#include <cstring>

#include "Knowledge.h"

namespace CDEECO {
	/**
	 * Knowledge fragment structure to be included directly in the packet
	 *
	 * \ingroup cdeeco
	 */
	struct KnowledgeFragment {
		/// Maximum packet size
		static const size_t MAX_PACKET_SIZE = 128;
		/// Maximum size of the data part
		static const size_t MAX_DATA_SIZE = 112;
		/**
		 * CDEECo++ magic value
		 *
		 * Used to identify the CDEECo++ packets
		 */
		static const uint32_t CDEECO_MAGIC = 0xcdeec044;

		/// Knowledge type
		Type type;
		/// Knowledge component id
		Id id;
		/// Fragment size
		size_t size;
		/// Fragment offset in the knowledge
		size_t offset;
		/// Fragment data
		char data[MAX_DATA_SIZE];

		/**
		 * Get used length of the packet
		 *
		 * @return Size of used packet part
		 */
		size_t length() const {
			return sizeof(KnowledgeFragment) - MAX_DATA_SIZE + size;
		}
	};

	// Ensure knowledge fragment fits into a packet
	static_assert(KnowledgeFragment::MAX_PACKET_SIZE == sizeof(KnowledgeFragment), "Knowledge fragment size not equal to packet size.");
}

#endif // KNOWLEDGE_FRAGMENT_H
