/**
 * @file    queue.h
 * @brief   This file defines macros used to generate type definitions and
 *          methods for statically allocated generic queues in C.
 *
 * @author  Jawad Kabir
 */

#ifndef _QUEUE_H
#define _QUEUE_H

#include <stdbool.h>
#include <string.h>

/**
 * @brief Generic static queue type definition.
 *
 * @param T     Type of contained element
 * @param _size Size of statically allocated array of elements
 */
#define QUEUE_T(T, _size) \
    struct {                                                                    \
        size_t MAX_SIZE;                                                  		\
        size_t size;                                                            \
        size_t front;                                                           \
        size_t back;                                                            \
        T data[_size];                                                          \
    }


/**
 * @brief Queue object with initial values.
 *
 * @param _size Maximum size of the queue
 */
#define QUEUE_INIT(_size) \
    { .MAX_SIZE = _size, .front = 0, .back = 0, .size = 0 }


/**
 * @brief Check if queue is empty.
 *
 * @param queue Pointer to queue.
 *
 * @retval true: Queue is empty.
 * @retval false: Queue is non-empty.
 */
#define QUEUE_ISEMPTY(queue) \
    (((queue)->size == 0) ? true : false)


/**
 * @brief Gets element from index in queue.
 *
 * @param queue Pointer to queue.
 * @param index Index relative to front of queue from which to retrieve element.
 *
 * @return Element from index in queue.
 */
#define QUEUE_ELEM(queue, index) \
    ((queue)->data[(queue)->front + (index) % (queue)->MAX_SIZE])


/**
 * @brief Get the element at the front of the queue.
 *
 * @param queue Pointer to queue.
 *
 * @return Element from front of the queue.
 */
#define QUEUE_FRONT(queue) \
    ((queue)->data[(queue)->front])


/**
 * @brief Get the element at the back of the queue.
 *
 * @param queue Pointer to queue.
 *
 * @return Element from back of the queue.
 */
#define QUEUE_BACK(queue) \
    ((queue)->data[(queue)->back])


/**
 * @brief Add element to the back of the queue.
 *
 * @param queue Pointer to queue.
 * @param elem Pointer to element that will be pushed.
 */
#define QUEUE_PUSH(queue, elem) \
    if ((queue)->size < (queue)->MAX_SIZE) {                                    \
        memcpy(&((queue)->data[(queue)->back]), elem, sizeof(*elem));           \
        (queue)->size++; (queue)->back++;                                       \
        (queue)->back %= (queue)->MAX_SIZE;                                     \
    }


/**
 * @brief Remove element from the front of the queue.
 *
 * @param queue Pointer to queue.
 */
#define QUEUE_POP(queue) \
    if ((queue)->size > 0) {                                                    \
        (queue)->size--; (queue)->front++;                                      \
        (queue)->front %= (queue)->MAX_SIZE;                                    \
    }


#endif
