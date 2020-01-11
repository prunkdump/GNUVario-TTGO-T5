#ifndef __RINGBUFFER_H
#define __RINGBUFFER_H

/**
 * Home: https://github.com/rickkas7/SerialBufferRK
 * License: MIT
 */

/**
 * @brief Thread and interrupt-safe (with caveats) circular buffer (ring buffer) class
 *
 * This class assumes a single reader thread and a single writer thread (or interrupt). For example, it
 * works great if you read out of loop() and write from a single interrupt handler. It is not safe for
 * multiple reader or multiple writer use cases!
 *
 * Assumption: Writing a size_t value is atomic. It definitely is safe on ARM processors.
 */
template <class T>
class RingBuffer {
public:
	/**
	 * @brief Construct a buffer of size elements of T
	 *
	 * @param elems Pointer to a buffer of size elements of type T
	 *
	 * @param size Number of elements
	 */
	RingBuffer(T *elems, size_t size) : elems(elems), size(size) {};

	/**
	 * @brief Destructor
	 */
	~RingBuffer() {};

	/**
	 * @brief Returns the number of elements that can be read right now (0 = nothing can be read right now)
	 *
	 * This is mainly for informational purposes. It's more efficient to call preRead() and check for a non-NULL
	 * return value than it is to call availableForRead().
	 */
	size_t availableForRead() const {
		return (size + head - tail) % size;
	}

	/**
	 * @brief Non-copy version of read. Returns a pointer to the data to be read next or NULL if there is no data right now.
	 *
	 * If preRead() returns a non-null value you must call postRead() to consume the data, otherwise the next time
	 * you call preRead() you'll get the same data back. Don't call postRead() if you get NULL back from preRead()!
	 *
	 * It's OK to not call postRead() if you're doing a peek at the data - look at the data that will be read without
	 * removing it.
	 */
	T *preRead() const {
		if (head == tail) {
			return NULL;
		}
		else {
			return &elems[tail];
		}
	}

	/**
	 * @brief Indicates that you have finished reading the data in the pointer returned by preRead() and it can be reused.
	 *
	 * Only call postRead() if preRead() returned a non-null value!
	 */
	void postRead() {
		size_t next = (tail + 1) % size;
		tail = next;
	}

	/**
	 * @brief Read with copy. You can use this instead of preRead() and postRead().
	 *
	 * @param elem Filled in with the data that was read. Left unchanged if there's no data to be read.
	 *
	 * @return Returns true if an element was copied or false if there was no data to read.
	 */
	bool read(T *elem) {
		T *src = preRead();
		if (src != NULL) {
			*elem = *src;
			postRead();
			return true;
		}
		else {
			return false;
		}
	}

	/**
	 * @brief Clear outstanding entries, called from the read thread
	 */
	void readClear() {
		tail = head;
	}

	/**
	 * @brief Non-copy version of write. Returns a pointer to the buffer to write to or NULL if there is no space.
	 *
	 * If preWrite() returns a non-null value you must call postWrite() to commit the data, otherwise the data
	 * will not be saved. Don't call postWrite() if you get NULL back from preWrite()!
	 */
	T *preWrite() const {
		size_t next = (head + 1) % size;
		if (next == tail) {
			// No space to write
			return NULL;
		}
		return &elems[head];
	}

	/**
	 * @brief Commits the write. Only call if preWrite() returned a non-NULL value.
	 */
	void postWrite() {
		size_t next = (head + 1) % size;
		head = next;
	}

	/**
	 * @brief Write with copy. You can use this instead of preWrite() and postWrite(). elem is copied.
	 *
	 * @return Returns true if the operation succeeded (there was space in the buffer).
	 */
	bool write(const T *elem) {
		T *dst = preWrite();
		if (dst != NULL) {
			*dst = *elem;
			postWrite();
			return true;
		}
		else {
			// No room to write
			return false;
		}
	}

private:
	T *elems;
	size_t size;
	volatile size_t head = 0;
	volatile size_t tail = 0;
};



#endif /* __RINGBUFFER_H */
