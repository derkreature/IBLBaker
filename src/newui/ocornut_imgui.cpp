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
#include <IblIVertexBuffer.h>
#include <IblITexture.h>
#include <IblTextureMgr.h>
#include <IblShaderMgr.h>
#include <IblUIRenderer.h>
#include <IblVertexDeclarationMgr.h>
#include <IblInputState.h>
#include <IblApplication.h>

static void imguiRender(ImDrawList** const _lists, int cmd_lists_count);

struct OcornutImguiContext
{
    void render(ImDrawList** const _lists, int _count)
    {
        const float width  = ImGui::GetIO().DisplaySize.x;
        const float height = ImGui::GetIO().DisplaySize.y;

        Ibl::UIRenderer * uiRenderer = Ibl::UIRenderer::renderer();
        Ibl::IDevice* device = uiRenderer->device();

        float L = 0;
        float R = float(device->backbuffer()->width());
        float T = 0;
        float B = float(device->backbuffer()->height());

        Ibl::Matrix44f ortho;
        const float mvp[4][4] =
        {
            { 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
            { 0.0f, 2.0f / (T - B), 0.0f, 0.0f, },
            { 0.0f, 0.0f, 0.5f, 0.0f },
            { (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f },
        };
        memcpy(&ortho._m[0][0], &mvp[0][0], sizeof(ortho._m));
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
        memset(m_KeyRepeatTimes, 0, sizeof(float) * 512);

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(1280.0f, 720.0f);
        io.DeltaTime = 1.0f / 60.0f;

        // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
        io.KeyMap[ImGuiKey_Tab] = DIK_TAB;                              
        io.KeyMap[ImGuiKey_LeftArrow] = DIK_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = DIK_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = DIK_UP;
        io.KeyMap[ImGuiKey_DownArrow] = DIK_UP;
        io.KeyMap[ImGuiKey_Home] = DIK_HOME;
        io.KeyMap[ImGuiKey_End] = DIK_END;
        io.KeyMap[ImGuiKey_Delete] = DIK_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = DIK_BACK;
        io.KeyMap[ImGuiKey_Enter] = DIK_RETURN;
        io.KeyMap[ImGuiKey_Escape] = DIK_ESCAPE;
        io.KeyMap[ImGuiKey_A] = 'A';
        io.KeyMap[ImGuiKey_C] = 'C';
        io.KeyMap[ImGuiKey_V] = 'V';
        io.KeyMap[ImGuiKey_X] = 'X';
        io.KeyMap[ImGuiKey_Y] = 'Y';
        io.KeyMap[ImGuiKey_Z] = 'Z';


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


    bool scanCodeToAscii(HKL keyboardLayout, uint8_t* keyboardState, uint32_t scancode, uint8_t& result)
    {
        UINT vk = MapVirtualKeyEx(scancode, 1, keyboardLayout);
        uint16_t converted[4];
        if (ToAsciiEx(vk, scancode, keyboardState, converted, 0, keyboardLayout) == 1)
        {
            result = converted[0] & 0x7f;
            return true;
        }
        return false;
    }

    // TODO: Fix this quick hack for keyboard input.
    void beginFrame(Ibl::InputState* inputState, int32_t _mx, int32_t _my, uint8_t _button, int _width, int _height, char _inputChar, uint8_t _viewId)
    {
        m_viewId = _viewId;
        ImGuiIO& io = ImGui::GetIO();

        static HKL keyboardLayout = GetKeyboardLayout(0);
        static uint8_t keyboardState[256];
        GetKeyboardState(keyboardState);

        for (uint32_t scanCode = 0; scanCode < 512; scanCode++)
        { 
            io.KeysDown[scanCode] = inputState->getKeyState(scanCode);

            if (!io.KeysDown[scanCode])
            {
                m_KeyRepeatTimes[scanCode] = 0.0f;
            }

            // If keycode and key state converts to ascii, add it as an input character.
            if (io.KeysDown[scanCode])
            {   
                if (m_KeyRepeatTimes[scanCode] > 0.2)
                {
                    m_KeyRepeatTimes[scanCode] = 0.0f;
                }

                if (m_KeyRepeatTimes[scanCode] == 0.0f)
                {
                    uint8_t asciiInput = (uint8_t)(0);
                    if (scanCodeToAscii(keyboardLayout, keyboardState, scanCode, asciiInput))
                    {
                        io.AddInputCharacter(asciiInput);
                    }
                }

                m_KeyRepeatTimes[scanCode] += 1.0f / 60.0f;
            }
        }
         
        io.MouseWheel = inputState->_z / 100.0f;
        io.KeyCtrl = (inputState->getKeyState(DIK_LCONTROL) || inputState->getKeyState(DIK_RCONTROL)) ? 1 : 0;
        io.KeyShift = (inputState->getKeyState(DIK_LSHIFT) || inputState->getKeyState(DIK_RSHIFT)) ? 1 : 0;

        io.DisplaySize = ImVec2((float)_width, (float)_height);
        io.DeltaTime = 1.0f / 60.0f;
        io.MousePos = ImVec2((float)_mx, (float)_my);
        io.MouseDown[0] = 0 != (_button & IMGUI_MBUT_LEFT);

        ImGui::NewFrame();

        //ImGui::ShowTestWindow();
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

    float                       m_KeyRepeatTimes[512];
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
