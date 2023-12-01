#include <VisualizerScene.hpp>

#include <GLInclude.hpp>
#include <MeshBuilder.hpp>

using namespace em;

VisualizerScene::VisualizerScene()
{
    mainCamera.setName("main-camera");
}

VisualizerScene::~VisualizerScene()
{
}

void VisualizerScene::update(float dt)
{
    
}

void VisualizerScene::draw()
{
    static MeshBuilder* meshBuilder = nullptr;

    if(!meshBuilder)
    {
        VertexFormat vtxFmt;

        vtxFmt.size = 1;
        vtxFmt[0].data = EMVF_ATTRB_USAGE_POS |
                         EMVF_ATTRB_TYPE_FLOAT |
                         EMVF_ATTRB_SIZE(3) |
                         EMVF_ATTRB_NORMALIZED_FALSE;

        meshBuilder = new MeshBuilder(vtxFmt);

        meshBuilder->index(6, 0, 1, 2, 0, 2, 3);
        meshBuilder->
        vertex(nullptr, -0.5f, -0.5f, 0.0f).
        vertex(nullptr,  0.5f, -0.5f, 0.0f).
        vertex(nullptr,  0.5f,  0.5f, 0.0f).
        vertex(nullptr, -0.5f,  0.5f, 0.0f);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    meshBuilder->drawElements(GL_TRIANGLES);
}