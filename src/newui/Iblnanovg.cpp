/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#define NVG_ANTIALIAS 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nanovg.h"
#include <IblIVertexDeclaration.h>
#include <IblDynamicRenderer.h>
#include <IblIShader.h>
#include <IblGpuVariable.h>
#include <IblITexture.h>
#include <IblVertexDeclarationMgr.h>
#include <IblIIndexBuffer.h>
#include <IblIVertexBuffer.h>
#include <IblShaderMgr.h>
#include <IblUIRenderer.h>

namespace
{
//#include "vs_nanovg_fill.bin.h"
//#include "fs_nanovg_fill.bin.h"
    static Ibl::IDevice* s_Device;
    static Ibl::UIRenderer* _uiRenderer;
    static Ibl::IVertexDeclaration* s_nvgDecl;

    enum GLNVGshaderType
    {
        NSVG_SHADER_FILLGRAD,
        NSVG_SHADER_FILLIMG,
        NSVG_SHADER_SIMPLE,
        NSVG_SHADER_IMG
    };

    // These are additional flags on top of NVGimageFlags.
    enum NVGimageFlagsGL {
        NVG_IMAGE_NODELETE = 1<<16, // Do not delete GL texture handle.
    };

    struct GLNVGtexture
    {
        Ibl::ITexture* texture;
        int id;
        int width, height;
        int type;
        int flags;
    };

    enum GLNVGcallType
    {
        GLNVG_FILL,
        GLNVG_CONVEXFILL,
        GLNVG_STROKE,
        GLNVG_TRIANGLES,
    };

    struct GLNVGcall
    {
        int type;
        int image;
        int pathOffset;
        int pathCount;
        int vertexOffset;
        int vertexCount;
        int uniformOffset;
    };

    struct GLNVGpath
    {
        int fillOffset;
        int fillCount;
        int strokeOffset;
        int strokeCount;
    };

    struct GLNVGfragUniforms
    {
        float scissorMat[16]; // matrices are actually 3 vec4s
        float paintMat[16];
        NVGcolor innerCol;
        NVGcolor outerCol;

        // u_scissorExtScale
        float scissorExt[2];
        float scissorScale[2];

        // u_extentRadius
        float extent[2];
        float radius;

        // u_params
        float feather;
        float strokeMult;
        float texType;
        float type;
    };

    struct GLNVGcontext
    {
        const Ibl::IShader* prog;
        Ibl::ITexture* s_tex;

        uint64_t state;
        int      th;

        Ibl::IVertexBuffer* tvb;
        uint8_t viewid;

        struct GLNVGtexture* textures;
        float view[2];
        int ntextures;
        int ctextures;
        int textureId;
        int vertBuf;
        int fragSize;
        int edgeAntiAlias;

        // Per frame buffers
        struct GLNVGcall* calls;
        int ccalls;
        int ncalls;
        struct GLNVGpath* paths;
        int cpaths;
        int npaths;
        struct NVGvertex* verts;
        int cverts;
        int nverts;
        unsigned char* uniforms;
        int cuniforms;
        int nuniforms;
    };

    static struct GLNVGtexture* glnvg__allocTexture(struct GLNVGcontext* gl)
    {
        struct GLNVGtexture* tex = NULL;
        int i;

        for (i = 0; i < gl->ntextures; i++)
        {
            if (gl->textures[i].texture == nullptr)
            {
                tex = &gl->textures[i];
                break;
            }
        }

        if (tex == NULL)
        {
            if (gl->ntextures+1 > gl->ctextures)
            {
                int old = gl->ctextures;
                gl->ctextures = (gl->ctextures == 0) ? 2 : gl->ctextures*2;
                gl->textures = (struct GLNVGtexture*)realloc(gl->textures, sizeof(struct GLNVGtexture)*gl->ctextures);
                memset(&gl->textures[old], 0xff, (gl->ctextures-old)*sizeof(struct GLNVGtexture) );

                if (gl->textures == NULL)
                {
                    return NULL;
                }
            }
            tex = &gl->textures[gl->ntextures++];
        }

        memset(tex, 0, sizeof(*tex));

        return tex;
    }

    static struct GLNVGtexture* glnvg__findTexture(struct GLNVGcontext* gl, int id)
    {
        int i;
        for (i = 0; i < gl->ntextures; i++)
        {
            if (gl->textures[i].id == id)
                return &gl->textures[id];
        }

        return nullptr;
    }

    static int glnvg__deleteTexture(struct GLNVGcontext* gl, int id)
    {
        for (int ii = 0; ii < gl->ntextures; ii++)
        {
            if (gl->textures[id].id == id)
            {
                safedelete(gl->textures[id].texture);
                memset(&gl->textures[ii], 0, sizeof(gl->textures[ii]));
                return 1;
            }
        }

        return 0;
    }

    static int nvgRenderCreate(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        if (Ibl::ShaderMgr* shaderMgr = s_Device->shaderMgr())
        {
            if (!shaderMgr->addShader("nanovg_fill.fx", gl->prog, true, false))
            {
                return 0;
            }
        }

        /* // In place set.
        gl->u_scissorMat      = bgfx::createUniform("u_scissorMat",      bgfx::UniformType::Uniform3x3fv);
        gl->u_paintMat        = bgfx::createUniform("u_paintMat",        bgfx::UniformType::Uniform3x3fv);
        gl->u_innerCol        = bgfx::createUniform("u_innerCol",        bgfx::UniformType::Uniform4fv);
        gl->u_outerCol        = bgfx::createUniform("u_outerCol",        bgfx::UniformType::Uniform4fv);
        gl->u_viewSize        = bgfx::createUniform("u_viewSize",        bgfx::UniformType::Uniform2fv);
        gl->u_scissorExtScale = bgfx::createUniform("u_scissorExtScale", bgfx::UniformType::Uniform4fv);
        gl->u_extentRadius    = bgfx::createUniform("u_extentRadius",    bgfx::UniformType::Uniform4fv);
        gl->u_params          = bgfx::createUniform("u_params",          bgfx::UniformType::Uniform4fv);
        gl->s_tex             = bgfx::createUniform("s_tex",             bgfx::UniformType::Uniform1i);
        */

        std::vector<Ibl::VertexElement> vertexElements;
        vertexElements.push_back(Ibl::VertexElement(0, 0, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::POSITION, 0));
        vertexElements.push_back(Ibl::VertexElement(0, 8, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::TEXCOORD, 0));
        vertexElements.push_back(Ibl::VertexElement(0xFF, 0, Ibl::UNUSED, 0, 0, 0));

        Ibl::VertexDeclarationParameters resource = Ibl::VertexDeclarationParameters(vertexElements);
        if (Ibl::IVertexDeclaration* vertexDeclaration =
            Ibl::VertexDeclarationMgr::vertexDeclarationMgr()->createVertexDeclaration(&resource))
        {
            s_nvgDecl = vertexDeclaration;
        }

        int align = 16;
        gl->fragSize = sizeof(struct GLNVGfragUniforms) + align - sizeof(struct GLNVGfragUniforms) % align;

        // TODO, build UIRenderer.

        return 1;
    }

    static int nvgRenderCreateTexture(void* _userPtr, int _type, int _width, int _height, int _flags, const unsigned char* _rgba)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__allocTexture(gl);

        if (tex == nullptr)
        {
            return 0;
        }

        tex->width  = _width;
        tex->height = _height;
        tex->type   = _type;
        tex->flags  = _flags;

        uint32_t bytesPerPixel = NVG_TEXTURE_RGBA == tex->type ? 4 : 1;
        uint32_t pitch = tex->width * bytesPerPixel;

        if (NULL != _rgba)
        {
            //mem = bgfx::alloc(tex->height * pitch);
            //bgfx::imageSwizzleBgra8(tex->width, tex->height, pitch, _rgba, mem->data);
        }

        static int textureId = 1;

        Ibl::TextureParameters textureData =
            Ibl::TextureParameters("Normal Offsets Texture",
            Ibl::TwoD,
            Ibl::Procedural,
            NVG_TEXTURE_RGBA == _type ? Ibl::PF_BYTE_RGBA : Ibl::PF_L8,
            Ibl::Vector3i(tex->width, tex->height, 1));
        tex->texture = s_Device->createTexture(&textureData);
        tex->texture->write(_rgba);
        tex->id = textureId;
        textureId++;


        return tex->id;
    }

    static int nvgRenderDeleteTexture(void* _userPtr, int image)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        return glnvg__deleteTexture(gl, image);
    }

    static int nvgRenderUpdateTexture(void* _userPtr, int image, int x, int y, int w, int h, const unsigned char* data)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__findTexture(gl, image);
        if (tex == NULL)
        {
            return 0;
        }

        uint32_t bytesPerPixel = NVG_TEXTURE_RGBA == tex->type ? 4 : 1;
        uint32_t pitch = tex->width * bytesPerPixel;

        tex->texture->writeSubRegion(data, x, y, w, h, bytesPerPixel);

        return 1;
    }

    static int nvgRenderGetTextureSize(void* _userPtr, int image, int* w, int* h)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__findTexture(gl, image);

        if (tex->id == 0)
        {
            return 0;
        }

        *w = tex->width;
        *h = tex->height;

        return 1;
    }

    static void glnvg__xformIdentity(float* t)
    {
        t[0] = 1.0f; t[1] = 0.0f;
        t[2] = 0.0f; t[3] = 1.0f;
        t[4] = 0.0f; t[5] = 0.0f;
    }

    static void glnvg__xformInverse(float* inv, float* t)
    {
        double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
        if (det > -1e-6 && det < 1e-6) {
            glnvg__xformIdentity(t);
            return;
        }
        invdet = 1.0 / det;
        inv[0] = (float)(t[3] * invdet);
        inv[2] = (float)(-t[2] * invdet);
        inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
        inv[1] = (float)(-t[1] * invdet);
        inv[3] = (float)(t[0] * invdet);
        inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
    }

    static void glnvg__xformToMat3x4(float* m3, float* t)
    {
        m3[0] = t[0];
        m3[1] = t[1];
        m3[2] = 0.0f;
        m3[3] = 0.0f;
        m3[4] = t[2];
        m3[5] = t[3];
        m3[6] = 0.0f;
        m3[7] = 0.0f;
        m3[8] = t[4];
        m3[9] = t[5];
        m3[10] = 1.0f;
        m3[11] = 0.0f;
    }

    static void nvg_copyMatrix3to4(float* pDest, const float* pSource)
    {
        unsigned int i;
        for (i = 0; i < 4; i++)
        {
            memcpy(&pDest[i * 4], &pSource[i * 3], sizeof(float) * 3);
        }
    }

    static int glnvg__convertPaint(struct GLNVGcontext* gl, struct GLNVGfragUniforms* frag, struct NVGpaint* paint,
                                   struct NVGscissor* scissor, float width, float fringe)
    {
        struct GLNVGtexture* tex = NULL;
        float invxform[6] = {};
        float transform[16] = {};

        memset(frag, 0, sizeof(*frag));

        frag->innerCol = paint->innerColor;
        frag->outerCol = paint->outerColor;

        glnvg__xformInverse(invxform, paint->xform);
        nvg_copyMatrix3to4(frag->paintMat, invxform);

        if (scissor->extent[0] < 0.5f || scissor->extent[1] < 0.5f)
        {
            memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
            frag->scissorExt[0] = 1.0f;
            frag->scissorExt[1] = 1.0f;
            frag->scissorScale[0] = 1.0f;
            frag->scissorScale[1] = 1.0f;
        }
        else
        {
            glnvg__xformInverse(invxform, scissor->xform);
            nvg_copyMatrix3to4(frag->scissorMat, invxform);
            frag->scissorExt[0] = scissor->extent[0];
            frag->scissorExt[1] = scissor->extent[1];
            frag->scissorScale[0] = sqrtf(scissor->xform[0]*scissor->xform[0] + scissor->xform[2]*scissor->xform[2]) / fringe;
            frag->scissorScale[1] = sqrtf(scissor->xform[1]*scissor->xform[1] + scissor->xform[3]*scissor->xform[3]) / fringe;
        }

        memcpy(frag->extent, paint->extent, sizeof(frag->extent));
        frag->strokeMult = (width*0.5f + fringe*0.5f) / fringe;

        gl->th = 0;

        if (paint->image != 0)
        {
            tex = glnvg__findTexture(gl, paint->image);
            if (tex == NULL)
            {
                return 0;
            }
            frag->type = NSVG_SHADER_FILLIMG;
            frag->texType = tex->type == NVG_TEXTURE_RGBA ? 0.0f : 1.0f;
            gl->th = tex->id;
        }
        else
        {
            frag->type = NSVG_SHADER_FILLGRAD;
            frag->radius = paint->radius;
            frag->feather = paint->feather;
        }

        return 1;
    }

    static void glnvg__mat3(float* dst, float* src)
    {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];

        dst[3] = src[4];
        dst[4] = src[5];
        dst[5] = src[6];

        dst[6] = src[8];
        dst[7] = src[9];
        dst[8] = src[10];
    }

    static struct GLNVGfragUniforms* nvg__fragUniformPtr(struct GLNVGcontext* gl, int i)
    {
        return (struct GLNVGfragUniforms*)&gl->uniforms[i];
    }

    static void nvgRenderSetUniforms(struct GLNVGcontext* gl, int uniformOffset, int image)
    {
        const Ibl::GpuVariable*      scissorMatVariable = nullptr;
        const Ibl::GpuVariable*      paintMatVariable = nullptr;
        const Ibl::GpuVariable*      innerColVariable= nullptr;
        const Ibl::GpuVariable*      outerColVariable = nullptr;

        const Ibl::GpuVariable*      scissorExtScaleVariable = nullptr;
        const Ibl::GpuVariable*      extentRadiusVariable = nullptr;
        const Ibl::GpuVariable*      paramsVariable = nullptr;
        const Ibl::GpuVariable*      texVariable = nullptr;

        //gl->prog->getTechniqueByName(std::string("basic"), importanceSamplingSpecularTechnique);
        gl->prog->getParameterByName("u_scissorMat", scissorMatVariable);
        gl->prog->getParameterByName("u_paintMat", paintMatVariable);
        gl->prog->getParameterByName("u_innerCol", innerColVariable);
        gl->prog->getParameterByName("u_outerCol", outerColVariable);
        gl->prog->getParameterByName("u_scissorExtScale", scissorExtScaleVariable);
        gl->prog->getParameterByName("u_extentRadius", extentRadiusVariable);
        gl->prog->getParameterByName("u_params", paramsVariable);
        gl->prog->getParameterByName("s_tex", texVariable);

        struct GLNVGfragUniforms* frag = nvg__fragUniformPtr(gl, uniformOffset);
        if (scissorMatVariable)
            scissorMatVariable->setMatrix(frag->scissorMat);
        if (paintMatVariable)
            paintMatVariable->setMatrix(frag->paintMat);

        if (innerColVariable)
            innerColVariable->setVector(frag->innerCol.rgba);
        if (outerColVariable)
            outerColVariable->setVector(frag->outerCol.rgba);
        if (scissorExtScaleVariable)
            scissorExtScaleVariable->set(&frag->scissorExt[0], sizeof(float) * 2);
        if (extentRadiusVariable)
            extentRadiusVariable->set(&frag->extent[0], sizeof(float) * 2);
        if (paramsVariable)
            paramsVariable->set(&frag->feather, sizeof(float));

        Ibl::ITexture* handle = nullptr;
        gl->th = 0;
        if (image != 0)
        {
            struct GLNVGtexture* tex = glnvg__findTexture(gl, image);
            if (tex != NULL)
            {
                handle = tex->texture;
                gl->th = tex->id;
                if (texVariable)
                    texVariable->setTexture(handle);
            }
        }
    }

    static void nvgRenderViewport(void* _userPtr, int width, int height)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        gl->view[0] = (float)width;
        gl->view[1] = (float)height;
        Ibl::Viewport viewport = Ibl::Viewport(0, 0, width, height, 0, 1);
        s_Device->setViewport(&viewport);
    }


    static void fan(uint32_t _start, uint32_t _count)
    {
        uint32_t numTris = _count-2;
        uint32_t* data = (uint32_t*)_uiRenderer->indexBuffer()->lock(numTris * 3);
        for (uint32_t ii = 0; ii < numTris; ++ii)
        {
            data[ii*3+0] = _start;
            data[ii*3+1] = _start + ii + 1;
            data[ii*3+2] = _start + ii + 2;
        }

        _uiRenderer->setDrawIndexed(true);
    }

    static void glnvg__fill(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        struct GLNVGpath* paths = &gl->paths[call->pathOffset];
        int i, npaths = call->pathCount;

        // set bindpoint for solid loc
        nvgRenderSetUniforms(gl, call->uniformOffset, 0);
        s_Device->setCullMode(Ibl::CullNone);
        s_Device->enableStencilTest();

        // TODO, need primitive types, culling and blend setup.

        for (i = 0; i < npaths; i++)
        {
            if (2 < paths[i].fillCount)
            {
                _uiRenderer->setPrimitiveType(Ibl::TriangleList);
                _uiRenderer->setShader(gl->prog);

                s_Device->setupStencil(0xff, 0xff, 
                                       Ibl::Always, Ibl::StencilKeep, Ibl::StencilKeep, Ibl::StencilIncrement,
                                       Ibl::Always, Ibl::StencilKeep, Ibl::StencilKeep, Ibl::StencilDecrement);

                _uiRenderer->setVertexBuffer(gl->tvb);
                //bgfx::setTexture(0, gl->s_tex, gl->th);
                fan(paths[i].fillOffset, paths[i].fillCount);

                // Input to render is the index count if an index buffer is bound.
                _uiRenderer->render((paths[i].fillCount-2)*3, 0);
                _uiRenderer->setDrawIndexed(false);
            }
        }

        // Draw aliased off-pixels
        nvgRenderSetUniforms(gl, call->uniformOffset + gl->fragSize, call->image);
        _uiRenderer->setPrimitiveType(Ibl::TriangleStrip);
        if (gl->edgeAntiAlias)
        {
            // Draw fringes
            for (i = 0; i < npaths; i++)
            {
                _uiRenderer->setShader(gl->prog);

                s_Device->setupStencil(0xff, 0xff, Ibl::Equal, Ibl::StencilKeep, Ibl::StencilKeep, Ibl::StencilKeep);

                _uiRenderer->setVertexBuffer(gl->tvb);
                //bgfx::setTexture(0, gl->s_tex, gl->th);
                //bgfx::submit(gl->viewid);
                _uiRenderer->render(paths[i].strokeCount, paths[i].strokeOffset);
            }
        }

        // Draw fill
        _uiRenderer->setShader(gl->prog);        
        _uiRenderer->setVertexBuffer(gl->tvb);
        s_Device->setupStencil(0xff, 0xff, Ibl::NotEqual, Ibl::StencilZero, Ibl::StencilZero, Ibl::StencilZero );

        _uiRenderer->render(call->vertexCount, call->vertexOffset);

        s_Device->disableStencilTest();
    }

    static void glnvg__convexFill(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {

        struct GLNVGpath* paths = &gl->paths[call->pathOffset];
        int i, npaths = call->pathCount;

        nvgRenderSetUniforms(gl, call->uniformOffset, call->image);
        _uiRenderer->setPrimitiveType(Ibl::TriangleList);

        for (i = 0; i < npaths; i++)
        {
            if (paths[i].fillCount == 0) continue;
            _uiRenderer->setShader(gl->prog);
            //bgfx::setState(gl->state);
            _uiRenderer->setVertexBuffer(gl->tvb);
            //bgfx::setTexture(0, gl->s_tex, gl->th);
            fan(paths[i].fillOffset, paths[i].fillCount);

            // Index count. Offset is baked into the indices.
            _uiRenderer->render((paths[i].fillCount - 2) * 3, 0);
        }

        _uiRenderer->setPrimitiveType(Ibl::TriangleStrip);
        if (gl->edgeAntiAlias)
        {
            // Draw fringes
            for (i = 0; i < npaths; i++)
            {
                _uiRenderer->setShader(gl->prog);
                //bgfx::setTexture(0, gl->s_tex, gl->th);
                // Vertex count and vertex offset.
                _uiRenderer->render(paths[i].strokeCount, paths[i].strokeOffset);
            }
        }
    }

    static void glnvg__stroke(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        struct GLNVGpath* paths = &gl->paths[call->pathOffset];
        int npaths = call->pathCount, i;
        nvgRenderSetUniforms(gl, call->uniformOffset, call->image);

        // Draw Strokes
        _uiRenderer->setPrimitiveType(Ibl::TriangleStrip);
        for (i = 0; i < npaths; i++)
        {
            _uiRenderer->setShader(gl->prog);

            _uiRenderer->setVertexBuffer(gl->tvb);
            //bgfx::setTexture(0, gl->s_tex, gl->th);
            _uiRenderer->render(paths[i].strokeCount, paths[i].strokeOffset);
        }
    }

    static void glnvg__triangles(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        _uiRenderer->setPrimitiveType(Ibl::TriangleList);
        if (3 <= call->vertexCount)
        {
            nvgRenderSetUniforms(gl, call->uniformOffset, call->image);

            _uiRenderer->setShader(gl->prog);
            //_uiRenderer->setState(gl->state);
            _uiRenderer->render(call->vertexCount, call->vertexOffset);
        }
    }

    static void nvgRenderFlush(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        if (gl->ncalls > 0)
        {
            gl->tvb = _uiRenderer->vertexBuffer(s_nvgDecl);
            size_t vertexBufferSize = gl->nverts * sizeof(struct NVGvertex);
            void* vertexBufferData = gl->tvb->lock(vertexBufferSize);
            memcpy(vertexBufferData, gl->verts, gl->nverts * sizeof(struct NVGvertex) );
            gl->tvb->unlock();

            // TODO: Blend Setup
//            gl->state = 0
            //                | BGFX_STATE_RGB_WRITE
            //                | BGFX_STATE_ALPHA_WRITE
            //    ;

// 			if (alphaBlend == NVG_PREMULTIPLIED_ALPHA)
// 			{
// 				gl->state |= BGFX_STATE_BLEND_FUNC_SEPARATE(
// 								  BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA
// 								, BGFX_STATE_BLEND_ONE,       BGFX_STATE_BLEND_INV_SRC_ALPHA
// 								);
// 			}
// 			else
/*            {
                gl->state |= BGFX_STATE_BLEND_FUNC(
                                BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA
                                );
            }
*/
            const Ibl::GpuVariable*      viewSizeVariable = nullptr;
            gl->prog->getParameterByName("u_viewSize", viewSizeVariable);
            if (viewSizeVariable)
                viewSizeVariable->set(gl->view, sizeof(float)*2);

            for (uint32_t ii = 0, num = gl->ncalls; ii < num; ++ii)
            {
                struct GLNVGcall* call = &gl->calls[ii];
                switch (call->type)
                {
                case GLNVG_FILL:
                    glnvg__fill(gl, call);
                    break;

                case GLNVG_CONVEXFILL:
                    glnvg__convexFill(gl, call);
                    break;

                case GLNVG_STROKE:
                    glnvg__stroke(gl, call);
                    break;

                case GLNVG_TRIANGLES:
                    glnvg__triangles(gl, call);
                    break;
                }
            }
        }

        // Reset calls
        gl->nverts    = 0;
        gl->npaths    = 0;
        gl->ncalls    = 0;
        gl->nuniforms = 0;
    }

    static int glnvg__maxVertCount(const struct NVGpath* paths, int npaths)
    {
        int i, count = 0;
        for (i = 0; i < npaths; i++)
        {
            count += paths[i].nfill;
            count += paths[i].nstroke;
        }
        return count;
    }

    static int glnvg__maxi(int a, int b) { return a > b ? a : b; }

    static struct GLNVGcall* glnvg__allocCall(struct GLNVGcontext* gl)
    {
        struct GLNVGcall* ret = NULL;
        if (gl->ncalls+1 > gl->ccalls)
        {
            gl->ccalls = gl->ccalls == 0 ? 32 : gl->ccalls * 2;
            gl->calls = (struct GLNVGcall*)realloc(gl->calls, sizeof(struct GLNVGcall) * gl->ccalls);
        }
        ret = &gl->calls[gl->ncalls++];
        memset(ret, 0, sizeof(struct GLNVGcall));
        return ret;
    }

    static int glnvg__allocPaths(struct GLNVGcontext* gl, int n)
    {
        int ret = 0;
        if (gl->npaths + n > gl->cpaths) {
            GLNVGpath* paths;
            int cpaths = glnvg__maxi(gl->npaths + n, 128) + gl->cpaths / 2; // 1.5x Overallocate
            paths = (GLNVGpath*)realloc(gl->paths, sizeof(GLNVGpath) * cpaths);
            if (paths == NULL) return -1;
            gl->paths = paths;
            gl->cpaths = cpaths;
        }
        ret = gl->npaths;
        gl->npaths += n;
        return ret;
    }

    static int glnvg__allocVerts(GLNVGcontext* gl, int n)
    {
        int ret = 0;
        if (gl->nverts+n > gl->cverts)
        {
            NVGvertex* verts;
            int cverts = glnvg__maxi(gl->nverts + n, 4096) + gl->cverts/2; // 1.5x Overallocate
            verts = (NVGvertex*)realloc(gl->verts, sizeof(NVGvertex) * cverts);
            if (verts == NULL) return -1;
            gl->verts = verts;
            gl->cverts = cverts;
        }
        ret = gl->nverts;
        gl->nverts += n;
        return ret;
    }

    static int glnvg__allocFragUniforms(struct GLNVGcontext* gl, int n)
    {
        int ret = 0, structSize = gl->fragSize;
        if (gl->nuniforms+n > gl->cuniforms)
        {
            gl->cuniforms = gl->cuniforms == 0 ? glnvg__maxi(n, 32) : gl->cuniforms * 2;
            gl->uniforms = (unsigned char*)realloc(gl->uniforms, gl->cuniforms * structSize);
        }
        ret = gl->nuniforms * structSize;
        gl->nuniforms += n;
        return ret;
    }

    static void glnvg__vset(struct NVGvertex* vtx, float x, float y, float u, float v)
    {
        vtx->x = x;
        vtx->y = y;
        vtx->u = u;
        vtx->v = v;
    }

    static void nvgRenderFill(void* _userPtr, struct NVGpaint* paint, struct NVGscissor* scissor, float fringe,
                                  const float* bounds, const struct NVGpath* paths, int npaths)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        struct GLNVGcall* call = glnvg__allocCall(gl);
        struct NVGvertex* quad;
        struct GLNVGfragUniforms* frag;
        int i, maxverts, offset;

        call->type = GLNVG_FILL;
        call->pathOffset = glnvg__allocPaths(gl, npaths);
        call->pathCount = npaths;
        call->image = paint->image;

        if (npaths == 1 && paths[0].convex)
        {
            call->type = GLNVG_CONVEXFILL;
        }

        // Allocate vertices for all the paths.
        maxverts = glnvg__maxVertCount(paths, npaths) + 6;
        offset = glnvg__allocVerts(gl, maxverts);

        for (i = 0; i < npaths; i++)
        {
            struct GLNVGpath* copy = &gl->paths[call->pathOffset + i];
            const struct NVGpath* path = &paths[i];
            memset(copy, 0, sizeof(struct GLNVGpath));
            if (path->nfill > 0)
            {
                copy->fillOffset = offset;
                copy->fillCount = path->nfill;
                memcpy(&gl->verts[offset], path->fill, sizeof(struct NVGvertex) * path->nfill);
                offset += path->nfill;
            }

            if (path->nstroke > 0)
            {
                copy->strokeOffset = offset;
                copy->strokeCount = path->nstroke;
                memcpy(&gl->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
                offset += path->nstroke;
            }
        }

        // Quad
        call->vertexOffset = offset;
        call->vertexCount = 6;
        quad = &gl->verts[call->vertexOffset];
        glnvg__vset(&quad[0], bounds[0], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[1], bounds[2], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[2], bounds[2], bounds[1], 0.5f, 1.0f);

        glnvg__vset(&quad[3], bounds[0], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[4], bounds[2], bounds[1], 0.5f, 1.0f);
        glnvg__vset(&quad[5], bounds[0], bounds[1], 0.5f, 1.0f);

        // Setup uniforms for draw calls
        if (call->type == GLNVG_FILL)
        {
            call->uniformOffset = glnvg__allocFragUniforms(gl, 2);
            // Simple shader for stencil
            frag = nvg__fragUniformPtr(gl, call->uniformOffset);
            memset(frag, 0, sizeof(*frag));
            frag->type = NSVG_SHADER_SIMPLE;
            // Fill shader
            glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset + gl->fragSize), paint, scissor, fringe, fringe);
        }
        else
        {
            call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
            // Fill shader
            glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset), paint, scissor, fringe, fringe);
        }
    }

    static void nvgRenderStroke(void* _userPtr, struct NVGpaint* paint, struct NVGscissor* scissor, float fringe,
                                    float strokeWidth, const struct NVGpath* paths, int npaths)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        struct GLNVGcall* call = glnvg__allocCall(gl);
        int i, maxverts, offset;

        call->type = GLNVG_STROKE;
        call->pathOffset = glnvg__allocPaths(gl, npaths);
        call->pathCount = npaths;
        call->image = paint->image;

        // Allocate vertices for all the paths.
        maxverts = glnvg__maxVertCount(paths, npaths);
        offset = glnvg__allocVerts(gl, maxverts);

        for (i = 0; i < npaths; i++)
        {
            struct GLNVGpath* copy = &gl->paths[call->pathOffset + i];
            const struct NVGpath* path = &paths[i];
            memset(copy, 0, sizeof(struct GLNVGpath));
            if (path->nstroke)
            {
                copy->strokeOffset = offset;
                copy->strokeCount = path->nstroke;
                memcpy(&gl->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
                offset += path->nstroke;
            }
        }

        // Fill shader
        call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
        glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset), paint, scissor, strokeWidth, fringe);
    }

    static void nvgRenderTriangles(void* _userPtr, struct NVGpaint* paint, struct NVGscissor* scissor,
                                       const struct NVGvertex* verts, int nverts)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGcall* call = glnvg__allocCall(gl);
        struct GLNVGfragUniforms* frag;

        call->type = GLNVG_TRIANGLES;
        call->image = paint->image;

        // Allocate vertices for all the paths.
        call->vertexOffset = glnvg__allocVerts(gl, nverts);
        call->vertexCount = nverts;
        memcpy(&gl->verts[call->vertexOffset], verts, sizeof(struct NVGvertex) * nverts);

        // Fill shader
        call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
        frag = nvg__fragUniformPtr(gl, call->uniformOffset);
        glnvg__convertPaint(gl, frag, paint, scissor, 1.0f, 1.0f);
        frag->type = NSVG_SHADER_IMG;
    }

    static void nvgRenderDelete(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        if (gl == NULL)
        {
            return;
        }

        for (uint32_t ii = 0, num = gl->ntextures; ii < num; ++ii)
        {
            if (gl->textures[ii].texture != nullptr)
            {
                safedelete(gl->textures[ii].texture);
                gl->textures[ii].id = 0;
            }
        }

        free(gl->textures);

        free(gl);
    }

} // namespace

NVGcontext* nvgCreate(int edgeaa, unsigned char viewid, void* device)
{
    s_Device = reinterpret_cast<Ibl::IDevice*>(device);

    struct NVGparams params;
    struct NVGcontext* ctx = NULL;
    struct GLNVGcontext* gl = (struct GLNVGcontext*)malloc(sizeof(struct GLNVGcontext));
    if (gl == NULL) goto error;
    
    
    memset(gl, 0, sizeof(struct GLNVGcontext));

    memset(&params, 0, sizeof(params));
    params.renderCreate         = nvgRenderCreate;
    params.renderCreateTexture  = nvgRenderCreateTexture;
    params.renderDeleteTexture  = nvgRenderDeleteTexture;
    params.renderUpdateTexture  = nvgRenderUpdateTexture;
    params.renderGetTextureSize = nvgRenderGetTextureSize;
    params.renderViewport       = nvgRenderViewport;
    params.renderFlush          = nvgRenderFlush;
    params.renderFill           = nvgRenderFill;
    params.renderStroke         = nvgRenderStroke;
    params.renderTriangles      = nvgRenderTriangles;
    params.renderDelete         = nvgRenderDelete;
    params.userPtr = gl;
    params.edgeAntiAlias = edgeaa;

    gl->edgeAntiAlias = edgeaa;
    gl->viewid = uint8_t(viewid);

    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) goto error;

    return ctx;

error:
    // 'gl' is freed by nvgDeleteInternal.
    if (ctx != NULL)
    {
        nvgDeleteInternal(ctx);
    }

    return NULL;
}

void nvgDelete(struct NVGcontext* ctx)
{
    nvgDeleteInternal(ctx);
}