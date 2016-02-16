#ifndef SRC_UTILS_GRAPHICS_RESOURCEPOOL_HPP_
#define SRC_UTILS_GRAPHICS_RESOURCEPOOL_HPP_

#include <Utils/Misc/Macros.hpp>
#include <Utils/Misc/MakeUnique.hpp>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>

class ExpiredException: public std::exception {
	inline const char* what() const KRONOS_NOTHROW {
		return "Attempt to activate expired resource";
	}
};

class InactiveException: public std::exception {
	inline const char* what() const KRONOS_NOTHROW {
		return "Attempt to access inactive resource";
	}
};

template<typename ResourceType>
class ResourcePool {
private:

	typedef unsigned int ID;

public:

	class Handle {
	public:

		/**
		 * Creates an invalid (expired) handle.
		 */
		Handle() :
			pool(nullptr),
			id(0) {
		}

		Handle(const Handle& other) :
			pool(other.pool),
			id(other.id) {
			if (pool != nullptr) {
				pool->incrementHandleCount(id);
			}
		}

		Handle& operator=(const Handle& other) {
			if (other.pool != nullptr) {
				other.pool->incrementHandleCount(other.id);
			}

			if (pool != nullptr) {
				pool->decrementHandleCount(id);
			}

			pool = other.pool;
			id = other.id;

			return *this;
		}

		~Handle() {
			if (pool != nullptr) {
				pool->decrementHandleCount(id);
			}
		}

		/**
		 * Returns a reference to the held resource.
		 *
		 * This can only be done if the resource is active, otherwise an exception will be thrown.
		 */
		ResourceType& getResource() const {
			if (!isActive()) {
				throw InactiveException();
			}

			return *pool->getResourceInfo(id).data;
		}

		/**
		 * Sets the activity status of the resource. A resource marked as "active" is considered "in-use" and will not
		 * be deallocated or recycled for a new handle.
		 *
		 * This will fail with an exception if the resource is expired.
		 */
		void setActive(bool active) {
			if (active && isExpired()) {
				throw ExpiredException();
			}

			if (pool != nullptr) {
				pool->setResourceActive(id, active);
			}
		}

		/**
		 * Returns the activity status of the resource.
		 */
		bool isActive() const {
			return pool != nullptr && pool->getResourceInfo(id).isActive();
		}

		/**
		 * Returns the expiration status of the resource. If a previously inactive resource is expired, it has been
		 * deallocated or recycled for a new handle. Therefore, a new resource has to be acquired.
		 */
		bool isExpired() const {
			return pool == nullptr || pool->getResourceInfo(id).isExpired();
		}

	private:

		Handle(ResourcePool<ResourceType>* pool, ID id) :
			pool(pool),
			id(id) {
			pool->incrementHandleCount(id);
		}

		ResourcePool<ResourceType>* pool;
		ID id;

		friend class ResourcePool<ResourceType> ;
	};

	static std::unique_ptr<ResourceType> defaultNewFunc() {
		return makeUnique<ResourceType>();
	}

	ResourcePool(std::function<std::unique_ptr<ResourceType>()> newFunc = defaultNewFunc) :
		poolSize(0),
		resourceCount(0),
		currentID(0),
		newFunc(newFunc) {
	}

	void setPoolSize(unsigned int poolSize) {
		bool sizeReduced = (poolSize < this->poolSize);

		this->poolSize = poolSize;

		if (sizeReduced) {
			cleanUpAll();
		}
	}

	unsigned int getPoolSize() const {
		return poolSize;
	}

	Handle acquire() {
		// Allocate ID.
		ID newID = currentID++;

		// Create resource info structure.
		ResourceInfo newResource;
		newResource.active = true;
		newResource.handleCount = 0;

		// Check if resource limit has been reached.
		if (resourceCount >= poolSize) {
			// Look for oldest available (inactive, but not expired) resource.
			for (auto it = resources.begin(); it != resources.end(); ++it) {
				ResourceInfo& resource = it->second;

				// Check if the resource is unused but contains data.
				if (!resource.isActive() && !resource.isExpired()) {
					// Create a new resource using the old resource's data pointer.
					newResource.data = std::move(resource.data);

					// Mark old resource as expired by setting its data pointer to null.
					resource.data = nullptr;

					// Decrement resource counter.
					resourceCount--;

					// Delete resource entry if there are no more references to it.
					if (resource.handleCount == 0) {
						resources.erase(it);
					}

					break;
				}
			}
		}

		// No available resource found?
		if (newResource.data == nullptr) {
			// Create fresh resource object.
			newResource.data = newFunc();

			// Increment resource counter.
			resourceCount++;
		}

		// Insert resource into resource map.
		resources.insert(std::move(std::make_pair(newID, std::move(newResource))));

		// Return handle to newly created resource.
		return Handle(this, newID);
	}

private:

	struct ResourceInfo {
		ResourceInfo() :
			data(nullptr),
			active(true),
			handleCount(0)
		{
		}
		
		ResourceInfo(ResourceInfo && resInfo) :
			data(std::move(resInfo.data)),
			active(std::move(resInfo.active)),
			handleCount(std::move(resInfo.handleCount))
		{
		}
		
		ResourceInfo & operator=(ResourceInfo && resInfo)
		{
			data = std::move(resInfo.data);
			active = std::move(resInfo.active);
			handleCount = std::move(resInfo.handleCount);
			
			return *this;
		}
		
		bool isActive() const {
			return active && handleCount != 0;
		}

		bool isExpired() const {
			return data == nullptr;
		}

		std::unique_ptr<ResourceType> data;
		bool active;
		unsigned int handleCount;
	};

	const ResourceInfo& getResourceInfo(ID id) const {
		auto entry = resources.find(id);

		if (entry == resources.end()) {
			throw std::out_of_range("Invalid resource ID requested");
		}

		return entry->second;
	}

	void cleanUp(typename std::map<ID, ResourceInfo>::iterator entry) {
		if (entry == resources.end()) {
			return;
		}

		ResourceInfo& resource = entry->second;

		// Check if there are too many resource, the resource is unused and contains data.
		if (resourceCount > poolSize && !resource.isActive() && !resource.isExpired()) {
			// Delete the resource's data by setting its pointer to null.
			resource.data = nullptr;

			// Decrement resource counter.
			resourceCount--;
		}

		// Check if the resource has no data and no open handles.
		if (resource.data == nullptr && resource.handleCount == 0) {
			// Delete the resource's pool entry.
			resources.erase(entry);
		}
	}

	void cleanUpAll() {
		for (auto it = resources.begin(); it != resources.end(); ++it) {
			cleanUp(it);
		}
	}

	void incrementHandleCount(ID id) {
		auto entry = resources.find(id);

		if (entry == resources.end()) {
			return;
		}

		entry->second.handleCount++;
	}

	void decrementHandleCount(ID id) {
		auto entry = resources.find(id);

		if (entry == resources.end()) {
			return;
		}

		entry->second.handleCount--;

		if (entry->second.handleCount == 0) {
			cleanUp(entry);
		}
	}

	void setResourceActive(ID id, bool active) {
		auto entry = resources.find(id);

		if (entry == resources.end()) {
			return;
		}

		entry->second.active = active;

		if (!active) {
			cleanUp(entry);
		}
	}

	unsigned int poolSize;
	unsigned int resourceCount;
	ID currentID;

	std::function<std::unique_ptr<ResourceType>()> newFunc;

	std::map<ID, ResourceInfo> resources;

	friend class Handle;
};

#endif
