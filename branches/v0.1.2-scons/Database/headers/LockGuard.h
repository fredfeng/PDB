#ifndef LOCK_GUARD_H_
#define LOCK_GUARD_H_

#include <pthread.h>


// simple RAII-style helper to hold a lock for the duration of its own
// lifetime, typically determined by some scoped block; similar to
// std::lock_guard, but for pthread_mutex_t rather than std::mutex

class LockGuard {
 public:
	// lock on construction
	explicit LockGuard (pthread_mutex_t &mutex)
		: mutex (mutex) {
		pthread_mutex_lock (&mutex);
	}

	// unlock on destruction
	~LockGuard () {
		pthread_mutex_unlock (&mutex);
	}

	// forbidden, to avoid double-unlock bugs
	LockGuard (const LockGuard &) = delete;
	LockGuard & operator = (const LockGuard &) = delete;

 private:
	// underlying system mutex to manage
	pthread_mutex_t &mutex;
};


#endif	// !LOCK_GUARD_H_
