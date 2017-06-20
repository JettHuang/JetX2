-- premake5.lua

-- solution
workspace "JetXAll"
    configurations { "Debug", "Release" }
    location "Build"
    language "C++"
    architecture "x86"
    
	-- Catch requires RTTI and exceptions
	exceptionhandling "On"
	rtti "On"


    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        targetsuffix("_d")

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
    
    filter "system:windows"
        defines { "XPLATFORM_WINDOWS" }

    filter "system:macosx"
        defines { "XPLATFORM_MACOSX" }
        buildoptions { "-Wunused-value -Wshadow -Wreorder -Wsign-compare -Wall" }

    filter {}

    targetdir("Build/Bin")  
    objdir("Build/Obj/%{prj.name}/%{cfg.longname}")
        
-- Helper function for set third parties
function Include_Thirdparty()
    includedirs {
        "../Src/ThirdParty/glew/include",
        "../Src/ThirdParty/glfw/include",
    }
    defines { "GLEW_STATIC" }
end

function Link_Thirparty()
    filter { "system:windows" }
        libdirs {
            "../Src/ThirdParty/glew/lib_win32",
            "../Src/ThirdParty/glfw/lib_win32"
        }

    filter { "system:macosx" }
        libdirs {
            "../Src/ThirdParty/glew/lib_osx",
            "../Src/ThirdParty/glfw/lib_osx"
        }

    filter { "kind:not StaticLib"}
        links {
            "glfw3",
            "GLEW"
        }

    filter {}
end

-- project Engine
project "JetXEngine"
    kind "StaticLib"

    files {
        -- AppFramework
        "../Src/AppFramework/Application.h",
        "../Src/AppFramework/Application.cpp",
        -- Foundation
        "../Src/Foundation/JetX.h",
        "../Src/Foundation/FileSystem.h",
        "../Src/Foundation/FileSystem.cpp",
        "../Src/Foundation/RefCounting.h",
        "../Src/Foundation/XMemory.h",
        "../Src/Foundation/XMemory.cpp",
        "../Src/Foundation/OutputDevice.h",
        "../Src/Foundation/OutputDevice.cpp",
        -- Renderer Interface
        "../Src/Renderer/Renderer.h",
        "../Src/Renderer/Renderer.cpp",
        "../Src/Renderer/RendererDefs.h",
        "../Src/Renderer/RendererState.h",
        "../Src/Renderer/RHIResource.h",
        -- OpenGL
        "../Src/Renderer/OpenGL/OpenGLCommand.cpp",
        "../Src/Renderer/OpenGL/OpenGLDataBuffer.cpp",
        "../Src/Renderer/OpenGL/OpenGLDataBuffer.h",
        "../Src/Renderer/OpenGL/OpenGLRenderer.h",
        "../Src/Renderer/OpenGL/OpenGLRenderer.cpp",
        "../Src/Renderer/OpenGL/OpenGLResource.cpp",
        "../Src/Renderer/OpenGL/OpenGLShader.h",
        "../Src/Renderer/OpenGL/OpenGLShader.cpp",
        "../Src/Renderer/OpenGL/OpenGLState.h",
        "../Src/Renderer/OpenGL/OpenGLState.cpp",
        "../Src/Renderer/OpenGL/OpenGLVertexDeclaration.h",
        "../Src/Renderer/OpenGL/OpenGLVertexDeclaration.cpp",
        "../Src/Renderer/OpenGL/OpenGLViewport.h",
        "../Src/Renderer/OpenGL/OpenGLViewport.cpp",
        "../Src/Renderer/OpenGL/PlatformOpenGL.h",
    }
    -- platform dependency files
    filter "system:windows"
        files {
            "../Src/Renderer/OpenGL/Windows/OpenGLWindows.h",
            "../Src/Renderer/OpenGL/Windows/OpenGLWindows.cpp",
        }

    filter "system:macosx"
        files {
            "../Src/Renderer/OpenGL/Mac/OpenGLMac.h",
            "../Src/Renderer/OpenGL/Mac/OpenGLMac.mm",
        }

    filter {}

    -- includes directory
    includedirs "../Src"
    
    -- third party
    Include_Thirdparty()
    Link_Thirparty()

---------------------------------------------------------------
-- Helper for link with JetXEngine
function Configure_JetXEngine()
    -- includes directory
    includedirs "../Src"
    dependson { "JetXEngine" }
    
    -- third party
    Include_Thirdparty()
    Link_Thirparty()

    -- OpenGL
    filter { "system:windows" }
        links { "OpenGL32" }
    filter { "system:not windows" }
        links { "OpenGL.framework",
				"Cocoa.framework",
				"IOKit.framework",
				"CoreFoundation.framework",
				"CoreVideo.framework" 
			}
    filter {}

end

-- FirsetExample
project "FirsetExample"
    kind "ConsoleApp"
    files {
        "../Src/Examples/FirstExample/FirstExample.cpp"
    }
    Configure_JetXEngine()

