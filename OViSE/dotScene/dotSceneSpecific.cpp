#ifndef dotSceneSpecific_h_
#define dotSceneSpecific_h_
#include "dotSceneSpecific.h"
#endif

dotSceneObjects::dotSceneSpecific::dotSceneSpecific() : dotSceneObject(entity) { } /// emergency constructor for a empty constructions.
dotSceneObjects::dotSceneSpecific::dotSceneSpecific(dotSceneElementTags tag) : dotSceneObject(tag) { }
// In C++ leider nicht m�glich (vererbt) //dotSceneObjects::dotSceneSpecific::dotSceneSpecific(std::string ElenentName) : dotSceneObject(ElenentName) { }
//dotSceneObjects::dotSceneSpecific::dotSceneSpecific(dotSceneSpecific obj) : dotSceneObject(obj) { }
dotSceneObjects::dotSceneSpecific::~dotSceneSpecific(void) { }
