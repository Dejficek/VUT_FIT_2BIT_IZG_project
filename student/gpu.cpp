/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <algorithm>


vertexHead::vertexHead() {
    this->isEnabled = false;
}

vertexHead::~vertexHead() {

}

void vertexHead::enable() {
    this->isEnabled = true;
}

void vertexHead::disable() {
    this->isEnabled = false;
}

void vertexHead::set(AttributeType type, uint64_t stride, uint64_t offset, BufferID buffer) {
    this->type = type;
    this->stride = stride;
    this->offset = offset;
    this->buffer = buffer;
}

vertexPuller:: vertexPuller() {

    for (ObjectID headID = 0; headID < maxAttributes; headID++) {
        vertexHead head;
        vertexHeadsMap.insert({ headID, head });
        this->indexBufferID = -1;
    }
}

vertexPuller:: ~vertexPuller() {
    for (ObjectID headID = 0; headID < maxAttributes; headID++) {
        vertexHeadsMap[headID].~vertexHead();
        vertexHeadsMap.erase(headID);
    }
}

void vertexPuller::setIndexing(IndexType type, BufferID buffer) {
    this->indexType = type;
    this->indexBufferID = buffer;
}

Program::Program() {
    ;
}

Program::~Program() {
    ;
}

void Program::setShaders(VertexShader vs, FragmentShader fs) {
    this->vs = vs;
    this->fs = fs;
}

FrameBuffer::FrameBuffer() {

}
FrameBuffer::FrameBuffer(uint32_t width, uint32_t height) {
    if (width > 0 && height > 0) {
        FrameBuffer::resize(width, height);
    }
}

FrameBuffer::~FrameBuffer() {

}

void FrameBuffer::resize(uint32_t width, uint32_t height) {
    if (width > 0 && height > 0) {
        this->height = height;
        this->width = width;

        RGBA color;
        color.RED = 0;
        color.GREEN = 0;
        color.BLUE = 0;
        color.ALPHA = 0;

        this->colorBuffer.assign(height * width, color);
        this->depthBuffer.assign(height * width, 1.0);
    }
}




/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty

}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
    
    
    BufferID newID = 0;
    while (isBuffer(newID)) {
        newID++;
    }

    bufferMap.insert({ newID, std::vector<unsigned char>(size) });

    return newID;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.

    bufferMap[buffer].clear();
    bufferMap.erase(buffer);
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>

    

    if (data != NULL) {
        if (uint64_t(bufferMap[buffer].size()) + offset + size > uint64_t(bufferMap[buffer].capacity())) {
            bufferMap[buffer].resize(uint64_t(bufferMap[buffer].size()) + offset + size);
        }

        //std::copy(&data, &data + size, bufferMap[buffer][offset]);
        //memcpy(bufferMap[buffer][offset], data, size * sizeof(void*));

        for (int i = 0; i < size; i++) {
            bufferMap[buffer][offset + i] = *((unsigned char*)data + i);
        }
    }
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>

    unsigned char* tmpData = (unsigned char*)data;
    for (int i = 0; i < size; i++) {
        *(tmpData + i) = bufferMap[buffer][i + offset];
    }
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>

    if (buffer == emptyID) return false;
    return (bufferMap.find(buffer) == bufferMap.end()) ? false : true;
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.


    VertexPullerID newID = 0;
    while (isVertexPuller(newID)) {
        newID++;
    }
    vertexPuller puller;
    vertexPullerMap.insert({ newID, puller });

    return newID;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>

    if (isVertexPuller(vao)) {
        vertexPullerMap.erase(vao);
    }
    

}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
   
    if (isVertexPuller(vao)) {
        if (head < maxAttributes) {
            vertexPullerMap[vao].vertexHeadsMap[head].set(type, stride, offset, buffer);
        }
    }
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>

    if (isVertexPuller(vao) && isBuffer(buffer)) {
        vertexPullerMap[vao].setIndexing(type, buffer);
    }
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
    if (isVertexPuller(vao)) {
        if (head < maxAttributes) {
            vertexPullerMap[vao].vertexHeadsMap[head].enable();
        }
    }
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
    if (isVertexPuller(vao)) {
        if (head < maxAttributes) {
            vertexPullerMap[vao].vertexHeadsMap[head].disable();
        }
    }

}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    if (isVertexPuller(vao)) {
        bindedVertexPuller = vao;
    }
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    bindedVertexPuller = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
    if (vao == emptyID) return false;
    return (vertexPullerMap.find(vao) == vertexPullerMap.end()) ? false : true;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
  //return emptyID;

    ProgramID newID = 0;
    while (isProgram(newID)) {
        newID++;
    }
    Program program;
    programMap.insert({ newID, program });

    return newID;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>

    if (isProgram(prg)) {
        programMap.erase(prg);
    }
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    programMap[prg].setShaders(vs, fs);
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
    if (isProgram(prg)) {
        if (attrib < maxAttributes) {
            programMap[prg].attributes[attrib] = type;
        }
    }

}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
    if (isProgram(prg)) {
        this->activeProgram = &programMap[prg];
    }
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
    if (prg == emptyID) return false;
    return (programMap.find(prg) == programMap.end()) ? false : true;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
    if (isProgram(prg)) {
        if (uniformId < maxUniforms) {
            programMap[prg].uniforms.uniform[uniformId].v1 = d;
        }
    }
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
    if (isProgram(prg)) {
        if (uniformId < maxUniforms) {
            programMap[prg].uniforms.uniform[uniformId].v2 = d;
        }
    }
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
    if (isProgram(prg)) {
        if (uniformId < maxUniforms) {
            programMap[prg].uniforms.uniform[uniformId].v3 = d;
        }
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
    if (isProgram(prg)) {
        if (uniformId < maxUniforms) {
            programMap[prg].uniforms.uniform[uniformId].v4 = d;
        }
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    if (isProgram(prg)) {
        if (uniformId < maxUniforms) {
            programMap[prg].uniforms.uniform[uniformId].m4 = d;
        }
    }
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
    FrameBuffer buffer = FrameBuffer(width, height);
    this->frameBuffer = buffer;
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.

}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
    this->frameBuffer.resize(width, height);
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>

    //return nullptr;
    return &frameBuffer.colorBuffer[0].RED;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
    return &frameBuffer.depthBuffer[0];
    //return nullptr;
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
    return this->frameBuffer.width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
    return this->frameBuffer.height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>

    uint32_t height = getFramebufferHeight();
    uint32_t width = getFramebufferWidth();
    uint32_t size = height * width;

    for (int i = 0; i < size; i++) {
        frameBuffer.colorBuffer[i].RED = r * 255;
        frameBuffer.colorBuffer[i].GREEN = g * 255;
        frameBuffer.colorBuffer[i].BLUE = b * 255;
        frameBuffer.colorBuffer[i].ALPHA = a * 255;
        frameBuffer.depthBuffer[i] = 1;
    }
}



void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>

    if (nofVertices % 3 != 0) {
        exit(1);
    }

    std::vector<OutVertex> primitive;
    std::vector<std::vector<OutVertex>> primitives;
    int counter = 0;

    for (uint32_t vertexNumber = 0; vertexNumber < nofVertices; vertexNumber++) {
        OutVertex outVertex;
        //std::vector<OutFragment> outFragments;
        
        InVertex vertex = vertexPullerMethod(vertexNumber);
        activeProgram->vs(outVertex, vertex, activeProgram->uniforms);

        primitive.push_back(outVertex);
        counter++;
        
        //primitive assembly
        if (counter == 3) {
            //primitives.push_back(primitive);

            primitives = clip(primitive);

            std::vector<OutVertex> primitiveToRasterize;
            /*for each (std::vector<OutVertex> primitive in primitives) {
                primitiveToRasterize = perspectiveDivision(primitive);
                primitiveToRasterize = viewpointTransformation(primitiveToRasterize);
                rasterize(primitiveToRasterize);

            }*/
            for (int i = 0; i < primitives.size(); i++) {
                primitiveToRasterize = perspectiveDivision(primitives[i]);
                primitiveToRasterize = viewpointTransformation(primitiveToRasterize);
                rasterize(primitiveToRasterize);
            }
            counter = 0;
        }
    }
}


//vertex pulling
InVertex GPU::vertexPullerMethod(uint32_t vertexNumber) {
    vertexPuller activeVertexPuller = vertexPullerMap[bindedVertexPuller];

    InVertex vertex;
    
    uint32_t offset;

    if (activeVertexPuller.indexBufferID != -1) {
        int index = 0;
        getBufferData(activeVertexPuller.indexBufferID, vertexNumber * (int)activeVertexPuller.indexType, (int)activeVertexPuller.indexType, &index);
        vertex.gl_VertexID = index;
    }
    else {
        vertex.gl_VertexID = vertexNumber;
    }

    for (ObjectID i = 0; i < maxAttributes; i++) {
        if (activeVertexPuller.vertexHeadsMap[i].isEnabled) {

            vertexHead currentHead = activeVertexPuller.vertexHeadsMap[i];
            
            uint32_t offset;

            offset = (currentHead.stride * vertex.gl_VertexID) + currentHead.offset;


            switch (activeVertexPuller.vertexHeadsMap[i].type) {
            case AttributeType::FLOAT:
                getBufferData(currentHead.buffer, offset, sizeof(float), &vertex.attributes[i].v1);
                break;
            case AttributeType::VEC2:
                getBufferData(currentHead.buffer, offset, sizeof(glm::vec2), &vertex.attributes[i].v2);
                break;
            case AttributeType::VEC3:
                getBufferData(currentHead.buffer, offset, sizeof(glm::vec3), &vertex.attributes[i].v3);
                break;
            case AttributeType::VEC4:
                getBufferData(currentHead.buffer, offset, sizeof(glm::vec4), &vertex.attributes[i].v4);
                break;
            }
        }
    }
    return vertex;
}


//clipping method
std::vector<std::vector<OutVertex>> GPU::clip(std::vector<OutVertex> primitive) {

    std::vector<std::vector<OutVertex>> out;
    std::vector<OutVertex> notVisible;
    std::vector<OutVertex> visible;

    std::vector<OutVertex> primitive1;
    std::vector<OutVertex> primitive2;
    
    /*for each (OutVertex vertex in primitive) {
        if (-vertex.gl_Position[3] > vertex.gl_Position[2]) {
            notVisible.push_back(vertex);
        }
        else {
            visible.push_back(vertex);
        }
    }*/
    for (int i = 0; i < primitive.size(); i++) {
        if (-primitive[i].gl_Position[3] > primitive[i].gl_Position[2]) {
            notVisible.push_back(primitive[i]);
        }
        else {
            visible.push_back(primitive[i]);
        }
    }

    switch (notVisible.size()) {
    case 0:
        // not clipping
        out.push_back(visible);
        break;
    case 1:
    {
        OutVertex notVisibleVertex = notVisible[0];

        //1
        OutVertex visibleVertex1 = visible[0];

        float t1 = (-notVisibleVertex.gl_Position[3] - notVisibleVertex.gl_Position[2]) /
            (visibleVertex1.gl_Position[3] - notVisibleVertex.gl_Position[3] +
                visibleVertex1.gl_Position[2] - notVisibleVertex.gl_Position[2]);

        OutVertex newVertex1;
        newVertex1.gl_Position[0] = notVisibleVertex.gl_Position[0] + t1 * (visibleVertex1.gl_Position[0] - notVisibleVertex.gl_Position[0]);
        newVertex1.gl_Position[1] = notVisibleVertex.gl_Position[1] + t1 * (visibleVertex1.gl_Position[1] - notVisibleVertex.gl_Position[1]);
        newVertex1.gl_Position[2] = notVisibleVertex.gl_Position[2] + t1 * (visibleVertex1.gl_Position[2] - notVisibleVertex.gl_Position[2]);
        newVertex1.gl_Position[3] = notVisibleVertex.gl_Position[3] + t1 * (visibleVertex1.gl_Position[3] - notVisibleVertex.gl_Position[3]);

        for (int i = 0; i < maxAttributes; i++) {
            newVertex1.attributes[i].v1 = notVisibleVertex.attributes[i].v1 + t1 * (visibleVertex1.attributes[i].v1 - notVisibleVertex.attributes[i].v1);
            newVertex1.attributes[i].v2 = notVisibleVertex.attributes[i].v2 + t1 * (visibleVertex1.attributes[i].v2 - notVisibleVertex.attributes[i].v2);
            newVertex1.attributes[i].v3 = notVisibleVertex.attributes[i].v3 + t1 * (visibleVertex1.attributes[i].v3 - notVisibleVertex.attributes[i].v3);
            newVertex1.attributes[i].v4 = notVisibleVertex.attributes[i].v4 + t1 * (visibleVertex1.attributes[i].v4 - notVisibleVertex.attributes[i].v4);
        }



        //2
        OutVertex visibleVertex2 = visible[1];

        float t2 = (-notVisibleVertex.gl_Position[3] - notVisibleVertex.gl_Position[2]) /
            (visibleVertex2.gl_Position[3] - notVisibleVertex.gl_Position[3] +
                visibleVertex2.gl_Position[2] - notVisibleVertex.gl_Position[2]);

        OutVertex newVertex2;
        newVertex2.gl_Position[0] = notVisibleVertex.gl_Position[0] + t2 * (visibleVertex2.gl_Position[0] - notVisibleVertex.gl_Position[0]);
        newVertex2.gl_Position[1] = notVisibleVertex.gl_Position[1] + t2 * (visibleVertex2.gl_Position[1] - notVisibleVertex.gl_Position[1]);
        newVertex2.gl_Position[2] = notVisibleVertex.gl_Position[2] + t2 * (visibleVertex2.gl_Position[2] - notVisibleVertex.gl_Position[2]);
        newVertex2.gl_Position[3] = notVisibleVertex.gl_Position[3] + t2 * (visibleVertex2.gl_Position[3] - notVisibleVertex.gl_Position[3]);

        for (int i = 0; i < maxAttributes; i++) {
            newVertex2.attributes[i].v1 = notVisibleVertex.attributes[i].v1 + t2 * (visibleVertex2.attributes[i].v1 - notVisibleVertex.attributes[i].v1);
            newVertex2.attributes[i].v2 = notVisibleVertex.attributes[i].v2 + t2 * (visibleVertex2.attributes[i].v2 - notVisibleVertex.attributes[i].v2);
            newVertex2.attributes[i].v3 = notVisibleVertex.attributes[i].v3 + t2 * (visibleVertex2.attributes[i].v3 - notVisibleVertex.attributes[i].v3);
            newVertex2.attributes[i].v4 = notVisibleVertex.attributes[i].v4 + t2 * (visibleVertex2.attributes[i].v4 - notVisibleVertex.attributes[i].v4);
        }

        //assembly
        
        primitive1.push_back(visibleVertex1);
        primitive1.push_back(visibleVertex2);
        primitive1.push_back(newVertex1);

        primitive2.push_back(visibleVertex2);
        primitive2.push_back(newVertex2);
        primitive2.push_back(newVertex1);

        out.push_back(primitive1);
        out.push_back(primitive2);
    }
        break;
    case 2:
    {
        OutVertex visibleVertex = visible[0];
        float t = 0;

        for (int i = 0; i < notVisible.size(); i++) {
            t = (-notVisible[i].gl_Position[3] - notVisible[i].gl_Position[2]) /
                (visibleVertex.gl_Position[3] - notVisible[i].gl_Position[3] +
                    visibleVertex.gl_Position[2] - notVisible[i].gl_Position[2]);

            OutVertex newVertex;
            newVertex.gl_Position[0] = notVisible[i].gl_Position[0] + t * (visibleVertex.gl_Position[0] - notVisible[i].gl_Position[0]);
            newVertex.gl_Position[1] = notVisible[i].gl_Position[1] + t * (visibleVertex.gl_Position[1] - notVisible[i].gl_Position[1]);
            newVertex.gl_Position[2] = notVisible[i].gl_Position[2] + t * (visibleVertex.gl_Position[2] - notVisible[i].gl_Position[2]);
            newVertex.gl_Position[3] = notVisible[i].gl_Position[3] + t * (visibleVertex.gl_Position[3] - notVisible[i].gl_Position[3]);

            for (uint32_t j = 0; j < maxAttributes; j++) {
                newVertex.attributes[i].v1 = notVisible[i].attributes[j].v1 + t * (visibleVertex.attributes[j].v1 - notVisible[i].attributes[j].v1);
                newVertex.attributes[i].v2 = notVisible[i].attributes[j].v2 + t * (visibleVertex.attributes[j].v2 - notVisible[i].attributes[j].v2);
                newVertex.attributes[i].v3 = notVisible[i].attributes[j].v3 + t * (visibleVertex.attributes[j].v3 - notVisible[i].attributes[j].v3);
                newVertex.attributes[i].v4 = notVisible[i].attributes[j].v4 + t * (visibleVertex.attributes[j].v4 - notVisible[i].attributes[j].v4);
            }
            visible.push_back(newVertex);
        }
        out.push_back(visible);
    }
        break;
    case 3:
        break;
    }
    return out;
}

//perspective division
std::vector<OutVertex> GPU::perspectiveDivision(std::vector<OutVertex> inPrimitive) {
    
    for (int i = 0; i < 3; i++) {
        inPrimitive[i].gl_Position[0] = inPrimitive[i].gl_Position[0] / inPrimitive[i].gl_Position[3];
        inPrimitive[i].gl_Position[1] = inPrimitive[i].gl_Position[1] / inPrimitive[i].gl_Position[3];
    }
    return inPrimitive;
}


//viewpoint transformation
std::vector<OutVertex> GPU::viewpointTransformation(std::vector<OutVertex> inPrimitive) {
    auto width = frameBuffer.width;
    auto height = frameBuffer.height;

    for (int i = 0; i < 3; i++) {
        inPrimitive[i].gl_Position[0] = inPrimitive[i].gl_Position[0] * (width / 2) + (width / 2);
        inPrimitive[i].gl_Position[1] = inPrimitive[i].gl_Position[1] * (height / 2) + (height / 2);
    }
    return inPrimitive;
}


//rasterization
void GPU::rasterize(std::vector<OutVertex> inPrimitive) {
    uint32_t size = getFramebufferHeight() * getFramebufferHeight();
    uint32_t width = getFramebufferWidth();
    uint32_t heigth = getFramebufferHeight();

    glm::vec4 v0 = inPrimitive[0].gl_Position;
    glm::vec4 v1 = inPrimitive[1].gl_Position;
    glm::vec4 v2 = inPrimitive[2].gl_Position;

    InFragment in;
    //std::vector<OutFragment> outFragments;

    auto low = std::min(std::min(v0[1], v1[1]), v2[1]);
    auto high = std::max(std::max(v0[1], v1[1]), v2[1]);
    auto left = std::min(std::min(v0[0], v1[0]), v2[0]);
    auto right = std::max(std::max(v0[0], v1[0]), v2[0]);

    for (int x = left; x < right; x++) {
        for (int y = low; y < high; y++) {
            RGBA color;

            if (inTriangle(v0, v1, x, y) <= 0 && inTriangle(v1, v2, x, y) <= 0 && inTriangle(v2, v0, x, y) <= 0) {
                //barycentric coordinates

                

                glm::vec4 point = glm::vec4(x + 0.5, y + 0.5, 0, 0);

                float ABCArea = area(edgeLenght(v0, v1), edgeLenght(v1, v2), edgeLenght(v2, v0));
                float CAPArea = area(edgeLenght(v2, v0), edgeLenght(v0, point), edgeLenght(point, v2));
                float ABPArea = area(edgeLenght(v0, v1), edgeLenght(v1, point), edgeLenght(point, v0));
                float BCPArea = area(edgeLenght(v1, v2), edgeLenght(v2, point), edgeLenght(point, v1));

                float v = CAPArea / ABCArea;
                float w = ABPArea / ABCArea;
                float u = BCPArea / ABCArea;

                
                float divisor = u / v0.w + v / v1.w + w / v2.w;

                float depth = (v0.z / v0.w * u / v0.w + v1.z / v1.w * v / v1.w + v2.z / v2.w * w / v2.w) / divisor;
                point.z = depth;

                for (int i = 0; i < maxAttributes; i++) {

                    switch (activeProgram->attributes[i]) {
                    case AttributeType::FLOAT:
                        in.attributes[i].v1 = ((inPrimitive[0].attributes[i].v1 * u / inPrimitive[0].gl_Position.w) +
                                               (inPrimitive[1].attributes[i].v1 * v / inPrimitive[1].gl_Position.w) +
                                               (inPrimitive[2].attributes[i].v1 * w / inPrimitive[2].gl_Position.w)) /
                                              ((u / inPrimitive[0].gl_Position.w) + (v / inPrimitive[1].gl_Position.w) + (w / inPrimitive[2].gl_Position.w));
                        break;
                    case AttributeType::VEC2:
                        in.attributes[i].v2 = ((inPrimitive[0].attributes[i].v2 * u / inPrimitive[0].gl_Position.w) +
                                               (inPrimitive[1].attributes[i].v2 * v / inPrimitive[1].gl_Position.w) +
                                               (inPrimitive[2].attributes[i].v2 * w / inPrimitive[2].gl_Position.w)) /
                                              ((u / inPrimitive[0].gl_Position.w) + (v / inPrimitive[1].gl_Position.w) + (w / inPrimitive[2].gl_Position.w));
                        break;
                    case AttributeType::VEC3:
                        in.attributes[i].v3 = ((inPrimitive[0].attributes[i].v3 * u / inPrimitive[0].gl_Position.w) +
                                               (inPrimitive[1].attributes[i].v3 * v / inPrimitive[1].gl_Position.w) +
                                               (inPrimitive[2].attributes[i].v3 * w / inPrimitive[2].gl_Position.w)) /
                                              ((u / inPrimitive[0].gl_Position.w) + (v / inPrimitive[1].gl_Position.w) + (w / inPrimitive[2].gl_Position.w));
                        break;
                    case AttributeType::VEC4:
                        in.attributes[i].v4 = ((inPrimitive[0].attributes[i].v4 * u / inPrimitive[0].gl_Position.w) +
                                               (inPrimitive[1].attributes[i].v4 * v / inPrimitive[1].gl_Position.w) +
                                               (inPrimitive[2].attributes[i].v4 * w / inPrimitive[2].gl_Position.w)) /
                                              ((u / inPrimitive[0].gl_Position.w) + (v / inPrimitive[1].gl_Position.w) + (w / inPrimitive[2].gl_Position.w));
                        break;
                    default:
                        break;
                    }
                }

                OutFragment out;
                in.gl_FragCoord = point;
                activeProgram->fs(out, in, activeProgram->uniforms);

                

                color.RED = out.gl_FragColor[0] * 255;
                color.GREEN = out.gl_FragColor[1] * 255;
                color.BLUE = out.gl_FragColor[2] * 255;
                color.ALPHA = out.gl_FragColor[3] * 255;

                //per fragment operations

                if (in.gl_FragCoord.z < frameBuffer.depthBuffer[y * width + x]) {
                    frameBuffer.colorBuffer[y * width + x] = color;
                    frameBuffer.depthBuffer[y * width + x] = in.gl_FragCoord.z;
                }

                //outFragments.push_back(out);
            } 
        }
    }
    //return outFragments;
}

float GPU::inTriangle(glm::vec4 v0, glm::vec4 v1, float px, float py) {
    px += 0.5;
    py += 0.5;
    return (px - v0[0]) * (v1[1] - v0[1]) - (py - v0[1]) * (v1[0] - v0[0]);
}

float GPU::triangleArea(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3) {
    return abs((v3[0] - v1[0]) * (v2[1] - v1[1]) - (v3[1] - v1[1]) * (v2[0] - v1[0]) / 2.0);
}

float GPU::edgeLenght(glm::vec4 v1, glm::vec4 v2) {
    return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

float GPU::area(float a, float b, float c) {
    float s = (a + b + c) / 2;
    return (float)sqrt(s * (s - a) * (s - b) * (s - c));
}



/// @}
