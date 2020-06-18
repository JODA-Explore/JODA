//
// Created by Nico Schäfer on 10/08/18.
//

#ifndef JODA_ALLOCATOR_H
#define JODA_ALLOCATOR_H

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/fwd.h>
#include <rapidjson/istreamwrapper.h>

/*
 * Here the rapidjson types used by the project are defined.
 * This enables rapid change of allocator or character types
 */

typedef rapidjson::CrtAllocator RJBaseAlloc;
typedef rapidjson::MemoryPoolAllocator<RJBaseAlloc> RJMemoryPoolAlloc;
typedef rapidjson::GenericDocument<rapidjson::UTF8<char>, RJMemoryPoolAlloc, RJBaseAlloc> RJDocument;
typedef rapidjson::GenericValue<rapidjson::UTF8<char>, RJMemoryPoolAlloc> RJValue;
typedef rapidjson::GenericPointer<RJValue, RJBaseAlloc> RJPointer;


#endif //JODA_RJFwd_H
