/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <map>
#include <vector>

struct RGBA {
    uint8_t RED;
    uint8_t GREEN;
    uint8_t BLUE;
    uint8_t ALPHA;
};



class vertexHead {
public:
    bool isEnabled;
    AttributeType type;
    uint64_t stride;
    uint64_t offset;
    BufferID buffer;

    vertexHead();
    ~vertexHead();
    void enable();
    void disable();
    void set(AttributeType type, uint64_t stride, uint64_t offset, BufferID buffer);

};



class vertexPuller {
public:
    IndexType indexType;
    BufferID indexBufferID;

    std::map<ObjectID, vertexHead> vertexHeadsMap;

    vertexPuller();
    ~vertexPuller();
    void setIndexing(IndexType type, BufferID buffer);
};



class Program {
public:
    VertexShader vs;
    FragmentShader fs;
    std::map<uint32_t, AttributeType> attributes;
    Uniforms uniforms;


    Program();
    ~Program();
    void setShaders(VertexShader vs, FragmentShader fs);
};



class FrameBuffer {
public:
    uint32_t width;
    uint32_t height;

    std::vector<RGBA> colorBuffer;
    std::vector<float> depthBuffer;
    

    FrameBuffer();
    FrameBuffer(uint32_t width, uint32_t height);
    ~FrameBuffer();

    void resize(uint32_t width, uint32_t height);
};

/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    InVertex vertexPullerMethod(uint32_t vertexNumber);
    std::vector<std::vector<OutVertex>> clip(std::vector<OutVertex> primitive);
    std::vector<OutVertex> perspectiveDivision(std::vector<OutVertex> inPrimitive);
    std::vector<OutVertex> viewpointTransformation(std::vector<OutVertex> inPrimitive);
    void rasterize(std::vector<OutVertex> inPrimitive);
    float inTriangle(glm::vec4 v1, glm::vec4 v2, float px, float py);
    float triangleArea(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3);
    float edgeLenght(glm::vec4 v1, glm::vec4 v2);
    float area(float a, float b, float c);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}


    std::map<BufferID, std::vector<unsigned char>> bufferMap;

    std::map<VertexPullerID, vertexPuller> vertexPullerMap;
    ObjectID bindedVertexPuller;

    std::map<ProgramID, Program> programMap;
    Program *activeProgram;

    FrameBuffer frameBuffer;
};


