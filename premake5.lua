workspace "RocketMQ"
	architecture "x64"
	startproject "RocketMQ"

	configurations
	{
		"Debug",
		"Release",
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "RocketMQ"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
	}

	defines
	{
		"WIN32",
	}

	includedirs
	{
		"src",
		"%{wks.location}/vendor/aliyun-mq/include",
		"C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0/ucrt",
	}

	libdirs
	{
		"%{wks.location}/vendor/aliyun-mq/lib/64",
	}

	links
	{
		"ONSClient4CPP.lib",
	}

	filter "system:windows"
		systemversion "latest"
		editAndContinue "Off"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"