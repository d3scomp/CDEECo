/**
 * Knowledge interface
 */
struct Knowledge {
};

/**
 * Knowledge trait
 *
 * Used for allowed fragment offset definition
 */
template<class KNOWLEDGE>
struct KnowledgeTrait {
	static constexpr void* offsets[] = { };
};

