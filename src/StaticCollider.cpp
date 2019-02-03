#include "..\include\StaticCollider.h"

StaticCollider::StaticCollider() {
   texture = NULL;
}

StaticCollider::~StaticCollider() {
   delete texture;
}
