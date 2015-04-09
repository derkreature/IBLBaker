IBLBaker
--------------

![IblBakerCover](https://github.com/derkreature/IBLBaker/blob/master/images/IBLBaker.jpg)


About
--------------
IBLBaker is provided under the MIT License(MIT)
Copyright(c) 2015 Matt Davidson.
Please see the LICENSE file for full details.

Feel free to contact me with questions, corrections or ideas.
https://github.com/derkreature/
http://www.derkreature.com/

Quick download link:
https://github.com/derkreature/IBLBaker/archive/master.zip

What on earth is this?
--------------

The IBLBaker grew out of a quick implementation of the Siggraph Unreal presentations on Physically Based Rendering that I put together for a presentation at Pixar on an OpenSubDiv implementation that I had written for realtime rendering.

The main reason that it is here is to provide aspiring rendering engineers a concise example of all the parts required to realtime evaluate light probes for physically based rendering. 

While this is *not* a complete implementation of the required rendering pipeline (I have omitted cavity, bloom, reflection occlusion and realtime ambient occlusion and IBL shadowing (Kreature does these, but I felt that they detracted from the core exercise), it should serve anyone looking to implement dynamic probes in good stead. (If it doesn't, email me, and I will rectify the problem).

At the very least it can be used for baking diffuse irradiance and specular pre-convolved environment cubemaps evaluated over roughness over mips computed using a user specified brdf.

Still Reading?
--------------

The baker uses importance sampling to evaluate the environment maps and visualize the results using the separable method proposed by Epic Games at Siggraph 2013.

The tool allows the user to save the computed environment maps and the BRDF Lut to disk for use in other pipelines.

I have provided example cgfx shaders for maya viewport 2.0 in the /maya directory with 2 example scenes in .ma format.

Please review the example maya workflow video at:
https://vimeo.com/110805546

The full purpose of this tool can be found in the following videos:
https://vimeo.com/100285383
https://vimeo.com/96235208

A Special thanks must be given to Munkyfun (www.munkyfun.com) who sponsored 
much of the work in preparing this work for an open source release.
Thank you so very much guys!

Codebase notes:
--------------
This codebase is based on an old version (circa 2009) of my personal research framework. It was developed more as an exercise to learn new techniques, so parts of it are fundamentally flawed.
One particular area is the use of abstract base types for render API abstraction. This is clearly a bad idea, and will go away before I release.
Mainly, I don't want anyone to ever think that this is a good idea.

If a class has an "I" in front of it, or D3D11 at the end, please hold your nose and ignore for the moment :).

I am holding off on pulling from my current framework(Kreature) until I have implemented Vulkan. 

Suggested prior reading:
--------------
http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

http://seblagarde.wordpress.com/2011/08/17/hello-world/

http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf

https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf

http://http.developer.nvidia.com/GPUGems3/gpugems3_ch20.html

http://blog.selfshadow.com/publications/s2013-shading-course/pixar/s2013_pbs_pixar_notes.pdf

http://blog.selfshadow.com/publications/s2013-shading-course/hoffman/s2013_pbs_physics_math_notes.pdf

http://blog.selfshadow.com/publications/s2012-shading-course/gotanda/s2012_pbs_beyond_blinn_slides_v3.pdf

http://www.pbrt.org/

Similar projects
------------

- [CubeMapGen](http://developer.amd.com/tools-and-sdks/archive/legacy-cpu-gpu-tools/cubemapgen/) - A well known tool for cubemap filtering from AMD.<br \>
- [CmftStudio](https://github.com/dariomanesku/cmftStudio) - A similar open-source project.
- [Marmoset Skyshop](http://www.marmoset.co/skyshop) - Commercial plugin for Unity3D Game engine.
- [Knald Lys](https://www.knaldtech.com/lys-open-beta/) - Commercial tool from KnaldTech.

Building:
--------------
    1.) Install Visual Studio 2013 Express for Windows Desktop.
         The project builds fine under express if you don't have Visual Studio 2013 installed.
            
    2.) Open \\IblBaker\src\IblBaker.sln in Visual Studio 2013.
    
    3.) Select a configuration, build and run.
    
    4.) (Very optional) If you plan on setting high sample counts (4096 or greater) with large maps (1024 or greater) it may be useful to alter your TDR delay to a larger value than 2 seconds (depending on the number of samples and the size of the maps that you would like to test with). This is at your discretion. I take no responsibility for the consequences.
    
        KeyPath   : HKEY_LOCAL_MACHINE\System\CurrentControlSet\Control\GraphicsDrivers
        KeyValue  : TdrDelay
        ValueType : REG_DWORD
        ValueData : Number of seconds to delay. 2 seconds is the default value.

Known Leaks:
--------------
- D3D11Effects has some serious issues. I will be replacing this in the near future in any case. (Post D3D12).
- Shutdown leaks abound after the latest GUI port. I've been in a bit of a hurry, so, uhhh, please forgive.

What about linux / osx / iris / bsd / ios / nextstep / rasberry pi / abacus support?
--------------
The rendering layer is platform agnostic in principle.
For the moment only the D3D11 layer has been implemented.
The application layer started out with the best intentions of being agnostic. It unfortunately now requires some work to port from Windows to other platforms.
I am working on an OpenGL 4.0 implementation for OSX and Linux at the moment.

How do you use it?
--------------
The application config file is an xml document that can be found in:
data/iblBakerConfig.xml

Upon starting you should see a pistol surrounded by the input probe environment mapped to sphere.

Controls:
---------
Mouse move will orbit the camera.

Left Control key down + mouse move will rotate the visualized object.

W and S keys zoom the camera in and out from the origin.

Mouse scroll zooms the camera in and out from the origin.

Buttons
--------------

"Load Environment" Button
New maps can be set by click on the "Import Environment" button.
Valid input environments are .dds by default.
It is probably better that they are setup as RGBA16F or RGBA32F.
Maps can be Lat / Long / equirectangular and cubemap layouts.

While only the DDS codec is enabled by default, it is possible to enable the FreeImage code by adding a project define "#if IBL_USE_ASS_IMP_AND_FREEIMAGE = 1". You will have to provide your own builds of FreeImage and AssImp. The reference codec based on Ogre is in src/codecs/IBLFreeImage.cpp/IBLFreeImage.h.
Prior warning if you are building static libraries, both AssImp and FreeImage depend on Zlib.

LDR inputs will also work, however, will not light well.
Additional sample environment maps are available in IBLBaker/data/sampleMaps/


Environments can be saved by clicking on the "Save Environment" button.
You must specify a .dds filename to export to. A number of files will be saved using the specified .dds filename. I recommend installing the nvidia photoshop dds plugin to view the mips of the saved cube maps.

"Compute" Button
The compute button forces invalidation of the IBLProbe and causes the probe to be resampled.

"Cancel" Button
The cancel button forces the IBLProbe to be marked valid and no further computation will take place until either a dependency to the probe is altered or "Compute" is clicked on.

Tweak Parameters
--------------
Exposure: Exposure parameters for tone mapping.

Gamma: Output gamma for color correction.

Input Gamma: The color space of the input environment map. 1.0 for linear.

Debug Visualization:

  No Debug: Renders the object lit.
  Normal: World space normals.
  Ambient Occlusion: The blue channel from the RMAOC specular map that contains the ambient occlusion.
  Albedo: The albedo map for the object.
  IBL Diffuse: The diffuse irradiance contribution from the environment.
  IBL Specular: The environment map value sample using reflection vector and the mip layer using the roughness value in the specular map.
  Metal: The green channel from the RMAOC specular map.
  Roughness: The red channel from the RMAOC specular map.
  BRDF: The computed bias/scale terms from the BRDF Lut using ndotv and roughness.

Specular Workflow: Inverts roughness and/or metalness terms for the visualized object.

Specular Intensity:A multiplier to the material specular term for the visualized object.

Roughness Gloss Scale: An inverse multiplier to the roughness term for the visualized object.

BRDF: 
The brdf that is current being used to evaluate the IBLProbe and render the object.
BRDFs can be found in data/shadersD3D11 in .brdf files.
These files are hlsl that are concatenated with the core shaders IblImportanceSamplingDiffuse.hlsl, IblImportanceSamplingSpecular.hlsl and IblBrdf.hlsl.
You can create new .brdf files to experiment.
.brdf files are loaded on application startup and are enumerated automatically.

The generated BRDF lut is displayed in the top right hand corner of the applciation

You may edit shaders while the application is running and they will be automatically reloaded.
There is no runtime guard around shader failure. If you write an invalid shader the application will crash. Failures are written data/ShadersD3D11/ShaderFailures with the error and warning list at the end of the file.

Diffuse Resolution: The per face dimensions of the diffuse cube map.

Specular Resolution: The per face dimensions of the specular cube map. A full mip chain will be created for the cubemap.

Mip Drop: The number of mips above the 1x1 face in the specular cube map to ignore when computing roughness convolutions.

Total Samples: The number of samples to use per pixel when importance sampling.

Samples per frame: The number of samples that are computed per update of the IBL Probe.
If the samples per frame != total samples, the IBL Probe will be progressively sampled.
This functionality is still experimental and has some bugs.
For the moment, please ensure that the total samples / number of samples per frame is a positive whole number.

Environment Scale: A scale term applied to every sample when importance sampling.

IBL Saturation: A saturation term applied to every sample when importance sampling.

IBL Contrast: Incomplete. A contrast term applied to every sample when importance sampling.
I still need to write some code to make this work correctly.

IBL Hue: A hue term applied to every sample when importance sampling.

Max R/G/B: A debug attribute that displays the maximum pixel value in the input environment image.

How does the tool work (a broad overview)?
--------------

The quick overview.
This is basically a complete implementation of the technique outlined by Epic in:
http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf

1.) Starting at Application::updateApplciation() in Application.cpp.
    _scene->update() is called which updates all of the IBLProbes and BRDFs.
    At this stage the BRDF lut is computed in Brdf::compute() using the compute shader in data/shaders/IBLBRDF.hlsl which is concatenated with the current .brdf file (which can be found in data/shaders/).

2.) At line 410 of Application::updateApplication in Application.cpp the IBLProbe is computed.
     The core of this operation can be found in IBLRenderPass::render()

     The IBLRenderPass computes the IBL (if it is not cached):

     - Projects the sphere geometry of the scene to a 2048x2048 cubemap using the geometry shader that we will nominate the "environment source". It would also be possible to render a scene using this technique to an environment map. The shader for this operation can be found under: data/ShadersD3D11/IBLEnvironmentSphereSpherical.fx.
     - Mip maps are generated for the environment source.
     - The diffuse irradiance environment map is computed by projecting a sphere using a geometry shader over the diffuse irradiance environment map and importance sampling per pixel using the source environment with the shader data/ShadersD3D11/IBLImportanceSamplingDiffuse.fx concatenated with the current .brdf file.
     - The specular environment map is computed by rendering to each mip of an environment map with an associated roughness term by projecting a sphere using a geometry shader over the specular environment map mip level and importance sampling per pixel using the source environment with the shader data/ShadersD3D11/IBLImportanceSamplingSpecular.fx concatenated with the current .brdf file.
     The ColorPass is then rendered using IblColorPass.cpp:
     - The pistol is rendered using the shader IblPBRDebug.fx . The inputs to this shader are the diffuse irradiance environment map, the specular roughness environment map, the brdf LUT, the object albedo map, the specular map and a normal map.

How does the tool work (a more detailed overview)?
--------------

1.) Computing the BRDF LUT:

    IBlBaker/data/shadersD3D11/IBLBrdf.hlsl
    For each pixel (x, y) in the target LUT:
       Compute roughness (0 to 1.0) based on pixel x coordinate.
       Compute normal dot view (NoV) (0 to 1.0) based on pixel y coordinate.
       Set normal as float3(0,0,1).  
       Set view as float3(sqrt(1.0-NoV*NoV), 0, NoV);
       Compute roughness squared (k==r2==r*r)
       Integrate number of importance samples for (roughness and NoV).
       For each sample:
            Compute a Hammersely coordinate.
            Convert Hammersley to world space coordinate based on roughness.
            Compute reflection vector L
            Compute NoL (normal dot light), NoH (normal dot half), VoH (view dot half)
            If NoL > 0
              Sample contributes.
              The following functions are implemented in the .brdf file.
              Compute the geometry term for the BRDF given roughness squared, NoV, NoL
              Compute the visibility term given G, VoH, NoH, NoV, NoL
              Compute the fresnel term given VoH.
              Sum the result given fresnel, geoemtry, visibility.
       Average result over number of samples.
       
2.) Computing the diffuse irradiance environment:

    IBlBaker/data/shadersD3D11/IBLImportanceSamplingDiffuse.fx
    Given a sphere mesh input centered at the world origin.
    Vertex shader:
    Transform vertex position to the world.
    Transform normal to world.
    
    Geometry shader:
    Transform the world position to 6 faces of a cubemap.
    
    Pixel Shader:
    For each pixel:
        Set View (V) = Normal (N).
        Compute the sample step as MaxSamples / SampleCountPerFrame.
        sampleStartId = sample offset (current offset in all samples).
        For (0 through to SampleCountPerFrame)
            Compute a Hammersley coordinate given the current sampleId
            H = Convert the Hammersely coordinate to a world coordinate H.
            L = reflection vector V, H
            NoL = Normal dot Light
            If the sample contributes.
            if (NoL > 0)
                The following functions are implemented in the .brdf file.
                pdf = evaluate the probability density function
                Mip map (lod) = compute the mip map using the solid angle, source map size, distortion and the pdf.
                Sample = Cubemap sample at coordinate H and mip map level lod.
                Sum the Sample to Result.xyz and Sum weight to Result.w.     
            Increment the sampleId by the by the sample step.
    
        Result = Divide Result.xyz by accumulated weight in Result.w.
        OutputResult = interpolate the last result to the current result by 1.0 / ConvolutionSamplesOffset)
   
3.) Computing the specular irradiance environment:

    IBlBaker/data/shadersD3D11/IBLImportanceSamplingSpecular.fx
    Given a sphere mesh input centered at the world origin and a mip map target that will be sampled using Roughness (0 to 1.0 over all mips, where mip=0 represents roughness 0 and mip = (NumMips-1) = 1.0.
    
    Vertex shader:
    Transform vertex position to the world.
    Transform normal to world.
    
    Geometry shader:
    Transform the world position to 6 faces of a cubemap.
    
    Pixel Shader:
    For each pixel:
        Set View (R) = Normal (R) = R = world vertex normal.
        Compute the sample step as MaxSamples / SampleCountPerFrame.
        sampleStartId = sample offset (current offset in all samples).
        For (0 through to SampleCountPerFrame)
            Compute a Hammersley coordinate given the current sampleId
            H = Convert the Hammersely coordinate to a world coordinate H given Roughness.
            L = reflection vector V, H
            NoL = Normal dot Light
            VoL = Normal dot View
           
            If the sample contributes.
            if (NoL > 0)
                The following functions are implemented in the .brdf file.
                pdf = evaluate the probability density function give NoL, VoL and Roughness
                Mip map (lod) = compute the mip map using the solid angle, source map size, distortion and the pdf.
                Sample = Cubemap sample at coordinate L and mip map level lod.
                Sum the Sample to Result.xyz and Sum weight to Result.w.     
            Increment the sampleId by the by the sample step.
    
        Result = Divide Result.xyz by accumulated weight in Result.w.
    OutputResult = interpolate the last result to the current result by 1.0 / ConvolutionSamplesOffset)


What would you like my input on?
--------------
So, this is left over documentation from the review release of the baker.
I am still very, very interested in finding out where/if my math is incorrect.
Please contact me through github (https://github.com/derkreature/). if you have any ideas or opinions.
1.) The math that is used in computing the IBL Probe.
     These files are:

     IBLBrdf.fx

     IBLImportanceSamplingDiffuse.fx

     IBLImportanceSamplingSpecular.fx

     schlick.brdf

     smith.brdf

     I will be writing more about the math in here, and will be relying on 
     questions from you in preparing a future invitation to other individuals 
     that I do not have a close working relationship within the industry.

2.) The math for shading the object:
     IBLPBRDebug.fx

3.) The validity of my method for progressive sampling using importance sampling and where I have gone wrong (if possible).

Am I taking contributions?
Yes, please. I would be happy to merge any changes you suggest into the release if they do not conflict with your employment situation and will not endanger the project's ability to be released publicly.

How far away am I from a public release?
This depends on whether my math is bunk or not.
I still need to fix some memory leaks and perform one final pass on the codebase so that it is clean/er. I hope to have the library available to the general public by the end of August / start of September.

Please email me if you have any questions.


The IBLBaker uses portions of:
==============

OGRE Engine under the MIT license.
--------------
http://www.ogre3d.org/
This was mainly for codec support after Microsoft dropped support for D3DX and 
decided that saving cubemaps with mips was something that their replacement
code didn't need to support.

Copyright (c) 2000-2012 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Cerberus by Andrew Maximov
--------------
Thank you to Andrew Maximov for the use of his pistol asset:
http://artisaverb.info/

HDR Probes by HDRLabs
--------------
IBLs form HDRLabs are used under the Creative Commons Attribution-Noncommercial-Share Alike 3.0 License.
http://www.hdrlabs.com/sibl/index.html

The Uncharted2Tonemap operator from:
--------------
http://filmicgames.com/archives/75

DXUT widgets
--------------
DXUT widgets from Microsoft DirectX SDK ported to use the IBLBaker abstraction.

EffectsD3D11 framework
--------------
Modified version of EffectsD3D11 framework shipped in June 2010 
DirectX SDK

AngelCode font manager:
--------------
Portions of the AngelCode Font manager:
   AngelCode Tool Box Library
   Copyright (c) 2007-2008 Andreas Jonsson
  
   This software is provided 'as-is', without any express or implied 
   warranty. In no event will the authors be held liable for any 
   damages arising from the use of this software.

   Permission is granted to anyone to use this software for any 
   purpose, including commercial applications, and to alter it and 
   redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you 
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product 
      documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and 
      must not be misrepresented as being the original software.

   3. This notice may not be removed or altered from any source 
      distribution.

AMDCubeMapGen
--------------
The cubemap seam fixer was templated and integrated into the project:
https://code.google.com/p/cubemapgen/
under the New BSD License.
Copyright (c) 2005, ATI Research, Inc.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, 
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Cupcake icon:
--------------
from: http://www.flaticon.com/free-icon/cupcake-sugar_12348
<div>Icon made by 
     <a href="http://www.freepik.com" alt="Freepik.com" title="Freepik.com">Freepik</a> from 
     <a href="http://www.flaticon.com/free-icon/cupcake-sugar_12348" 
      title="Flaticon">www.flaticon.com</a></div>
The author of this icon will be grateful if you use the following legal attribution
"Icon made by Freepik from Flaticon.com"

The IBLBaker is dependent on the following libraries and bits of code:
==============

FreeImage under the freeimage public license: (Optional).
--------------
http://freeimage.sourceforge.net/
http://freeimage.sourceforge.net/freeimage-license.txt

Pugixml
--------------
under the MIT license:
http://pugixml.org/

MurmerHash under the MIT license.
--------------
https://code.google.com/p/smhasher/

TinyObj (Replaced AssImp):
--------------------------
(Syoyo Fujita) (Some minor changes in the local build).
(Domo Syoyo san).
https://github.com/syoyo/tinyobjloader

Imgui
-----
https://github.com/ocornut/imgui

NanoVG
------
https://github.com/memononen/nanovg

Instructive code for drawing both ImGUI and NanoVG from BgFx :).
----------------------------------------------------------------
The more I read Bgfx, the more I love it.
https://github.com/bkaradzic/bgfx




 

