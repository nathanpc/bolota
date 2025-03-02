# Bolota

A note taking application for people that strictly use the
[outline method](https://www.goodnotes.com/blog/outline-note-taking-method) of
note taking and want a tool that was designed for taking notes in that style
as quickly and as productive as possible.

## The name

The name might seem a bit odd to some people, it's the portuguese name for an
[acorn](https://pt.wikipedia.org/wiki/Bolota), which I thought was fitting given
that most outline notes use bullet points (if you imagine enough they can turn
into acorns).

## Ports

This application has been ported to several of Microsoft's platforms, and thus
compilling may require additional setup for some of them.

### Windows Desktop

The application's main target was always the Windows desktop. The project
contains a Visual C++ 6 workspace under 'vc6/Bolota.dsw' so that the application
can be compiled into a binary that's compatible with every version of Windows
imaginable. This should be used for the release build.

It's important to note that in order to compile this project under Visual C++ 6
you must have installed the
[Windows Server 2003 Platform SDK](https://learn.microsoft.com/en-us/archive/msdn-technet-forums/e1147034-9b0b-4494-a5bc-6dfebb6b7eb1).
This provides some modern headers that ease the generational gap in Microsoft's
tools.

For ease of development a Visual Studio 2022 solution can be found at
'vs2022/Bolota.sln'. This is a convenience project and is meant to provide a
modern development environment for the project. The code that is written should
always be compatible with the Visual C++ 6 project.

### Windows CE (Handheld PC)

A project for Microsoft's abandoned Handheld PC initiative can be found at
'wince/Bolota.vcw'. This is an eMbedded Visual C++ 3.0 workspace and is targeted
at Windows CE 2.11.

### Windows Mobile

A port to Microsoft's mobile device platform has also been carried out and is
available as a Visual Studio 2005 solution under 'winmobile/Bolota.sln'. The
[Pocket PC 2002 SDK](https://archive.org/details/WMSDK) is required for
compilation.

## License

This library is free software; you may redistribute and/or modify it under the
terms of the [Mozilla Public License 2.0](https://www.mozilla.org/en-US/MPL/2.0/).
