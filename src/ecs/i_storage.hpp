// === IStorage ===
// IStorage is a base interface for storage types.
// Enables storing Storage<T> instances of different types
// in a single container (e.g., std::vector<IStorage*>).

#ifndef MACROENGINE_I_STORAGE_HPP
#define MACROENGINE_I_STORAGE_HPP

namespace macroengine {
	struct IStorage {
	protected:
		IStorage() = default;
		virtual ~IStorage() = default;
	};
}

#endif