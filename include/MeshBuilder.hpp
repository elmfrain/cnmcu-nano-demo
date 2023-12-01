#pragma once

#include <stdint.h>
#include <stack>
#include <vector>
#include <glm/glm.hpp>

#include <GLInclude.hpp>

// Vertex Attribute Members Masks
#define EMVF_ATTRB_USAGE_MASK      0xFF000000
#define EMVF_ATTRB_TYPE_SIZE_MASK  0x00F00000
#define EMVF_ATTRB_TYPE_MASK       0x00FF0000
#define EMVF_ATTRB_SIZE_MASK       0x0000FF00
#define EMVF_ATTRB_NORMALIZED_MASK 0x000000FF

// Vertex Attribute Usage Enum
#define EMVF_ATTRB_USAGE_POS       0x00000000 // Position, enum value 0
#define EMVF_ATTRB_USAGE_UV        0x01000000 // UV,       enum value 1
#define EMVF_ATTRB_USAGE_COLOR     0x02000000 // Color,    enum value 2
#define EMVF_ATTRB_USAGE_NORMAL    0x03000000 // Normal,   enum value 3
#define EMVF_ATTRB_USAGE_TEXID     0x04000000 // Texid,    enum value 4
#define EMVF_ATTRB_USAGE_OTHER     0x05000000 // Other,    enum value 5

// Vertex Attribute Type Enum
#define EMVF_ATTRB_TYPE_FLOAT      0x00400000 // Float           , size 4, enum value 0
#define EMVF_ATTRB_TYPE_INT        0x00410000 // Integer         , size 4, enum value 1
#define EMVF_ATTRB_TYPE_UINT       0x00420000 // Unsigned Integer, size 4, enum value 2
#define EMVF_ATTRB_TYPE_SHORT      0x00230000 // Short           , size 2, enum value 3
#define EMVF_ATTRB_TYPE_USHORT     0x00240000 // Unsigned Short  , size 2, enum value 4
#define EMVF_ATTRB_TYPE_BYTE       0x00150000 // Byte            , size 1, enum value 5
#define EMVF_ATTRB_TYPE_UBYTE      0x00160000 // Unsigned Byte   , size 1, enum value 6
#define EMVF_ATTRB_TYPE_DOUBLE     0x00870000 // Double          , size 8, enum value 7

// Vertex Attribute Size
#define EMVF_ATTRB_SIZE(x) ((uint32_t) x) << 8

// Vertex Normalized
#define EMVF_ATTRB_NORMALIZED_TRUE 1
#define EMVF_ATTRB_NORMALIZED_FALSE 0

namespace em 
{
    struct VertexAttribute
    {
        uint32_t data;

        inline uint32_t getUsage() const    { return data & EMVF_ATTRB_USAGE_MASK; }
        inline uint32_t getSize() const     { return (data & EMVF_ATTRB_SIZE_MASK) >> 8; }
        inline uint32_t getType() const     { return data & EMVF_ATTRB_TYPE_MASK; }
        inline uint32_t getTypeSize() const { return (data & EMVF_ATTRB_TYPE_SIZE_MASK) >> 20; }
        inline uint32_t getAPIType() const
        { 
            uint32_t type = getType();

            switch (type)
            {
            case EMVF_ATTRB_TYPE_FLOAT:
                return GL_FLOAT;
            case EMVF_ATTRB_TYPE_INT:
                return GL_INT;
            case EMVF_ATTRB_TYPE_UINT:
                return GL_UNSIGNED_INT;
            case EMVF_ATTRB_TYPE_SHORT:
                return GL_SHORT;
            case EMVF_ATTRB_TYPE_USHORT:
                return GL_UNSIGNED_SHORT;
            case EMVF_ATTRB_TYPE_BYTE:
                return GL_BYTE;
            case EMVF_ATTRB_TYPE_UBYTE:
                return GL_UNSIGNED_BYTE;
            default:
                return GL_FLOAT;
            }
        }
        inline int numBytes() const
        {
            uint32_t size = getSize();
            uint32_t typeSize = getTypeSize();

            return size * typeSize;
        }
        inline bool isNormalized() const { return data & EMVF_ATTRB_NORMALIZED_MASK; }
    };

    struct VertexFormat
    {
        int size = 0;
        VertexAttribute attributes[32];

        void apply() const;
        void unapply() const;

        VertexAttribute& operator[](int index)
        {
            return attributes[index];
        }

        inline int vertexNumBytes() const
        {
            int numBytes = 0;

            for(int i = 0; i < size; i++)
            {
                numBytes += attributes[i].numBytes();
            }

            return numBytes;
        }
    };

    class MeshBuilder
    {
    public:
        MeshBuilder(const VertexFormat& vtxFmt);
        MeshBuilder(const MeshBuilder&) = delete;
        ~MeshBuilder();

        float defaultNormal[3];
        float defaultUV[2];
        float defaultColor[4];

        void reset();
        void drawArrays(GLenum mode);
        void drawElements(GLenum mode);
        void drawArraysInstanced(GLenum mode, int instances);
        void drawElemenentsInstanced(GLenum mode, int instances);

        MeshBuilder& position(float x, float y, float z);
        MeshBuilder& normal(float x, float y, float z);
        MeshBuilder& normalDefault();
        MeshBuilder& uv(float u, float v);
        MeshBuilder& uvDefault();
        MeshBuilder& colorRGB(float r, float g, float b);
        MeshBuilder& colorRGBA(float r, float g, float b, float a);
        MeshBuilder& colorDefault();
        MeshBuilder& texid(uint32_t texid);

        MeshBuilder& vertex(void* notUsed, ...);
        MeshBuilder& index(size_t numIndicies, ...);
        MeshBuilder& indexv(size_t numIndicies, const uint32_t* indicies);

        const VertexFormat& getVertexFormat() const;
        const uint8_t* getVertexBuffer(size_t* getNumBytes) const;
        const uint32_t* getIndexBuffer(size_t* getNumBytes) const;

        glm::mat4& pushMatrix();
        glm::mat4& popMatrix();
        glm::mat4& resetMatrixStack();
        glm::mat4& getModelView();
    private:
        VertexFormat m_vertexFormat;

        // Modelview variables
        std::stack<glm::mat4> m_modelViewStack;
        glm::mat3 m_modelView3x3;

        // OpenGL variables
        GLuint m_glVBO;
        GLuint m_glEBO;
        GLuint m_glVAO;
        size_t m_glVertexBufferSize;
        size_t m_glElementBufferSize;

        // Book Keeping
        bool m_isRenderable;
        size_t m_numVerticies;
        size_t m_numIndicies;

        // Buffers
        std::vector<uint8_t> m_vertexDataBuffer;
        std::vector<uint8_t> m_indexDataBuffer;

        void initForRendering();
        void pushVertexData(size_t size, const void* data);
        void pushIndexData(size_t size, const void* data);
        void get3x3ModelView();
    };
}