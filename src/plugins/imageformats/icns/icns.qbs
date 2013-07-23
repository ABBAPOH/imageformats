import qbs.base 1.0

DynamicLibrary {
    name : "qicns"
    destinationDirectory: "imageformats"

    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: ["core", "gui"] }

    files : [
        "main.h",
        "main.cpp",
        "icnsformat.h",
        "icnsheader.h",
        "icnspalette.h",
        "icnsreader.h",
        "icnsreader.cpp"
    ]

    Group {
        name: "icns.json"
        condition: qbs.Qt.core.versionMajor >= 5
        files: [ "icns.json" ]
    }

    Group {
        name: "icns.rc"
        condition: qbs.targetOS.contains("windows")
        files: "icns.rc"
    }
}
