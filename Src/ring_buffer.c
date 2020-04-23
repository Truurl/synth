/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include "ring_buffer.h"


bool RingBuffer_Init(RingBuffer *ringBuffer, char *dataBuffer, size_t dataBufferSize) 
{
	assert(ringBuffer);
	assert(dataBuffer);
	assert(dataBufferSize > 0);
	
	if (dataBufferSize > 0) {
	  ringBuffer->head = 0;
	  ringBuffer->tail = 0;
	  ringBuffer->length = 0;
	  ringBuffer->capacity = dataBufferSize;
	  ringBuffer->buffer = dataBuffer;
	  return true;
	}
	return false;
}

bool RingBuffer_Clear(RingBuffer *ringBuffer) {

    assert(ringBuffer);

    if (ringBuffer->tail != 0 || ringBuffer->head != 0 || ringBuffer->length != 0) {
        ringBuffer->length = 0;
        ringBuffer->head = 0;
        ringBuffer->tail = 0;
        return true;
    }
    return false;
}

bool RingBuffer_IsEmpty(const RingBuffer *ringBuffer)
{
  assert(ringBuffer);

  if(ringBuffer->length == 0) {
      return true;
  }
  return false;

}

size_t RingBuffer_GetLen(const RingBuffer *ringBuffer)
{
	assert(ringBuffer);

	return ringBuffer->length * sizeof(char);
	
}

size_t RingBuffer_GetCapacity(const RingBuffer *ringBuffer)
{
	assert(ringBuffer);

	return  ringBuffer->capacity;

}


bool RingBuffer_PutChar(RingBuffer *ringBuffer, char c) {
    assert(ringBuffer);

    if (ringBuffer->length != ringBuffer->capacity) {

        ringBuffer->buffer[ringBuffer->head] = c;
        ringBuffer->length++;
        ringBuffer->head = ++ringBuffer->head % ringBuffer->capacity;

        return true;
    }
    return false;
}

bool RingBuffer_GetChar(RingBuffer *ringBuffer, char *c)
{
	assert(ringBuffer);
	assert(c);

  if ((ringBuffer->length != 0)) {

      *c = ringBuffer->buffer[ringBuffer->tail];
      ringBuffer->length--;
      ringBuffer->tail = ++ringBuffer->tail % ringBuffer->capacity;
      return true;

  }
  return false;
  
}
