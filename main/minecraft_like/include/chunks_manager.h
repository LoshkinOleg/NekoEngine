
#include "engine/component.h"

namespace neko {

struct Chunk {
    bool init = false; 
};


class ChunksManager : public neko::ComponentManager<Chunk, ComponentType::OTHER_TYPE>
{
public:
    ChunksManager(EntityManager& entityManager);

    Index AddComponent(Entity entity) override;

    void DestroyComponent(Entity entity) override;

    void SetComponent(Entity entity, const Chunk& component) override;

private:
};
}
