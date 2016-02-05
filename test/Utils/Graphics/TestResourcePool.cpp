#include <gtest/gtest.h>
#include <Utils/Graphics/ResourcePool.hpp>
#include <vector>

/**
 * Test resource that records all instance creations and destructions.
 */
class DummyResource {
public:

	struct Event {
		enum Type {
			CREATED, DESTROYED
		};

		Event(Type type, unsigned int id) :
			type(type),
			id(id) {
		}

		bool operator==(const Event& event) const {
			return type == event.type && id == event.id;
		}

		Type type;
		unsigned int id;
	};

	DummyResource() {
		id = counter++;
		events.push_back(Event(Event::CREATED, id));
	}

	~DummyResource() {
		events.push_back(Event(Event::DESTROYED, id));
	}

	static void reset() {
		counter = 0;
		events.clear();
	}

	static unsigned int counter;
	static std::vector<Event> events;

	unsigned int id;
};

unsigned int DummyResource::counter = 0;
std::vector<DummyResource::Event> DummyResource::events = std::vector<DummyResource::Event>();

TEST(TestResourcePool, CreateEmptyPool) {
	{
		DummyResource::reset();
		ResourcePool<DummyResource> pool;
		pool.setPoolSize(5);
	}

	EXPECT_EQ(0, DummyResource::events.size());
}

TEST(TestResourcePool, CreateSingleResource) {
	{
		DummyResource::reset();
		ResourcePool<DummyResource> pool;
		pool.setPoolSize(5);

		ResourcePool<DummyResource>::Handle handle1 = pool.acquire();

		EXPECT_EQ(1, DummyResource::events.size());

		EXPECT_EQ(DummyResource::Event(DummyResource::Event::CREATED, 0), DummyResource::events[0]);
	}

	EXPECT_EQ(2, DummyResource::events.size());

	EXPECT_EQ(DummyResource::Event(DummyResource::Event::DESTROYED, 0), DummyResource::events[1]);
}

TEST(TestResourcePool, PoolOverflow) {
	DummyResource::reset();
	ResourcePool<DummyResource> pool;
	pool.setPoolSize(5);

	ResourcePool<DummyResource>::Handle handles[6];

	for (std::size_t i = 0; i < 6; ++i) {
		handles[i] = pool.acquire();
	}

	EXPECT_EQ(6, DummyResource::events.size());

	for (std::size_t i = 0; i < 6; ++i) {
		EXPECT_EQ(DummyResource::Event(DummyResource::Event::CREATED, i), DummyResource::events[i]);
	}
}

TEST(TestResourcePool, HandleManualRelease) {
	DummyResource::reset();
	ResourcePool<DummyResource> pool;
	pool.setPoolSize(5);

	ResourcePool<DummyResource>::Handle handle = pool.acquire();

	handle.setActive(false);

	ResourcePool<DummyResource>::Handle handles[5];

	for (std::size_t i = 0; i < 5; ++i) {
		handles[i] = pool.acquire();
	}

	EXPECT_EQ(0, handles[4].getResource().id);
	EXPECT_TRUE(handle.isExpired());
}

TEST(TestResourcePool, HandleAutoRelease) {
	DummyResource::reset();
	ResourcePool<DummyResource> pool;
	pool.setPoolSize(5);

	{
		ResourcePool<DummyResource>::Handle handle = pool.acquire();
	}

	ResourcePool<DummyResource>::Handle handles[5];

	for (std::size_t i = 0; i < 5; ++i) {
		handles[i] = pool.acquire();
	}

	EXPECT_EQ(0, handles[4].getResource().id);
}
