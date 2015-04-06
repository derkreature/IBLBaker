/*
 * Copyright 2014-2015 Daniel Collin. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "imgui.h"
#include "iblimgui.h"
#include "ocornut_imgui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.c"

#include <IblIDevice.h>
#include <IblIShader.h>
#include <IblGpuVariable.h>
#include <IblGpuTechnique.h>
#include <IblIVertexDeclaration.h>
#include <IblDynamicRenderer.h>
#include <IblIVertexBuffer.h>
#include <IblITexture.h>
#include <IblTextureMgr.h>
#include <IblShaderMgr.h>
#include <IblUIRenderer.h>
#include <IblVertexDeclarationMgr.h>
#include <IblInputState.h>

static void imguiRender(ImDrawList** const _lists, int cmd_lists_count);

struct OcornutImguiContext
{
    void render(ImDrawList** const _lists, int _count)
    {
        const float width  = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;

        Ibl::UIRenderer * uiRenderer = Ibl::UIRenderer::renderer();
        Ibl::IDevice* device = uiRenderer->device();

        Ibl::Matrix44f ortho;
        ortho.makeOrthoOffCenterLH(0, width, 0, height, -1, 1.0);

        uiRenderer->setViewProj(ortho);

        // Render command lists
        for (int32_t ii = 0; ii < _count; ++ii)
        {
            uint32_t vtx_size = 0;

            const ImDrawList* cmd_list   = _lists[ii];
            const ImDrawVert* vtx_buffer = cmd_list->vtx_buffer.begin();

            const ImDrawCmd* pcmd_begin = cmd_list->commands.begin();
            const ImDrawCmd* pcmd_end   = cmd_list->commands.end();
            for (const ImDrawCmd* pcmd = pcmd_begin; pcmd != pcmd_end; pcmd++)
            {
                vtx_size += (uint32_t)pcmd->vtx_count;
            }

            
            Ibl::IVertexBuffer* vb = uiRenderer->vertexBuffer(m_decl);

            ImDrawVert* verts = (ImDrawVert*)vb->lock(vtx_size* sizeof(ImDrawVert));
            memcpy(verts, vtx_buffer, vtx_size * sizeof(ImDrawVert));
            vb->unlock();

            uint32_t vtx_offset = 0;
            for (const ImDrawCmd* pcmd = pcmd_begin; pcmd != pcmd_end; pcmd++)
            {
                // Need MSAA support reenabled.
                uiRenderer->device()->enableAlphaBlending();
                uiRenderer->device()->setupBlendPipeline(Ibl::BlendAlpha);

                device->setScissorRect(uint16_t(pcmd->clip_rect.x)
                        , uint16_t(pcmd->clip_rect.y)
                        , uint16_t(pcmd->clip_rect.z-pcmd->clip_rect.x)
                        , uint16_t(pcmd->clip_rect.w-pcmd->clip_rect.y));

                const Ibl::GpuVariable* textureVariable = nullptr;
                if (m_program->getParameterByName("s_tex", textureVariable))
                {
                    textureVariable->setTexture(m_texture);
                }

                uiRenderer->setVertexBuffer(vb);
                
                uiRenderer->setShader(m_program);
                uiRenderer->render(pcmd->vtx_count, vtx_offset);

                vtx_offset += pcmd->vtx_count;

            }

            uiRenderer->device()->disableAlphaBlending();
        }
    }

    void create(const void* _data, uint32_t _size, float _fontSize)
    {
        m_viewId = 31;

        Ibl::UIRenderer * uiRenderer = Ibl::UIRenderer::renderer();
        Ibl::IDevice* device = uiRenderer->device();


        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(1280.0f, 720.0f);
        io.DeltaTime = 1.0f / 60.0f;

        if (Ibl::ShaderMgr* shaderMgr = device->shaderMgr())
        {
            if (!shaderMgr->addShader("ocornut_imgui.fx", m_program, true, false))
            {
                LOG("Failed to load imgui color shader");
                assert(0);
            }
        }

        std::vector<Ibl::VertexElement> vertexElements;
        vertexElements.push_back(Ibl::VertexElement(0, 0, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::POSITION, 0));
        vertexElements.push_back(Ibl::VertexElement(0, 8, Ibl::FLOAT2, Ibl::METHOD_DEFAULT, Ibl::TEXCOORD, 0));
        vertexElements.push_back(Ibl::VertexElement(0, 16, Ibl::UBYTE4, Ibl::METHOD_DEFAULT, Ibl::COLOR, 0));
        vertexElements.push_back(Ibl::VertexElement(0xFF, 0, Ibl::UNUSED, 0, 0, 0));

        Ibl::VertexDeclarationParameters resource = Ibl::VertexDeclarationParameters(vertexElements);
        if (Ibl::IVertexDeclaration* vertexDeclaration =
            Ibl::VertexDeclarationMgr::vertexDeclarationMgr()->createVertexDeclaration(&resource))
        {
            m_decl = vertexDeclaration;
        }

        uint8_t* data;
        int32_t width;
        int32_t height;
        void* font = ImGui::MemAlloc(_size);
        memcpy(font, _data, _size);
        io.Fonts->AddFontFromMemoryTTF(font, _size, _fontSize);
        io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

        // Should be point.

        Ibl::TextureParameters textureData =
            Ibl::TextureParameters("Normal Offsets Texture",
            Ibl::TwoD,
            Ibl::Procedural,
            Ibl::PF_BYTE_RGBA,
            Ibl::Vector3i(width, height, 1));
        m_texture = device->createTexture(&textureData);
        m_texture->write(data);

        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 4.0f;
        io.RenderDrawListsFn = imguiRender;
    }

    void destroy()
    {
    }

    // TODO: Fix this quick hack for keyboard input.
    void beginFrame(Ibl::InputState* inputState, int32_t _mx, int32_t _my, uint8_t _button, int _width, int _height, char _inputChar, uint8_t _viewId)
    {
        m_viewId = _viewId;
        ImGuiIO& io = ImGui::GetIO();
        io.AddInputCharacter(_inputChar & 0x7f); // ASCII or GTFO! :)
        io.DisplaySize = ImVec2((float)_width, (float)_height);
        io.DeltaTime = 1.0f / 60.0f;
        io.MousePos = ImVec2((float)_mx, (float)_my);
        io.MouseDown[0] = 0 != (_button & IMGUI_MBUT_LEFT);

        ImGui::NewFrame();

		ImGui::ShowTestWindow();
    }

    void endFrame()
    {
        ImGui::Render();
    }

    Ibl::IVertexDeclaration*    m_decl;
    const Ibl::IShader*         m_program;
    Ibl::ITexture*              m_texture;
    Ibl::GpuVariable*           s_tex;
    uint8_t                     m_viewId;
};

static OcornutImguiContext s_ctx;

static void imguiRender(ImDrawList** const _lists, int _count)
{
    s_ctx.render(_lists, _count);
}

void IMGUI_create(const void* _data, uint32_t _size, float _fontSize)
{
    s_ctx.create(_data, _size, _fontSize);
}

void IMGUI_destroy()
{
    s_ctx.destroy();
}

void IMGUI_beginFrame(Ibl::InputState* inputState, int32_t _mx, int32_t _my, uint8_t _button, int _width, int _height, char _inputChar, uint8_t _viewId)
{
    s_ctx.beginFrame(inputState, _mx, _my, _button, _width, _height, _inputChar, _viewId);
}

void IMGUI_endFrame()
{
    s_ctx.endFrame();
}
