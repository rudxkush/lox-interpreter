#pragma once

#include <vector>
#include <memory>
#include "../common/value.h"

class GarbageCollector {
private:
    static const size_t GC_HEAP_GROW_FACTOR = 2;
    
    std::vector<std::shared_ptr<LoxObject>> objects;
    size_t bytesAllocated;
    size_t nextGC;
    
    std::vector<Value*> grayStack;
    
    void markRoots();
    void traceReferences();
    void blackenObject(std::shared_ptr<LoxObject> object);
    void markValue(Value value);
    void markObject(std::shared_ptr<LoxObject> object);
    void sweep();
    void freeObjects();

public:
    GarbageCollector();
    ~GarbageCollector();
    
    void collectGarbage();
    void* allocate(size_t size);
    void* reallocate(void* pointer, size_t oldSize, size_t newSize);
    void free(void* pointer, size_t size);
    
    // Object tracking
    void addObject(std::shared_ptr<LoxObject> object);
    void removeObject(std::shared_ptr<LoxObject> object);
    
    // Statistics
    size_t getBytesAllocated() const { return bytesAllocated; }
    size_t getNextGC() const { return nextGC; }
    size_t getObjectCount() const { return objects.size(); }
    
    // Debugging
    void printStats();
    void enableStressGC(bool enable);
    
private:
    bool stressGC = false;
};