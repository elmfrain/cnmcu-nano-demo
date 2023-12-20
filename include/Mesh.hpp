#pragma once

#include "MeshBuilder.hpp"
#include "GLInclude.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>

namespace em
{
    class Mesh
    {
    public:
        typedef std::shared_ptr<Mesh> Ptr;
    
        static std::vector<Ptr> load(const char* filename);
        Mesh(const Mesh& copy) = delete;
    
        Mesh();
        ~Mesh();
    
        uint32_t getTextureHandle() const;
    
        void putMeshArrays(MeshBuilder& meshBuilder) const;
        void putMeshElements(MeshBuilder& meshBuilder) const;
    
        void makeRenderable(VertexFormat vtxFmt);
        void render(int mode) const;
        void renderInstanced(int mode, int instances) const;
    
        const glm::vec3* getPositions() const;
        const glm::vec2* getUVs() const;
        const glm::vec3* getNormals() const;
        const glm::vec4* getColors() const;
        const uint32_t* getIndicies() const;
    
        size_t numVerticies() const;
        size_t numIndicies() const;
        const char* getName() const;
    private:
        std::string m_name;
    
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec2> m_uvs;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec4> m_colors;
        std::vector<uint32_t> m_indicies;
        size_t m_numVerticies;
        size_t m_numIndicies;
    
        bool m_isRenderable;
        mutable bool m_hasAdvisedAboutNotBeingRenderableYet;
        GLuint m_glVAO;
        GLuint m_glVBO;
        GLuint m_glEBO;
        GLuint m_glTexture;
    
        void putVertex(MeshBuilder& meshBuilder, const VertexFormat& vtxFmt, uint32_t vertexID) const;
    };
}